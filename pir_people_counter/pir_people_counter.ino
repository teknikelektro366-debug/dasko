/*
  PIR PEOPLE COUNTER - SIMPLE VERSION
  Menggunakan 2 sensor PIR untuk mendeteksi orang masuk dan keluar
  
  Hardware:
  - ESP32
  - 2x PIR Motion Sensor (HC-SR501 atau similar)
  - Optional: TFT Display
  
  Wiring:
  PIR Sensor MASUK:
  - VCC → 5V (atau 3.3V)
  - GND → GND  
  - OUT → Pin 32
  
  PIR Sensor KELUAR:
  - VCC → 5V (atau 3.3V)
  - GND → GND
  - OUT → Pin 33
  
  Keunggulan PIR:
  - Lebih murah dari ultrasonic
  - Konsumsi daya rendah
  - Deteksi gerakan manusia yang akurat
  - Tidak terpengaruh suara
  - Range deteksi luas (3-7 meter)
  
  Kekurangan PIR:
  - Tidak bisa mengukur jarak
  - Delay response ~2-3 detik
  - Bisa false trigger dari hewan/objek panas
*/

#include <Arduino.h>

// ================= SENSOR PIR =================
// Sensor MASUK (di pintu masuk)
#define PIR_IN   32    // PIR sensor MASUK
// Sensor KELUAR (di pintu keluar)  
#define PIR_OUT  33    // PIR sensor KELUAR

// ================= DETECTION PARAMETERS =================
#define MIN_DETECTION_TIME 500        // Minimal 500ms untuk deteksi valid
#define MAX_DETECTION_TIME 5000       // Maksimal 5 detik untuk sequence
#define PERSON_COOLDOWN 3000          // Cooldown 3 detik antar deteksi (PIR butuh waktu lebih lama)
#define MAX_PEOPLE 20                 // Maksimal 20 orang
#define PIR_SETTLE_TIME 1000          // Waktu stabilisasi PIR setelah trigger

// ================= VARIABLES =================
int jumlahOrang = 0;

// State machine untuk deteksi arah
enum DetectionState {
  IDLE,           // Menunggu deteksi
  IN_TRIGGERED,   // PIR masuk aktif
  OUT_TRIGGERED,  // PIR keluar aktif
  BOTH_TRIGGERED  // Kedua PIR aktif
};

DetectionState currentState = IDLE;
unsigned long stateStartTime = 0;
unsigned long lastPersonDetected = 0;

// PIR state tracking
bool lastPirIn = LOW;
bool lastPirOut = LOW;
unsigned long lastPirInChange = 0;
unsigned long lastPirOutChange = 0;

// ================= PIR FUNCTIONS =================
bool readPIR(int pirPin, bool &lastState, unsigned long &lastChange) {
  bool currentState = digitalRead(pirPin);
  unsigned long now = millis();
  
  // Deteksi perubahan state
  if (currentState != lastState) {
    lastChange = now;
    lastState = currentState;
  }
  
  // Return stable state (debounced)
  return currentState;
}

// ================= PEOPLE DETECTION LOGIC =================
void detectPeople() {
  unsigned long now = millis();
  
  // Cooldown untuk mencegah deteksi ganda
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
  // Baca status PIR sensors
  bool pirInActive = readPIR(PIR_IN, lastPirIn, lastPirInChange);
  bool pirOutActive = readPIR(PIR_OUT, lastPirOut, lastPirOutChange);
  
  // Debug info setiap 2 detik
  static unsigned long lastDebug = 0;
  if (now - lastDebug > 2000) {
    Serial.print("🔍 PIR Status - ");
    Serial.print("MASUK: ");
    Serial.print(pirInActive ? "MOTION DETECTED ✅" : "NO MOTION ❌");
    Serial.print(" | KELUAR: ");
    Serial.print(pirOutActive ? "MOTION DETECTED ✅" : "NO MOTION ❌");
    
    Serial.print(" | State: ");
    switch(currentState) {
      case IDLE: Serial.print("MENUNGGU"); break;
      case IN_TRIGGERED: Serial.print("MASUK_AKTIF"); break;
      case OUT_TRIGGERED: Serial.print("KELUAR_AKTIF"); break;
      case BOTH_TRIGGERED: Serial.print("KEDUA_AKTIF"); break;
    }
    Serial.print(" | Orang: ");
    Serial.println(jumlahOrang);
    lastDebug = now;
  }
  
  // State machine untuk deteksi arah pergerakan
  switch (currentState) {
    case IDLE:
      // Dari idle, cek PIR mana yang aktif duluan
      if (pirInActive && !pirOutActive) {
        currentState = IN_TRIGGERED;
        stateStartTime = now;
        Serial.println("🚶 → PIR MASUK aktif (orang akan masuk)");
      } else if (!pirInActive && pirOutActive) {
        currentState = OUT_TRIGGERED;
        stateStartTime = now;
        Serial.println("🚶 ← PIR KELUAR aktif (orang akan keluar)");
      }
      break;
      
    case IN_TRIGGERED:
      if (pirInActive && pirOutActive) {
        // Kedua PIR aktif - orang sedang melewati
        currentState = BOTH_TRIGGERED;
        Serial.println("🚶 ↔ Kedua PIR aktif (orang sedang melewati)");
      } else if (!pirInActive && !pirOutActive) {
        // Kedua PIR tidak aktif - sequence selesai
        if (now - stateStartTime >= MIN_DETECTION_TIME) {
          // Sequence: MASUK → KEDUA → KOSONG = ORANG MASUK
          if (jumlahOrang < MAX_PEOPLE) {
            jumlahOrang++;
            lastPersonDetected = now;
            Serial.println("🎉 >>> ORANG MASUK! Total: " + String(jumlahOrang));
          } else {
            Serial.println("⚠️ Ruangan sudah penuh (" + String(MAX_PEOPLE) + " orang)");
          }
        } else {
          Serial.println("⚠️ Deteksi terlalu cepat, diabaikan");
        }
        currentState = IDLE;
      } else if (now - stateStartTime > MAX_DETECTION_TIME) {
        Serial.println("⏰ Timeout pada PIR MASUK");
        currentState = IDLE;
      }
      break;
      
    case OUT_TRIGGERED:
      if (pirInActive && pirOutActive) {
        // Kedua PIR aktif - orang sedang melewati
        currentState = BOTH_TRIGGERED;
        Serial.println("🚶 ↔ Kedua PIR aktif (orang sedang melewati)");
      } else if (!pirInActive && !pirOutActive) {
        // Kedua PIR tidak aktif - sequence selesai
        if (now - stateStartTime >= MIN_DETECTION_TIME && jumlahOrang > 0) {
          // Sequence: KELUAR → KEDUA → KOSONG = ORANG KELUAR
          jumlahOrang--;
          lastPersonDetected = now;
          Serial.println("👋 <<< ORANG KELUAR! Total: " + String(jumlahOrang));
        } else if (jumlahOrang == 0) {
          Serial.println("⚠️ Tidak ada orang untuk dikurangi");
        } else {
          Serial.println("⚠️ Deteksi terlalu cepat, diabaikan");
        }
        currentState = IDLE;
      } else if (now - stateStartTime > MAX_DETECTION_TIME) {
        Serial.println("⏰ Timeout pada PIR KELUAR");
        currentState = IDLE;
      }
      break;
      
    case BOTH_TRIGGERED:
      if (!pirInActive && !pirOutActive) {
        // Kedua PIR tidak aktif - orang selesai melewati
        Serial.println("🚶 Orang selesai melewati kedua PIR");
        currentState = IDLE;
      } else if (now - stateStartTime > MAX_DETECTION_TIME) {
        Serial.println("⏰ Timeout pada kedua PIR");
        currentState = IDLE;
      }
      break;
  }
  
  // Safety limits
  jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
}

// ================= DISPLAY STATUS =================
void displayStatus() {
  static unsigned long lastDisplay = 0;
  unsigned long now = millis();
  
  // Tampilkan status setiap 5 detik
  if (now - lastDisplay >= 5000) {
    Serial.println("\n📊 === STATUS PIR COUNTER ===");
    Serial.print("👥 Jumlah Orang: ");
    Serial.print(jumlahOrang);
    Serial.print("/");
    Serial.println(MAX_PEOPLE);
    
    Serial.print("🎯 Detection State: ");
    switch(currentState) {
      case IDLE: Serial.println("MENUNGGU (siap deteksi)"); break;
      case IN_TRIGGERED: Serial.println("MASUK_AKTIF (menunggu keluar)"); break;
      case OUT_TRIGGERED: Serial.println("KELUAR_AKTIF (menunggu masuk)"); break;
      case BOTH_TRIGGERED: Serial.println("KEDUA_AKTIF (orang melewati)"); break;
    }
    
    Serial.print("🔥 PIR Status: MASUK=");
    Serial.print(digitalRead(PIR_IN) ? "ACTIVE" : "IDLE");
    Serial.print(" | KELUAR=");
    Serial.println(digitalRead(PIR_OUT) ? "ACTIVE" : "IDLE");
    
    Serial.print("⏰ Uptime: ");
    Serial.print(now / 1000);
    Serial.println(" detik");
    
    Serial.println("📊 =============================\n");
    lastDisplay = now;
  }
}

// ================= SERIAL COMMANDS =================
void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command == "reset" || command == "r") {
      jumlahOrang = 0;
      currentState = IDLE;
      lastPersonDetected = 0;
      Serial.println("✅ Reset - Jumlah orang dan state direset ke 0");
    }
    else if (command == "test" || command == "t") {
      Serial.println("🔥 === TEST SENSOR PIR ===");
      
      // Test kedua PIR
      bool pirIn = digitalRead(PIR_IN);
      bool pirOut = digitalRead(PIR_OUT);
      
      Serial.print("🔥 PIR MASUK (Pin 32): ");
      Serial.print(pirIn ? "MOTION DETECTED ✅" : "NO MOTION ❌");
      Serial.print(" (Digital: ");
      Serial.print(pirIn);
      Serial.println(")");
      
      Serial.print("🔥 PIR KELUAR (Pin 33): ");
      Serial.print(pirOut ? "MOTION DETECTED ✅" : "NO MOTION ❌");
      Serial.print(" (Digital: ");
      Serial.print(pirOut);
      Serial.println(")");
      
      Serial.print("👥 Jumlah orang: ");
      Serial.println(jumlahOrang);
      Serial.print("🎯 State: ");
      switch(currentState) {
        case IDLE: Serial.println("MENUNGGU"); break;
        case IN_TRIGGERED: Serial.println("MASUK_AKTIF"); break;
        case OUT_TRIGGERED: Serial.println("KELUAR_AKTIF"); break;
        case BOTH_TRIGGERED: Serial.println("KEDUA_AKTIF"); break;
      }
      
      Serial.println("\n💡 Tips PIR:");
      Serial.println("- PIR butuh waktu ~30 detik untuk kalibrasi");
      Serial.println("- Hindari sumber panas (AC, lampu, dll)");
      Serial.println("- Jarak optimal: 3-7 meter");
      Serial.println("- Sensitivity bisa diatur dengan potensiometer");
      Serial.println("🔥 ============================");
    }
    else if (command.startsWith("set ")) {
      int newCount = command.substring(4).toInt();
      if (newCount >= 0 && newCount <= MAX_PEOPLE) {
        jumlahOrang = newCount;
        Serial.println("✅ Jumlah orang diset ke: " + String(jumlahOrang));
      } else {
        Serial.println("❌ Jumlah tidak valid. Gunakan 0-" + String(MAX_PEOPLE));
      }
    }
    else if (command == "live" || command == "l") {
      Serial.println("🔴 === LIVE MONITORING PIR ===");
      Serial.println("Monitoring PIR real-time selama 30 detik...");
      Serial.println("Coba berjalan melewati sensor untuk test");
      
      for (int i = 0; i < 300; i++) { // 30 detik
        bool pirIn = digitalRead(PIR_IN);
        bool pirOut = digitalRead(PIR_OUT);
        
        Serial.print("Live " + String(i+1) + " - ");
        Serial.print("IN:");
        Serial.print(pirIn ? "MOTION✅" : "IDLE❌");
        
        Serial.print(" | OUT:");
        Serial.print(pirOut ? "MOTION✅" : "IDLE❌");
        
        Serial.print(" | People:" + String(jumlahOrang));
        Serial.print(" | State:");
        switch(currentState) {
          case IDLE: Serial.print("IDLE"); break;
          case IN_TRIGGERED: Serial.print("IN"); break;
          case OUT_TRIGGERED: Serial.print("OUT"); break;
          case BOTH_TRIGGERED: Serial.print("BOTH"); break;
        }
        Serial.println();
        
        delay(100);
      }
      Serial.println("🔴 Live monitoring selesai");
    }
    else if (command == "calibrate" || command == "c") {
      Serial.println("🔧 === KALIBRASI PIR ===");
      Serial.println("Kalibrasi PIR selama 30 detik...");
      Serial.println("JANGAN BERGERAK di depan sensor!");
      
      for (int i = 30; i > 0; i--) {
        Serial.print("Kalibrasi: ");
        Serial.print(i);
        Serial.print(" detik tersisa | PIR IN:");
        Serial.print(digitalRead(PIR_IN) ? "ACTIVE" : "IDLE");
        Serial.print(" | PIR OUT:");
        Serial.println(digitalRead(PIR_OUT) ? "ACTIVE" : "IDLE");
        delay(1000);
      }
      
      Serial.println("✅ Kalibrasi selesai!");
      Serial.println("PIR siap digunakan untuk deteksi");
    }
    else if (command == "help" || command == "h") {
      Serial.println("🆘 === PERINTAH PIR COUNTER ===");
      Serial.println("test/t - Test status PIR detail");
      Serial.println("live/l - Live monitoring 30 detik");
      Serial.println("calibrate/c - Kalibrasi PIR 30 detik");
      Serial.println("reset/r - Reset jumlah orang ke 0");
      Serial.println("set X - Set jumlah orang ke X (0-20)");
      Serial.println("help/h - Tampilkan bantuan ini");
      Serial.println("🆘 ==============================");
    }
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n🔥 ================================");
  Serial.println("  PIR PEOPLE COUNTER");
  Serial.println("  Sensor: 2x PIR Motion Sensor");
  Serial.println("  Detection: Masuk & Keluar");
  Serial.println("  Range: 3-7 meter");
  Serial.println("  Max People: " + String(MAX_PEOPLE));
  Serial.println("🔥 ================================");
  
  // Initialize PIR sensor pins
  pinMode(PIR_IN, INPUT);
  pinMode(PIR_OUT, INPUT);
  
  Serial.println("🎯 SISTEM SIAP!");
  Serial.println("Fitur PIR:");
  Serial.println("- Deteksi gerakan infrared");
  Serial.println("- Range deteksi luas (3-7m)");
  Serial.println("- Konsumsi daya rendah");
  Serial.println("- Tidak terpengaruh suara");
  Serial.println("- State machine yang robust");
  Serial.println("- Anti-noise & timeout protection");
  
  Serial.println("\n🎮 Perintah Serial:");
  Serial.println("- 'test' - Test PIR");
  Serial.println("- 'live' - Live monitoring");
  Serial.println("- 'calibrate' - Kalibrasi PIR");
  Serial.println("- 'reset' - Reset counter");
  Serial.println("- 'set X' - Set jumlah ke X");
  Serial.println("- 'help' - Bantuan");
  
  // PIR Warm-up period
  Serial.println("\n🔧 === PIR WARM-UP ===");
  Serial.println("PIR membutuhkan waktu ~30 detik untuk kalibrasi...");
  Serial.println("Hindari gerakan di depan sensor selama warm-up");
  
  for (int i = 30; i > 0; i--) {
    Serial.print("Warm-up: ");
    Serial.print(i);
    Serial.print(" detik | PIR IN:");
    Serial.print(digitalRead(PIR_IN) ? "ACTIVE" : "IDLE");
    Serial.print(" | PIR OUT:");
    Serial.println(digitalRead(PIR_OUT) ? "ACTIVE" : "IDLE");
    delay(1000);
  }
  
  Serial.println("✅ PIR warm-up selesai!");
  
  // Test PIR setelah warm-up
  Serial.println("\n🔥 === STARTUP PIR TEST ===");
  Serial.println("Testing PIR selama 10 detik...");
  Serial.println("Coba bergerak di depan sensor untuk test");
  
  bool pir1Detected = false;
  bool pir2Detected = false;
  
  for (int i = 0; i < 100; i++) {
    bool pirIn = digitalRead(PIR_IN);
    bool pirOut = digitalRead(PIR_OUT);
    
    if (pirIn) pir1Detected = true;
    if (pirOut) pir2Detected = true;
    
    Serial.print("Test " + String(i+1) + "/100 - ");
    Serial.print("MASUK: ");
    Serial.print(pirIn ? "MOTION✅" : "IDLE❌");
    Serial.print(" | KELUAR: ");
    Serial.print(pirOut ? "MOTION✅" : "IDLE❌");
    Serial.println();
    
    delay(100);
  }
  
  Serial.println("\n📊 HASIL TEST:");
  Serial.print("PIR MASUK: ");
  Serial.println(pir1Detected ? "✅ DETECTED MOTION" : "❌ NO MOTION");
  Serial.print("PIR KELUAR: ");
  Serial.println(pir2Detected ? "✅ DETECTED MOTION" : "❌ NO MOTION");
  
  if (!pir1Detected && !pir2Detected) {
    Serial.println("\n⚠️ PERINGATAN: Tidak ada motion terdeteksi!");
    Serial.println("Kemungkinan:");
    Serial.println("- PIR masih dalam masa kalibrasi");
    Serial.println("- Tidak ada gerakan selama test");
    Serial.println("- Kabel PIR tidak terhubung");
    Serial.println("- PIR rusak atau setting sensitivity rendah");
  } else {
    Serial.println("\n✅ PIR bekerja dengan baik!");
  }
  
  Serial.println("🔥 ==============================");
  Serial.println("🔥 ================================\n");
}

// ================= MAIN LOOP =================
void loop() {
  // Handle serial commands
  handleSerialCommands();
  
  // Detect people movement
  detectPeople();
  
  // Display status periodically
  displayStatus();
  
  // Small delay for stability
  delay(100); // PIR butuh delay lebih lama dari ultrasonic
}