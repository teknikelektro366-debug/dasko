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

// Change Detection Configuration - ULTRA FAST UPDATES
#define TEMP_CHANGE_THRESHOLD 0.3    // Kirim jika suhu berubah >= 0.3°C (lebih sensitif)
#define HUMIDITY_CHANGE_THRESHOLD 1.5 // Kirim jika humidity berubah >= 1.5% (lebih sensitif)
#define LIGHT_CHANGE_THRESHOLD 30     // Kirim jika cahaya berubah >= 30 lux (lebih sensitif)
#define MAX_TIME_WITHOUT_UPDATE 120000 // Kirim paksa setiap 2 menit (lebih sering)
#define MIN_UPDATE_INTERVAL 500       // Minimum 0.5 detik antar update (ULTRA FAST)
#define AC_CONTROL_CHECK_INTERVAL 5000 // Check AC control setiap 5 detik (lebih sering)

// Device Information
#define DEVICE_ID "ESP32_Ultrasonic_Production"
#define DEVICE_LOCATION "Lab Teknik Tegangan Tinggi"

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
void sendDataToAPI(String reason);
void sendDataToAPI();
void updateSensorData();
void connectWiFi();
bool makeHTTPSRequest(const char* url, const char* method, const char* payload, String& response);
void checkACControlAPI();
void kontrolAC(String &acStatus, int &setTemp);
void detectPeople();
void readSensors();
long readUltrasonicDistance(int triggerPin, int echoPin);

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
  bool ultrasonic_in = false;
  bool ultrasonic_out = false;
  int wifi_rssi = 0;
} previousData;

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
  
  // Set timeout - ULTRA FAST
  client.setTimeout(8000);   // Reduced for faster response
  https.setTimeout(8000);    // Reduced for faster response
  
  if (!https.begin(client, url)) {
    Serial.println("✗ HTTPS connection failed");
    return false;
  }
  
  // Set headers
  https.addHeader("Content-Type", "application/json");
  https.addHeader("Accept", "application/json");
  https.addHeader("User-Agent", "ESP32-Ultrasonic-Production/1.0");
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
    
    // FORCE IMMEDIATE UPDATE TO WEBSITE
    updateSensorData();
    sendDataToAPI("Ultrasonic entry - Count: " + String(jumlahOrang));
  }
  
  // Deteksi KELUAR: Sensor DALAM aktif (dari tidak aktif ke aktif)  
  if (sensorData.objectOutDetected && !lastOutDetected && jumlahOrang > 0) {
    jumlahOrang--;
    lastPersonDetected = now;
    Serial.println("🚶 ← ULTRASONIC DALAM AKTIF - ORANG KELUAR!");
    Serial.println("📊 Jumlah orang: " + String(jumlahOrang) + "/20");
    
    // FORCE IMMEDIATE UPDATE TO WEBSITE
    updateSensorData();
    sendDataToAPI("Ultrasonic exit - Count: " + String(jumlahOrang));
  } else if (sensorData.objectOutDetected && !lastOutDetected && jumlahOrang == 0) {
    Serial.println("⚠ Ultrasonic DALAM aktif tapi count sudah 0");
    // STILL SEND UPDATE TO WEBSITE FOR LOGGING
    updateSensorData();
    sendDataToAPI("Ultrasonic exit attempt when count is 0");
  }
  
  // Update state terakhir
  lastInDetected = sensorData.objectInDetected;
  lastOutDetected = sensorData.objectOutDetected;
  
  // Safety limits
  jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
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
  
  // Check ultrasonic sensor change
  if (currentData.ultrasonic1 != previousData.ultrasonic_in || currentData.ultrasonic2 != previousData.ultrasonic_out) {
    hasChange = true;
  }
  
  // Check WiFi signal change
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
  previousData.ultrasonic_in = currentData.ultrasonic1;
  previousData.ultrasonic_out = currentData.ultrasonic2;
  previousData.wifi_rssi = currentData.wifiRSSI;
}

// ================= SEND DATA TO HOSTING API - ENHANCED =================
void sendDataToAPI(String reason) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("✗ Cannot send to API - WiFi not connected");
    return;
  }
  
  Serial.println("📤 Sending ultrasonic data to website...");
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
  doc["ultrasonic_in"] = currentData.ultrasonic1;
  doc["ultrasonic_out"] = currentData.ultrasonic2;
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  doc["timestamp"] = millis();
  doc["status"] = "active";
  doc["update_reason"] = reason;
  doc["update_type"] = "ultrasonic_detection";
  doc["control_mode"] = acControl.controlMode;
  doc["manual_override"] = acControl.manualOverride;
  doc["hosting_domain"] = hostingDomain;
  doc["ssl_enabled"] = true;
  doc["sensor_type"] = "HC-SR04_Ultrasonic";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  String response;
  if (makeHTTPSRequest(apiURL, "POST", jsonString.c_str(), response)) {
    Serial.println("✅ SUCCESS: Ultrasonic data sent to website!");
    
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
    Serial.println("❌ FAILED: Could not send ultrasonic data to website");
  }
}

// Overload tanpa parameter (default reason)
void sendDataToAPI() {
  sendDataToAPI("Change detected");
}

// ================= TFT DISPLAY =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ULTRASONIC - HOSTING");
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
  tft.println("Sensor: HC-SR04 Ultrasonic");
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
  if (millis() - lastTftUpdate > 10000) {
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
    
    lastTftUpdate = millis();
  }
}

// ================= AC CONTROL =================
void kontrolAC(String &acStatus, int &setTemp) {
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
        acStatus = "MANUAL OFF";
        setTemp = 0;
      } else if (ac1ON && ac2ON) {
        acStatus = "MANUAL 2 AC";
        setTemp = (targetTemp1 + targetTemp2) / 2;
      } else {
        acStatus = "MANUAL 1 AC";
        setTemp = ac1ON ? targetTemp1 : targetTemp2;
      }
      
      Serial.println("Manual AC Control from hosting: " + acStatus + " @ " + String(setTemp) + "°C");
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
  }
  
  // Apply AC changes only if different
  if (ac1ON != lastAC1 || ac2ON != lastAC2 || targetTemp1 != lastTemp1 || targetTemp2 != lastTemp2) {
    Serial.println("AC Change: " + acStatus + " @ " + String(setTemp) + "°C");
    
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
  
  // Update ultrasonic status
  currentData.ultrasonic1 = sensorData.objectInDetected;
  currentData.ultrasonic2 = sensorData.objectOutDetected;
  
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
  Serial.println("  ULTRASONIC - PRODUCTION HOSTING");
  Serial.println("  Sensor: HC-SR04 Ultrasonic");
  Serial.println("  Domain: dasko.fst.unja.ac.id");
  Serial.println("  SSL/HTTPS: Enabled");
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
  
  // Connect to WiFi
  connectWiFi();
  
  // Initialize timing
  lastForceUpdate = millis();
  lastACControlCheck = millis();
  
  Serial.println("ULTRASONIC PRODUCTION SYSTEM READY!");
  Serial.println("Features:");
  Serial.println("- HTTPS/SSL communication");
  Serial.println("- Production hosting integration");
  Serial.println("- HC-SR04 ultrasonic sensors");
  Serial.println("- Ultra fast detection: 300ms cooldown");
  Serial.println("- Range: 1-15cm detection");
  Serial.println("- LUAR sensor = +1 person");
  Serial.println("- DALAM sensor = -1 person");
  
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
  Serial.println("================================\n");
}

// ================= MAIN LOOP - ULTRA FAST =================
void loop() {
  // Read sensors and update data
  readSensors();
  detectPeople();
  updateSensorData();
  
  // Check for AC control commands from hosting (every 5 seconds)
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
  
  // Reconnect WiFi if disconnected (check every 15 seconds)
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 15000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi reconnecting...");
      connectWiFi();
    }
    lastWiFiCheck = millis();
  }
  
  delay(30);  // Ultra fast main loop delay
}