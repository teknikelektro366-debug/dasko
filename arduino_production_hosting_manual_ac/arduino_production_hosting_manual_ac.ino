#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// ================= TFT =================
TFT_eSPI tft;

// ================= CONFIGURATION =================
// WiFi Configuration
const char* ssid = "LAB TEKNIK TEGANGAN TINGGI";
const char* password = "LABTTT2026";

// Production Hosting Configuration
const char* hostingDomain = "dasko.fst.unja.ac.id";
const char* apiURL = "https://dasko.fst.unja.ac.id/api/sensor/data";
const char* acControlURL = "https://dasko.fst.unja.ac.id/api/ac/control";

// SSL Configuration
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

// Change Detection Configuration
#define TEMP_CHANGE_THRESHOLD 0.5    // Kirim jika suhu berubah >= 0.5°C
#define HUMIDITY_CHANGE_THRESHOLD 2.0 // Kirim jika humidity berubah >= 2%
#define LIGHT_CHANGE_THRESHOLD 50     // Kirim jika cahaya berubah >= 50 lux
#define MAX_TIME_WITHOUT_UPDATE 300000 // Kirim paksa setiap 5 menit
#define MIN_UPDATE_INTERVAL 2000      // Minimum 2 detik antar update
#define AC_CONTROL_CHECK_INTERVAL 10000 // Check AC control setiap 10 detik

// Device Information
#define DEVICE_ID "ESP32_Smart_Energy_Production"
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
    Serial.println("✓ Production API: " + String(apiURL));
    Serial.println("✓ AC Control API: " + String(acControlURL));
    Serial.println("✓ Domain: " + String(hostingDomain));
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI = WiFi.RSSI();
  } else {
    Serial.println("✗ WiFi FAILED!");
    currentData.wifiStatus = "Failed";
  }
  Serial.println("========================");
}

// ================= HTTPS REQUEST HELPER =================
bool makeHTTPSRequest(const char* url, const char* method, const char* payload, String& response) {
  WiFiClientSecure client;
  HTTPClient https;
  
  // Set SSL certificate
  client.setCACert(rootCACertificate);
  
  // Set timeout
  client.setTimeout(15000);
  https.setTimeout(15000);
  
  if (!https.begin(client, url)) {
    Serial.println("✗ HTTPS connection failed");
    return false;
  }
  
  // Set headers
  https.addHeader("Content-Type", "application/json");
  https.addHeader("Accept", "application/json");
  https.addHeader("User-Agent", "ESP32-SmartEnergy-Production/1.0");
  https.addHeader("X-Requested-With", "ESP32");
  
  int httpResponseCode;
  if (strcmp(method, "POST") == 0) {
    httpResponseCode = https.POST(payload);
  } else {
    httpResponseCode = https.GET();
  }
  
  if (httpResponseCode > 0) {
    response = https.getString();
    https.end();
    return (httpResponseCode == 200 || httpResponseCode == 201);
  } else {
    Serial.println("✗ HTTPS request failed: " + String(httpResponseCode));
    https.end();
    return false;
  }
}

// ================= AC CONTROL API =================
void checkACControlAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  // Build URL with query parameters
  String url = String(acControlURL) + "?device_id=" + DEVICE_ID + "&location=" + DEVICE_LOCATION;
  String response;
  
  if (makeHTTPSRequest(url.c_str(), "GET", "", response)) {
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
        acControl.expiresAt = millis() + 300000; // 5 minutes default
      } else {
        acControl.expiresAt = 0; // No expiry
      }
      
      Serial.println("✓ AC Control received from hosting: " + acControl.controlMode);
      Serial.println("  AC1: " + String(acControl.ac1Status ? "ON" : "OFF") + " (" + String(acControl.ac1Temperature) + "°C)");
      Serial.println("  AC2: " + String(acControl.ac2Status ? "ON" : "OFF") + " (" + String(acControl.ac2Temperature) + "°C)");
      Serial.println("  By: " + acControl.createdBy);
    }
  } else {
    // No active control found, use auto mode
    if (acControl.hasActiveControl) {
      Serial.println("✓ No active control from hosting, returning to auto mode");
      acControl.controlMode = "auto";
      acControl.manualOverride = false;
      acControl.hasActiveControl = false;
    }
  }
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

// ================= SEND DATA TO HOSTING API =================
void sendDataToAPI(String reason = "Change detected") {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
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
  doc["hosting_domain"] = hostingDomain;
  doc["ssl_enabled"] = true;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  String response;
  if (makeHTTPSRequest(apiURL, "POST", jsonString.c_str(), response)) {
    Serial.println("✓ Data sent to hosting: " + reason);
    
    // Update previous data and timing
    updatePreviousData();
    lastAPIUpdate = millis();
    lastForceUpdate = millis();
    
    // Parse response untuk ID
    StaticJsonDocument<256> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);
    if (!error && responseDoc["success"] && responseDoc["data"]["id"]) {
      Serial.println("✓ Hosting saved ID: " + String(responseDoc["data"]["id"].as<int>()));
    }
  } else {
    Serial.println("✗ Failed to send data to hosting");
  }
}

// ================= TFT DISPLAY =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("SMART ENERGY - HOSTING");
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
  tft.println("Mode: Production Hosting");
  tft.setCursor(10, 235);
  tft.println("Domain: dasko.fst.unja.ac.id");
  tft.setCursor(10, 250);
  tft.println("SSL: Enabled");
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
    
    // Domain status
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(10, 235);
    tft.println("Domain: dasko.fst.unja.ac.id");
    
    // SSL status
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 250);
    tft.println("SSL: Enabled & Verified");
    
    // API status
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 265);
    tft.print("Last Update: ");
    if (lastAPIUpdate > 0) {
      tft.print((millis() - lastAPIUpdate) / 1000);
      tft.println("s ago");
    } else {
      tft.println("Never");
    }
    
    // Control mode
    tft.setCursor(10, 280);
    tft.print("Control: ");
    tft.setTextColor(acControl.manualOverride ? TFT_YELLOW : TFT_GREEN);
    tft.println(acControl.controlMode);
    
    // Manual override info
    if (acControl.manualOverride) {
      tft.setTextColor(TFT_YELLOW);
      tft.setCursor(10, 295);
      tft.print("Manual by: ");
      tft.println(acControl.createdBy);
    }
    
    lastTftUpdate = millis();
  }
}

// ================= PROXIMITY SENSOR - SIMPLIFIED AND FIXED =================
void bacaProximity() {
  bool inNow  = digitalRead(PROX_IN);
  bool outNow = digitalRead(PROX_OUT);
  unsigned long now = millis();
  
  // Update current proximity status (LOW = detected with INPUT_PULLUP)
  currentData.proximity1 = (inNow == LOW);
  currentData.proximity2 = (outNow == LOW);
  
  // Debug status sensor setiap 10 detik
  static unsigned long lastDebugPrint = 0;
  if (now - lastDebugPrint > 10000) {
    Serial.print("Proximity Debug - IN: ");
    Serial.print(inNow == LOW ? "DETECTED" : "CLEAR");
    Serial.print(" (");
    Serial.print(inNow);
    Serial.print(") | OUT: ");
    Serial.print(outNow == LOW ? "DETECTED" : "CLEAR");
    Serial.print(" (");
    Serial.print(outNow);
    Serial.print(") | Total: ");
    Serial.println(jumlahOrang);
    lastDebugPrint = now;
  }
  
  // Cooldown untuk mencegah deteksi ganda
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
  // Deteksi perubahan state dengan debouncing
  static unsigned long lastInChange = 0;
  static unsigned long lastOutChange = 0;
  static bool lastInState = HIGH;
  static bool lastOutState = HIGH;
  
  // Debouncing untuk sensor IN
  if (inNow != lastInState) {
    lastInChange = now;
    lastInState = inNow;
  }
  
  // Debouncing untuk sensor OUT  
  if (outNow != lastOutState) {
    lastOutChange = now;
    lastOutState = outNow;
  }
  
  // Deteksi falling edge (HIGH -> LOW) dengan debouncing 200ms
  if (lastIn == HIGH && inNow == LOW && inTime == 0 && (now - lastInChange > 200)) {
    inTime = now;
    Serial.println("✓ Sensor IN activated");
  }
  
  if (lastOut == HIGH && outNow == LOW && outTime == 0 && (now - lastOutChange > 200)) {
    outTime = now;
    Serial.println("✓ Sensor OUT activated");
  }
  
  // MASUK: IN dulu, baru OUT (dalam waktu yang wajar)
  if (inTime > 0 && outTime > 0 && inTime < outTime) {
    unsigned long interval = outTime - inTime;
    Serial.print("Checking MASUK - Interval: ");
    Serial.print(interval);
    Serial.println(" ms");
    
    if (interval < MAX_INTERVAL && interval > 100) {
      if (jumlahOrang < 20) {  // Batas maksimal 20 orang
        jumlahOrang++;
        lastPersonDetected = now;
        Serial.print(">>> ORANG MASUK! Total: ");
        Serial.println(jumlahOrang);
      } else {
        Serial.println("⚠ Ruangan sudah penuh (20 orang)");
      }
    } else {
      Serial.println("❌ Interval tidak valid untuk MASUK");
    }
    inTime = 0;
    outTime = 0;
  }
  
  // KELUAR: OUT dulu, baru IN (dalam waktu yang wajar)
  if (inTime > 0 && outTime > 0 && outTime < inTime) {
    unsigned long interval = inTime - outTime;
    Serial.print("Checking KELUAR - Interval: ");
    Serial.print(interval);
    Serial.println(" ms");
    
    if (interval < MAX_INTERVAL && interval > 100 && jumlahOrang > 0) {
      jumlahOrang--;
      lastPersonDetected = now;
      Serial.print("<<< ORANG KELUAR! Total: ");
      Serial.println(jumlahOrang);
    } else if (jumlahOrang == 0) {
      Serial.println("⚠ Tidak ada orang untuk dikurangi");
    } else {
      Serial.println("❌ Interval tidak valid untuk KELUAR");
    }
    inTime = 0;
    outTime = 0;
  }
  
  // Reset dengan timeout yang lebih pendek
  if (inTime > 0 && outTime == 0 && (now - inTime) > TIMEOUT) {
    Serial.println("⏰ Timeout reset IN sensor");
    inTime = 0;
  }
  if (outTime > 0 && inTime == 0 && (now - outTime) > TIMEOUT) {
    Serial.println("⏰ Timeout reset OUT sensor");
    outTime = 0;
  }
  
  // Reset jika kedua sensor aktif terlalu lama (kemungkinan error)
  if (inTime > 0 && outTime > 0 && (now - min(inTime, outTime)) > (TIMEOUT * 2)) {
    Serial.println("⏰ Force reset both sensors (too long active)");
    inTime = 0;
    outTime = 0;
  }
  
  jumlahOrang = constrain(jumlahOrang, 0, 20);
  
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
      // Use manual control settings from hosting
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
      
      Serial.println("Manual AC Control from hosting: " + status + " @ " + String(temp) + "°C");
    } else {
      // Manual control expired, return to auto
      Serial.println("Manual control from hosting expired, returning to auto mode");
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
    else if (jumlahOrang <= 20) {  // Updated for max 20 people
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
  Serial.println("  SMART ENERGY - PRODUCTION HOSTING");
  Serial.println("  Domain: dasko.fst.unja.ac.id");
  Serial.println("  SSL/HTTPS: Enabled");
  Serial.println("  Manual AC Control: Supported");
  Serial.println("================================");
  
  // Initialize pins dengan pull-up untuk proximity sensor
  pinMode(PROX_IN, INPUT_PULLUP);   // Coba dengan pull-up internal
  pinMode(PROX_OUT, INPUT_PULLUP);  // Coba dengan pull-up internal
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
  
  Serial.println("PRODUCTION SYSTEM READY!");
  Serial.println("Features:");
  Serial.println("- HTTPS/SSL communication");
  Serial.println("- Production hosting integration");
  Serial.println("- Change-based data updates");
  Serial.println("- Manual AC control via web dashboard");
  Serial.println("- Auto mode fallback");
  Serial.println("- Control expiry support");
  Serial.println("- Max 20 people capacity");
  Serial.println("\nSerial Commands:");
  Serial.println("- 'reset' or 'r' - Reset people count");
  Serial.println("- 'test' or 't' - Show sensor status");
  Serial.println("- 'set X' - Set people count to X");
  Serial.println("- 'help' or 'h' - Show commands");
  
  // Test proximity sensors
  Serial.println("\n=== PROXIMITY SENSOR TEST ===");
  Serial.print("Pin 32 (IN) - Raw: "); Serial.print(digitalRead(PROX_IN));
  Serial.print(" | Active when LOW: "); Serial.println(digitalRead(PROX_IN) == LOW ? "DETECTED" : "CLEAR");
  Serial.print("Pin 33 (OUT) - Raw: "); Serial.print(digitalRead(PROX_OUT));
  Serial.print(" | Active when LOW: "); Serial.println(digitalRead(PROX_OUT) == LOW ? "DETECTED" : "CLEAR");
  Serial.println("Configuration: INPUT_PULLUP (HIGH=clear, LOW=detected)");
  Serial.println("Max people: 20");
  Serial.println("Detection sequence: IN->OUT = enter, OUT->IN = exit");
  Serial.println("==============================");
  Serial.println("================================\n");
}

// ================= MAIN LOOP =================
void loop() {
  // Check for serial commands (for debugging/manual control)
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command == "reset" || command == "r") {
      jumlahOrang = 0;
      inTime = 0;
      outTime = 0;
      lastPersonDetected = 0;
      Serial.println("✓ Manual reset - People count set to 0");
    }
    else if (command == "test" || command == "t") {
      Serial.println("=== SENSOR STATUS ===");
      Serial.print("IN sensor (pin 32): "); 
      Serial.println(digitalRead(PROX_IN) == LOW ? "DETECTED" : "CLEAR");
      Serial.print("OUT sensor (pin 33): "); 
      Serial.println(digitalRead(PROX_OUT) == LOW ? "DETECTED" : "CLEAR");
      Serial.print("People count: "); Serial.println(jumlahOrang);
      Serial.print("WiFi: "); Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
      Serial.println("====================");
    }
    else if (command.startsWith("set ")) {
      int newCount = command.substring(4).toInt();
      if (newCount >= 0 && newCount <= 20) {
        jumlahOrang = newCount;
        Serial.println("✓ People count set to: " + String(jumlahOrang));
      } else {
        Serial.println("❌ Invalid count. Use 0-20");
      }
    }
    else if (command == "help" || command == "h") {
      Serial.println("=== AVAILABLE COMMANDS ===");
      Serial.println("reset/r - Reset people count to 0");
      Serial.println("test/t - Show sensor status");
      Serial.println("set X - Set people count to X (0-20)");
      Serial.println("help/h - Show this help");
      Serial.println("==========================");
    }
  }
  
  // Read sensors and update data
  bacaProximity();
  updateSensorData();
  
  // Check for AC control commands from hosting (every 10 seconds)
  if (millis() - lastACControlCheck >= AC_CONTROL_CHECK_INTERVAL) {
    checkACControlAPI();
    lastACControlCheck = millis();
  }
  
  // Control AC (auto or manual from hosting)
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
  
  // Check if we should send data to hosting
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
  
  // Send data to hosting if needed
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