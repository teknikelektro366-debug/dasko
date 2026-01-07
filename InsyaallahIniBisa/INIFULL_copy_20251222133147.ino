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
// WiFi Configuration - Multiple Networks Support
const char* ssid = "YOUR_WIFI_SSID";        // Ganti dengan nama WiFi Anda
const char* password = "YOUR_WIFI_PASSWORD"; // Ganti dengan password WiFi Anda

// Alternative WiFi networks (fallback)
const char* ssid2 = "AndroidAP";
const char* password2 = "12345678";

const char* ssid3 = "LAPTOP_HOTSPOT";
const char* password3 = "password123";

// Production Hosting Configuration
const char* hostingDomain = "dasko.fst.unja.ac.id";
const char* apiURL = "https://dasko.fst.unja.ac.id/api/sensor/data";
const char* acControlURL = "https://dasko.fst.unja.ac.id/api/ac/control";

// SSL Configuration - Updated root certificate
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

// Optimized Configuration - Enhanced Stability
#define TEMP_CHANGE_THRESHOLD 0.3    // Lebih sensitif untuk suhu
#define HUMIDITY_CHANGE_THRESHOLD 1.5 // Lebih sensitif untuk humidity
#define LIGHT_CHANGE_THRESHOLD 30     // Lebih sensitif untuk cahaya
#define MAX_TIME_WITHOUT_UPDATE 180000 // Kirim paksa setiap 3 menit (lebih sering)
#define MIN_UPDATE_INTERVAL 1500      // Minimum 1.5 detik antar update (lebih cepat)
#define AC_CONTROL_CHECK_INTERVAL 8000 // Check AC control setiap 8 detik (lebih sering)

// Device Information
#define DEVICE_ID "ESP32_Smart_Energy_UNJA"
#define DEVICE_LOCATION "Ruang Dosen Prodi Teknik Elektro - UNJA"

// ================= IR =================
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

#define IR_PIN 4
IRPanasonicAc ac1(IR_PIN);
IRPanasonicAc ac2(IR_PIN);

// ================= SENSOR =================
#define PROX_IN   32    // Pin sensor MASUK
#define PROX_OUT  33    // Pin sensor KELUAR
#define DHTPIN    27
#define DHTTYPE   DHT22
#define LDR_PIN   34

DHT dht(DHTPIN, DHTTYPE);

// ================= VARIABLES =================
int jumlahOrang = 0;
int lastJumlahOrang = -1;

// Proximity timing - Enhanced stability and accuracy
bool lastIn  = HIGH;
bool lastOut = HIGH;
unsigned long inTime = 0;
unsigned long outTime = 0;
unsigned long lastPersonDetected = 0;
#define MAX_INTERVAL 1500    // Reduced to 1.5 seconds for better accuracy
#define TIMEOUT 2500         // Reduced timeout for faster reset
#define PERSON_COOLDOWN 800  // Reduced cooldown for better responsiveness
#define DEBOUNCE_DELAY 150   // Debounce delay for sensor stability

// AC Control State
struct ACControlState {
  String controlMode = "auto";
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

// ================= WIFI SCANNER =================
void scanWiFiNetworks() {
  Serial.println("\n=== SCANNING WIFI NETWORKS ===");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  int n = WiFi.scanNetworks();
  Serial.println("Scan completed!");
  
  if (n == 0) {
    Serial.println("❌ No networks found");
  } else {
    Serial.println("✅ Found " + String(n) + " networks:");
    Serial.println("No. | SSID                     | Signal | Security");
    Serial.println("----|--------------------------|--------|----------");
    
    for (int i = 0; i < n; ++i) {
      String ssidName = WiFi.SSID(i);
      int32_t rssi = WiFi.RSSI(i);
      String security = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secured";
      
      Serial.printf("%2d  | %-24s | %4d   | %s\n", 
                    i + 1, 
                    ssidName.c_str(), 
                    rssi, 
                    security.c_str());
    }
  }
  Serial.println("================================");
  Serial.println("💡 TIP: Update WiFi credentials in code with available network");
  Serial.println("💡 TIP: Use 2.4GHz networks (ESP32 doesn't support 5GHz)");
  Serial.println("");
}

// ================= WIFI CONNECTION - ENHANCED =================
void connectWiFi() {
  Serial.println("=== CONNECTING TO WIFI ===");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  
  // Try multiple networks
  struct WiFiNetwork {
    const char* ssid;
    const char* password;
  };
  
  WiFiNetwork networks[] = {
    {ssid, password},
    {ssid2, password2},
    {ssid3, password3}
  };
  
  bool connected = false;
  
  for (int net = 0; net < 3 && !connected; net++) {
    Serial.println("Trying network: " + String(networks[net].ssid));
    WiFi.begin(networks[net].ssid, networks[net].password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      Serial.println("✅ WiFi CONNECTED!");
      Serial.println("✅ Network: " + String(networks[net].ssid));
      Serial.println("✅ IP: " + WiFi.localIP().toString());
      Serial.println("✅ Signal: " + String(WiFi.RSSI()) + " dBm");
      Serial.println("✅ Production API: " + String(apiURL));
      Serial.println("✅ AC Control API: " + String(acControlURL));
      Serial.println("✅ Domain: " + String(hostingDomain));
      currentData.wifiStatus = "Connected";
      currentData.wifiRSSI = WiFi.RSSI();
      break;
    } else {
      Serial.println("❌ Failed to connect to: " + String(networks[net].ssid));
    }
  }
  
  if (!connected) {
    Serial.println("❌ ALL WIFI NETWORKS FAILED!");
    Serial.println("📋 TROUBLESHOOTING STEPS:");
    Serial.println("1. Check WiFi credentials in code");
    Serial.println("2. Ensure WiFi network is available");
    Serial.println("3. Check ESP32 antenna connection");
    Serial.println("4. Try mobile hotspot as backup");
    Serial.println("5. Check 2.4GHz vs 5GHz network");
    Serial.println("");
    Serial.println("🔧 QUICK FIX OPTIONS:");
    Serial.println("- Use mobile hotspot: AndroidAP / 12345678");
    Serial.println("- Use laptop hotspot: LAPTOP_HOTSPOT / password123");
    Serial.println("- Update WiFi credentials in code");
    Serial.println("");
    Serial.println("🔄 CONTINUING IN OFFLINE MODE...");
    enableOfflineMode();
    currentData.wifiStatus = "Offline Mode";
    currentData.wifiRSSI = 0;
  }
  Serial.println("========================");
}

// ================= HTTPS REQUEST HELPER - ENHANCED =================
bool makeHTTPSRequest(const char* url, const char* method, const char* payload, String& response) {
  WiFiClientSecure client;
  HTTPClient https;
  
  // Set SSL certificate
  client.setCACert(rootCACertificate);
  
  // Enhanced timeout settings
  client.setTimeout(8000);   // Reduced for faster response
  https.setTimeout(8000);    // Reduced for faster response
  
  // Connection retry logic
  int retryCount = 0;
  const int maxRetries = 2;
  
  while (retryCount < maxRetries) {
    if (https.begin(client, url)) {
      break;
    }
    retryCount++;
    Serial.println("⚠️ HTTPS connection attempt " + String(retryCount) + "/" + String(maxRetries) + " failed");
    if (retryCount < maxRetries) {
      delay(1000);  // Wait before retry
    }
  }
  
  if (retryCount >= maxRetries) {
    Serial.println("❌ HTTPS connection failed after " + String(maxRetries) + " attempts");
    return false;
  }
  
  // Set headers with enhanced information
  https.addHeader("Content-Type", "application/json");
  https.addHeader("Accept", "application/json");
  https.addHeader("User-Agent", "ESP32-SmartEnergy-UNJA/2.0");
  https.addHeader("X-Requested-With", "ESP32");
  https.addHeader("X-Device-ID", DEVICE_ID);
  https.addHeader("X-Location", DEVICE_LOCATION);
  
  int httpResponseCode;
  if (strcmp(method, "POST") == 0) {
    httpResponseCode = https.POST(payload);
  } else {
    httpResponseCode = https.GET();
  }
  
  if (httpResponseCode > 0) {
    response = https.getString();
    https.end();
    
    // Enhanced response logging
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      Serial.println("✅ HTTPS " + String(method) + " successful (Code: " + String(httpResponseCode) + ")");
      return true;
    } else {
      Serial.println("⚠️ HTTPS " + String(method) + " warning (Code: " + String(httpResponseCode) + ")");
      Serial.println("   Response: " + response.substring(0, 100) + "...");
      return false;
    }
  } else {
    Serial.println("❌ HTTPS " + String(method) + " failed with code: " + String(httpResponseCode));
    https.end();
    return false;
  }
}

// ================= AC CONTROL API =================
void checkACControlAPI() {
  if (offlineMode || WiFi.status() != WL_CONNECTED) {
    // In offline mode, use local auto control only
    if (acControl.hasActiveControl) {
      Serial.println("📴 OFFLINE: Disabling manual control, using local auto mode");
      acControl.controlMode = "auto";
      acControl.manualOverride = false;
      acControl.hasActiveControl = false;
    }
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

// ================= CHANGE DETECTION - ENHANCED =================
bool hasSignificantChange() {
  bool hasChange = false;
  
  // Check people count change - MOST IMPORTANT
  if (currentData.jumlahOrang != previousData.people_count) {
    Serial.println("🔄 People count changed: " + String(previousData.people_count) + " → " + String(currentData.jumlahOrang));
    hasChange = true;
  }
  
  // Check AC status change
  if (currentData.acStatus != previousData.ac_status) {
    Serial.println("🔄 AC status changed: " + previousData.ac_status + " → " + currentData.acStatus);
    hasChange = true;
  }
  
  // Check AC temperature change
  if (currentData.setTemp != previousData.set_temperature) {
    Serial.println("🔄 AC temp changed: " + String(previousData.set_temperature) + " → " + String(currentData.setTemp));
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

// ================= OFFLINE MODE SUPPORT =================
bool offlineMode = false;

void enableOfflineMode() {
  offlineMode = true;
  Serial.println("🔄 ENABLING OFFLINE MODE");
  Serial.println("✅ System will continue working without WiFi");
  Serial.println("✅ Proximity detection: ACTIVE");
  Serial.println("✅ AC control: LOCAL AUTO MODE");
  Serial.println("✅ TFT display: ACTIVE");
  Serial.println("❌ API updates: DISABLED");
  Serial.println("❌ Manual control from web: DISABLED");
  Serial.println("");
  Serial.println("💡 Connect WiFi later to enable full features");
}

// ================= SEND DATA TO HOSTING API - ENHANCED =================
void sendDataToAPI(String reason = "Change detected") {
  if (offlineMode) {
    Serial.println("📴 OFFLINE MODE: Skipping API update");
    Serial.println("   Reason: " + reason);
    Serial.println("   People Count: " + String(currentData.jumlahOrang));
    Serial.println("   AC Status: " + currentData.acStatus);
    return;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Cannot send to API - WiFi not connected");
    Serial.println("💡 System will continue in offline mode");
    return;
  }
  
  Serial.println("📤 Preparing to send data to website...");
  Serial.println("   Reason: " + reason);
  Serial.println("   People Count: " + String(currentData.jumlahOrang));
  Serial.println("   AC Status: " + currentData.acStatus);
  
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
  doc["update_type"] = "immediate_update";
  doc["control_mode"] = acControl.controlMode;
  doc["manual_override"] = acControl.manualOverride;
  doc["hosting_domain"] = hostingDomain;
  doc["sensor_type"] = "Proximity_Sensor";
  doc["detection_range"] = "1-10cm_adjustable";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.println("📡 Sending JSON: " + jsonString);
  
  String response;
  if (makeHTTPSRequest(apiURL, "POST", jsonString.c_str(), response)) {
    Serial.println("✅ SUCCESS: Data sent to website!");
    Serial.println("   Response: " + response);
    
    // Update previous data and timing
    updatePreviousData();
    lastAPIUpdate = millis();
    lastForceUpdate = millis();
    
    // Parse response untuk ID
    StaticJsonDocument<256> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);
    if (!error && responseDoc["success"] && responseDoc["data"]["id"]) {
      Serial.println("✅ Website saved with ID: " + String(responseDoc["data"]["id"].as<int>()));
    }
  } else {
    Serial.println("❌ FAILED: Could not send data to website");
    Serial.println("   Check your internet connection and website status");
  }
}

// ================= TFT DISPLAY - ENHANCED =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("SMART ENERGY UNJA");
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
  tft.println("Mode: Production UNJA");
  tft.setCursor(10, 235);
  tft.println("Domain: dasko.fst.unja.ac.id");
  tft.setCursor(10, 250);
  tft.println("SSL: Enabled & Verified");
  tft.setCursor(10, 265);
  tft.println("Control: Enhanced Auto");
}

void updateTFT(String acStatus, int setSuhu, float suhuRuang) {
  // Update main values with better formatting
  tft.fillRect(180, 55, 140, 35, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(180, 60);
  tft.setTextColor(jumlahOrang > 0 ? TFT_GREEN : TFT_DARKGREY);
  tft.print(jumlahOrang);
  
  tft.fillRect(180, 95, 140, 30, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(180, 100);
  // Color coding for AC status
  if (acStatus.indexOf("OFF") >= 0) {
    tft.setTextColor(TFT_RED);
  } else if (acStatus.indexOf("MANUAL") >= 0) {
    tft.setTextColor(TFT_YELLOW);
  } else {
    tft.setTextColor(TFT_CYAN);
  }
  tft.print(acStatus);
  
  tft.fillRect(180, 135, 140, 30, TFT_BLACK);
  tft.setCursor(180, 140);
  tft.setTextColor(setSuhu > 0 ? TFT_YELLOW : TFT_DARKGREY);
  if (setSuhu == 0) tft.print("-");
  else {
    tft.print(setSuhu);
    tft.print(" C");
  }
  
  tft.fillRect(180, 175, 140, 30, TFT_BLACK);
  tft.setCursor(180, 180);
  // Color coding for room temperature
  if (suhuRuang > 30) {
    tft.setTextColor(TFT_RED);
  } else if (suhuRuang > 25) {
    tft.setTextColor(TFT_YELLOW);
  } else {
    tft.setTextColor(TFT_GREEN);
  }
  tft.print(suhuRuang, 1);  // Show 1 decimal place
  tft.print(" C");
  
  // Update status (less frequent but more informative)
  static unsigned long lastTftUpdate = 0;
  if (millis() - lastTftUpdate > 8000) { // Update every 8 seconds
    tft.fillRect(10, 235, 310, 80, TFT_BLACK);
    tft.setTextSize(1);
    
    // Domain status
    tft.setTextColor(WiFi.status() == WL_CONNECTED ? TFT_GREEN : TFT_RED);
    tft.setCursor(10, 235);
    tft.println("Domain: dasko.fst.unja.ac.id");
    
    // SSL status
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 250);
    tft.println("SSL: Enabled & Verified");
    
    // API status with timing
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 265);
    tft.print("Last Update: ");
    if (lastAPIUpdate > 0) {
      unsigned long secondsAgo = (millis() - lastAPIUpdate) / 1000;
      tft.print(secondsAgo);
      tft.print("s ago");
      if (secondsAgo > 60) {
        tft.setTextColor(TFT_YELLOW);
        tft.print(" (!)");
      }
    } else {
      tft.setTextColor(TFT_RED);
      tft.print("Never");
    }
    tft.println();
    
    // Control mode with enhanced info
    tft.setCursor(10, 280);
    tft.setTextColor(TFT_WHITE);
    tft.print("Control: ");
    tft.setTextColor(acControl.manualOverride ? TFT_YELLOW : TFT_GREEN);
    tft.print(acControl.controlMode);
    
    // WiFi signal strength
    tft.setCursor(10, 295);
    tft.setTextColor(TFT_WHITE);
    tft.print("WiFi: ");
    int rssi = WiFi.RSSI();
    if (rssi > -50) {
      tft.setTextColor(TFT_GREEN);
      tft.print("Excellent");
    } else if (rssi > -70) {
      tft.setTextColor(TFT_YELLOW);
      tft.print("Good");
    } else {
      tft.setTextColor(TFT_RED);
      tft.print("Weak");
    }
    tft.print(" (");
    tft.print(rssi);
    tft.print("dBm)");
    
    lastTftUpdate = millis();
  }
}

// ================= PROXIMITY SENSOR - ENHANCED LOGIC =================
void bacaProximity() {
  bool inNow  = digitalRead(PROX_IN);
  bool outNow = digitalRead(PROX_OUT);
  unsigned long now = millis();
  
  // Update current sensor data for API
  currentData.proximity1 = (inNow == LOW);
  currentData.proximity2 = (outNow == LOW);
  
  // Cooldown untuk mencegah deteksi ganda
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
  // Debug status sensor setiap 10 detik (reduced frequency)
  static unsigned long lastDebugPrint = 0;
  if (now - lastDebugPrint > 10000) {
    Serial.print("📍 Proximity Status - IN: ");
    Serial.print(inNow == LOW ? "DETECTED" : "CLEAR");
    Serial.print(" | OUT: ");
    Serial.print(outNow == LOW ? "DETECTED" : "CLEAR");
    Serial.print(" | Count: ");
    Serial.print(jumlahOrang);
    Serial.print(" | WiFi: ");
    Serial.print(WiFi.status() == WL_CONNECTED ? "OK" : "FAIL");
    Serial.print(" | RSSI: ");
    Serial.println(WiFi.RSSI());
    lastDebugPrint = now;
  }
  
  static unsigned long lastInTrigger = 0;
  static unsigned long lastOutTrigger = 0;
  
  // Deteksi transisi dari HIGH ke LOW dengan debounce yang lebih baik
  if (lastIn == HIGH && inNow == LOW && inTime == 0) {
    if (now - lastInTrigger > DEBOUNCE_DELAY) {
      inTime = now;
      lastInTrigger = now;
      Serial.println("🔵 IN sensor triggered at " + String(now));
    }
  }
  
  if (lastOut == HIGH && outNow == LOW && outTime == 0) {
    if (now - lastOutTrigger > DEBOUNCE_DELAY) {
      outTime = now;
      lastOutTrigger = now;
      Serial.println("🔴 OUT sensor triggered at " + String(now));
    }
  }
  
  // MASUK: IN sensor dulu, kemudian OUT sensor
  if (inTime > 0 && outTime > 0 && inTime < outTime) {
    unsigned long interval = outTime - inTime;
    if (interval < MAX_INTERVAL && interval > 50) { // Minimum 50ms untuk validitas
      jumlahOrang++;
      lastPersonDetected = now;
      Serial.println("✅ PERSON ENTERED! Interval: " + String(interval) + "ms | Total: " + String(jumlahOrang));
      
      // Immediate update to website
      updateSensorData();
      sendDataToAPI("Person entered - Count: " + String(jumlahOrang));
    } else {
      Serial.println("⚠️ Invalid entry interval: " + String(interval) + "ms (expected: 50-" + String(MAX_INTERVAL) + "ms)");
    }
    inTime = 0;
    outTime = 0;
  }
  
  // KELUAR: OUT sensor dulu, kemudian IN sensor
  if (inTime > 0 && outTime > 0 && outTime < inTime) {
    unsigned long interval = inTime - outTime;
    if (interval < MAX_INTERVAL && interval > 50 && jumlahOrang > 0) {
      jumlahOrang--;
      lastPersonDetected = now;
      Serial.println("✅ PERSON EXITED! Interval: " + String(interval) + "ms | Total: " + String(jumlahOrang));
      
      // Immediate update to website
      updateSensorData();
      sendDataToAPI("Person exited - Count: " + String(jumlahOrang));
    } else if (jumlahOrang == 0) {
      Serial.println("⚠️ Exit attempt when count is already 0");
    } else {
      Serial.println("⚠️ Invalid exit interval: " + String(interval) + "ms (expected: 50-" + String(MAX_INTERVAL) + "ms)");
    }
    inTime = 0;
    outTime = 0;
  }
  
  // Reset dengan timeout untuk mencegah stuck state
  if (inTime > 0 && outTime == 0 && (now - inTime) > TIMEOUT) {
    Serial.println("⏰ IN sensor timeout after " + String(TIMEOUT) + "ms, resetting");
    inTime = 0;
  }
  if (outTime > 0 && inTime == 0 && (now - outTime) > TIMEOUT) {
    Serial.println("⏰ OUT sensor timeout after " + String(TIMEOUT) + "ms, resetting");
    outTime = 0;
  }
  
  // Safety limits dengan logging
  int oldCount = jumlahOrang;
  jumlahOrang = constrain(jumlahOrang, 0, 50);
  if (oldCount != jumlahOrang) {
    Serial.println("⚠️ People count constrained from " + String(oldCount) + " to " + String(jumlahOrang));
  }
  
  // Update previous states
  lastIn = inNow;
  lastOut = outNow;
}

// ================= AC CONTROL - ENHANCED LOGIC =================
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
      
      Serial.println("🎛️ Manual AC Control: " + status + " @ " + String(temp) + "°C (by " + acControl.createdBy + ")");
    } else {
      // Manual control expired, return to auto
      Serial.println("⏰ Manual control expired, returning to auto mode");
      acControl.manualOverride = false;
      acControl.hasActiveControl = false;
      acControl.controlMode = "auto";
    }
  }
  
  // Auto mode - Enhanced logic based on people count and room temperature
  if (!acControl.manualOverride || !acControl.hasActiveControl) {
    float roomTemp = currentData.suhuRuang;
    
    if (jumlahOrang == 0) {
      status = "AUTO OFF";
      temp = 0;
    }
    else if (jumlahOrang <= 3) {
      ac1ON = true;
      targetTemp1 = (roomTemp > 28) ? 24 : 25;  // Adaptive temperature
      temp = targetTemp1;
      status = "AUTO 1 AC";
    }
    else if (jumlahOrang <= 8) {
      ac1ON = true;
      targetTemp1 = (roomTemp > 30) ? 22 : 23;  // More cooling for higher room temp
      temp = targetTemp1;
      status = "AUTO 1 AC";
    }
    else if (jumlahOrang <= 15) {
      ac1ON = true;
      ac2ON = true;
      targetTemp1 = targetTemp2 = (roomTemp > 32) ? 21 : 22;
      temp = targetTemp1;
      status = "AUTO 2 AC";
    }
    else {
      ac1ON = true;
      ac2ON = true;
      targetTemp1 = targetTemp2 = (roomTemp > 35) ? 19 : 20;  // Maximum cooling
      temp = targetTemp1;
      status = "AUTO MAX";
    }
  }
  
  // Apply AC changes only if different (with better logging)
  if (ac1ON != lastAC1 || ac2ON != lastAC2 || targetTemp1 != lastTemp1 || targetTemp2 != lastTemp2) {
    Serial.println("🌡️ AC State Change:");
    Serial.println("   Status: " + status + " @ " + String(temp) + "°C");
    Serial.println("   People: " + String(jumlahOrang) + " | Room Temp: " + String(currentData.suhuRuang) + "°C");
    Serial.println("   AC1: " + String(ac1ON ? "ON" : "OFF") + " (" + String(targetTemp1) + "°C)");
    Serial.println("   AC2: " + String(ac2ON ? "ON" : "OFF") + " (" + String(targetTemp2) + "°C)");
    
    // Send IR commands with error handling
    try {
      if (!ac1ON && !ac2ON) {
        Serial.println("📡 Turning OFF both ACs...");
        ac1.off(); ac1.send();
        delay(200);  // Increased delay for reliability
        ac2.off(); ac2.send();
        delay(200);
      } else {
        if (ac1ON) {
          Serial.println("📡 Configuring AC1: ON @ " + String(targetTemp1) + "°C");
          ac1.on();
          ac1.setMode(kPanasonicAcCool);
          ac1.setTemp(targetTemp1);
          ac1.setFan(kPanasonicAcFanAuto);  // Auto fan speed
          ac1.send();
          delay(200);
        } else {
          Serial.println("📡 Turning OFF AC1...");
          ac1.off(); ac1.send();
          delay(200);
        }
        
        if (ac2ON) {
          Serial.println("📡 Configuring AC2: ON @ " + String(targetTemp2) + "°C");
          ac2.on();
          ac2.setMode(kPanasonicAcCool);
          ac2.setTemp(targetTemp2);
          ac2.setFan(kPanasonicAcFanAuto);  // Auto fan speed
          ac2.send();
          delay(200);
        } else {
          Serial.println("📡 Turning OFF AC2...");
          ac2.off(); ac2.send();
          delay(200);
        }
      }
      
      Serial.println("✅ AC commands sent successfully");
    } catch (...) {
      Serial.println("❌ Error sending AC commands");
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
  
  // Update proximity status (sudah diupdate di readProximitySensors)
  // currentData.proximity1 dan proximity2 sudah diset di readProximitySensors()
  
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
  Serial.println("  SMART ENERGY UNJA - PROXIMITY DETECTION");
  Serial.println("  Domain: dasko.fst.unja.ac.id");
  Serial.println("  SSL/HTTPS: Enabled & Verified");
  Serial.println("  Detection: Enhanced Proximity Logic");
  Serial.println("  Manual AC Control: Supported");
  Serial.println("  University: Universitas Jambi");
  Serial.println("================================");
  
  // Initialize proximity pins dengan pull-up
  pinMode(PROX_IN, INPUT_PULLUP);   // GPIO 32 (PROX_IN)
  pinMode(PROX_OUT, INPUT_PULLUP);  // GPIO 33 (PROX_OUT)
  pinMode(LDR_PIN, INPUT);          // GPIO 34 (LDR Analog)
  
  // Initialize sensors
  dht.begin();
  ac1.begin();
  ac2.begin();
  
  // Initialize TFT
  tft.init();
  tft.setRotation(1);
  drawUI();
  
  // Scan for available WiFi networks first
  scanWiFiNetworks();
  
  // Connect to WiFi
  connectWiFi();
  
  // Initialize timing
  lastForceUpdate = millis();
  lastACControlCheck = millis();
  
  Serial.println("PROXIMITY PRODUCTION SYSTEM READY!");
  Serial.println("Enhanced Features:");
  Serial.println("- HTTPS/SSL communication with retry logic");
  Serial.println("- Production hosting integration (UNJA)");
  Serial.println("- Enhanced proximity detection with debouncing");
  Serial.println("- Adaptive AC control based on room temperature");
  Serial.println("- Stable detection: 800ms cooldown, 150ms debounce");
  Serial.println("- IN sensor first, then OUT sensor = +1 person");
  Serial.println("- OUT sensor first, then IN sensor = -1 person");
  Serial.println("- Manual AC control via web dashboard");
  Serial.println("- Auto mode with temperature-based logic");
  Serial.println("- Enhanced error handling and logging");
  
  // Test proximity sensors with enhanced feedback
  Serial.println("\n=== PROXIMITY SENSOR TEST ===");
  Serial.println("Pin Configuration:");
  Serial.println("- PROX_IN: GPIO 32 (Sensor MASUK)");
  Serial.println("- PROX_OUT: GPIO 33 (Sensor KELUAR)");
  Serial.println("- DHT22: GPIO 27 (Temperature & Humidity)");
  Serial.println("- LDR: GPIO 34 (Light Level - Analog)");
  Serial.println("- IR LED: GPIO 4 (AC Control)");
  Serial.println("Testing Proximity sensors for 5 seconds...");
  Serial.println("Note: LOW = Object detected (within range)");
  Serial.println("Expected: HIGH when clear, LOW when object detected");
  
  bool sensorWorking = false;
  for (int i = 0; i < 10; i++) {
    bool prox1 = digitalRead(PROX_IN);
    bool prox2 = digitalRead(PROX_OUT);
    bool detected1 = (prox1 == LOW);  // LOW = detected
    bool detected2 = (prox2 == LOW);  // LOW = detected
    
    Serial.print("Test " + String(i+1) + "/10 - ");
    Serial.print("IN: " + String(prox1 ? "HIGH" : "LOW"));
    if (detected1) {
      Serial.print(" ✅ DETECTED");
      sensorWorking = true;
    } else {
      Serial.print(" ❌ CLEAR");
    }
    
    Serial.print(" | OUT: " + String(prox2 ? "HIGH" : "LOW"));
    if (detected2) {
      Serial.print(" ✅ DETECTED");
      sensorWorking = true;
    } else {
      Serial.print(" ❌ CLEAR");
    }
    
    if (detected1 || detected2) {
      Serial.print(" *** OBJECT IN RANGE! ***");
    }
    Serial.println();
    delay(500);
  }
  
  Serial.println("Proximity sensor test complete.");
  if (sensorWorking) {
    Serial.println("✅ Sensors appear to be working - detected objects during test");
  } else {
    Serial.println("⚠️ No objects detected during test - sensors may need calibration");
  }
  Serial.println("Detection Logic: LOW = Object detected, HIGH = Clear");
  Serial.println("Entry: IN sensor triggers first, then OUT sensor");
  Serial.println("Exit: OUT sensor triggers first, then IN sensor");
  Serial.println("If sensors don't work, check:");
  Serial.println("  1. Wiring connections (VCC, GND, Signal)");
  Serial.println("  2. Power supply (3.3V or 5V as required)");
  Serial.println("  3. Sensor positioning and range adjustment");
  Serial.println("==============================");
  Serial.println("================================\n");
}

// ================= MAIN LOOP - ENHANCED OPTIMIZATION =================
void loop() {
  // Read proximity sensors and detect people (highest priority)
  bacaProximity();
  updateSensorData();
  
  // Check for AC control commands from hosting (every 8 seconds)
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
  
  // Update TFT if people count changed or periodically
  static unsigned long lastTftRefresh = 0;
  if (jumlahOrang != lastJumlahOrang || (millis() - lastTftRefresh > 5000)) {
    updateTFT(acStatus, setTemp, currentData.suhuRuang);
    
    if (jumlahOrang != lastJumlahOrang) {
      Serial.println("========================");
      Serial.println("📊 SENSOR STATUS UPDATE:");
      Serial.print("👥 People Count  : "); Serial.println(jumlahOrang);
      Serial.print("❄️ AC Status     : "); Serial.println(acStatus);
      Serial.print("🌡️ Set AC Temp   : "); Serial.print(setTemp); Serial.println("°C");
      Serial.print("🏠 Room Temp     : "); Serial.print(currentData.suhuRuang); Serial.println("°C");
      Serial.print("💧 Humidity      : "); Serial.print(currentData.humidity); Serial.println("%");
      Serial.print("💡 Light Level   : "); Serial.println(currentData.lightLevel);
      Serial.print("📶 WiFi Signal   : "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
      Serial.print("🎛️ Control Mode  : "); Serial.println(acControl.controlMode);
      Serial.println("========================");
      lastJumlahOrang = jumlahOrang;
    }
    lastTftRefresh = millis();
  }
  
  // Check if we should send data to hosting
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
      sendReason = "Periodic update (" + String(MAX_TIME_WITHOUT_UPDATE/1000) + "s interval)";
    }
  }
  
  // Send data to hosting if needed
  if (shouldSend) {
    sendDataToAPI(sendReason);
  }
  
  // Enhanced WiFi monitoring and reconnection (check every 20 seconds)
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 20000) {
    if (!offlineMode && WiFi.status() != WL_CONNECTED) {
      Serial.println("⚠️ WiFi disconnected, attempting reconnection...");
      connectWiFi();
      
      // If still can't connect after 3 attempts, enable offline mode
      static int reconnectAttempts = 0;
      if (WiFi.status() != WL_CONNECTED) {
        reconnectAttempts++;
        if (reconnectAttempts >= 3) {
          Serial.println("🔄 Multiple reconnection failures, enabling offline mode");
          enableOfflineMode();
          reconnectAttempts = 0;
        }
      } else {
        reconnectAttempts = 0;
        if (offlineMode) {
          Serial.println("✅ WiFi reconnected! Disabling offline mode");
          offlineMode = false;
        }
      }
    } else if (offlineMode) {
      // Periodically try to reconnect in offline mode
      static int offlineReconnectAttempts = 0;
      offlineReconnectAttempts++;
      if (offlineReconnectAttempts >= 10) { // Try every 200 seconds (20s * 10)
        Serial.println("🔄 Offline mode: Attempting WiFi reconnection...");
        connectWiFi();
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("✅ WiFi reconnected! Disabling offline mode");
          offlineMode = false;
        }
        offlineReconnectAttempts = 0;
      }
    } else {
      // Check signal strength when connected
      int rssi = WiFi.RSSI();
      if (rssi < -80) {
        Serial.println("⚠️ WiFi signal weak (" + String(rssi) + " dBm), monitoring connection...");
      }
    }
    lastWiFiCheck = millis();
  }
  
  // Memory monitoring (every 60 seconds)
  static unsigned long lastMemoryCheck = 0;
  if (millis() - lastMemoryCheck > 60000) {
    uint32_t freeHeap = ESP.getFreeHeap();
    if (freeHeap < 50000) {  // Less than 50KB free
      Serial.println("⚠️ Low memory warning: " + String(freeHeap) + " bytes free");
    }
    lastMemoryCheck = millis();
  }
  
  delay(80);  // Optimized main loop delay for better responsiveness
}