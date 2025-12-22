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
const char* acControlURL = "http://192.168.0.111:8000/api/ac/control";

// Change Detection Configuration
#define TEMP_CHANGE_THRESHOLD 0.5    // Kirim jika suhu berubah >= 0.5°C
#define HUMIDITY_CHANGE_THRESHOLD 2.0 // Kirim jika humidity berubah >= 2%
#define LIGHT_CHANGE_THRESHOLD 50     // Kirim jika cahaya berubah >= 50 lux
#define MAX_TIME_WITHOUT_UPDATE 300000 // Kirim paksa setiap 5 menit
#define MIN_UPDATE_INTERVAL 2000      // Minimum 2 detik antar update
#define AC_CONTROL_CHECK_INTERVAL 10000 // Check AC control setiap 10 detik

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

// AC Control State
struct ACControlState {
  String controlMode = "auto";        // auto, manual, schedule
  bool manualOverride = false;
  bool ac1Status = false;
  bool ac2Status = false;
  int ac1Temperature = 25;
  int ac2Temperature = 25;
  unsigned long expiresAt = 0;
  String createdBy = "";
  bool hasActiveControl = false;
} acControl;

// AC Hardware State
bool lastAC1 = false;
bool lastAC2 = false;
int lastTemp1 = -1;
int lastTemp2 = -1;

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
unsigned long lastACControlCheck = 0;

// ================= WIFI CONNECTION =================
void connectWiFi() {
  Serial.println("=== CONNECTING TO WIFI ===");
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
    Serial.println("✓ IP: " + WiFi.localIP().toString());
    Serial.println("✓ API: " + String(apiURL));
    Serial.println("✓ AC Control: " + String(acControlURL));
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI = WiFi.RSSI();
  } else {
    Serial.println("✗ WiFi FAILED!");
    currentData.wifiStatus = "Failed";
  }
  Serial.println("========================");
}

// ================= AC CONTROL API =================
void checkACControlAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  http.setTimeout(10000);
  
  if (!http.begin(acControlURL)) {
    return;
  }
  
  // Add query parameters
  String url = String(acControlURL) + "?device_id=" + DEVICE_ID + "&location=" + DEVICE_LOCATION;
  http.begin(url);
  http.addHeader("Accept", "application/json");
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error && doc["success"]) {
      JsonObject data = doc["data"];
      
      // Update AC control state
      acControl.controlMode = data["control_mode"].as<String>();
      acControl.manualOverride = data["manual_override"];
      acControl.ac1Status = data["ac1_status"];
      acControl.ac2Status = data["ac2_status"];
      acControl.ac1Temperature = data["ac1_temperature"];
      acControl.ac2Temperature = data["ac2_temperature"];
      acControl.createdBy = data["created_by"].as<String>();
      acControl.hasActiveControl = true;
      
      // Parse expires_at if present
      if (data["expires_at"]) {
        // For simplicity, we'll use a timeout approach
        acControl.expiresAt = millis() + 300000; // 5 minutes default
      } else {
        acControl.expiresAt = 0; // No expiry
      }
      
      Serial.println("✓ AC Control received: " + acControl.controlMode);
      Serial.println("  AC1: " + String(acControl.ac1Status ? "ON" : "OFF") + " (" + String(acControl.ac1Temperature) + "°C)");
      Serial.println("  AC2: " + String(acControl.ac2Status ? "ON" : "OFF") + " (" + String(acControl.ac2Temperature) + "°C)");
      Serial.println("  By: " + acControl.createdBy);
    }
  } else if (httpResponseCode > 0) {
    // No active control found, use auto mode
    if (acControl.hasActiveControl) {
      Serial.println("✓ No active control, returning to auto mode");
      acControl.controlMode = "auto";
      acControl.manualOverride = false;
      acControl.hasActiveControl = false;
    }
  }
  
  http.end();
}

// ================= CHANGE DETECTION =================
bool hasSignificantChange() {
  bool hasChange = false;
  
  // Check people count change
  if (currentData.jumlahOrang != previousData.people_count) {
    hasChange = true;
  }
  
  // Check AC status change
  if (currentData.acStatus != previousData.ac_status) {
    hasChange = true;
  }
  
  // Check AC temperature change
  if (currentData.setTemp != previousData.set_temperature) {
    hasChange = true;
  }
  
  // Check room temperature change
  if (abs(currentData.suhuRuang - previousData.room_temperature) >= TEMP_CHANGE_THRESHOLD) {
    hasChange = true;
  }
  
  // Check humidity change
  if (abs(currentData.humidity - previousData.humidity) >= HUMIDITY_CHANGE_THRESHOLD) {
    hasChange = true;
  }
  
  // Check light level change
  if (abs(currentData.lightLevel - previousData.light_level) >= LIGHT_CHANGE_THRESHOLD) {
    hasChange = true;
  }
  
  // Check proximity sensor change
  if (currentData.proximity1 != previousData.proximity_in || currentData.proximity2 != previousData.proximity_out) {
    hasChange = true;
  }
  
  // Check WiFi signal change (significant change = 10 dBm difference)
  if (abs(currentData.wifiRSSI - previousData.wifi_rssi) >= 10) {
    hasChange = true;
  }
  
  return hasChange;
}

bool shouldForceUpdate() {
  unsigned long now = millis();
  return (now - lastForceUpdate >= MAX_TIME_WITHOUT_UPDATE);
}

bool canSendUpdate() {
  unsigned long now = millis();
  return (now - lastAPIUpdate >= MIN_UPDATE_INTERVAL);
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
    return;
  }
  
  HTTPClient http;
  http.setTimeout(15000);
  http.setConnectTimeout(10000);
  
  if (!http.begin(apiURL)) {
    return;
  }
  
  // Set headers
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("User-Agent", "ESP32-SmartEnergy-ManualControl/1.0");
  
  // Buat JSON data
  StaticJsonDocument<700> doc;
  doc["device_id"] = DEVICE_ID;
  doc["location"] = DEVICE_LOCATION;
  doc["people_count"] = currentData.jumlahOrang;
  doc["ac_status"] = currentData.acStatus;
  doc["set_temperature"] = currentData.setTemp;
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
  doc["control_mode"] = acControl.controlMode;
  doc["manual_override"] = acControl.manualOverride;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Kirim POST request
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode == 200 || httpResponseCode == 201) {
    Serial.println("✓ Data sent: " + reason);
    
    // Update previous data and timing
    updatePreviousData();
    lastAPIUpdate = millis();
    lastForceUpdate = millis();
    
    // Parse response untuk ID
    String response = http.getString();
    StaticJsonDocument<256> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);
    if (!error && responseDoc["success"] && responseDoc["data"]["id"]) {
      Serial.println("✓ Saved ID: " + String(responseDoc["data"]["id"].as<int>()));
    }
  } else if (httpResponseCode > 0) {
    Serial.println("⚠ HTTP " + String(httpResponseCode));
  } else {
    Serial.println("✗ Connection failed");
  }
  
  http.end();
}

// ================= TFT DISPLAY =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("SMART ENERGY - MANUAL AC");
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
  tft.println("Mode: Manual AC Control");
  tft.setCursor(10, 235);
  tft.println("WiFi: Connecting...");
  tft.setCursor(10, 250);
  tft.println("API: Waiting...");
  tft.setCursor(10, 265);
  tft.println("Control: Auto");
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
  
  // Update status (less frequent)
  static unsigned long lastTftUpdate = 0;
  if (millis() - lastTftUpdate > 10000) { // Update every 10 seconds
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
    
    // Control mode
    tft.setCursor(10, 265);
    tft.print("Control: ");
    tft.setTextColor(acControl.manualOverride ? TFT_YELLOW : TFT_GREEN);
    tft.println(acControl.controlMode);
    
    // Manual override info
    if (acControl.manualOverride) {
      tft.setTextColor(TFT_YELLOW);
      tft.setCursor(10, 280);
      tft.print("Manual by: ");
      tft.println(acControl.createdBy);
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
  
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
  static unsigned long lastInTrigger = 0;
  static unsigned long lastOutTrigger = 0;
  
  if (lastIn == HIGH && inNow == LOW && inTime == 0) {
    if (now - lastInTrigger > 200) {
      inTime = now;
      lastInTrigger = now;
    }
  }
  
  if (lastOut == HIGH && outNow == LOW && outTime == 0) {
    if (now - lastOutTrigger > 200) {
      outTime = now;
      lastOutTrigger = now;
    }
  }
  
  // MASUK: IN dulu, baru OUT
  if (inTime > 0 && outTime > 0 && inTime < outTime) {
    unsigned long interval = outTime - inTime;
    if (interval < MAX_INTERVAL && interval > 50) {
      jumlahOrang++;
      lastPersonDetected = now;
      Serial.println(">>> MASUK! Total: " + String(jumlahOrang));
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
      Serial.println("<<< KELUAR! Total: " + String(jumlahOrang));
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
  int targetTemp1 = 25;
  int targetTemp2 = 25;
  
  // Check if manual override is active and not expired
  if (acControl.manualOverride && acControl.hasActiveControl) {
    if (acControl.expiresAt == 0 || millis() < acControl.expiresAt) {
      // Use manual control settings
      ac1ON = acControl.ac1Status;
      ac2ON = acControl.ac2Status;
      targetTemp1 = acControl.ac1Temperature;
      targetTemp2 = acControl.ac2Temperature;
      
      if (!ac1ON && !ac2ON) {
        status = "MANUAL OFF";
        temp = 0;
      } else if (ac1ON && ac2ON) {
        status = "MANUAL 2 AC";
        temp = (targetTemp1 + targetTemp2) / 2;
      } else {
        status = "MANUAL 1 AC";
        temp = ac1ON ? targetTemp1 : targetTemp2;
      }
      
      Serial.println("Manual AC Control: " + status + " @ " + String(temp) + "°C");
    } else {
      // Manual control expired, return to auto
      Serial.println("Manual control expired, returning to auto mode");
      acControl.manualOverride = false;
      acControl.hasActiveControl = false;
      acControl.controlMode = "auto";
    }
  }
  
  // Auto mode (default or when manual expires)
  if (!acControl.manualOverride || !acControl.hasActiveControl) {
    if (jumlahOrang == 0) {
      status = "AC OFF";
      temp = 0;
    }
    else if (jumlahOrang <= 5) {
      ac1ON = true;
      targetTemp1 = 25;
      temp = 25;
      status = "1 AC ON";
    }
    else if (jumlahOrang <= 10) {
      ac1ON = true;
      targetTemp1 = 22;
      temp = 22;
      status = "1 AC ON";
    }
    else if (jumlahOrang <= 15) {
      ac1ON = true;
      ac2ON = true;
      targetTemp1 = 22;
      targetTemp2 = 22;
      temp = 22;
      status = "2 AC ON";
    }
    else {
      ac1ON = true;
      ac2ON = true;
      targetTemp1 = 20;
      targetTemp2 = 20;
      temp = 20;
      status = "2 AC MAX";
    }
  }
  
  // Apply AC changes only if different
  if (ac1ON != lastAC1 || ac2ON != lastAC2 || targetTemp1 != lastTemp1 || targetTemp2 != lastTemp2) {
    Serial.println("AC Change: " + status + " @ " + String(temp) + "°C");
    
    if (!ac1ON && !ac2ON) {
      ac1.off(); ac1.send();
      ac2.off(); ac2.send();
    } else {
      if (ac1ON) {
        ac1.on();
        ac1.setMode(kPanasonicAcCool);
        ac1.setTemp(targetTemp1);
        ac1.send();
      } else {
        ac1.off(); ac1.send();
      }
      
      if (ac2ON) {
        ac2.on();
        ac2.setMode(kPanasonicAcCool);
        ac2.setTemp(targetTemp2);
        ac2.send();
      } else {
        ac2.off(); ac2.send();
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
  
  Serial.println("\n================================");
  Serial.println("  SMART ENERGY - MANUAL AC CONTROL");
  Serial.println("  Supports manual AC control via API");
  Serial.println("================================");
  
  // Initialize pins
  pinMode(PROX_IN, INPUT);
  pinMode(PROX_OUT, INPUT);
  pinMode(LDR_PIN, INPUT);
  
  // Initialize sensors
  dht.begin();
  ac1.begin();
  ac2.begin();
  
  // Initialize TFT
  tft.init();
  tft.setRotation(1);
  drawUI();
  
  // Connect to WiFi
  connectWiFi();
  
  // Initialize timing
  lastForceUpdate = millis();
  lastACControlCheck = millis();
  
  Serial.println("SYSTEM READY!");
  Serial.println("Features:");
  Serial.println("- Change-based data updates");
  Serial.println("- Manual AC control via API");
  Serial.println("- Auto mode fallback");
  Serial.println("- Control expiry support");
  Serial.println("================================\n");
}

// ================= MAIN LOOP =================
void loop() {
  // Read sensors and update data
  bacaProximity();
  updateSensorData();
  
  // Check for AC control commands (every 10 seconds)
  if (millis() - lastACControlCheck >= AC_CONTROL_CHECK_INTERVAL) {
    checkACControlAPI();
    lastACControlCheck = millis();
  }
  
  // Control AC (auto or manual)
  String acStatus = currentData.acStatus;
  int setTemp = currentData.setTemp;
  kontrolAC(acStatus, setTemp);
  currentData.acStatus = acStatus;
  currentData.setTemp = setTemp;
  
  // Update TFT if people count changed
  if (jumlahOrang != lastJumlahOrang) {
    updateTFT(acStatus, setTemp, currentData.suhuRuang);
    lastJumlahOrang = jumlahOrang;
  }
  
  // Check if we should send data
  bool shouldSend = false;
  String sendReason = "";
  
  if (hasSignificantChange()) {
    if (canSendUpdate()) {
      shouldSend = true;
      sendReason = "Change detected";
    }
  } else if (shouldForceUpdate()) {
    if (canSendUpdate()) {
      shouldSend = true;
      sendReason = "Force update (" + String(MAX_TIME_WITHOUT_UPDATE/1000) + "s)";
    }
  }
  
  // Send data if needed
  if (shouldSend) {
    sendDataToAPI(sendReason);
  }
  
  // Reconnect WiFi if disconnected (check every 30 seconds)
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 30000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi reconnecting...");
      connectWiFi();
    }
    lastWiFiCheck = millis();
  }
  
  delay(100);
}