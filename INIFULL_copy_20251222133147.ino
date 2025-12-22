#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ================= TFT =================
TFT_eSPI tft;

// ================= WIFI & WEB =================
const char* ssid = "NAMA_WIFI_ANDA";        // Ganti dengan nama WiFi Anda
const char* password = "PASSWORD_WIFI_ANDA"; // Ganti dengan password WiFi Anda
const char* serverURL = "https://dasko.fst.unja.ac.id/dashboard";

unsigned long lastWebUpdate = 0;
#define WEB_UPDATE_INTERVAL 10000  // Kirim data setiap 10 detik

// ================= IR =================
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

#define IR_PIN 4
IRPanasonicAc ac1(IR_PIN);
IRPanasonicAc ac2(IR_PIN);

// ================= SENSOR =================
#define PROX_IN   32
#define PROX_OUT  33
#define DHTPIN    27
#define DHTTYPE   DHT22
#define LDR_PIN   34

DHT dht(DHTPIN, DHTTYPE);

// ================= VARIABLE =================
int jumlahOrang = 0;
int lastJumlahOrang = -1;

// Proximity timing dengan anti double counting
bool lastIn  = HIGH;
bool lastOut = HIGH;
unsigned long inTime = 0;
unsigned long outTime = 0;
unsigned long lastPersonDetected = 0;  // Cooldown antar deteksi
#define MAX_INTERVAL 1000
#define TIMEOUT 2000  // Reset jika tidak ada pasangan dalam 2 detik
#define PERSON_COOLDOWN 1500  // Minimal 1.5 detik antar deteksi orang

// AC State (anti kirim ulang IR)
bool lastAC1 = false;
bool lastAC2 = false;
int  lastTemp = -1;

// ================= WIFI CONNECTION =================
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("WiFi connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
  }
}

// ================= SEND DATA TO DASHBOARD UNJA =================
void sendDataToWebsite(int jumlahOrang, String acStatus, int setTemp, float suhuRuang) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping dashboard update");
    return;
  }
  
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "ESP32-RoomMonitor/1.0");
  
  // Buat JSON data untuk dashboard UNJA
  StaticJsonDocument<300> doc;
  doc["device_id"] = "ESP32_Room_Monitor";
  doc["location"] = "Ruang Meeting";
  doc["jumlah_orang"] = jumlahOrang;
  doc["ac_status"] = acStatus;
  doc["set_suhu"] = setTemp;
  doc["suhu_ruang"] = suhuRuang;
  doc["max_capacity"] = 50;
  doc["timestamp"] = millis();
  doc["status"] = "active";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.print("Sending to UNJA Dashboard: ");
  Serial.println(jsonString);
  
  // Kirim POST request ke dashboard UNJA
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Dashboard response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      Serial.println("✓ Data berhasil dikirim ke dashboard UNJA");
    }
  } else {
    Serial.print("✗ Error sending to dashboard: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

// ================= TFT =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(10, 10);
  tft.println("STATUS RUANGAN");
  tft.drawFastHLine(10, 50, 300, TFT_DARKGREY);
  
  tft.setTextSize(2);
  tft.setCursor(10, 70);
  tft.println("Jumlah Orang:");
  tft.setCursor(10, 120);
  tft.println("AC Status:");
  tft.setCursor(10, 170);
  tft.println("Set Suhu AC:");
  tft.setCursor(10, 220);
  tft.println("Suhu Ruang:");
}

void updateTFT(String acStatus, int setSuhu, float suhuRuang) {
  tft.fillRect(200, 65, 130, 40, TFT_BLACK);
  tft.setTextSize(4);
  tft.setCursor(200, 65);
  tft.setTextColor(TFT_GREEN);
  tft.print(jumlahOrang);
  
  tft.fillRect(200, 115, 130, 35, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(200, 120);
  tft.setTextColor(TFT_CYAN);
  tft.print(acStatus);
  
  tft.fillRect(200, 165, 130, 35, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(200, 170);
  tft.setTextColor(TFT_YELLOW);
  if (setSuhu == 0) tft.print("-");
  else {
    tft.print(setSuhu);
    tft.print(" C");
  }
  
  tft.fillRect(200, 215, 130, 35, TFT_BLACK);
  tft.setCursor(200, 220);
  tft.print(suhuRuang);
  tft.print(" C");
}

// ================= PROXIMITY - ANTI SPAM & DELAY =================
void bacaProximity() {
  bool inNow  = digitalRead(PROX_IN);
  bool outNow = digitalRead(PROX_OUT);
  unsigned long now = millis();
  
  // Debug status berkala (setiap 3 detik) untuk monitoring
  static unsigned long lastStatusPrint = 0;
  if (now - lastStatusPrint > 3000) {
    Serial.print("Status: IN=");
    Serial.print(inNow == LOW ? "AKTIF" : "OFF");
    Serial.print(" OUT=");
    Serial.print(outNow == LOW ? "AKTIF" : "OFF");
    Serial.print(" Jumlah=");
    Serial.println(jumlahOrang);
    lastStatusPrint = now;
  }
  
  // Cooldown - jangan deteksi terlalu cepat berturut-turut
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;  // Skip deteksi jika masih dalam cooldown
  }
  
  // Deteksi falling edge (HIGH -> LOW) dengan debouncing
  static unsigned long lastInTrigger = 0;
  static unsigned long lastOutTrigger = 0;
  
  if (lastIn == HIGH && inNow == LOW && inTime == 0) {
    // Debouncing: hindari trigger berulang dalam 200ms
    if (now - lastInTrigger > 200) {
      inTime = now;
      lastInTrigger = now;
      Serial.println("✓ Sensor IN triggered");
    }
  }
  
  if (lastOut == HIGH && outNow == LOW && outTime == 0) {
    // Debouncing: hindari trigger berulang dalam 200ms
    if (now - lastOutTrigger > 200) {
      outTime = now;
      lastOutTrigger = now;
      Serial.println("✓ Sensor OUT triggered");
    }
  }
  
  // MASUK: IN dulu, baru OUT
  if (inTime > 0 && outTime > 0 && inTime < outTime) {
    unsigned long interval = outTime - inTime;
    if (interval < MAX_INTERVAL && interval > 50) {  // Minimal 50ms untuk validitas
      jumlahOrang++;
      lastPersonDetected = now;  // Set cooldown
      Serial.print(">>> ORANG MASUK! Interval: ");
      Serial.print(interval);
      Serial.print("ms | Total: ");
      Serial.println(jumlahOrang);
    }
    // Reset tanpa pesan error (mengurangi spam)
    inTime = 0;
    outTime = 0;
  }
  
  // KELUAR: OUT dulu, baru IN - DENGAN VALIDASI KETAT
  if (inTime > 0 && outTime > 0 && outTime < inTime) {
    unsigned long interval = inTime - outTime;
    // Hanya kurangi jika ada orang di ruangan DAN interval valid
    if (interval < MAX_INTERVAL && interval > 50 && jumlahOrang > 0) {
      jumlahOrang--;
      lastPersonDetected = now;  // Set cooldown
      Serial.print("<<< ORANG KELUAR! Interval: ");
      Serial.print(interval);
      Serial.print("ms | Total: ");
      Serial.println(jumlahOrang);
    } else if (jumlahOrang == 0) {
      Serial.println("⚠ Tidak ada orang - deteksi OUT diabaikan");
    }
    // Reset tanpa pesan error (mengurangi spam)
    inTime = 0;
    outTime = 0;
  }
  
  // Reset dengan pesan minimal (kurangi spam)
  if (outTime > 0 && inTime == 0 && (now - outTime) > 800) {
    outTime = 0;
    // Tidak perlu pesan reset (mengurangi spam)
  }
  
  if (inTime > 0 && outTime == 0 && (now - inTime) > TIMEOUT) {
    inTime = 0;
    // Tidak perlu pesan reset (mengurangi spam)
  }
  
  if (outTime > 0 && (now - outTime) > TIMEOUT) {
    outTime = 0;
    // Tidak perlu pesan reset (mengurangi spam)
  }
  
  jumlahOrang = constrain(jumlahOrang, 0, 20);
  
  lastIn = inNow;
  lastOut = outNow;
}

// ================= AC + IR =================
void kontrolAC(String &status, int &temp) {
  bool ac1ON = false;
  bool ac2ON = false;
  
  if (jumlahOrang == 0) {
    status = "AC OFF";
    temp = 0;
  }
  else if (jumlahOrang <= 5) {
    ac1ON = true;
    temp = 25;
    status = "1 AC ON";
  }
  else if (jumlahOrang <= 10) {
    ac1ON = true;
    temp = 20;
    status = "1 AC ON";
  }
  else {
    ac1ON = true;
    ac2ON = true;
    temp = 20;
    status = "2 AC ON";
  }
  
  // ===== Kirim IR hanya jika berubah =====
  if (ac1ON != lastAC1 || ac2ON != lastAC2 || temp != lastTemp) {
    if (!ac1ON && !ac2ON) {
      ac1.off(); ac1.send();
      ac2.off(); ac2.send();
    } else {
      if (ac1ON) {
        ac1.on();
        ac1.setMode(kPanasonicAcCool);
        ac1.setTemp(temp);
        ac1.send();
      } else {
        ac1.off(); ac1.send();
      }
      
      if (ac2ON) {
        ac2.on();
        ac2.setMode(kPanasonicAcCool);
        ac2.setTemp(temp);
        ac2.send();
      } else {
        ac2.off(); ac2.send();
      }
    }
    
    lastAC1 = ac1ON;
    lastAC2 = ac2ON;
    lastTemp = temp;
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  
  pinMode(PROX_IN, INPUT);
  pinMode(PROX_OUT, INPUT);
  pinMode(LDR_PIN, INPUT);
  
  dht.begin();
  ac1.begin();
  ac2.begin();
  
  tft.init();
  tft.setRotation(1);
  drawUI();
  
  // Connect to WiFi
  connectWiFi();
  
  Serial.println("SYSTEM READY");
}

// ================= LOOP =================
void loop() {
  bacaProximity();
  
  float suhu = dht.readTemperature();
  String acStatus;
  int setTemp;
  kontrolAC(acStatus, setTemp);
  
  if (jumlahOrang != lastJumlahOrang) {
    updateTFT(acStatus, setTemp, suhu);
    Serial.println("========================");
    Serial.print("Jumlah Orang : "); Serial.println(jumlahOrang);
    Serial.print("AC Status    : "); Serial.println(acStatus);
    Serial.print("Set Suhu AC  : "); Serial.println(setTemp);
    Serial.print("Suhu Ruang   : "); Serial.println(suhu);
    Serial.println("========================");
    lastJumlahOrang = jumlahOrang;
  }
  
  // Kirim data ke dashboard UNJA setiap 10 detik
  unsigned long now = millis();
  if (now - lastWebUpdate > WEB_UPDATE_INTERVAL) {
    sendDataToWebsite(jumlahOrang, acStatus, setTemp, suhu);
    lastWebUpdate = now;
  }
}