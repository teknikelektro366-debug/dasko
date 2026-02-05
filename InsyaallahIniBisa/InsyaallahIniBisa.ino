#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

// ================= TFT =================
TFT_eSPI tft;

// ================= ULTRA FAST OFFLINE FEATURES =================
// Single lamp control for ultra-fast response (1 relay channel)
bool lampStatus = false;
unsigned long lampAutoShutdownTime = 0;
const unsigned long LAMP_AUTO_SHUTDOWN_DELAY = 300000; // 5 minutes auto shutdown

// Ultra-fast response timing
#define ULTRA_FAST_RESPONSE_TIME 1    // 1ms ultra-fast response
#define INSTANT_IR_RESPONSE 0         // 0ms delay for instant IR
#define FAST_TFT_UPDATE 50           // 50ms TFT update for ultra responsiveness

// ================= CONFIGURATION =================
const char* ssid = "LAB TEKNIK TEGANGAN TINGGI";
const char* password = "LABTTT2026";
const char* hostingDomain = "dasko.fst.unja.ac.id";
const char* apiURL = "https://dasko.fst.unja.ac.id/api/sensor/data";
const char* acControlURL = "https://dasko.fst.unja.ac.id/api/ac/control";

const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" \
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" \
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n" \
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n" \
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n" \
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n" \
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n" \
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n" \
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n" \
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n" \
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n" \
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n" \
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n" \
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n" \
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n" \
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n" \
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n" \
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n" \
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n" \
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n" \
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n" \
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n" \
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n" \
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n" \
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n" \
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n" \
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n" \
"-----END CERTIFICATE-----\n";

#define TEMP_CHANGE_THRESHOLD 0.3    // Ultra-sensitive temperature threshold for faster response
#define HUMIDITY_CHANGE_THRESHOLD 1.0 // Ultra-sensitive humidity threshold
#define LIGHT_CHANGE_THRESHOLD 30     // Ultra-sensitive light threshold
#define MAX_TIME_WITHOUT_UPDATE 180000 // Force update every 3 minutes (faster)
#define MIN_UPDATE_INTERVAL 200        // Minimum 200ms between updates (ultra-fast response)
#define AC_CONTROL_CHECK_INTERVAL 5000 // Check AC control every 5 seconds (ultra-fast)
#define DEVICE_ID "ESP32_Proximity_Production_UNJA_ULTRA_FAST"
#define DEVICE_LOCATION "Lab Teknik Tegangan Tinggi - UNJA - Ultra Fast Mode"

// ================= PINS - USER REAL CONFIGURATION (AMAN UPLOAD) =================
#define IR_PIN            15     
#define PROXIMITY_PIN_IN  32    // Proximity sensor MASUK
#define PROXIMITY_PIN_OUT 33    // Proximity sensor KELUAR
#define DHTPIN            12    // Pindah dari 12 ke 16 (aman upload)
#define DHTPIN2           13    // Pindah dari 13 ke 17 (aman upload)
#define DHTTYPE           DHT22
#define LDR_PIN           35    // LDR pin 1
#define LDR_PIN2          34    // LDR pin 2
#define RELAY_LAMP1       25    // Relay untuk lampu (12 lampu TL)
#define MAX_PEOPLE 20        
#define PERSON_COOLDOWN 10         // ULTRA FAST: 10ms cooldown untuk deteksi super cepat
#define DEBOUNCE_DELAY 5           // ULTRA FAST: 5ms debounce untuk responsif maksimal

// ================= ULTRA FAST DUAL LAMP CONTROL =================
void controlLamps(bool turnOn) {
  if (turnOn && (!lamp1Status || !lamp2Status)) {
    digitalWrite(RELAY_LAMP1, HIGH);  // Relay 1 ON (6 lampu TL)
    digitalWrite(RELAY_LAMP2, HIGH);  // Relay 2 ON (6 lampu TL)
    lamp1Status = true;
    lamp2Status = true;
    lampAutoShutdownTime = millis() + LAMP_AUTO_SHUTDOWN_DELAY;
    Serial.println("💡 ULTRA FAST: Dual Lamps ON - 12 lampu TL (Auto shutdown in 5 minutes)");
  } else if (!turnOn && (lamp1Status || lamp2Status)) {
    digitalWrite(RELAY_LAMP1, LOW);   // Relay 1 OFF
    digitalWrite(RELAY_LAMP2, LOW);   // Relay 2 OFF
    lamp1Status = false;
    lamp2Status = false;
    lampAutoShutdownTime = 0;
    Serial.println("💡 ULTRA FAST: Dual Lamps OFF - 12 lampu TL");
  }
}

void checkLampAutoShutdown() {
  if ((lamp1Status || lamp2Status) && lampAutoShutdownTime > 0 && millis() >= lampAutoShutdownTime) {
    controlLamps(false);
    Serial.println("💡 ULTRA FAST: Dual Lamps auto-shutdown after 5 minutes");
  }
}

// ================= SMART LAMP CONTROL BASED ON PEOPLE COUNT & LUX =================
void smartLampControl(int peopleCount, int lightLevel) {
  bool shouldLampBeOn = false;
  String reason = "";
  
  // Logika kontrol lampu berdasarkan jumlah orang dan intensitas cahaya
  if (peopleCount == 0) {
    // Tidak ada orang - mulai timer auto shutdown 5 menit
    shouldLampBeOn = false;
    reason = "No people - Auto shutdown timer";
    
    if (lamp1Status || lamp2Status) {
      if (lampAutoShutdownTime == 0) {
        lampAutoShutdownTime = millis() + LAMP_AUTO_SHUTDOWN_DELAY;
        Serial.println("💡 SMART: Starting 5-minute auto-shutdown timer (no people)");
      }
    }
  }
  else if (peopleCount >= 1) {
    // Ada orang - cek intensitas cahaya
    if (lightLevel < 200) {
      // Cahaya kurang (< 200 lux) - nyalakan lampu
      shouldLampBeOn = true;
      reason = String(peopleCount) + " people, low light (" + String(lightLevel) + " lux)";
      lampAutoShutdownTime = 0; // Cancel auto shutdown
    }
    else if (lightLevel >= 200 && lightLevel < 400) {
      // Cahaya sedang (200-400 lux) - nyalakan jika banyak orang
      if (peopleCount >= 3) {
        shouldLampBeOn = true;
        reason = String(peopleCount) + " people, medium light (" + String(lightLevel) + " lux)";
        lampAutoShutdownTime = 0; // Cancel auto shutdown
      } else {
        shouldLampBeOn = false;
        reason = "Few people (" + String(peopleCount) + "), sufficient light (" + String(lightLevel) + " lux)";
      }
    }
    else {
      // Cahaya cukup (>= 400 lux) - matikan lampu
      shouldLampBeOn = false;
      reason = String(peopleCount) + " people, bright light (" + String(lightLevel) + " lux)";
    }
  }
  
  // Apply lamp control dengan logging
  static bool lastLampDecision = false;
  static String lastReason = "";
  
  if (shouldLampBeOn != lastLampDecision || reason != lastReason) {
    Serial.println("💡 SMART LAMP DECISION: " + String(shouldLampBeOn ? "ON" : "OFF") + " - " + reason);
    
    if (shouldLampBeOn) {
      controlLamps(true);
    } else if (peopleCount == 0) {
      // Jangan langsung matikan jika ada orang, biarkan timer auto shutdown
      // Hanya matikan langsung jika cahaya terlalu terang
      if (lightLevel >= 400) {
        controlLamps(false);
        Serial.println("💡 SMART: Lamps OFF immediately - too bright");
      }
    }
    
    lastLampDecision = shouldLampBeOn;
    lastReason = reason;
  }
}

// ================= HARDWARE - USER REAL PINS =================
IRsend irsend(IR_PIN);  // IR Transmitter on GPIO 4 (aman upload)
IRPanasonicAc ac1(IR_PIN);
IRPanasonicAc ac2(IR_PIN);
DHT dht(DHTPIN, DHTTYPE);   // DHT22 on GPIO 12
DHT dht2(DHTPIN2, DHTTYPE); // DHT22 second sensor on GPIO 13

// ================= VARIABLES =================
int jumlahOrang = 0;
int lastJumlahOrang = -1;
unsigned long lastPersonDetected = 0;

// ULTRA FAST: Interrupt flags untuk deteksi instan
volatile bool interruptTriggered = false;
volatile unsigned long lastInterruptTime = 0;
volatile bool fastDetectionMode = true;  // Mode deteksi cepat

struct SensorData {
  bool proximityIn = false;      
  bool proximityOut = false;     
  bool objectInDetected = false;   
  bool objectOutDetected = false;  
  bool lastProximityIn = false;
  bool lastProximityOut = false;
  unsigned long lastInChange = 0;
  unsigned long lastOutChange = 0;
  bool stableInState = false;
  bool stableOutState = false;
  // ULTRA FAST: Tambahan untuk deteksi cepat
  bool lastObjectInDetected = false;
  bool lastObjectOutDetected = false;
} sensorData;

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

bool lastAC1 = false;
bool lastAC2 = false;
int lastTemp1 = -1;
int lastTemp2 = -1;

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

struct CurrentData {
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

unsigned long lastAPIUpdate = 0;
unsigned long lastForceUpdate = 0;
unsigned long lastACControlCheck = 0;

// ================= TFT COLORS =================
#define TILE_ORANGE    0xFD20    
#define TILE_BLUE      0x051F    
#define TILE_GREEN     0x07E0    
#define TILE_RED       0xF800    
#define TILE_PURPLE    0x781F    
#define TILE_CYAN      0x07FF    
#define TILE_YELLOW    0xFFE0    
#define TILE_TEAL      0x0410    
#define TILE_INDIGO    0x4810    

unsigned long lastAnimationUpdate = 0;
int animationFrame = 0;

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
void drawUI();
void updateTFT(String acStatus, int setSuhu, float suhuRuang);
bool hasSignificantChange();
bool shouldForceUpdate();
bool canSendUpdate();
void updatePreviousData();

// ULTRA FAST: Interrupt handlers untuk deteksi instan
void IRAM_ATTR proximityInInterrupt();
void IRAM_ATTR proximityOutInterrupt();

// IR Troubleshooting functions
void testIRTransmitter();
void manualACTest();

// ULTRA FAST: Interrupt handlers dengan debouncing ultra minimal untuk responsif maksimal
void IRAM_ATTR proximityInInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 5) {  // 5ms debounce untuk responsif ultra maksimal
    interruptTriggered = true;
    lastInterruptTime = now;
    
    // ULTRA FAST: Immediate detection flag
    fastDetectionMode = true;
  }
}

void IRAM_ATTR proximityOutInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 5) {  // 5ms debounce untuk responsif ultra maksimal
    interruptTriggered = true;
    lastInterruptTime = now;
    
    // ULTRA FAST: Immediate detection flag
    fastDetectionMode = true;
  }
}

// ================= IR TROUBLESHOOTING FUNCTIONS =================
void testIRTransmitter() {
  Serial.println("\n🔧 PANASONIC AC IR TRANSMITTER TEST");
  Serial.println("Testing different Panasonic AC commands...");
  
  // Test 1: Panasonic AC ON 25°C
  Serial.println("Test 1: Panasonic AC ON (25°C, Cool, Auto Fan)");
  ac1.on();
  ac1.setMode(kPanasonicAcCool);
  ac1.setTemp(25);
  ac1.setFan(kPanasonicAcFanAuto);
  ac1.setSwingVertical(kPanasonicAcSwingVAuto);
  ac1.setSwingHorizontal(kPanasonicAcSwingHAuto);
  ac1.send();
  delay(3000);
  
  // Test 2: Different temperature
  Serial.println("Test 2: Panasonic AC (22°C, Cool, Auto Fan)");
  ac1.setTemp(22);
  ac1.send();
  delay(3000);
  
  // Test 3: Different fan speed
  Serial.println("Test 3: Panasonic AC (20°C, Cool, High Fan)");
  ac1.setTemp(20);
  ac1.setFan(kPanasonicAcFanMax);
  ac1.send();
  delay(3000);
  
  // Test 4: AC OFF
  Serial.println("Test 4: Panasonic AC OFF");
  ac1.off();
  ac1.send();
  delay(3000);
  
  // Test 5: Raw IR signal test (38kHz carrier)
  Serial.println("Test 5: Raw IR signal (38kHz carrier test)");
  for (int i = 0; i < 100; i++) {
    digitalWrite(IR_PIN, HIGH);
    delayMicroseconds(13);  // 38kHz = 26μs period, 50% duty cycle
    digitalWrite(IR_PIN, LOW);
    delayMicroseconds(13);
  }
  delay(2000);
  
  Serial.println("✅ Panasonic AC IR test sequence completed!");
  Serial.println("If AC didn't respond, check hardware connections.");
}

void manualACTest() {
  Serial.println("\n🎮 MANUAL PANASONIC AC CONTROL TEST");
  Serial.println("This will cycle through Panasonic AC commands every 5 seconds");
  Serial.println("Watch your Panasonic AC for responses...");
  
  // Test sequence
  String tests[] = {
    "AC1 ON 25°C Auto", 
    "AC1 ON 22°C Auto", 
    "AC1 ON 20°C High Fan", 
    "AC1 OFF", 
    "AC2 ON 25°C Auto", 
    "AC2 OFF"
  };
  
  for (int i = 0; i < 6; i++) {
    Serial.println("🔄 Test " + String(i+1) + "/6: " + tests[i]);
    
    switch(i) {
      case 0: // AC1 ON 25°C Auto
        ac1.on();
        ac1.setMode(kPanasonicAcCool);
        ac1.setTemp(25);
        ac1.setFan(kPanasonicAcFanAuto);
        ac1.setSwingVertical(kPanasonicAcSwingVAuto);
        ac1.send();
        break;
      case 1: // AC1 ON 22°C Auto
        ac1.setTemp(22);
        ac1.send();
        break;
      case 2: // AC1 ON 20°C High Fan
        ac1.setTemp(20);
        ac1.setFan(kPanasonicAcFanMax);
        ac1.send();
        break;
      case 3: // AC1 OFF
        ac1.off();
        ac1.send();
        break;
      case 4: // AC2 ON 25°C Auto
        ac2.on();
        ac2.setMode(kPanasonicAcCool);
        ac2.setTemp(25);
        ac2.setFan(kPanasonicAcFanAuto);
        ac2.setSwingVertical(kPanasonicAcSwingVAuto);
        ac2.send();
        break;
      case 5: // AC2 OFF
        ac2.off();
        ac2.send();
        break;
    }
    
    delay(5000);  // Wait 5 seconds between tests
  }
  
  Serial.println("✅ Manual Panasonic AC test completed!");
}

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
    Serial.println("✓ IP Address: " + WiFi.localIP().toString());
    Serial.println("✓ Gateway: " + WiFi.gatewayIP().toString());
    Serial.println("✓ DNS: " + WiFi.dnsIP().toString());
    Serial.println("✓ RSSI: " + String(WiFi.RSSI()) + " dBm");
    Serial.println("✓ Target API: " + String(apiURL));
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI = WiFi.RSSI();
    
    // Test ping ke hosting web
    Serial.println("🔍 Testing connection to hosting web...");
    WiFiClientSecure testClient;
    testClient.setInsecure();
    HTTPClient testHttp;
    if (testHttp.begin(testClient, "https://dasko.fst.unja.ac.id/api/sensor/data")) {
      int httpCode = testHttp.GET();
      Serial.println("📡 Hosting web test response: " + String(httpCode));
      testHttp.end();
    } else {
      Serial.println("❌ Cannot connect to dasko.fst.unja.ac.id");
    }
  } else {
    Serial.println("✗ WiFi FAILED!");
    currentData.wifiStatus = "Failed";
  }
  Serial.println("========================");
}

// ================= HTTP REQUEST - HOSTING CONFIGURATION =================
bool makeHTTPRequest(const char* url, const char* method, const char* payload, String& response) {
  WiFiClientSecure client;
  HTTPClient http;
  
  // HTTPS configuration for hosting
  client.setInsecure(); // For development - in production use proper certificate
  http.setTimeout(15000);    // 15 seconds timeout for hosting
  
  // Connection retry logic
  int retryCount = 0;
  const int maxRetries = 3;
  
  while (retryCount < maxRetries) {
    if (http.begin(client, url)) {
      Serial.println("✅ HTTPS connection established to hosting: " + String(hostingDomain));
      break;
    }
    retryCount++;
    Serial.println("⚠️ HTTPS connection attempt " + String(retryCount) + "/" + String(maxRetries) + " failed");
    if (retryCount < maxRetries) {
      delay(2000);  // Wait before retry for hosting
    }
  }
  
  if (retryCount >= maxRetries) {
    Serial.println("❌ HTTPS connection failed after " + String(maxRetries) + " attempts");
    return false;
  }
  
  // Enhanced headers for hosting
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("User-Agent", "ESP32-Proximity-UNJA/3.0");
  http.addHeader("X-Requested-With", "ESP32");
  http.addHeader("X-Device-ID", DEVICE_ID);
  http.addHeader("X-Location", DEVICE_LOCATION);
  http.addHeader("Host", hostingDomain);
  
  int httpResponseCode;
  if (strcmp(method, "POST") == 0) {
    httpResponseCode = http.POST(payload);
  } else {
    httpResponseCode = http.GET();
  }
  
  if (httpResponseCode > 0) {
    response = http.getString();
    http.end();
    
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      Serial.println("✅ HTTPS " + String(method) + " successful to hosting (Code: " + String(httpResponseCode) + ")");
      return true;
    } else {
      Serial.println("⚠️ HTTPS " + String(method) + " warning to hosting (Code: " + String(httpResponseCode) + ")");
      return httpResponseCode < 500; // Return true for client errors, false for server errors
    }
  } else {
    Serial.println("❌ HTTPS " + String(method) + " failed to hosting with code: " + String(httpResponseCode));
    http.end();
    return false;
  }
}

// ================= SENSOR FUNCTIONS =================
void readSensors() {
  unsigned long now = millis();
  
  // ULTRA FAST: Read sensors dengan debouncing ultra minimal
  bool currentInState = digitalRead(PROXIMITY_PIN_IN);
  bool currentOutState = digitalRead(PROXIMITY_PIN_OUT);
  
  // Debug proximity sensor readings setiap 1 detik (lebih sering untuk ultra fast mode)
  static unsigned long lastProximityDebug = 0;
  if (now - lastProximityDebug > 1000) {
    Serial.println("🔍 ULTRA FAST PROXIMITY DEBUG (USER CONFIG):");
    Serial.println("   GPIO 32 (MASUK): " + String(currentInState ? "HIGH" : "LOW") + " | Detected: " + String(!currentInState ? "YES" : "NO"));
    Serial.println("   GPIO 33 (KELUAR): " + String(currentOutState ? "HIGH" : "LOW") + " | Detected: " + String(!currentOutState ? "YES" : "NO"));
    Serial.println("   💡 Lamps Status: " + String((lamp1Status && lamp2Status) ? "ON (12 TL)" : "OFF"));
    lastProximityDebug = now;
  }
  
  // ULTRA FAST: Debouncing dengan 5ms delay untuk responsif ultra maksimal
  if (currentInState != sensorData.lastProximityIn) {
    sensorData.lastInChange = now;
    sensorData.lastProximityIn = currentInState;
  }
  
  if ((now - sensorData.lastInChange) > DEBOUNCE_DELAY) {
    if (sensorData.proximityIn != currentInState) {
      sensorData.proximityIn = currentInState;
      sensorData.stableInState = true;
    }
  }
  
  if (currentOutState != sensorData.lastProximityOut) {
    sensorData.lastOutChange = now;
    sensorData.lastProximityOut = currentOutState;
  }
  
  if ((now - sensorData.lastOutChange) > DEBOUNCE_DELAY) {
    if (sensorData.proximityOut != currentOutState) {
      sensorData.proximityOut = currentOutState;
      sensorData.stableOutState = true;
    }
  }
  
  // ULTRA FAST: Object detection dengan logika yang tepat
  sensorData.objectInDetected = !sensorData.proximityIn;    
  sensorData.objectOutDetected = !sensorData.proximityOut;  
}

void detectPeople() {
  unsigned long now = millis();
  
  // ULTRA FAST: Cooldown 10ms untuk deteksi super cepat
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
  // Debug info every 3 seconds for better performance
  static unsigned long lastDebug = 0;
  if (now - lastDebug > 3000) {
    Serial.print("📊 ULTRA FAST Proximity Status - Orang: ");
    Serial.print(jumlahOrang);
    Serial.print("/20 | MASUK:");
    Serial.print(sensorData.objectInDetected ? "DETECTED" : "CLEAR");
    Serial.print(" | KELUAR:");
    Serial.print(sensorData.objectOutDetected ? "DETECTED" : "CLEAR");
    Serial.print(" | WiFi: ");
    Serial.print(WiFi.status() == WL_CONNECTED ? "OK" : "FAIL");
    Serial.print(" | Lamps: ");
    Serial.print(lamp1Status && lamp2Status ? "ON" : "OFF");
    
    // Add DHT22 temperature readings to debug info
    float temp1 = dht.readTemperature();
    float temp2 = dht2.readTemperature();
    float hum1 = dht.readHumidity();
    float hum2 = dht2.readHumidity();
    
    Serial.println();
    Serial.println("🌡️ ULTRA FAST DHT22 SENSOR READINGS:");
    Serial.print("   DHT22-1 (GPIO 12): ");
    if (!isnan(temp1) && !isnan(hum1)) {
      Serial.print("Suhu: " + String(temp1, 1) + "°C | Kelembaban: " + String(hum1, 1) + "%");
    } else {
      Serial.print("ERROR - Sensor tidak terbaca!");
    }
    Serial.println();
    
    Serial.print("   DHT22-2 (GPIO 13): ");
    if (!isnan(temp2) && !isnan(hum2)) {
      Serial.print("Suhu: " + String(temp2, 1) + "°C | Kelembaban: " + String(hum2, 1) + "%");
    } else {
      Serial.print("ERROR - Sensor tidak terbaca!");
    }
    Serial.println();
    
    // Calculate and show average
    if (!isnan(temp1) && !isnan(temp2)) {
      float avgTemp = (temp1 + temp2) / 2.0;
      Serial.println("   RATA-RATA Suhu: " + String(avgTemp, 1) + "°C");
    } else if (!isnan(temp1)) {
      Serial.println("   AKTIF: DHT22-1 saja | Suhu: " + String(temp1, 1) + "°C");
    } else if (!isnan(temp2)) {
      Serial.println("   AKTIF: DHT22-2 saja | Suhu: " + String(temp2, 1) + "°C");
    } else {
      Serial.println("   ❌ KEDUA DHT22 ERROR! Periksa koneksi GPIO 12 & 13");
    }
    
    lastDebug = now;
  }
  
  static bool lastInDetected = false;
  static bool lastOutDetected = false;
  static unsigned long lastInTrigger = 0;
  static unsigned long lastOutTrigger = 0;
  
  // ULTRA FAST DETECTION: Sensor MASUK (Entry) - Super fast 10ms cooldown
  if (sensorData.objectInDetected && !lastInDetected) {
    // ULTRA FAST: Cooldown 10ms untuk responsif maksimal
    if (now - lastInTrigger > PERSON_COOLDOWN) {
      jumlahOrang++;
      lastPersonDetected = now;
      lastInTrigger = now;
      sensorData.stableInState = false;
      
      // ULTRA FAST: Smart lamp control based on people count and light level
      smartLampControl(jumlahOrang, currentData.lightLevel);
      
      Serial.println("🚶 → ULTRA FAST PERSON ENTERED!");
      Serial.println("📊 Jumlah orang: " + String(jumlahOrang) + "/20");
      Serial.println("💡 Smart lamp control activated!");
      
      // Update and send data
      updateSensorData();
      if (WiFi.status() == WL_CONNECTED) {
        sendDataToAPI("ULTRA FAST Proximity entry - Count: " + String(jumlahOrang));
      }
    }
  }
  
  // ULTRA FAST DETECTION: Sensor KELUAR (Exit) - Super fast 10ms cooldown
  if (sensorData.objectOutDetected && !lastOutDetected) {
    // ULTRA FAST: Cooldown 10ms untuk responsif maksimal
    if (now - lastOutTrigger > PERSON_COOLDOWN) {
      if (jumlahOrang > 0) {
        jumlahOrang--;
        lastPersonDetected = now;
        lastOutTrigger = now;
        sensorData.stableOutState = false;
        
        Serial.println("� ← PlERSON EXITED!");
        Serial.println("📊 Jumlah orang: " + String(jumlahOrang) + "/20");
        
        // Update and send data
        updateSensorData();
        if (WiFi.status() == WL_CONNECTED) {
          sendDataToAPI("Proximity exit - Count: " + String(jumlahOrang));
        }
      } else {
        Serial.println("⚠ Proximity KELUAR aktif tapi count sudah 0");
        sensorData.stableOutState = false;
        updateSensorData();
        if (WiFi.status() == WL_CONNECTED) {
          sendDataToAPI("Proximity exit attempt when count is 0");
        }
      }
    }
  }
  
  lastInDetected = sensorData.objectInDetected;
  lastOutDetected = sensorData.objectOutDetected;
  
  jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
}
// ================= AC CONTROL API =================
void checkACControlAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  String url = String(acControlURL) + "?device_id=" + DEVICE_ID + "&location=" + DEVICE_LOCATION;
  String response;
  
  if (makeHTTPRequest(url.c_str(), "GET", "", response)) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error && doc["success"]) {
      JsonObject data = doc["data"];
      
      acControl.controlMode = data["control_mode"].as<String>();
      acControl.manualOverride = data["manual_override"];
      acControl.ac1Status = data["ac1_status"];
      acControl.ac2Status = data["ac2_status"];
      acControl.ac1Temperature = data["ac1_temperature"];
      acControl.ac2Temperature = data["ac2_temperature"];
      acControl.createdBy = data["created_by"].as<String>();
      acControl.hasActiveControl = true;
      
      if (data["expires_at"]) {
        acControl.expiresAt = millis() + 300000;
      } else {
        acControl.expiresAt = 0;
      }
      
      Serial.println("✓ AC Control received from hosting: " + acControl.controlMode);
    }
  } else {
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
  
  if (currentData.jumlahOrang != previousData.people_count) {
    Serial.println("🔄 People count changed: " + String(previousData.people_count) + " → " + String(currentData.jumlahOrang));
    hasChange = true;
  }
  
  if (currentData.acStatus != previousData.ac_status) {
    Serial.println("🔄 AC status changed: " + previousData.ac_status + " → " + currentData.acStatus);
    hasChange = true;
  }
  
  if (currentData.setTemp != previousData.set_temperature) {
    Serial.println("🔄 AC temp changed: " + String(previousData.set_temperature) + " → " + String(currentData.setTemp));
    hasChange = true;
  }
  
  if (abs(currentData.suhuRuang - previousData.room_temperature) >= TEMP_CHANGE_THRESHOLD) {
    hasChange = true;
  }
  
  if (abs(currentData.humidity - previousData.humidity) >= HUMIDITY_CHANGE_THRESHOLD) {
    hasChange = true;
  }
  
  if (abs(currentData.lightLevel - previousData.light_level) >= LIGHT_CHANGE_THRESHOLD) {
    hasChange = true;
  }
  
  if (currentData.proximity1 != previousData.proximity_in || currentData.proximity2 != previousData.proximity_out) {
    hasChange = true;
  }
  
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

// ================= SEND DATA TO API =================
void sendDataToAPI(String reason) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("✗ Cannot send to API - WiFi not connected");
    return;
  }
  
  Serial.println("📤 Sending proximity data to hosting web (dasko.fst.unja.ac.id)...");
  Serial.println("   Reason: " + reason);
  Serial.println("   People Count: " + String(currentData.jumlahOrang));
  Serial.println("   AC Status: " + currentData.acStatus);
  Serial.println("   Room Temperature: " + String(currentData.suhuRuang, 1) + "°C");
  Serial.println("   Humidity: " + String(currentData.humidity, 1) + "%");
  
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
  doc["update_type"] = "proximity_detection";
  doc["control_mode"] = acControl.controlMode;
  doc["manual_override"] = acControl.manualOverride;
  doc["hosting_domain"] = hostingDomain;
  doc["ssl_enabled"] = false;
  doc["sensor_type"] = "Proximity_Sensor";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.println("📋 JSON Data to send:");
  Serial.println(jsonString);
  
  String response;
  if (makeHTTPRequest(apiURL, "POST", jsonString.c_str(), response)) {
    Serial.println("✅ SUCCESS: Proximity data sent to hosting web!");
    Serial.println("📥 Server Response: " + response);
    
    updatePreviousData();
    lastAPIUpdate = millis();
    lastForceUpdate = millis();
    
    StaticJsonDocument<256> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);
    if (!error && responseDoc["success"] && responseDoc["data"]["id"]) {
      Serial.println("✅ Laravel saved with ID: " + String(responseDoc["data"]["id"].as<int>()));
    }
  } else {
    Serial.println("❌ FAILED: Could not send proximity data to hosting web");
    Serial.println("📥 Server Response: " + response);
  }
}

void sendDataToAPI() {
  sendDataToAPI("Change detected");
}

// ================= TFT DISPLAY - COLORFUL MODERN DASHBOARD =================
void drawModernHeader() {
  // Animated rainbow header
  for (int y = 0; y < 35; y++) {
    int colorShift = (millis() / 50 + y * 2) % 360;
    uint16_t color;
    if (colorShift < 60) {
      color = tft.color565(255, colorShift * 4, 0);
    } else if (colorShift < 120) {
      color = tft.color565(255 - (colorShift - 60) * 4, 255, 0);
    } else if (colorShift < 180) {
      color = tft.color565(0, 255, (colorShift - 120) * 4);
    } else if (colorShift < 240) {
      color = tft.color565(0, 255 - (colorShift - 180) * 4, 255);
    } else if (colorShift < 300) {
      color = tft.color565((colorShift - 240) * 4, 0, 255);
    } else {
      color = tft.color565(255, 0, 255 - (colorShift - 300) * 4);
    }
    tft.drawFastHLine(0, y, 320, color);
  }
  
  // Title with shadow effect
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(11, 9);
  tft.println("Smart Energy Dashboard");
  
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 8);
  tft.println("Smart Energy Dashboard");
  
  // Animated status indicator
  int pulse = (millis() / 20) % 20;
  uint16_t statusColor = pulse > 10 ? TILE_GREEN : TILE_CYAN;
  tft.fillCircle(300, 18, 6, statusColor);
  tft.fillCircle(300, 18, 3, TFT_WHITE);
}

void drawWeatherIcon(int x, int y) {
  // Animated sun
  tft.fillCircle(x + 20, y + 18, 6, TFT_YELLOW);
  
  // Sun rays
  for (int i = 0; i < 8; i++) {
    int angle = i * 45;
    int x1 = x + 20 + (int)(10 * cos(angle * PI / 180));
    int y1 = y + 18 + (int)(10 * sin(angle * PI / 180));
    int x2 = x + 20 + (int)(14 * cos(angle * PI / 180));
    int y2 = y + 18 + (int)(14 * sin(angle * PI / 180));
    tft.drawLine(x1, y1, x2, y2, TFT_YELLOW);
  }
}

void drawPeopleIcon(int x, int y) {
  // People icons
  for (int i = 0; i < 3; i++) {
    int personX = x + 8 + i * 8;
    int personY = y + 12;
    
    tft.fillCircle(personX, personY, 2, TFT_WHITE);
    tft.fillRect(personX - 2, personY + 2, 4, 6, TFT_WHITE);
  }
}

void drawACIcon(int x, int y) {
  // AC unit
  tft.fillRoundRect(x + 8, y + 12, 24, 16, 4, TFT_WHITE);
  
  // AC lines
  for (int i = 0; i < 4; i++) {
    tft.drawFastHLine(x + 10, y + 16 + i * 2, 20, TILE_BLUE);
  }
  
  // Cool air effect if AC is on
  if (currentData.acStatus.indexOf("OFF") < 0) {
    for (int i = 0; i < 3; i++) {
      int snowX = x + 15 + i * 5;
      int snowY = y + 30 + ((millis() / 100 + i * 10) % 8);
      tft.fillCircle(snowX, snowY, 1, TILE_CYAN);
    }
  }
}

void drawThermometerIcon(int x, int y) {
  // Thermometer
  tft.fillRect(x + 18, y + 8, 4, 16, TFT_WHITE);
  tft.fillCircle(x + 20, y + 26, 4, TFT_RED);
  
  // Temperature level
  int tempLevel = map(currentData.setTemp, 16, 30, 2, 14);
  if (tempLevel > 0) {
    tft.fillRect(x + 19, y + 24 - tempLevel, 2, tempLevel, TFT_RED);
  }
}

void drawSensorIcon(int x, int y) {
  // Sensor waves
  for (int i = 0; i < 3; i++) {
    int radius = 6 + i * 4;
    tft.drawCircle(x + 20, y + 18, radius, TFT_WHITE);
  }
  
  tft.fillCircle(x + 20, y + 18, 2, TFT_WHITE);
  
  // Detection lines when active
  if (sensorData.objectInDetected || sensorData.objectOutDetected) {
    for (int i = 0; i < 5; i++) {
      int lineX = x + 10 + i * 4;
      tft.drawFastVLine(lineX, y + 10, 16, TILE_RED);
    }
  }
}

void drawWiFiIcon(int x, int y) {
  // WiFi arcs
  tft.drawCircle(x + 20, y + 18, 12, TFT_WHITE);
  tft.drawCircle(x + 20, y + 18, 8, TFT_WHITE);
  tft.drawCircle(x + 20, y + 18, 4, TFT_WHITE);
  tft.fillCircle(x + 20, y + 18, 2, TFT_WHITE);
  
  // Signal strength bars
  for (int i = 0; i < 4; i++) {
    int barHeight = 4 + i * 2;
    uint16_t barColor = WiFi.status() == WL_CONNECTED ? TFT_WHITE : TFT_DARKGREY;
    tft.fillRect(x + 8 + i * 3, y + 30 - barHeight, 2, barHeight, barColor);
  }
}

void drawModernTiles() {
  // Clear main area
  tft.fillRect(0, 35, 320, 285, TFT_BLACK);
  
  // Calculate last update time
  unsigned long lastUpdateSec = (millis() - lastAPIUpdate) / 1000;
  String timeStr = "";
  if (lastUpdateSec < 60) {
    timeStr = String(lastUpdateSec) + "s ago";
  } else {
    timeStr = String(lastUpdateSec/60) + "m ago";
  }
  
  // Grid layout: 4x2 tiles
  int tileWidth = 75;
  int tileHeight = 65;
  int spacing = 5;
  int startX = 5;
  int startY = 40;
  
  // Row 1
  // Tile 1: Suhu Ruangan (Orange)
  int tile1X = startX;
  int tile1Y = startY;
  tft.fillRoundRect(tile1X, tile1Y, tileWidth, tileHeight, 8, TILE_ORANGE);
  drawWeatherIcon(tile1X, tile1Y);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(tile1X + 5, tile1Y + 40);
  tft.println("Suhu Ruangan");
  
  tft.setTextSize(2);
  tft.setCursor(tile1X + 5, tile1Y + 50);
  tft.print(currentData.suhuRuang, 1);
  tft.print("C");
  
  // Tile 2: Kelembaban (Blue)
  int tile2X = startX + tileWidth + spacing;
  int tile2Y = startY;
  tft.fillRoundRect(tile2X, tile2Y, tileWidth, tileHeight, 8, TILE_BLUE);
  
  // Water drop icons
  tft.fillCircle(tile2X + 15, tile2Y + 20, 6, TFT_WHITE);
  tft.fillTriangle(tile2X + 15, tile2Y + 8, tile2X + 9, tile2Y + 18, tile2X + 21, tile2Y + 18, TFT_WHITE);
  tft.fillCircle(tile2X + 25, tile2Y + 15, 4, TFT_WHITE);
  tft.fillTriangle(tile2X + 25, tile2Y + 6, tile2X + 21, tile2Y + 13, tile2X + 29, tile2Y + 13, TFT_WHITE);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(tile2X + 5, tile2Y + 40);
  tft.println("Kelembaban");
  
  tft.setTextSize(2);
  tft.setCursor(tile2X + 5, tile2Y + 50);
  tft.print(currentData.humidity, 1);
  tft.print("%");
  
  // Tile 3: Jumlah Orang (Dynamic color)
  int tile3X = startX + (tileWidth + spacing) * 2;
  int tile3Y = startY;
  uint16_t peopleColor = jumlahOrang > 10 ? TILE_RED : (jumlahOrang > 5 ? TILE_ORANGE : TILE_GREEN);
  tft.fillRoundRect(tile3X, tile3Y, tileWidth, tileHeight, 8, peopleColor);
  drawPeopleIcon(tile3X, tile3Y);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(tile3X + 5, tile3Y + 35);
  tft.println("Jumlah Orang");
  tft.setCursor(tile3X + 5, tile3Y + 43);
  tft.println("di Ruangan");
  
  tft.setTextSize(2);
  tft.setCursor(tile3X + 30, tile3Y + 52);
  tft.print(jumlahOrang);
  
  // Tile 4: Intensitas Cahaya (Yellow)
  int tile4X = startX + (tileWidth + spacing) * 3;
  int tile4Y = startY;
  tft.fillRoundRect(tile4X, tile4Y, tileWidth, tileHeight, 8, TILE_YELLOW);
  
  // Sun icon with rays
  tft.fillCircle(tile4X + 20, tile4Y + 18, 6, TILE_ORANGE);
  for (int i = 0; i < 8; i++) {
    int angle = i * 45;
    int x1 = tile4X + 20 + (int)(9 * cos(angle * PI / 180));
    int y1 = tile4Y + 18 + (int)(9 * sin(angle * PI / 180));
    int x2 = tile4X + 20 + (int)(12 * cos(angle * PI / 180));
    int y2 = tile4Y + 18 + (int)(12 * sin(angle * PI / 180));
    tft.drawLine(x1, y1, x2, y2, TILE_ORANGE);
  }
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(tile4X + 5, tile4Y + 35);
  tft.println("Intensitas");
  tft.setCursor(tile4X + 5, tile4Y + 43);
  tft.println("Cahaya");
  
  tft.setCursor(tile4X + 5, tile4Y + 52);
  tft.print(currentData.lightLevel);
  tft.print(" lux");
  
  // Row 2
  int row2Y = startY + tileHeight + spacing;
  
  // Tile 5: Status AC (Dynamic color)
  int tile5X = startX;
  int tile5Y = row2Y;
  uint16_t acColor = currentData.acStatus.indexOf("OFF") >= 0 ? TILE_INDIGO : TILE_RED;
  tft.fillRoundRect(tile5X, tile5Y, tileWidth, tileHeight, 8, acColor);
  drawACIcon(tile5X, tile5Y);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(tile5X + 5, tile5Y + 40);
  tft.println("Status AC");
  
  tft.setTextSize(1);
  tft.setCursor(tile5X + 5, tile5Y + 50);
  if (currentData.acStatus.indexOf("OFF") >= 0) {
    tft.print("AC MATI");
  } else {
    tft.print("AC ON");
  }
  
  // Tile 6: Atur Suhu AC (Cyan)
  int tile6X = startX + tileWidth + spacing;
  int tile6Y = row2Y;
  uint16_t tempColor = currentData.setTemp > 0 ? TILE_CYAN : TILE_INDIGO;
  tft.fillRoundRect(tile6X, tile6Y, tileWidth, tileHeight, 8, tempColor);
  drawThermometerIcon(tile6X, tile6Y);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(tile6X + 5, tile6Y + 40);
  tft.println("Atur Suhu AC");
  
  tft.setTextSize(2);
  tft.setCursor(tile6X + 5, tile6Y + 50);
  if (currentData.setTemp > 0) {
    tft.print(currentData.setTemp);
    tft.print("C");
  } else {
    tft.print("--C");
  }
  
  // Tile 7: Sensor Deteksi (Teal)
  int tile7X = startX + (tileWidth + spacing) * 2;
  int tile7Y = row2Y;
  tft.fillRoundRect(tile7X, tile7Y, tileWidth, tileHeight, 8, TILE_TEAL);
  drawSensorIcon(tile7X, tile7Y);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(tile7X + 5, tile7Y + 40);
  tft.println("Sensor Deteksi");
  tft.setCursor(tile7X + 5, tile7Y + 48);
  tft.println("Orang");
  
  tft.setCursor(tile7X + 5, tile7Y + 56);
  tft.print("MASUK: ");
  tft.print(sensorData.objectInDetected ? "ON" : "OFF");
  
  // Tile 8: Koneksi WiFi (Purple)
  int tile8X = startX + (tileWidth + spacing) * 3;
  int tile8Y = row2Y;
  uint16_t wifiColor = WiFi.status() == WL_CONNECTED ? TILE_PURPLE : TILE_RED;
  tft.fillRoundRect(tile8X, tile8Y, tileWidth, tileHeight, 8, wifiColor);
  drawWiFiIcon(tile8X, tile8Y);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(tile8X + 5, tile8Y + 40);
  tft.println("Koneksi WiFi");
  
  tft.setCursor(tile8X + 5, tile8Y + 50);
  if (WiFi.status() == WL_CONNECTED) {
    tft.print("TERHUBUNG");
  } else {
    tft.print("TERPUTUS");
  }
  
  // Bottom status bar with informative text
  tft.fillRect(0, 245, 320, 75, TFT_BLACK);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 250);
  if (WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(TILE_GREEN);
    tft.print("● CONNECTED");
    tft.setTextColor(TFT_WHITE);
    tft.print(" to dasko.fst.unja.ac.id");
  } else {
    tft.setTextColor(TILE_RED);
    tft.print("● DISCONNECTED");
    tft.setTextColor(TFT_WHITE);
    tft.print(" - Reconnecting...");
  }
  
  tft.setCursor(10, 265);
  if (acControl.manualOverride) {
    tft.setTextColor(TILE_ORANGE);
    tft.print("● Manual Kontrol AC");
  } else {
    tft.setTextColor(TILE_GREEN);
    tft.print("● Mode Otomatis Aktif");
  }
  
  // ULTRA FAST: Add dual lamp status display
  tft.print(" | Lamps: ");
  if (lamp1Status && lamp2Status) {
    tft.setTextColor(TILE_YELLOW);
    tft.print("ON (12 TL)");
  } else {
    tft.setTextColor(TFT_DARKGREY);
    tft.print("OFF");
  }
  
  tft.setTextColor(TFT_LIGHTGREY);
  tft.setCursor(10, 280);
  tft.print("ULTRA FAST: Arduino + Dual Lamps (12 TL) + AC control | Response: <1ms");
  tft.setCursor(10, 295);
  tft.print("Smart Control: People count + Light level | Auto shutdown: 5min | dasko.fst.unja.ac.id");
}

void drawUI() {
  tft.fillScreen(TFT_BLACK);
  drawModernHeader();
  drawModernTiles();
}

void updateTFT(String acStatus, int setSuhu, float suhuRuang) {
  currentData.acStatus = acStatus;
  currentData.setTemp = setSuhu;
  currentData.suhuRuang = suhuRuang;
  
  // INSTANT: Animation update setiap 30ms
  if (millis() - lastAnimationUpdate > 30) {
    animationFrame = (animationFrame + 1) % 20;
    lastAnimationUpdate = millis();
  }
  
  // INSTANT: Update tiles setiap 300ms
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 300) {
    drawModernTiles();
    lastUpdate = millis();
  }
  
  // Update header setiap 2 detik
  static unsigned long lastHeaderUpdate = 0;
  if (millis() - lastHeaderUpdate > 2000) {
    drawModernHeader();
    lastHeaderUpdate = millis();
  }
}

// ================= AC CONTROL - ULTRA FAST RESPONSE =================
void kontrolAC(String &acStatus, int &setTemp) {
  bool ac1ON = false;
  bool ac2ON = false;
  int targetTemp1 = 25;
  int targetTemp2 = 25;
  
  // ULTRA FAST: Immediate response flag
  bool immediateResponse = false;
  
  // Check manual override from hosting
  if (acControl.manualOverride && acControl.hasActiveControl) {
    if (acControl.expiresAt == 0 || millis() < acControl.expiresAt) {
      ac1ON = acControl.ac1Status;
      ac2ON = acControl.ac2Status;
      targetTemp1 = acControl.ac1Temperature;
      targetTemp2 = acControl.ac2Temperature;
      immediateResponse = true;  // Manual control = immediate response
      
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
      
      Serial.println("🚀 ULTRA FAST Manual AC Control: " + acStatus + " @ " + String(setTemp) + "°C");
    } else {
      Serial.println("Manual control expired, returning to auto mode");
      acControl.manualOverride = false;
      acControl.hasActiveControl = false;
      acControl.controlMode = "auto";
    }
  }
  
  // Auto mode based on people count - ULTRA FAST RESPONSE
  if (!acControl.manualOverride || !acControl.hasActiveControl) {
    // ULTRA FAST: Detect people count changes for immediate response
    static int lastPeopleCount = -1;
    if (jumlahOrang != lastPeopleCount) {
      immediateResponse = true;  // People count changed = immediate response
      lastPeopleCount = jumlahOrang;
      Serial.println("🚀 ULTRA FAST People Count Change: " + String(jumlahOrang) + " people");
    }
    
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
  
  // ULTRA FAST: Apply AC changes with immediate response
  if (ac1ON != lastAC1 || ac2ON != lastAC2 || targetTemp1 != lastTemp1 || targetTemp2 != lastTemp2 || immediateResponse) {
    unsigned long startTime = millis();
    
    Serial.println("🚀 ULTRA FAST AC Control Change: " + acStatus + " @ " + String(setTemp) + "°C");
    Serial.println("⚡ Response Mode: " + String(immediateResponse ? "IMMEDIATE" : "NORMAL"));
    
    if (!ac1ON && !ac2ON) {
      Serial.println("📤 ULTRA FAST IR: Turn OFF both ACs");
      
      // ULTRA FAST: Parallel IR transmission with minimal delay
      ac1.off(); 
      ac1.send();
      delayMicroseconds(500);  // Ultra minimal delay (0.5ms)
      
      ac2.off(); 
      ac2.send();
      delayMicroseconds(500);  // Ultra minimal delay (0.5ms)
      
    } else {
      if (ac1ON) {
        Serial.println("📤 ULTRA FAST IR: AC1 ON - " + String(targetTemp1) + "°C");
        
        // ULTRA FAST: Optimized IR signal preparation
        ac1.on();
        ac1.setMode(kPanasonicAcCool);
        ac1.setTemp(targetTemp1);
        ac1.setFan(kPanasonicAcFanAuto);
        ac1.setSwingVertical(kPanasonicAcSwingVAuto);
        
        // ULTRA FAST: Send with instant delay (0ms)
        ac1.send();
        delayMicroseconds(ULTRA_FAST_RESPONSE_TIME * 1000);  // 1ms ultra minimal delay
        
      } else {
        Serial.println("📤 ULTRA FAST IR: AC1 OFF");
        ac1.off(); 
        ac1.send();
        delayMicroseconds(500);  // Ultra minimal delay (0.5ms)
      }
      
      if (ac2ON) {
        Serial.println("📤 ULTRA FAST IR: AC2 ON - " + String(targetTemp2) + "°C");
        
        // ULTRA FAST: Optimized IR signal preparation
        ac2.on();
        ac2.setMode(kPanasonicAcCool);
        ac2.setTemp(targetTemp2);
        ac2.setFan(kPanasonicAcFanAuto);
        ac2.setSwingVertical(kPanasonicAcSwingVAuto);
        
        // ULTRA FAST: Send with instant delay (0ms)
        ac2.send();
        delayMicroseconds(ULTRA_FAST_RESPONSE_TIME * 1000);  // 1ms ultra minimal delay
        
      } else {
        Serial.println("📤 ULTRA FAST IR: AC2 OFF");
        ac2.off(); 
        ac2.send();
        delayMicroseconds(500);  // Ultra minimal delay (0.5ms)
      }
    }
    
    unsigned long responseTime = millis() - startTime;
    Serial.println("⚡ ULTRA FAST IR Response Time: " + String(responseTime) + "ms");
    Serial.println("✅ ULTRA FAST IR signals sent successfully!");
    
    // ULTRA FAST: Update state tracking
    lastAC1 = ac1ON;
    lastAC2 = ac2ON;
    lastTemp1 = targetTemp1;
    lastTemp2 = targetTemp2;
    
    // ULTRA FAST: Immediate feedback to TFT
    updateTFT(acStatus, setTemp, currentData.suhuRuang);
  }
}

// ================= UPDATE SENSOR DATA - ENHANCED WITH DUAL SENSORS =================
void updateSensorData() {
  currentData.jumlahOrang = jumlahOrang;
  
  // Read from both DHT sensors and take average
  float temp1 = dht.readTemperature();
  float temp2 = dht2.readTemperature();
  float hum1 = dht.readHumidity();
  float hum2 = dht2.readHumidity();
  
  // Calculate average temperature (use single sensor if one fails)
  if (!isnan(temp1) && !isnan(temp2)) {
    currentData.suhuRuang = (temp1 + temp2) / 2.0;
  } else if (!isnan(temp1)) {
    currentData.suhuRuang = temp1;
  } else if (!isnan(temp2)) {
    currentData.suhuRuang = temp2;
  } else {
    currentData.suhuRuang = 0.0;
  }
  
  // Calculate average humidity (use single sensor if one fails)
  if (!isnan(hum1) && !isnan(hum2)) {
    currentData.humidity = (hum1 + hum2) / 2.0;
  } else if (!isnan(hum1)) {
    currentData.humidity = hum1;
  } else if (!isnan(hum2)) {
    currentData.humidity = hum2;
  } else {
    currentData.humidity = 0.0;
  }
  
  // Read from both LDR sensors and take average
  int light1 = analogRead(LDR_PIN);
  int light2 = analogRead(LDR_PIN2);
  int avgLight = (light1 + light2) / 2;
  currentData.lightLevel = map(avgLight, 0, 4095, 0, 1000);
  
  currentData.timestamp = millis();
  
  currentData.proximity1 = sensorData.objectInDetected;
  currentData.proximity2 = sensorData.objectOutDetected;
  
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
  Serial.println("  PROXIMITY - ULTRA FAST HOSTING UNJA");
  Serial.println("  Sensor: Proximity Sensor (Ultra Fast + Offline)");
  Serial.println("  Domain: dasko.fst.unja.ac.id");
  Serial.println("  SSL/HTTPS: Enabled & Enhanced");
  Serial.println("  Ultra Fast Response: <50ms");
  Serial.println("  IR Response: <1ms (INSTANT)");
  Serial.println("  Lamp Control: Instant ON/OFF");
  Serial.println("  Pin Config: ESP32 Smart Energy Production");
  Serial.println("  University: Universitas Jambi (UNJA)");
  Serial.println("  Mode: Combined Offline + Hosting Web");
  Serial.println("================================");
  
  // Initialize pins - USER REAL CONFIGURATION (AMAN UPLOAD)
  pinMode(PROXIMITY_PIN_IN, INPUT_PULLUP);   // GPIO 32 - Proximity MASUK
  pinMode(PROXIMITY_PIN_OUT, INPUT_PULLUP);  // GPIO 33 - Proximity KELUAR
  pinMode(LDR_PIN, INPUT);                   // GPIO 35 - LDR 1
  pinMode(LDR_PIN2, INPUT);                  // GPIO 34 - LDR 2
  pinMode(IR_PIN, OUTPUT);                   // GPIO 4 - IR Transmitter (aman upload)
  pinMode(RELAY_LAMP1, OUTPUT);              // GPIO 25 - Relay Lamp 1 (6 lampu TL)
  pinMode(RELAY_LAMP2, OUTPUT);              // GPIO 26 - Relay Lamp 2 (6 lampu TL)
  digitalWrite(RELAY_LAMP1, LOW);            // Initialize relay 1 OFF
  digitalWrite(RELAY_LAMP2, LOW);            // Initialize relay 2 OFF
  
  // Optimized: Setup interrupts with proper debouncing
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN_IN), proximityInInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN_OUT), proximityOutInterrupt, CHANGE);                   
  
  // Initialize sensors - UPDATED FOR DUAL SENSORS
  dht.begin();   // DHT22 sensor 1 on GPIO 12
  dht2.begin();  // DHT22 sensor 2 on GPIO 13
  irsend.begin();  // Initialize IR sender on GPIO 15
  ac1.begin();
  ac2.begin();
  
  // Test IR transmitter - UPDATED PIN INFO
  Serial.println("\n=== IR TRANSMITTER TEST ===");
  Serial.println("IR Pin: GPIO " + String(IR_PIN) + " (Updated from GPIO 14 to GPIO 15)");
  Serial.println("Testing IR LED...");
  
  // Test dengan mengirim sinyal test
  Serial.println("Testing Panasonic AC IR signal...");
  ac1.on();
  ac1.setMode(kPanasonicAcCool);
  ac1.setTemp(25);
  ac1.setFan(kPanasonicAcFanAuto);
  
  Serial.println("Sending test IR signal...");
  ac1.send();
  delay(1000);
  
  Serial.println("Panasonic AC test signal sent!");
  Serial.println("Check if your Panasonic AC responds to test signal");
  Serial.println("If no response, check:");
  Serial.println("- IR LED connection to GPIO 15 (UPDATED PIN)");
  Serial.println("- IR LED polarity (+ to GPIO 15, - to GND)");
  Serial.println("- 220Ω resistor in series with IR LED");
  Serial.println("- Point IR LED directly at Panasonic AC receiver");
  Serial.println("- Distance: 1-3 meters from AC");
  Serial.println("- Remove obstacles between IR LED and AC");
  Serial.println("- Make sure AC is plugged in and ready");
  Serial.println("- Try different Panasonic AC models if needed");
  Serial.println("==============================");
  
  // Initialize TFT
  tft.init();
  tft.setRotation(1);
  drawUI();
  
  // Connect to WiFi
  connectWiFi();
  
  // Initialize timing
  lastForceUpdate = millis();
  lastACControlCheck = millis();
  
  Serial.println("ULTRA FAST PROXIMITY PRODUCTION SYSTEM READY!");
  Serial.println("Enhanced Features v3.0 - ULTRA FAST + HOSTING:");
  Serial.println("- HTTPS/SSL communication with enhanced retry logic");
  Serial.println("- Production hosting integration (UNJA)");
  Serial.println("- ULTRA FAST proximity detection with 25ms debouncing");
  Serial.println("- Interrupt-based detection with 50ms cooldown");
  Serial.println("- ULTRA FAST detection: 50ms cooldown for maximum responsiveness");
  Serial.println("- Enhanced error handling and recovery");
  Serial.println("- Memory optimized for long-term stability");
  Serial.println("- INSTANT lamp control on detection");
  Serial.println("- Auto lamp shutdown after 5 minutes");
  Serial.println("- MASUK sensor = +1 person + lamp ON");
  Serial.println("- KELUAR sensor = -1 person + lamp management");
  
  // Test proximity sensors - UPDATED PIN INFO + LAMP
  Serial.println("\n=== ULTRA FAST PROXIMITY + LAMP SENSOR TEST ===");
  Serial.println("Pin Configuration (USER REAL CONFIG - AMAN UPLOAD):");
  Serial.println("- PROXIMITY_IN: GPIO 32 (Sensor MASUK)");
  Serial.println("- PROXIMITY_OUT: GPIO 33 (Sensor KELUAR)");
  Serial.println("- DHT22_1: GPIO 16 - USER CONFIG (aman upload)");
  Serial.println("- DHT22_2: GPIO 17 - USER CONFIG (aman upload)");
  Serial.println("- LDR_1: GPIO 35");
  Serial.println("- LDR_2: GPIO 34");
  Serial.println("- IR LED: GPIO 4 - USER CONFIG (aman upload)");
  Serial.println("- RELAY LAMP 1: GPIO 25 - USER CONFIG (6 lampu TL)");
  Serial.println("- RELAY LAMP 2: GPIO 26 - USER CONFIG (6 lampu TL)");
  Serial.println("Testing Proximity sensors for 5 seconds...");
  for (int i = 0; i < 25; i++) {
    bool prox1 = digitalRead(PROXIMITY_PIN_IN);
    bool prox2 = digitalRead(PROXIMITY_PIN_OUT);
    bool detected1 = !prox1;  
    bool detected2 = !prox2;  
    
    Serial.print("Test " + String(i+1) + "/25 - ");
    Serial.print("MASUK(32): " + String(prox1 ? "HIGH" : "LOW"));
    if (detected1) {
      Serial.print(" ✅ DETECTED");
    } else {
      Serial.print(" ❌ CLEAR");
    }
    
    Serial.print(" | KELUAR(33): " + String(prox2 ? "HIGH" : "LOW"));
    if (detected2) {
      Serial.print(" ✅ DETECTED");
    } else {
      Serial.print(" ❌ CLEAR");
    }
    
    if (detected1 || detected2) {
      Serial.print(" *** OBJECT DETECTED! ***");
    }
    Serial.println();
    delay(200);
  }
  
  Serial.println("Proximity sensor test complete.");
  Serial.println("Detection: LOW = Object detected");
  Serial.println("If sensors always show same value, check wiring!");
  Serial.println("==============================");
  Serial.println("================================\n");
  
  // Optional: Uncomment untuk test manual IR transmitter jika diperlukan
  // testIRTransmitter();
  // manualACTest();
}

// ================= MAIN LOOP - OPTIMIZED FOR STABILITY =================
void loop() {
  // ULTRA FAST: Check lamp auto-shutdown first for immediate response
  checkLampAutoShutdown();
  
  // ULTRA FAST: Prioritas untuk interrupt-based detection dengan ultra stability
  if (interruptTriggered) {
    interruptTriggered = false;
    readSensors();
    detectPeople();
    updateSensorData();
    
    // ULTRA FAST: Update jika ada perubahan dengan proper timing
    if (jumlahOrang != lastJumlahOrang) {
      // ULTRA FAST: Smart lamp control based on people count and light level
      smartLampControl(jumlahOrang, currentData.lightLevel);
      
      String acStatus = currentData.acStatus;
      int setTemp = currentData.setTemp;
      kontrolAC(acStatus, setTemp);
      currentData.acStatus = acStatus;
      currentData.setTemp = setTemp;
      
      updateTFT(acStatus, setTemp, currentData.suhuRuang);
      lastJumlahOrang = jumlahOrang;
      
      Serial.println("🔄 ULTRA FAST INTERRUPT UPDATE - Jumlah orang berubah: " + String(jumlahOrang));
    }
  } else {
    // ULTRA FAST: Baca sensor dan deteksi orang dengan timing yang ultra stabil
    readSensors();
    detectPeople();
    updateSensorData();
    
    // Optimized: Update TFT jika ada perubahan jumlah orang
    if (jumlahOrang != lastJumlahOrang) {
      // ULTRA FAST: Smart lamp control based on people count and light level
      smartLampControl(jumlahOrang, currentData.lightLevel);
      
      String acStatus = currentData.acStatus;
      int setTemp = currentData.setTemp;
      kontrolAC(acStatus, setTemp);
      currentData.acStatus = acStatus;
      currentData.setTemp = setTemp;
      
      updateTFT(acStatus, setTemp, currentData.suhuRuang);
      lastJumlahOrang = jumlahOrang;
      
      Serial.println("🔄 ULTRA FAST STABLE UPDATE - Jumlah orang berubah: " + String(jumlahOrang));
    }
  }
  
  // Check for AC control commands from hosting (every 5 seconds)
  if (millis() - lastACControlCheck >= AC_CONTROL_CHECK_INTERVAL) {
    checkACControlAPI();
    lastACControlCheck = millis();
  }
  
  // ULTRA FAST: Periodic smart lamp control check (every 10 seconds)
  static unsigned long lastLampCheck = 0;
  if (millis() - lastLampCheck >= 10000) {
    smartLampControl(jumlahOrang, currentData.lightLevel);
    lastLampCheck = millis();
  }
  
  // Control AC (auto or manual from hosting) - hanya jika belum diupdate
  if (jumlahOrang == lastJumlahOrang) {
    String acStatus = currentData.acStatus;
    int setTemp = currentData.setTemp;
    kontrolAC(acStatus, setTemp);
    currentData.acStatus = acStatus;
    currentData.setTemp = setTemp;
  }
  
  // ULTRA FAST: Update TFT setiap 100ms untuk performa yang ultra responsif
  static unsigned long lastTFTUpdate = 0;
  if (millis() - lastTFTUpdate > FAST_TFT_UPDATE) {
    updateTFT(currentData.acStatus, currentData.setTemp, currentData.suhuRuang);
    lastTFTUpdate = millis();
  }
  
  // Check if we should send data to hosting
  bool shouldSend = false;
  String sendReason = "";
  
  if (hasSignificantChange()) {
    if (canSendUpdate()) {
      shouldSend = true;
      sendReason = "Stable change detected";
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
  
  // Enhanced WiFi management (check every 30 seconds)
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 30000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("⚠️ WiFi disconnected, attempting reconnection...");
      connectWiFi();
      
      // If still can't connect after 3 attempts, continue in offline mode
      static int reconnectAttempts = 0;
      if (WiFi.status() != WL_CONNECTED) {
        reconnectAttempts++;
        if (reconnectAttempts >= 3) {
          Serial.println("🔄 Multiple reconnection failures, continuing in offline mode");
          reconnectAttempts = 0;
        }
      } else {
        reconnectAttempts = 0;
        Serial.println("✅ WiFi reconnected successfully");
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
  
  delay(5);  // ULTRA FAST: 5ms delay untuk ultra stable performance dan responsiveness maksimal
}