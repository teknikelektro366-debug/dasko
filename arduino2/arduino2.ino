#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ================= TFT =================
TFT_eSPI tft;

// ================= CONFIGURATION =================
// WiFi Configuration
const char* ssid = "LAB TEKNIK TEGANGAN TINGGI";
const char* password = "LABTTT2026";

// Laravel API Configuration
// IP laptop Anda: 192.168.0.111
const char* apiURL = "http://192.168.0.111:8000/api/sensor/data";

// Alternative URLs (uncomment if needed):
// const char* apiURL = "http://localhost:8000/api/sensor/data";  // Jika sama device
// const char* apiURL = "https://your-ngrok-url.ngrok-free.app/api/sensor/data";  // Jika pakai ngrok

// Timing Configuration
unsigned long lastAPIUpdate = 0;
#define API_UPDATE_INTERVAL 5000  // Kirim ke API setiap 5 detik
#define WIFI_RETRY_INTERVAL 30000 // Coba reconnect WiFi setiap 30 detik

// Device Information
#define DEVICE_ID "ESP32_Smart_Energy"
#define DEVICE_LOCATION "Lab Teknik Tegangan Tinggi"

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

// Proximity timing
bool lastIn  = HIGH;
bool lastOut = HIGH;
unsigned long inTime = 0;
unsigned long outTime = 0;
unsigned long lastPersonDetected = 0;
#define MAX_INTERVAL 1000
#define TIMEOUT 2000
#define PERSON_COOLDOWN 1500

// AC State
bool lastAC1 = false;
bool lastAC2 = false;
int  lastTemp = -1;

// Data structure untuk API
struct SensorData {
  int jumlahOrang = 0;
  String acStatus = "OFF";
  int setTemp = 16;
  float suhuRuang = 0.0;
  float humidity = 0.0;
  int lightLevel = 0;
  bool proximity1 = false;
  bool proximity2 = false;
  unsigned long timestamp = 0;
  String wifiStatus = "Disconnected";
  int wifiRSSI = 0;
} currentData;

// ================= WIFI CONNECTION =================
void connectWiFi() {
  Serial.println("=== CONNECTING TO WIFI ===");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✓ WiFi CONNECTED!");
    Serial.print("✓ IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("✓ Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("✓ API URL: ");
    Serial.println(apiURL);
    
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI = WiFi.RSSI();
  } else {
    Serial.println("✗ WiFi CONNECTION FAILED!");
    Serial.println("✗ Check SSID and password");
    currentData.wifiStatus = "Failed";
  }
  Serial.println("========================");
}

// ================= SEND DATA TO LARAVEL API =================
void sendDataToAPI(int jumlahOrang, String acStatus, int setTemp, float suhuRuang, float humidity, int lightLevel) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠ WiFi not connected, skipping API update");
    return;
  }
  
  Serial.println("=== SENDING TO LARAVEL API ===");
  
  HTTPClient http;
  
  // Set timeout dan konfigurasi
  http.setTimeout(15000); // 15 second timeout
  http.setConnectTimeout(10000); // 10 second connect timeout
  
  // Begin connection
  Serial.print("Connecting to: ");
  Serial.println(apiURL);
  
  if (!http.begin(apiURL)) {
    Serial.println("✗ Failed to begin HTTP connection");
    return;
  }
  
  // Set headers
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("User-Agent", "ESP32-SmartEnergy/1.0");
  
  // Buat JSON data
  StaticJsonDocument<512> doc;
  doc["device_id"] = DEVICE_ID;
  doc["location"] = DEVICE_LOCATION;
  doc["people_count"] = jumlahOrang;
  doc["ac_status"] = acStatus;
  doc["set_temperature"] = setTemp;
  doc["room_temperature"] = suhuRuang;
  doc["humidity"] = humidity;
  doc["light_level"] = lightLevel;
  doc["proximity_in"] = currentData.proximity1;
  doc["proximity_out"] = currentData.proximity2;
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  doc["timestamp"] = millis();
  doc["status"] = "active";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.print("JSON Data: ");
  Serial.println(jsonString);
  Serial.print("JSON Size: ");
  Serial.print(jsonString.length());
  Serial.println(" bytes");
  
  // Kirim POST request
  Serial.println("Sending POST request...");
  int httpResponseCode = http.POST(jsonString);
  
  Serial.print("Response Code: ");
  Serial.println(httpResponseCode);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Response: ");
    Serial.println(response);
    
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      Serial.println("✓ SUCCESS - Data sent to Laravel!");
      
      // Parse response
      StaticJsonDocument<256> responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);
      if (!error && responseDoc["success"]) {
        Serial.println("✓ Laravel confirmed data saved");
        if (responseDoc["data"]["id"]) {
          Serial.print("✓ Saved with ID: ");
          Serial.println(responseDoc["data"]["id"].as<int>());
        }
      }
    } else {
      Serial.println("⚠ Unexpected response code");
    }
  } else {
    Serial.println("✗ HTTP ERROR!");
    Serial.print("✗ Error code: ");
    Serial.println(httpResponseCode);
    Serial.print("✗ Error: ");
    Serial.println(http.errorToString(httpResponseCode));
    
    // Detailed error messages
    switch(httpResponseCode) {
      case -1:
        Serial.println("✗ Connection refused - Check if Laravel server is running");
        Serial.println("✗ Run: php artisan serve");
        break;
      case -2:
        Serial.println("✗ Connection failed - Check API URL");
        break;
      case -3:
        Serial.println("✗ Connection lost");
        break;
      case -4:
        Serial.println("✗ No stream");
        break;
      case -5:
        Serial.println("✗ No HTTP server");
        break;
      case -6:
        Serial.println("✗ Too less RAM");
        break;
      case -7:
        Serial.println("✗ Encoding error");
        break;
      case -8:
        Serial.println("✗ Stream write error");
        break;
      case -9:
        Serial.println("✗ Read timeout");
        break;
      case -10:
        Serial.println("✗ Connection timeout");
        break;
      case -11:
        Serial.println("✗ Disconnected");
        break;
      default:
        Serial.println("✗ Unknown error");
    }
  }
  
  http.end();
  Serial.println("========================");
}

// ================= TFT DISPLAY =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(10, 10);
  tft.println("SMART ENERGY");
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
  
  // WiFi status
  tft.setTextSize(1);
  tft.setCursor(10, 270);
  tft.println("WiFi: Connecting...");
  tft.setCursor(10, 285);
  tft.println("API: Waiting...");
}

void updateTFT(String acStatus, int setSuhu, float suhuRuang) {
  // Update main values
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
  
  // Update WiFi status
  static unsigned long lastTftUpdate = 0;
  if (millis() - lastTftUpdate > 3000) {
    tft.fillRect(10, 270, 300, 45, TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(WiFi.isConnected() ? TFT_GREEN : TFT_RED);
    tft.setCursor(10, 270);
    tft.print("WiFi: ");
    tft.println(WiFi.isConnected() ? "Connected" : "Disconnected");
    
    if (WiFi.isConnected()) {
      tft.setTextColor(TFT_GREEN);
      tft.setCursor(10, 285);
      tft.println("API: SENDING DATA");
      tft.setCursor(10, 300);
      tft.print("IP: ");
      tft.println(WiFi.localIP().toString());
    } else {
      tft.setTextColor(TFT_RED);
      tft.setCursor(10, 285);
      tft.println("API: OFFLINE");
    }
    lastTftUpdate = millis();
  }
}

// ================= PROXIMITY SENSOR =================
void bacaProximity() {
  bool inNow  = digitalRead(PROX_IN);
  bool outNow = digitalRead(PROX_OUT);
  unsigned long now = millis();
  
  currentData.proximity1 = (inNow == LOW);
  currentData.proximity2 = (outNow == LOW);
  
  // Cooldown
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
  // Deteksi falling edge
  static unsigned long lastInTrigger = 0;
  static unsigned long lastOutTrigger = 0;
  
  if (lastIn == HIGH && inNow == LOW && inTime == 0) {
    if (now - lastInTrigger > 200) {
      inTime = now;
      lastInTrigger = now;
      Serial.println("✓ Sensor IN triggered");
    }
  }
  
  if (lastOut == HIGH && outNow == LOW && outTime == 0) {
    if (now - lastOutTrigger > 200) {
      outTime = now;
      lastOutTrigger = now;
      Serial.println("✓ Sensor OUT triggered");
    }
  }
  
  // MASUK: IN dulu, baru OUT
  if (inTime > 0 && outTime > 0 && inTime < outTime) {
    unsigned long interval = outTime - inTime;
    if (interval < MAX_INTERVAL && interval > 50) {
      jumlahOrang++;
      lastPersonDetected = now;
      Serial.print(">>> ORANG MASUK! Total: ");
      Serial.println(jumlahOrang);
    }
    inTime = 0;
    outTime = 0;
  }
  
  // KELUAR: OUT dulu, baru IN
  if (inTime > 0 && outTime > 0 && outTime < inTime) {
    unsigned long interval = inTime - outTime;
    if (interval < MAX_INTERVAL && interval > 50 && jumlahOrang > 0) {
      jumlahOrang--;
      lastPersonDetected = now;
      Serial.print("<<< ORANG KELUAR! Total: ");
      Serial.println(jumlahOrang);
    }
    inTime = 0;
    outTime = 0;
  }
  
  // Reset dengan timeout
  if (inTime > 0 && outTime == 0 && (now - inTime) > TIMEOUT) {
    inTime = 0;
  }
  if (outTime > 0 && inTime == 0 && (now - outTime) > TIMEOUT) {
    outTime = 0;
  }
  
  jumlahOrang = constrain(jumlahOrang, 0, 50);
  
  lastIn = inNow;
  lastOut = outNow;
}

// ================= AC CONTROL =================
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
    temp = 22;
    status = "1 AC ON";
  }
  else if (jumlahOrang <= 15) {
    ac1ON = true;
    ac2ON = true;
    temp = 22;
    status = "2 AC ON";
  }
  else {
    ac1ON = true;
    ac2ON = true;
    temp = 20;
    status = "2 AC MAX";
  }
  
  // Kirim IR hanya jika berubah
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
      }
      if (ac2ON) {
        ac2.on();
        ac2.setMode(kPanasonicAcCool);
        ac2.setTemp(temp);
        ac2.send();
      }
    }
    
    lastAC1 = ac1ON;
    lastAC2 = ac2ON;
    lastTemp = temp;
  }
}

// ================= UPDATE REALTIME DATA =================
void updateRealtimeData() {
  currentData.jumlahOrang = jumlahOrang;
  currentData.suhuRuang = dht.readTemperature();
  currentData.humidity = dht.readHumidity();
  currentData.lightLevel = map(analogRead(LDR_PIN), 0, 4095, 0, 1000);
  currentData.timestamp = millis();
  
  if (isnan(currentData.suhuRuang)) currentData.suhuRuang = 0.0;
  if (isnan(currentData.humidity)) currentData.humidity = 0.0;
  
  if (WiFi.isConnected()) {
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI = WiFi.RSSI();
  } else {
    currentData.wifiStatus = "Disconnected";
    currentData.wifiRSSI = 0;
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n");
  Serial.println("================================");
  Serial.println("  SMART ENERGY SYSTEM v2.0");
  Serial.println("  ESP32 to Laravel API");
  Serial.println("================================");
  
  // Initialize pins
  pinMode(PROX_IN, INPUT);
  pinMode(PROX_OUT, INPUT);
  pinMode(LDR_PIN, INPUT);
  
  // Initialize sensors
  Serial.println("Initializing sensors...");
  dht.begin();
  ac1.begin();
  ac2.begin();
  Serial.println("✓ Sensors initialized");
  
  // Initialize TFT
  Serial.println("Initializing display...");
  tft.init();
  tft.setRotation(1);
  drawUI();
  Serial.println("✓ Display initialized");
  
  // Connect to WiFi
  connectWiFi();
  
  Serial.println("================================");
  Serial.println("SYSTEM READY!");
  Serial.println("================================\n");
}

// ================= MAIN LOOP =================
void loop() {
  // Read sensors
  bacaProximity();
  updateRealtimeData();
  
  // Control AC
  String acStatus = currentData.acStatus;
  int setTemp = currentData.setTemp;
  kontrolAC(acStatus, setTemp);
  currentData.acStatus = acStatus;
  currentData.setTemp = setTemp;
  
  // Update TFT
  if (jumlahOrang != lastJumlahOrang) {
    updateTFT(acStatus, setTemp, currentData.suhuRuang);
    lastJumlahOrang = jumlahOrang;
  }
  
  // Kirim data ke Laravel API
  unsigned long now = millis();
  if (now - lastAPIUpdate > API_UPDATE_INTERVAL) {
    sendDataToAPI(jumlahOrang, acStatus, setTemp, currentData.suhuRuang, 
                  currentData.humidity, currentData.lightLevel);
    lastAPIUpdate = now;
  }
  
  // Reconnect WiFi jika terputus
  static unsigned long lastWiFiCheck = 0;
  if (now - lastWiFiCheck > WIFI_RETRY_INTERVAL) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, reconnecting...");
      connectWiFi();
    }
    lastWiFiCheck = now;
  }
  
  delay(100);
}
