#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>

// ================= TFT =================
TFT_eSPI tft;

// ================= IR =================
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

#define IR_PIN 14            // Pin IR LED untuk kontrol AC (LED IR langsung + resistor 220Ω)
IRPanasonicAc ac1(IR_PIN);
IRPanasonicAc ac2(IR_PIN);

// ================= SENSOR =================
// Ultrasonic Sensors HC-SR04 (sesuai pin configuration)
#define TRIGGER_PIN_IN  2    // Pin trigger sensor MASUK (sisi LUAR)
#define ECHO_PIN_IN     15   // Pin echo sensor MASUK (sisi LUAR)
#define TRIGGER_PIN_OUT 13   // Pin trigger sensor KELUAR (sisi DALAM)
#define ECHO_PIN_OUT    12   // Pin echo sensor KELUAR (sisi DALAM)

#define DHTPIN    27
#define DHTTYPE   DHT22
#define LDR_PIN   34         // Analog pin untuk LDR

DHT dht(DHTPIN, DHTTYPE);

// Ultrasonic Detection Parameters
#define MAX_DISTANCE 20     // Jarak maksimum sensor (20 cm)
#define MIN_DISTANCE 1       // Jarak minimum deteksi (1 cm)
#define MAX_DETECTION 15     // Range deteksi optimal (1-15 cm)
#define MAX_PEOPLE 20        // Kapasitas maksimal ruangan

// ================= FUNCTION DECLARATIONS =================
void kontrolAC(String &acStatus, int &setTemp);
void detectPeople();
void readSensors();
long readUltrasonicDistance(int triggerPin, int echoPin);
void updateSensorData();

// ================= VARIABLES =================
int jumlahOrang = 0;
int lastJumlahOrang = -1;

// Ultrasonic timing - ULTRA FAST RESPONSE
unsigned long lastPersonDetected = 0;
#define PERSON_COOLDOWN 300     // Cooldown 300ms antar deteksi (ULTRA FAST)

// Sensor readings - ULTRASONIC
struct SensorData {
  int distanceIn = 0;      // Sensor LUAR (masuk)
  int distanceOut = 0;     // Sensor DALAM (keluar)
  bool objectInDetected = false;   // Objek terdeteksi di sensor LUAR
  bool objectOutDetected = false;  // Objek terdeteksi di sensor DALAM
} sensorData;

// Current sensor data
struct CurrentData {
  int jumlahOrang = 0;
  String acStatus = "OFF";
  int setTemp = 0;
  float suhuRuang = 0.0;
  float humidity = 0.0;
  int lightLevel = 0;
  bool ultrasonic1 = false;
  bool ultrasonic2 = false;
  unsigned long timestamp = 0;
} currentData;

// AC Hardware State
bool lastAC1 = false;
bool lastAC2 = false;
int lastTemp1 = -1;
int lastTemp2 = -1;

// ================= ULTRASONIC FUNCTIONS =================
long readUltrasonicDistance(int triggerPin, int echoPin) {
  // Kirim pulse trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  // Baca pulse echo dengan timeout yang cepat
  long duration = pulseIn(echoPin, HIGH, 20000); // Timeout 20ms (ultra fast)
  
  // Hitung jarak dalam cm
  long distance = duration * 0.034 / 2;
  
  // Return 0 jika out of range atau error
  if (distance <= 0 || distance > MAX_DISTANCE) {
    return 0;
  }
  
  return distance;
}

// ================= SENSOR FUNCTIONS =================
void readSensors() {
  // Baca jarak dari kedua sensor dengan delay minimal
  sensorData.distanceIn = readUltrasonicDistance(TRIGGER_PIN_IN, ECHO_PIN_IN);
  delay(10); // Delay minimal antar sensor
  sensorData.distanceOut = readUltrasonicDistance(TRIGGER_PIN_OUT, ECHO_PIN_OUT);
  
  // Validasi pembacaan (0 = out of range atau error)
  if (sensorData.distanceIn == 0) sensorData.distanceIn = MAX_DISTANCE + 1;
  if (sensorData.distanceOut == 0) sensorData.distanceOut = MAX_DISTANCE + 1;
  
  // Deteksi objek dalam range 1-15cm
  sensorData.objectInDetected = (sensorData.distanceIn >= MIN_DISTANCE && 
                                sensorData.distanceIn <= MAX_DETECTION);
  sensorData.objectOutDetected = (sensorData.distanceOut >= MIN_DISTANCE && 
                                 sensorData.distanceOut <= MAX_DETECTION);
}

// ================= ULTRASONIC DETECTION - ULTRA FAST =================
void detectPeople() {
  unsigned long now = millis();
  
  // Cooldown untuk mencegah deteksi ganda (ultra fast)
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
  // Debug info setiap 2 detik
  static unsigned long lastDebug = 0;
  if (now - lastDebug > 2000) {
    Serial.print("📊 Ultrasonic - Orang: ");
    Serial.print(jumlahOrang);
    Serial.print("/20 | LUAR:");
    Serial.print(sensorData.distanceIn);
    Serial.print("cm");
    Serial.print(sensorData.objectInDetected ? "✅" : "❌");
    Serial.print(" | DALAM:");
    Serial.print(sensorData.distanceOut);
    Serial.print("cm");
    Serial.print(sensorData.objectOutDetected ? "✅" : "❌");
    Serial.println();
    lastDebug = now;
  }
  
  // LOGIKA ULTRA CEPAT: Deteksi berdasarkan sensor mana yang aktif
  static bool lastInDetected = false;
  static bool lastOutDetected = false;
  
  // Deteksi MASUK: Sensor LUAR aktif (dari tidak aktif ke aktif)
  if (sensorData.objectInDetected && !lastInDetected) {
    jumlahOrang++;
    lastPersonDetected = now;
    Serial.println("🚶 → ULTRASONIC LUAR AKTIF - ORANG MASUK!");
    Serial.println("📊 Jumlah orang: " + String(jumlahOrang) + "/20");
  }
  
  // Deteksi KELUAR: Sensor DALAM aktif (dari tidak aktif ke aktif)  
  if (sensorData.objectOutDetected && !lastOutDetected && jumlahOrang > 0) {
    jumlahOrang--;
    lastPersonDetected = now;
    Serial.println("🚶 ← ULTRASONIC DALAM AKTIF - ORANG KELUAR!");
    Serial.println("📊 Jumlah orang: " + String(jumlahOrang) + "/20");
  } else if (sensorData.objectOutDetected && !lastOutDetected && jumlahOrang == 0) {
    Serial.println("⚠ Ultrasonic DALAM aktif tapi count sudah 0");
  }
  
  // Update state terakhir
  lastInDetected = sensorData.objectInDetected;
  lastOutDetected = sensorData.objectOutDetected;
  
  // Safety limits
  jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
}

// ================= TFT DISPLAY =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ULTRASONIC - STANDALONE");
  tft.drawFastHLine(10, 40, 300, TFT_DARKGREY);
  
  tft.setTextSize(2);
  tft.setCursor(10, 60);
  tft.println("Jumlah Orang:");
  tft.setCursor(10, 100);
  tft.println("AC Status:");
  tft.setCursor(10, 140);
  tft.println("Set Suhu AC:");
  tft.setCursor(10, 180);
  tft.println("Suhu Ruang:");
  tft.setCursor(10, 220);
  tft.println("Kelembaban:");
  
  // Status
  tft.setTextSize(1);
  tft.setCursor(10, 260);
  tft.println("Sensor: HC-SR04 Ultrasonic");
  tft.setCursor(10, 275);
  tft.println("Mode: Standalone (No WiFi)");
  tft.setCursor(10, 290);
  tft.println("Control: Auto AC");
}

void updateTFT(String acStatus, int setSuhu, float suhuRuang, float humidity) {
  // Update main values
  tft.fillRect(180, 55, 140, 35, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(180, 60);
  tft.setTextColor(TFT_GREEN);
  tft.print(jumlahOrang);
  
  tft.fillRect(180, 95, 140, 30, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(180, 100);
  tft.setTextColor(TFT_CYAN);
  tft.print(acStatus);
  
  tft.fillRect(180, 135, 140, 30, TFT_BLACK);
  tft.setCursor(180, 140);
  tft.setTextColor(TFT_YELLOW);
  if (setSuhu == 0) tft.print("-");
  else {
    tft.print(setSuhu);
    tft.print(" C");
  }
  
  tft.fillRect(180, 175, 140, 30, TFT_BLACK);
  tft.setCursor(180, 180);
  tft.setTextColor(TFT_WHITE);
  tft.print(suhuRuang);
  tft.print(" C");
  
  tft.fillRect(180, 215, 140, 30, TFT_BLACK);
  tft.setCursor(180, 220);
  tft.print(humidity);
  tft.print(" %");
  
  // Update status (less frequent)
  static unsigned long lastTftUpdate = 0;
  if (millis() - lastTftUpdate > 5000) {
    tft.fillRect(10, 260, 310, 50, TFT_BLACK);
    tft.setTextSize(1);
    
    // Sensor status
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(10, 260);
    tft.println("Sensor: HC-SR04 Ultrasonic");
    
    // Mode status
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 275);
    tft.println("Mode: Standalone (No WiFi)");
    
    // Uptime
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 290);
    tft.print("Uptime: ");
    tft.print(millis() / 1000);
    tft.println(" seconds");
    
    lastTftUpdate = millis();
  }
}

// ================= AC CONTROL =================
void kontrolAC(String &acStatus, int &setTemp) {
  bool ac1ON = false;
  bool ac2ON = false;
  int targetTemp1 = 25;
  int targetTemp2 = 25;
  
  // Auto mode berdasarkan jumlah orang
  if (jumlahOrang == 0) {
    acStatus = "AC OFF";
    setTemp = 0;
  }
  else if (jumlahOrang <= 5) {
    ac1ON = true;
    targetTemp1 = 25;
    setTemp = 25;
    acStatus = "1 AC ON";
  }
  else if (jumlahOrang <= 10) {
    ac1ON = true;
    targetTemp1 = 22;
    setTemp = 22;
    acStatus = "1 AC ON";
  }
  else if (jumlahOrang <= 15) {
    ac1ON = true;
    ac2ON = true;
    targetTemp1 = 22;
    targetTemp2 = 22;
    setTemp = 22;
    acStatus = "2 AC ON";
  }
  else {
    ac1ON = true;
    ac2ON = true;
    targetTemp1 = 20;
    targetTemp2 = 20;
    setTemp = 20;
    acStatus = "2 AC MAX";
  }
  
  // Apply AC changes only if different
  if (ac1ON != lastAC1 || ac2ON != lastAC2 || targetTemp1 != lastTemp1 || targetTemp2 != lastTemp2) {
    Serial.println("AC Change: " + acStatus + " @ " + String(setTemp) + "°C");
    
    if (!ac1ON && !ac2ON) {
      ac1.off(); ac1.send();
      ac2.off(); ac2.send();
      Serial.println("🔴 AC OFF - Semua AC dimatikan");
    } else {
      if (ac1ON) {
        ac1.on();
        ac1.setMode(kPanasonicAcCool);
        ac1.setTemp(targetTemp1);
        ac1.send();
        Serial.println("🟢 AC1 ON - Suhu: " + String(targetTemp1) + "°C");
      } else {
        ac1.off(); ac1.send();
        Serial.println("🔴 AC1 OFF");
      }
      
      if (ac2ON) {
        ac2.on();
        ac2.setMode(kPanasonicAcCool);
        ac2.setTemp(targetTemp2);
        ac2.send();
        Serial.println("🟢 AC2 ON - Suhu: " + String(targetTemp2) + "°C");
      } else {
        ac2.off(); ac2.send();
        Serial.println("🔴 AC2 OFF");
      }
    }
    
    lastAC1 = ac1ON;
    lastAC2 = ac2ON;
    lastTemp1 = targetTemp1;
    lastTemp2 = targetTemp2;
  }
}

// ================= UPDATE SENSOR DATA =================
void updateSensorData() {
  currentData.jumlahOrang = jumlahOrang;
  currentData.suhuRuang = dht.readTemperature();
  currentData.humidity = dht.readHumidity();
  currentData.lightLevel = map(analogRead(LDR_PIN), 0, 4095, 0, 1000);
  currentData.timestamp = millis();
  
  if (isnan(currentData.suhuRuang)) currentData.suhuRuang = 0.0;
  if (isnan(currentData.humidity)) currentData.humidity = 0.0;
  
  // Update ultrasonic status
  currentData.ultrasonic1 = sensorData.objectInDetected;
  currentData.ultrasonic2 = sensorData.objectOutDetected;
}

// ================= SERIAL COMMANDS =================
void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();
    
    if (command == "STATUS") {
      Serial.println("\n=== SYSTEM STATUS ===");
      Serial.println("Jumlah Orang: " + String(jumlahOrang) + "/20");
      Serial.println("AC Status: " + currentData.acStatus);
      Serial.println("Set Temperature: " + String(currentData.setTemp) + "°C");
      Serial.println("Room Temperature: " + String(currentData.suhuRuang) + "°C");
      Serial.println("Humidity: " + String(currentData.humidity) + "%");
      Serial.println("Light Level: " + String(currentData.lightLevel));
      Serial.println("Ultrasonic IN: " + String(sensorData.distanceIn) + "cm");
      Serial.println("Ultrasonic OUT: " + String(sensorData.distanceOut) + "cm");
      Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
      Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
      Serial.println("====================\n");
    }
    else if (command == "RESET") {
      jumlahOrang = 0;
      Serial.println("✅ People count reset to 0");
    }
    else if (command.startsWith("SET ")) {
      int count = command.substring(4).toInt();
      if (count >= 0 && count <= MAX_PEOPLE) {
        jumlahOrang = count;
        Serial.println("✅ People count set to " + String(count));
      } else {
        Serial.println("❌ Invalid count. Use 0-" + String(MAX_PEOPLE));
      }
    }
    else if (command == "HELP") {
      Serial.println("\n=== AVAILABLE COMMANDS ===");
      Serial.println("STATUS - Show system status");
      Serial.println("RESET - Reset people count to 0");
      Serial.println("SET <number> - Set people count (0-20)");
      Serial.println("HELP - Show this help");
      Serial.println("==========================\n");
    }
    else {
      Serial.println("❌ Unknown command. Type HELP for available commands.");
    }
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n================================");
  Serial.println("  ULTRASONIC - STANDALONE");
  Serial.println("  Sensor: HC-SR04 Ultrasonic");
  Serial.println("  Mode: Standalone (No WiFi)");
  Serial.println("  Ultra Fast Response: 300ms");
  Serial.println("  Pin Config: ESP32 Smart Energy Production");
  Serial.println("================================");
  
  // Initialize ultrasonic pins (sesuai pin configuration)
  pinMode(TRIGGER_PIN_IN, OUTPUT);   // GPIO 2 (TRIG_IN)
  pinMode(ECHO_PIN_IN, INPUT);       // GPIO 15 (ECHO_IN)
  pinMode(TRIGGER_PIN_OUT, OUTPUT);  // GPIO 13 (TRIG_OUT)
  pinMode(ECHO_PIN_OUT, INPUT);      // GPIO 12 (ECHO_OUT)
  pinMode(LDR_PIN, INPUT);           // GPIO 34 (LDR Analog)
  
  digitalWrite(TRIGGER_PIN_IN, LOW);
  digitalWrite(TRIGGER_PIN_OUT, LOW);
  
  // Initialize sensors
  dht.begin();
  ac1.begin();
  ac2.begin();
  
  // Initialize TFT
  tft.init();
  tft.setRotation(1);
  drawUI();
  
  Serial.println("ULTRASONIC STANDALONE SYSTEM READY!");
  Serial.println("Features:");
  Serial.println("- No WiFi/Internet required");
  Serial.println("- HC-SR04 ultrasonic sensors");
  Serial.println("- Ultra fast detection: 300ms cooldown");
  Serial.println("- Range: 1-15cm detection");
  Serial.println("- LUAR sensor = +1 person");
  Serial.println("- DALAM sensor = -1 person");
  Serial.println("- Auto AC control based on occupancy");
  Serial.println("- Serial commands available");
  
  // Test ultrasonic sensors
  Serial.println("\n=== ULTRASONIC SENSOR TEST ===");
  Serial.println("Pin Configuration (ESP32 Smart Energy Production):");
  Serial.println("- TRIGGER_IN: GPIO 2 (Sensor LUAR)");
  Serial.println("- ECHO_IN: GPIO 15 (Sensor LUAR)");
  Serial.println("- TRIGGER_OUT: GPIO 13 (Sensor DALAM)");
  Serial.println("- ECHO_OUT: GPIO 12 (Sensor DALAM)");
  Serial.println("- DHT22: GPIO 27");
  Serial.println("- LDR: GPIO 34 (Analog)");
  Serial.println("- IR LED: GPIO 14 (+ resistor 220Ω)");
  Serial.println("Testing HC-SR04 sensors for 5 seconds...");
  for (int i = 0; i < 25; i++) {
    int dist1 = readUltrasonicDistance(TRIGGER_PIN_IN, ECHO_PIN_IN);
    delay(50);
    int dist2 = readUltrasonicDistance(TRIGGER_PIN_OUT, ECHO_PIN_OUT);
    
    Serial.print("Test " + String(i+1) + "/25 - ");
    Serial.print("LUAR(2/15): " + String(dist1) + "cm");
    if (dist1 >= MIN_DISTANCE && dist1 <= MAX_DETECTION) {
      Serial.print(" ✅");
    } else {
      Serial.print(" ❌");
    }
    
    Serial.print(" | DALAM(13/12): " + String(dist2) + "cm");
    if (dist2 >= MIN_DISTANCE && dist2 <= MAX_DETECTION) {
      Serial.print(" ✅");
    } else {
      Serial.print(" ❌");
    }
    
    if ((dist1 >= MIN_DISTANCE && dist1 <= MAX_DETECTION) || 
        (dist2 >= MIN_DISTANCE && dist2 <= MAX_DETECTION)) {
      Serial.print(" *** OBJECT DETECTED! ***");
    }
    Serial.println();
    delay(100);
  }
  
  Serial.println("Ultrasonic sensor test complete.");
  Serial.println("Detection range: 1-15cm");
  Serial.println("If sensors show 0cm, check wiring!");
  Serial.println("==============================");
  Serial.println("\nType 'HELP' for available serial commands");
  Serial.println("================================\n");
}

// ================= MAIN LOOP - ULTRA FAST =================
void loop() {
  // Read sensors and update data
  readSensors();
  detectPeople();
  updateSensorData();
  
  // Control AC (auto mode only)
  String acStatus = currentData.acStatus;
  int setTemp = currentData.setTemp;
  kontrolAC(acStatus, setTemp);
  currentData.acStatus = acStatus;
  currentData.setTemp = setTemp;
  
  // Update TFT if people count changed
  if (jumlahOrang != lastJumlahOrang) {
    updateTFT(acStatus, setTemp, currentData.suhuRuang, currentData.humidity);
    lastJumlahOrang = jumlahOrang;
  }
  
  // Handle serial commands
  handleSerialCommands();
  
  delay(30);  // Ultra fast main loop delay
}