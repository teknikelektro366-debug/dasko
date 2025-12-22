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
const char* apiURL = "http://192.168.0.111:8000/api/sensor/data";

// Change Detection Configuration
#define TEMP_CHANGE_THRESHOLD 0.5    // Kirim jika suhu berubah >= 0.5°C
#define HUMIDITY_CHANGE_THRESHOLD 2.0 // Kirim jika humidity berubah >= 2%
#define LIGHT_CHANGE_THRESHOLD 50     // Kirim jika cahaya berubah >= 50 lux
#define MAX_TIME_WITHOUT_UPDATE 300000 // Kirim paksa setiap 5 menit (300 detik)
#define MIN_UPDATE_INTERVAL 2000      // Minimum 2 detik antar update (debouncing)

// AC Mode Configuration - PILIH SALAH SATU
#define AC_MODE_MANUAL false         // true = input manual via serial, false = auto
#define AC_MODE_AUTO_CONTROL true    // true = kontrol otomatis berdasarkan jumlah orang

// Device Information
#define DEVICE_ID "ESP32_Smart_Energy_ChangeDetection"
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

// ================= VARIABLES =================
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

// AC State - REAL STATUS VARIABLES
bool currentAC1Status = false;  // Status AC1 saat ini (dari hardware atau manual)
bool currentAC2Status = false;  // Status AC2 saat ini (dari hardware atau manual)
int currentACTemp = 25;         // Temperature setting saat ini
String currentACStatusText = "AC OFF";

// Previous sensor values for change detection
struct PreviousData {
  int people_count = -1;
  String ac_status = "";
  int set_temperature = -1;
  float room_temperature = -999.0;
  float humidity = -999.0;
  int light_level = -1;
  bool proximity_in = false;
  bool proximity_out = false;
  int wifi_rssi = 0;
} previousData;

// Current sensor data
struct SensorData {
  int jumlahOrang = 0;
  String acStatus = "OFF";
  int setTemp = 0;
  float suhuRuang = 0.0;
  float humidity = 0.0;
  int lightLevel = 0;
  bool proximity1 = false;
  bool proximity2 = false;
  unsigned long timestamp = 0;
  String wifiStatus = "Disconnected";
  int wifiRSSI = 0;
} currentData;

// Timing variables
unsigned long lastAPIUpdate = 0;
unsigned long lastForceUpdate = 0;

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
    currentData.wifiStatus = "Failed";
  }
  Serial.println("========================");
}

// ================= MANUAL AC CONTROL =================
void checkManualACControl() {
  if (Serial.available()) {
    String command = Serial.readString();
    command.trim();
    command.toUpperCase();
    
    if (command == "AC1ON") {
      currentAC1Status = true;
      Serial.println("✓ AC1 turned ON manually");
      updateACStatus();
    }
    else if (command == "AC1OFF") {
      currentAC1Status = false;
      Serial.println("✓ AC1 turned OFF manually");
      updateACStatus();
    }
    else if (command == "AC2ON") {
      currentAC2Status = true;
      Serial.println("✓ AC2 turned ON manually");
      updateACStatus();
    }
    else if (command == "AC2OFF") {
      currentAC2Status = false;
      Serial.println("✓ AC2 turned OFF manually");
      updateACStatus();
    }
    else if (command.startsWith("TEMP")) {
      int temp = command.substring(4).toInt();
      if (temp >= 16 && temp <= 30) {
        currentACTemp = temp;
        Serial.println("✓ AC temperature set to " + String(temp) + "°C");
        updateACStatus();
      }
    }
    else if (command == "STATUS") {
      Serial.println("=== CURRENT AC STATUS ===");
      Serial.println("AC1: " + String(currentAC1Status ? "ON" : "OFF"));
      Serial.println("AC2: " + String(currentAC2Status ? "ON" : "OFF"));
      Serial.println("Temperature: " + String(currentACTemp) + "°C");
      Serial.println("Status Text: " + currentACStatusText);
      Serial.println("========================");
    }
    else if (command == "HELP") {
      Serial.println("=== MANUAL AC COMMANDS ===");
      Serial.println("AC1ON    - Turn on AC1");
      Serial.println("AC1OFF   - Turn off AC1");
      Serial.println("AC2ON    - Turn on AC2");
      Serial.println("AC2OFF   - Turn off AC2");
      Serial.println("TEMP25   - Set temperature to 25°C");
      Serial.println("STATUS   - Show current status");
      Serial.println("HELP     - Show this help");
      Serial.println("========================");
    }
  }
}

void updateACStatus() {
  // Update status text based on current AC state
  if (!currentAC1Status && !currentAC2Status) {
    currentACStatusText = "AC OFF";
    currentACTemp = 0;
  } else if (currentAC1Status && !currentAC2Status) {
    currentACStatusText = "1 AC ON";
  } else if (currentAC1Status && currentAC2Status) {
    currentACStatusText = "2 AC ON";
  } else if (!currentAC1Status && currentAC2Status) {
    currentACStatusText = "1 AC ON (Unit 2)";
  }
  
  Serial.println("🔄 AC Status updated: " + currentACStatusText + " @ " + String(currentACTemp) + "°C");
}

// ================= AUTO AC CONTROL =================
void autoControlAC() {
  bool shouldAC1 = false;
  bool shouldAC2 = false;
  int shouldTemp = 25;
  
  // Logic berdasarkan jumlah orang
  if (jumlahOrang == 0) {
    shouldAC1 = false;
    shouldAC2 = false;
    shouldTemp = 0;
  }
  else if (jumlahOrang <= 5) {
    shouldAC1 = true;
    shouldAC2 = false;
    shouldTemp = 25;
  }
  else if (jumlahOrang <= 10) {
    shouldAC1 = true;
    shouldAC2 = false;
    shouldTemp = 22;
  }
  else if (jumlahOrang <= 15) {
    shouldAC1 = true;
    shouldAC2 = true;
    shouldTemp = 22;
  }
  else {
    shouldAC1 = true;
    shouldAC2 = true;
    shouldTemp = 20;
  }
  
  // Update jika ada perubahan
  if (shouldAC1 != currentAC1Status || shouldAC2 != currentAC2Status || shouldTemp != currentACTemp) {
    Serial.println("🤖 Auto AC Control Change:");
    Serial.println("  People: " + String(jumlahOrang));
    Serial.println("  AC1: " + String(currentAC1Status ? "ON" : "OFF") + " → " + String(shouldAC1 ? "ON" : "OFF"));
    Serial.println("  AC2: " + String(currentAC2Status ? "ON" : "OFF") + " → " + String(shouldAC2 ? "ON" : "OFF"));
    Serial.println("  Temp: " + String(currentACTemp) + "°C → " + String(shouldTemp) + "°C");
    
    currentAC1Status = shouldAC1;
    currentAC2Status = shouldAC2;
    currentACTemp = shouldTemp;
    
    updateACStatus();
    
    // Send IR commands
    sendIRCommands();
  }
}

void sendIRCommands() {
  Serial.println("📡 Sending IR commands...");
  
  if (!currentAC1Status && !currentAC2Status) {
    ac1.off(); ac1.send();
    ac2.off(); ac2.send();
    Serial.println("IR: Both AC turned OFF");
  } else {
    if (currentAC1Status) {
      ac1.on();
      ac1.setMode(kPanasonicAcCool);
      ac1.setTemp(currentACTemp);
      ac1.send();
      Serial.println("IR: AC1 ON at " + String(currentACTemp) + "°C");
    } else {
      ac1.off(); ac1.send();
      Serial.println("IR: AC1 OFF");
    }
    
    if (currentAC2Status) {
      ac2.on();
      ac2.setMode(kPanasonicAcCool);
      ac2.setTemp(currentACTemp);
      ac2.send();
      Serial.println("IR: AC2 ON at " + String(currentACTemp) + "°C");
    } else {
      ac2.off(); ac2.send();
      Serial.println("IR: AC2 OFF");
    }
  }
}

// ================= CHANGE DETECTION =================
bool hasSignificantChange() {
  bool hasChange = false;
  String changeReason = "Changes detected: ";
  
  // Check people count change
  if (currentData.jumlahOrang != previousData.people_count) {
    hasChange = true;
    changeReason += "People(" + String(previousData.people_count) + "→" + String(currentData.jumlahOrang) + ") ";
  }
  
  // Check AC status change
  if (currentData.acStatus != previousData.ac_status) {
    hasChange = true;
    changeReason += "AC(" + previousData.ac_status + "→" + currentData.acStatus + ") ";
  }
  
  // Check AC temperature change
  if (currentData.setTemp != previousData.set_temperature) {
    hasChange = true;
    changeReason += "ACTemp(" + String(previousData.set_temperature) + "→" + String(currentData.setTemp) + ") ";
  }
  
  // Check room temperature change
  if (abs(currentData.suhuRuang - previousData.room_temperature) >= TEMP_CHANGE_THRESHOLD) {
    hasChange = true;
    changeReason += "Temp(" + String(previousData.room_temperature, 1) + "→" + String(currentData.suhuRuang, 1) + ") ";
  }
  
  // Check humidity change
  if (abs(currentData.humidity - previousData.humidity) >= HUMIDITY_CHANGE_THRESHOLD) {
    hasChange = true;
    changeReason += "Humidity(" + String(previousData.humidity, 1) + "→" + String(currentData.humidity, 1) + ") ";
  }
  
  // Check light level change
  if (abs(currentData.lightLevel - previousData.light_level) >= LIGHT_CHANGE_THRESHOLD) {
    hasChange = true;
    changeReason += "Light(" + String(previousData.light_level) + "→" + String(currentData.lightLevel) + ") ";
  }
  
  // Check proximity sensor change
  if (currentData.proximity1 != previousData.proximity_in || currentData.proximity2 != previousData.proximity_out) {
    hasChange = true;
    changeReason += "Proximity ";
  }
  
  // Check WiFi signal change
  if (abs(currentData.wifiRSSI - previousData.wifi_rssi) >= 10) {
    hasChange = true;
    changeReason += "WiFi(" + String(previousData.wifi_rssi) + "→" + String(currentData.wifiRSSI) + ") ";
  }
  
  if (hasChange) {
    Serial.println("🔄 " + changeReason);
  }
  
  return hasChange;
}

bool shouldForceUpdate() {
  unsigned long now = millis();
  if (now - lastForceUpdate >= MAX_TIME_WITHOUT_UPDATE) {
    Serial.println("⏰ Force update - " + String(MAX_TIME_WITHOUT_UPDATE/1000) + " seconds elapsed");
    return true;
  }
  return false;
}

bool canSendUpdate() {
  unsigned long now = millis();
  if (now - lastAPIUpdate < MIN_UPDATE_INTERVAL) {
    Serial.println("⏳ Debouncing - too soon to send update");
    return false;
  }
  return true;
}

void updatePreviousData() {
  previousData.people_count = currentData.jumlahOrang;
  previousData.ac_status = currentData.acStatus;
  previousData.set_temperature = currentData.setTemp;
  previousData.room_temperature = currentData.suhuRuang;
  previousData.humidity = currentData.humidity;
  previousData.light_level = currentData.lightLevel;
  previousData.proximity_in = currentData.proximity1;
  previousData.proximity_out = currentData.proximity2;
  previousData.wifi_rssi = currentData.wifiRSSI;
}

// ================= SEND DATA TO LARAVEL API =================
void sendDataToAPI(String reason = "Change detected") {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠ WiFi not connected, skipping API update");
    return;
  }
  
  Serial.println("=== SENDING TO LARAVEL API ===");
  Serial.println("Reason: " + reason);
  
  HTTPClient http;
  http.setTimeout(15000);
  http.setConnectTimeout(10000);
  
  if (!http.begin(apiURL)) {
    Serial.println("✗ Failed to begin HTTP connection");
    return;
  }
  
  // Set headers
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("User-Agent", "ESP32-SmartEnergy-ChangeDetection/1.0");
  
  // Buat JSON data dengan AC status yang REAL
  StaticJsonDocument<600> doc;
  doc["device_id"] = DEVICE_ID;
  doc["location"] = DEVICE_LOCATION;
  doc["people_count"] = currentData.jumlahOrang;
  doc["ac_status"] = currentACStatusText;  // Status AC yang sebenarnya
  doc["set_temperature"] = currentACTemp;  // Temperature yang sebenarnya
  doc["room_temperature"] = currentData.suhuRuang;
  doc["humidity"] = currentData.humidity;
  doc["light_level"] = currentData.lightLevel;
  doc["proximity_in"] = currentData.proximity1;
  doc["proximity_out"] = currentData.proximity2;
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  doc["timestamp"] = millis();
  doc["status"] = "active";
  doc["update_reason"] = reason;
  doc["update_type"] = "change_based";
  doc["ac_mode"] = AC_MODE_MANUAL ? "manual" : "auto";
  doc["ac1_status"] = currentAC1Status;
  doc["ac2_status"] = currentAC2Status;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.print("JSON Data: ");
  Serial.println(jsonString);
  
  // Kirim POST request
  int httpResponseCode = http.POST(jsonString);
  
  Serial.print("Response Code: ");
  Serial.println(httpResponseCode);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      Serial.println("✓ SUCCESS - Data sent to Laravel!");
      
      // Update previous data and timing
      updatePreviousData();
      lastAPIUpdate = millis();
      lastForceUpdate = millis();
      
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
      Serial.println("⚠ Unexpected response code: " + response);
    }
  } else {
    Serial.println("✗ HTTP ERROR!");
    Serial.print("✗ Error code: ");
    Serial.println(httpResponseCode);
    Serial.print("✗ Error: ");
    Serial.println(http.errorToString(httpResponseCode));
  }
  
  http.end();
  Serial.println("========================");
}

// ================= TFT DISPLAY =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("SMART ENERGY - REAL AC");
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
  
  // Status
  tft.setTextSize(1);
  tft.setCursor(10, 220);
  tft.println("Mode: " + String(AC_MODE_MANUAL ? "Manual" : "Auto"));
  tft.setCursor(10, 235);
  tft.println("WiFi: Connecting...");
  tft.setCursor(10, 250);
  tft.println("API: Waiting for changes...");
}

void updateTFT(String acStatus, int setSuhu, float suhuRuang) {
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
  tft.print(suhuRuang);
  tft.print(" C");
  
  // Update status
  static unsigned long lastTftUpdate = 0;
  if (millis() - lastTftUpdate > 5000) {
    tft.fillRect(10, 235, 310, 80, TFT_BLACK);
    tft.setTextSize(1);
    
    // WiFi status
    tft.setTextColor(WiFi.isConnected() ? TFT_GREEN : TFT_RED);
    tft.setCursor(10, 235);
    tft.print("WiFi: ");
    tft.println(WiFi.isConnected() ? "Connected" : "Disconnected");
    
    // API status
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 250);
    tft.print("Last Update: ");
    if (lastAPIUpdate > 0) {
      tft.print((millis() - lastAPIUpdate) / 1000);
      tft.println("s ago");
    } else {
      tft.println("Never");
    }
    
    // AC Status detail
    tft.setCursor(10, 265);
    tft.print("AC1:");
    tft.setTextColor(currentAC1Status ? TFT_GREEN : TFT_RED);
    tft.print(currentAC1Status ? "ON " : "OFF ");
    tft.setTextColor(TFT_WHITE);
    tft.print("AC2:");
    tft.setTextColor(currentAC2Status ? TFT_GREEN : TFT_RED);
    tft.println(currentAC2Status ? "ON" : "OFF");
    
    // Mode status
    tft.setCursor(10, 280);
    tft.setTextColor(TFT_CYAN);
    tft.println(AC_MODE_MANUAL ? "Manual Control Mode" : "Auto Control Mode");
    
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
  
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
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

// ================= UPDATE SENSOR DATA =================
void updateSensorData() {
  currentData.jumlahOrang = jumlahOrang;
  currentData.suhuRuang = dht.readTemperature();
  currentData.humidity = dht.readHumidity();
  currentData.lightLevel = map(analogRead(LDR_PIN), 0, 4095, 0, 1000);
  currentData.timestamp = millis();
  
  // Use REAL AC status
  currentData.acStatus = currentACStatusText;
  currentData.setTemp = currentACTemp;
  
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
  Serial.println("  SMART ENERGY - REAL AC DATA");
  Serial.println("  ESP32 to Laravel API");
  Serial.println("  Change-based with Real AC Status");
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
  
  // Initialize timing
  lastForceUpdate = millis();
  
  // Initialize AC status
  updateACStatus();
  
  Serial.println("================================");
  Serial.println("REAL AC STATUS MODE ACTIVE!");
  Serial.println("AC Mode: " + String(AC_MODE_MANUAL ? "Manual Control" : "Auto Control"));
  Serial.println("Commands: Type HELP for manual commands");
  Serial.println("Thresholds:");
  Serial.println("- Temperature: ±" + String(TEMP_CHANGE_THRESHOLD) + "°C");
  Serial.println("- Humidity: ±" + String(HUMIDITY_CHANGE_THRESHOLD) + "%");
  Serial.println("- Light: ±" + String(LIGHT_CHANGE_THRESHOLD) + " lux");
  Serial.println("- Force update: " + String(MAX_TIME_WITHOUT_UPDATE/1000) + " seconds");
  Serial.println("================================\n");
}

// ================= MAIN LOOP =================
void loop() {
  // Read sensors and update data
  bacaProximity();
  updateSensorData();
  
  // Handle AC control based on mode
  if (AC_MODE_MANUAL) {
    checkManualACControl();  // Check for manual commands
  }
  
  if (AC_MODE_AUTO_CONTROL) {
    autoControlAC();  // Automatic control based on people count
  }
  
  // Update TFT if people count changed
  if (jumlahOrang != lastJumlahOrang) {
    updateTFT(currentACStatusText, currentACTemp, currentData.suhuRuang);
    lastJumlahOrang = jumlahOrang;
  }
  
  // Check if we should send data
  bool shouldSend = false;
  String sendReason = "";
  
  if (hasSignificantChange()) {
    if (canSendUpdate()) {
      shouldSend = true;
      sendReason = "Significant change detected";
    }
  } else if (shouldForceUpdate()) {
    if (canSendUpdate()) {
      shouldSend = true;
      sendReason = "Force update - " + String(MAX_TIME_WITHOUT_UPDATE/1000) + "s elapsed";
    }
  }
  
  // Send data if needed
  if (shouldSend) {
    sendDataToAPI(sendReason);
  }
  
  // Reconnect WiFi if disconnected
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 30000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, reconnecting...");
      connectWiFi();
    }
    lastWiFiCheck = millis();
  }
  
  delay(100);
}