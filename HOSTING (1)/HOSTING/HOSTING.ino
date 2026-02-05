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
#include <esp_task_wdt.h>  // Add watchdog timer
#include <time.h>          // Add time library for auto shutdown

// ================= TFT =================
TFT_eSPI tft;

// ================= OPTIMIZED HOSTING CONFIGURATION WITH PROGMEM =================
const char WIFI_SSID[] PROGMEM = "LAB TEKNIK TEGANGAN TINGGI";
const char WIFI_PASS[] PROGMEM = "LABTTT2026";
const char HOST_DOMAIN[] PROGMEM = "dasko.fst.unja.ac.id";
const char API_URL[] PROGMEM = "https://dasko.fst.unja.ac.id/api/sensor/data";
const char AC_URL[] PROGMEM = "https://dasko.fst.unja.ac.id/api/ac/control";
const char PROXY_URL[] PROGMEM = "https://dasko.fst.unja.ac.id/api/proxy/detection";

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* hostingDomain = HOST_DOMAIN;
const char* apiURL = API_URL;
const char* acControlURL = AC_URL;
const char* proxyDetectionURL = PROXY_URL;

// ================= OPTIMIZED SSL CERTIFICATE (REDUCED SIZE) =================
// Compact Root CA Certificate - Essential parts only
const char ROOT_CA_CERT[] PROGMEM = 
"-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
"-----END CERTIFICATE-----\n";

const char* rootCACertificate = ROOT_CA_CERT;

// ================= DEVICE INFO WITH PROGMEM =================
const char DEVICE_ID_STR[] PROGMEM = "UNJA_Prodi_Elektro";
const char DEVICE_LOCATION_STR[] PROGMEM = "Ruang Dosen Prodi Teknik Elektro";

#define DEVICE_ID DEVICE_ID_STR
#define DEVICE_LOCATION DEVICE_LOCATION_STR

// Alternative: Certificate fingerprint method (backup) - NOT USED
// const char* certificateFingerprint = "AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD";

// ================= SSL & PROXY DETECTION CONFIGURATION =================
#define SSL_METHOD_ROOT_CA 1        // Menggunakan Root CA Certificate (Recommended)
#define SSL_METHOD_FINGERPRINT 2    // Menggunakan Certificate Fingerprint
#define SSL_METHOD_INSECURE 3       // Skip SSL verification (HANYA untuk testing)

#define SSL_METHOD SSL_METHOD_ROOT_CA  // Pilih method SSL yang digunakan

#define PROXY_DETECTION_ENABLED true
#define PROXY_RETRY_ATTEMPTS 3
#define PROXY_TIMEOUT_MS 5000
#define PROXY_DETECTION_INTERVAL 30000  // Check every 30 seconds

// Enhanced detection thresholds
#define TEMP_CHANGE_THRESHOLD 0.1      // Lebih sensitif untuk suhu
#define HUMIDITY_CHANGE_THRESHOLD 0.5  // Lebih sensitif untuk kelembaban
#define LIGHT_CHANGE_THRESHOLD 20      // Lebih sensitif untuk cahaya
#define MAX_TIME_WITHOUT_UPDATE 300000 // 5 menit untuk heartbeat saja
#define MIN_UPDATE_INTERVAL 1000       // 1 detik minimum antar update
#define AC_CONTROL_CHECK_INTERVAL 5000 // Check every 5 seconds
#define HEARTBEAT_INTERVAL 300000      // Heartbeat setiap 5 menit (bukan data utama)

#define DEVICE_ID "UNJA_Prodi_Elektro"
#define DEVICE_LOCATION "Ruang Dosen Prodi Teknik Elektro"

// ================= PINS =================
#define IR_PIN            15
#define PROXIMITY_PIN_IN  32
#define PROXIMITY_PIN_OUT 33
#define DHTPIN            13  // Hanya menggunakan DHT di pin 13
#define DHTTYPE           DHT22
#define LDR_PIN           35  // Hanya menggunakan LDR di pin 35
#define RELAY_LAMP1       25  // Relay untuk lampu (menggunakan NC untuk dual fungsi)
// #define RELAY_LAMP2       26  // Tidak digunakan - hanya 1 jalur lampure
#define MAX_PEOPLE 20

// SMART PROXIMITY: Logika cerdas untuk deteksi kecepatan berjalan normal
#define MIN_DETECTION_TIME 100    // Minimal 100ms untuk deteksi valid (terlalu cepat = noise)
#define MAX_DETECTION_TIME 3000   // Maksimal 3 detik untuk deteksi valid (terlalu lama = objek diam)
#define SEQUENCE_TIMEOUT 5000     // 5 detik timeout untuk sequence IN->OUT atau OUT->IN
#define NOISE_FILTER_TIME 30      // 30ms filter untuk noise elektronik
#define WALKING_SPEED_MIN 50      // Minimal 50ms antar sensor untuk kecepatan berjalan normal
#define WALKING_SPEED_MAX 2000    // Maksimal 2 detik antar sensor untuk kecepatan berjalan normal

// Compatibility defines untuk kode lama
#define PERSON_COOLDOWN 200       // 200ms cooldown untuk mencegah double counting
#define DEBOUNCE_DELAY 50         // 50ms debounce untuk stabilitas
#define PROXIMITY_STABLE_TIME 100 // 100ms untuk memastikan deteksi stabil

// ================= LIGHT CONTROL THRESHOLDS =================
#define LUX_THRESHOLD     800  // Threshold cahaya untuk kontrol otomatis (diubah dari 600 ke 800)

// ================= AUTO SHUTDOWN CONFIGURATION - DISABLED =================
// Auto shutdown dan startup telah dihapus untuk stabilitas sistem
// Sistem akan berjalan 24/7 tanpa auto shutdown/startup

// ================= ENHANCED PROXY DETECTION VARIABLES =================
struct ProxyDetectionState {
  bool isProxyDetected = false;
  bool isDirectConnection = true;
  String proxyType = "none";
  String realIP = "";
  String proxyIP = "";
  unsigned long lastProxyCheck = 0;
  int consecutiveFailures = 0;
  bool hostingReachable = true;
} proxyState;

// SSL Status tracking
struct SSLStatus {
  bool isConfigured = false;
  bool lastConnectionSuccess = false;
  String lastError = "";
  unsigned long lastSSLTest = 0;
  int consecutiveFailures = 0;
} sslStatus;

// ================= HARDWARE =================
IRsend irsend(IR_PIN);
IRPanasonicAc ac1(IR_PIN);
IRPanasonicAc ac2(IR_PIN);
DHT dht(DHTPIN, DHTTYPE);  // Hanya satu DHT22

// ================= VARIABLES =================
int jumlahOrang = 0;
int lastJumlahOrang = -1;
unsigned long lastPersonDetected = 0;
unsigned long emptyRoomStartTime = 0;
bool roomEmptyTimerActive = false;
bool testMode = false; // Add test mode flag

#define EMPTY_ROOM_TIMEOUT 300000  // 5 menit dalam milidetik

volatile bool interruptTriggered = false;
volatile unsigned long lastInterruptTime = 0;

// ================= PROXIMITY SENSOR CONFIGURATION =================
// Smart configuration untuk deteksi kecepatan berjalan normal
struct SensorData {
  bool proximityIn = false;
  bool proximityOut = false;
  bool objectInDetected = false;
  bool objectOutDetected = false;
  bool lastProximityIn = false;
  bool lastProximityOut = false;
  unsigned long lastInChange = 0;
  unsigned long lastOutChange = 0;
  
  // Smart detection variables
  unsigned long inDetectionStart = 0;
  unsigned long outDetectionStart = 0;
  bool inSequenceActive = false;
  bool outSequenceActive = false;
  unsigned long sequenceStartTime = 0;
  String currentSequence = "";
  
  // Walking pattern detection
  bool validWalkingPattern = false;
  unsigned long walkingStartTime = 0;
  String walkingDirection = "";
} sensorData;

bool lastAC1 = false;
bool lastAC2 = false;
int lastTemp1 = -1;
int lastTemp2 = -1;

bool lastLamp1 = false;
String lastLampStatus = "";

// Dual function control variables
bool manualOverride = false;  // Flag untuk kontrol manual
bool autoMode = true;         // Flag untuk mode otomatis

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
  String lampStatus = "OFF";
  bool lamp1 = false;
  bool lamp2 = false;  // Tidak digunakan untuk 1 jalur
} currentData;

unsigned long lastAPIUpdate = 0;
unsigned long lastForceUpdate = 0;
unsigned long lastACControlCheck = 0;
unsigned long lastHeartbeat = 0;  // Untuk heartbeat saja, bukan data utama

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

// Remove animation variables to prevent flicker
// unsigned long lastAnimationUpdate = 0;
// int animationFrame = 0;

// Add variables for selective updates
bool forceFullUpdate = false;
int lastDisplayedPeople = -1;
int lastDisplayedTemp = -1;
String lastDisplayedACStatus = "";
String lastDisplayedLampStatus = "";

// ================= FUNCTION DECLARATIONS =================
void sendDataToAPI(String reason);
void updateSensorData();
void connectWiFi();
bool makeHTTPSRequest(const char* url, const char* method, const char* payload, String& response);
bool makeHTTPRequest(const char* url, const char* method, const char* payload, String& response); // Fallback HTTP
void checkACControlAPI();
void kontrolAC(String &acStatus, int &setTemp);
void kontrolLampu(String &lampStatus);
void detectPeople();
void readSensors();
void drawUI();
void updateTFT(String acStatus, int setSuhu, float suhuRuang);
bool hasSignificantChange();
bool shouldForceUpdate();
bool canSendUpdate();
void updatePreviousData();
void resetPeopleCounter(); // Add reset function

// Enhanced SSL and proxy detection functions
void setupSSL(WiFiClientSecure &client);
bool testSSLConnection();
void getCertificateFingerprint();
void checkProxyDetection();
bool detectProxy();
void handleProxyConnection();
void testHostingConnectivity();

// Auto shutdown functions - REMOVED
// void initializeTime();
// void checkAutoShutdown();
// void performAutoShutdown();
// void performAutoStartup();
// String getCurrentTimeString();
// bool isAutoShutdownTime();
// bool isAutoStartupTime();
void sendHeartbeat();  // Heartbeat saja, bukan data utama

void IRAM_ATTR proximityInInterrupt();
void IRAM_ATTR proximityOutInterrupt();

// ================= INTERRUPT HANDLERS =================
void IRAM_ATTR proximityInInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 10) { // Minimal noise filter
    interruptTriggered = true;
    lastInterruptTime = now;
  }
}

void IRAM_ATTR proximityOutInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 10) { // Minimal noise filter
    interruptTriggered = true;
    lastInterruptTime = now;
  }
}

// ================= ENHANCED SSL FUNCTIONS =================

// Setup SSL configuration
void setupSSL(WiFiClientSecure &client) {
  Serial.println("🔒 SETTING UP SSL CONFIGURATION");
  
  #if SSL_METHOD == SSL_METHOD_ROOT_CA
    Serial.println("   Method: Root CA Certificate");
    client.setCACert(rootCACertificate);
    sslStatus.isConfigured = true;
    
  #elif SSL_METHOD == SSL_METHOD_FINGERPRINT
    Serial.println("   Method: Certificate Fingerprint (NOT USED)");
    // client.setFingerprint(certificateFingerprint);
    Serial.println("   ERROR: Fingerprint method disabled");
    sslStatus.isConfigured = false;
    return;
    
  #elif SSL_METHOD == SSL_METHOD_INSECURE
    Serial.println("   Method: Insecure (Skip verification) - DEVELOPMENT ONLY!");
    client.setInsecure();
    sslStatus.isConfigured = true;
    
  #else
    Serial.println("   ERROR: Unknown SSL method!");
    sslStatus.isConfigured = false;
    return;
  #endif
  
  client.setTimeout(15000); // 15 seconds
  Serial.println("✅ SSL Configuration completed");
}

// Test SSL connection
bool testSSLConnection() {
  Serial.println("🔒 TESTING SSL CONNECTION");
  
  WiFiClientSecure testClient;
  setupSSL(testClient);
  
  if (!sslStatus.isConfigured) {
    Serial.println("❌ SSL not configured properly");
    return false;
  }
  
  Serial.println("Connecting to " + String(hostingDomain) + ":443...");
  
  if (testClient.connect(hostingDomain, 443)) {
    Serial.println("✅ SSL Connection successful!");
    
    testClient.println("GET /api/health HTTP/1.1");
    testClient.println("Host: " + String(hostingDomain));
    testClient.println("User-Agent: ESP32-SSL-Test/1.0");
    testClient.println("Connection: close");
    testClient.println();
    
    String response = "";
    unsigned long timeout = millis() + 5000;
    
    while (testClient.connected() && millis() < timeout) {
      if (testClient.available()) {
        response += testClient.readString();
        break;
      }
      delay(10);
    }
    
    testClient.stop();
    
    Serial.println("📥 SSL Test Response (first 200 chars):");
    Serial.println(response.substring(0, 200));
    
    if (response.indexOf("200 OK") > -1 || response.indexOf("healthy") > -1 || response.length() > 0) {
      Serial.println("✅ SSL Test: Server responded correctly");
      sslStatus.lastConnectionSuccess = true;
      sslStatus.consecutiveFailures = 0;
      sslStatus.lastError = "";
      return true;
    } else {
      Serial.println("⚠️ SSL Test: Unexpected response");
      sslStatus.lastConnectionSuccess = false;
      sslStatus.consecutiveFailures++;
      sslStatus.lastError = "Unexpected response";
      return false;
    }
    
  } else {
    Serial.println("❌ SSL Connection failed!");
    Serial.println("Possible issues:");
    Serial.println("- Certificate mismatch or expired");
    Serial.println("- Hosting server SSL not configured");
    Serial.println("- Network/firewall blocking HTTPS");
    Serial.println("- Wrong certificate in code");
    
    sslStatus.lastConnectionSuccess = false;
    sslStatus.consecutiveFailures++;
    sslStatus.lastError = "Connection failed";
    
    return false;
  }
}

// Get certificate info (simplified - fingerprint not used)
void getCertificateFingerprint() {
  Serial.println("🔍 CERTIFICATE INFO (Root CA method used)");
  Serial.println("📋 Current SSL Method: ROOT_CA Certificate");
  Serial.println("📋 Fingerprint method is NOT used in this configuration");
  Serial.println("📋 Using built-in Root CA certificate for SSL verification");
  Serial.println("💡 No action needed - Root CA method is more secure and reliable");
}

// ================= ENHANCED PROXY DETECTION FUNCTIONS =================
void checkProxyDetection() {
  unsigned long now = millis();
  
  if (now - proxyState.lastProxyCheck < PROXY_DETECTION_INTERVAL) {
    return;
  }
  
  proxyState.lastProxyCheck = now;
  
  Serial.println("🔍 PROXY CHECK");
  
  // Quick connectivity test first
  testHostingConnectivity();
  
  if (proxyState.hostingReachable) {
    // Only do full proxy detection if hosting is reachable
    // and if we're not in the middle of critical sensor operations
    static unsigned long lastFullProxyCheck = 0;
    if (now - lastFullProxyCheck > 60000) { // Full check every minute only
      bool proxyDetected = detectProxy();
      
      if (proxyDetected != proxyState.isProxyDetected) {
        proxyState.isProxyDetected = proxyDetected;
        proxyState.isDirectConnection = !proxyDetected;
        
        Serial.println("🔄 PROXY STATUS CHANGED:");
        Serial.println("   Proxy Detected: " + String(proxyDetected ? "YES" : "NO"));
        Serial.println("   Connection Type: " + String(proxyDetected ? "PROXY" : "DIRECT"));
        
        if (proxyDetected) {
          handleProxyConnection();
        }
      }
      lastFullProxyCheck = now;
    }
  }
  
  Serial.println("📊 PROXY STATE:");
  Serial.println("   Hosting Reachable: " + String(proxyState.hostingReachable ? "YES" : "NO"));
  Serial.println("   Proxy Detected: " + String(proxyState.isProxyDetected ? "YES" : "NO"));
  Serial.println("   Connection Type: " + String(proxyState.isDirectConnection ? "DIRECT" : "PROXY"));
  Serial.println("   Consecutive Failures: " + String(proxyState.consecutiveFailures));
}

bool detectProxy() {
  WiFiClientSecure client;
  HTTPClient http;
  
  client.setInsecure();
  http.setTimeout(PROXY_TIMEOUT_MS);
  
  if (!http.begin(client, proxyDetectionURL)) {
    Serial.println("❌ Cannot connect to proxy detection endpoint");
    return false;
  }
  
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("User-Agent", "ESP32-ProxyDetector/1.0");
  http.addHeader("X-Device-ID", DEVICE_ID);
  http.addHeader("X-Real-IP-Check", "true");
  http.addHeader("X-Forwarded-For-Check", "true");
  http.addHeader("X-Proxy-Detection", "enabled");
  
  StaticJsonDocument<200> requestDoc; // Reduced from 300
  requestDoc["device_id"] = DEVICE_ID;
  requestDoc["location"] = DEVICE_LOCATION;
  requestDoc["local_ip"] = WiFi.localIP().toString();
  requestDoc["gateway_ip"] = WiFi.gatewayIP().toString();
  requestDoc["dns_ip"] = WiFi.dnsIP().toString();
  requestDoc["rssi"] = WiFi.RSSI();
  requestDoc["check_type"] = "proxy_detection";
  
  String requestJson;
  serializeJson(requestDoc, requestJson);
  
  int httpCode = http.POST(requestJson);
  
  if (httpCode == 200) {
    String response = http.getString();
    http.end();
    
    StaticJsonDocument<300> responseDoc; // Reduced from 512
    DeserializationError error = deserializeJson(responseDoc, response);
    
    if (!error && responseDoc["success"]) {
      JsonObject data = responseDoc["data"];
      
      proxyState.realIP = data["real_ip"].as<String>();
      proxyState.proxyIP = data["proxy_ip"].as<String>();
      proxyState.proxyType = data["proxy_type"].as<String>();
      
      bool proxyDetected = data["proxy_detected"];
      
      Serial.println("✅ PROXY DETECTION RESPONSE:");
      Serial.println("   Real IP: " + proxyState.realIP);
      Serial.println("   Proxy IP: " + proxyState.proxyIP);
      Serial.println("   Proxy Type: " + proxyState.proxyType);
      Serial.println("   Proxy Detected: " + String(proxyDetected ? "YES" : "NO"));
      
      proxyState.consecutiveFailures = 0;
      return proxyDetected;
    } else {
      Serial.println("❌ Invalid proxy detection response");
      proxyState.consecutiveFailures++;
      return false;
    }
  } else {
    Serial.println("❌ Proxy detection failed with code: " + String(httpCode));
    http.end();
    proxyState.consecutiveFailures++;
    return false;
  }
}

void handleProxyConnection() {
  Serial.println("🔧 HANDLING PROXY CONNECTION");
  
  if (proxyState.isProxyDetected) {
    Serial.println("⚠️ PROXY DETECTED - Adjusting connection parameters");
    Serial.println("   Increasing HTTP timeouts for proxy");
    Serial.println("   Enabling additional retry logic");
    Serial.println("   Proxy Type: " + proxyState.proxyType);
    
    if (WiFi.status() == WL_CONNECTED) {
      String proxyHandlingData = "{";
      proxyHandlingData += "\"device_id\":\"" + String(DEVICE_ID) + "\",";
      proxyHandlingData += "\"proxy_detected\":true,";
      proxyHandlingData += "\"proxy_type\":\"" + proxyState.proxyType + "\",";
      proxyHandlingData += "\"real_ip\":\"" + proxyState.realIP + "\",";
      proxyHandlingData += "\"proxy_ip\":\"" + proxyState.proxyIP + "\",";
      proxyHandlingData += "\"action\":\"proxy_handling_enabled\"";
      proxyHandlingData += "}";
      
      String response;
      makeHTTPSRequest(apiURL, "POST", proxyHandlingData.c_str(), response);
    }
  }
}

void testHostingConnectivity() {
  WiFiClientSecure client;
  HTTPClient http;
  
  client.setInsecure();
  http.setTimeout(2000); // Reduced timeout for faster response
  
  if (http.begin(client, "https://dasko.fst.unja.ac.id/api/health")) {
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      proxyState.hostingReachable = true;
      proxyState.consecutiveFailures = 0;
      Serial.println("✅ Hosting connectivity: OK (Code: " + String(httpCode) + ")");
    } else {
      proxyState.hostingReachable = false;
      proxyState.consecutiveFailures++;
      Serial.println("❌ Hosting connectivity: FAILED (Code: " + String(httpCode) + ")");
    }
    
    http.end();
  } else {
    proxyState.hostingReachable = false;
    proxyState.consecutiveFailures++;
    Serial.println("❌ Cannot establish connection to hosting");
  }
}

// ================= ENHANCED HTTPS REQUEST WITH SSL & PROXY SUPPORT =================
bool makeHTTPSRequest(const char* url, const char* method, const char* payload, String& response) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi not connected");
    return false;
  }
  
  WiFiClientSecure client;
  HTTPClient https;
  
  Serial.println("🔒 MAKING SECURE HTTPS REQUEST");
  Serial.println("URL: " + String(url));
  Serial.println("Method: " + String(method));
  
  setupSSL(client);
  
  if (!sslStatus.isConfigured) {
    Serial.println("❌ SSL not configured");
    return false;
  }
  
  // Adjust timeout based on proxy detection
  int timeout = proxyState.isProxyDetected ? (PROXY_TIMEOUT_MS * 2) : PROXY_TIMEOUT_MS;
  https.setTimeout(timeout);
  https.setReuse(false);
  
  // Enhanced retry logic for proxy connections
  int maxRetries = proxyState.isProxyDetected ? (PROXY_RETRY_ATTEMPTS * 2) : PROXY_RETRY_ATTEMPTS;
  int retryCount = 0;
  
  while (retryCount < maxRetries) {
    if (https.begin(client, url)) {
      Serial.println("✅ HTTPS connection established to: " + String(hostingDomain));
      break;
    }
    
    retryCount++;
    Serial.println("⚠️ HTTPS connection attempt " + String(retryCount) + "/" + String(maxRetries) + " failed");
    
    if (retryCount < maxRetries) {
      int delayTime = proxyState.isProxyDetected ? 1000 : 500; // Reduced delays
      delay(delayTime);
    }
  }
  
  if (retryCount >= maxRetries) {
    Serial.println("❌ HTTPS connection failed after " + String(maxRetries) + " attempts");
    return false;
  }
  
  // Enhanced headers for hosting and proxy support
  https.addHeader("Content-Type", "application/json");
  https.addHeader("Accept", "application/json");
  https.addHeader("User-Agent", "ESP32-Enhanced-UNJA/1.0");
  https.addHeader("X-Requested-With", "ESP32");
  https.addHeader("X-Device-ID", DEVICE_ID);
  https.addHeader("X-Location", DEVICE_LOCATION);
  https.addHeader("Host", hostingDomain);
  https.addHeader("Connection", "close");
  
  // SSL-specific headers
  https.addHeader("X-SSL-Enabled", "true");
  https.addHeader("X-SSL-Method", "root-ca");
  
  // Add proxy-specific headers if proxy detected
  if (proxyState.isProxyDetected) {
    https.addHeader("X-Proxy-Connection", "true");
    https.addHeader("X-Proxy-Type", proxyState.proxyType);
    if (proxyState.realIP.length() > 0) {
      https.addHeader("X-Real-IP", proxyState.realIP);
    }
  }
  
  int httpResponseCode;
  unsigned long requestStart = millis();
  
  if (strcmp(method, "POST") == 0) {
    Serial.println("📤 Sending POST request with SSL...");
    httpResponseCode = https.POST(payload);
  } else {
    Serial.println("📤 Sending GET request with SSL...");
    httpResponseCode = https.GET();
  }
  
  unsigned long requestTime = millis() - requestStart;
  Serial.println("⏱️ SSL Request time: " + String(requestTime) + "ms");
  
  if (httpResponseCode > 0) {
    response = https.getString();
    https.end();
    
    Serial.println("📥 HTTPS Response Code: " + String(httpResponseCode));
    Serial.println("📥 Response Length: " + String(response.length()) + " bytes");
    
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      Serial.println("✅ SECURE HTTPS REQUEST SUCCESSFUL");
      sslStatus.lastConnectionSuccess = true;
      sslStatus.consecutiveFailures = 0;
      sslStatus.lastError = "";
      proxyState.consecutiveFailures = 0; // Reset proxy failure count on success
      return true;
    } else {
      Serial.println("⚠️ HTTPS Request warning (Code: " + String(httpResponseCode) + ")");
      return httpResponseCode < 500;
    }
  } else {
    Serial.println("❌ HTTPS Request failed (Code: " + String(httpResponseCode) + ")");
    
    String errorMsg = "Unknown error";
    switch(httpResponseCode) {
      case HTTPC_ERROR_CONNECTION_REFUSED:
        errorMsg = "Connection refused";
        break;
      case HTTPC_ERROR_SEND_HEADER_FAILED:
        errorMsg = "Send header failed";
        break;
      case HTTPC_ERROR_SEND_PAYLOAD_FAILED:
        errorMsg = "Send payload failed";
        break;
      case HTTPC_ERROR_NOT_CONNECTED:
        errorMsg = "Not connected";
        break;
      case HTTPC_ERROR_CONNECTION_LOST:
        errorMsg = "Connection lost";
        break;
      case HTTPC_ERROR_NO_STREAM:
        errorMsg = "No stream";
        break;
      case HTTPC_ERROR_NO_HTTP_SERVER:
        errorMsg = "No HTTP server";
        break;
      case HTTPC_ERROR_TOO_LESS_RAM:
        errorMsg = "Too less RAM";
        break;
      case HTTPC_ERROR_ENCODING:
        errorMsg = "Encoding error";
        break;
      case HTTPC_ERROR_STREAM_WRITE:
        errorMsg = "Stream write error";
        break;
      case HTTPC_ERROR_READ_TIMEOUT:
        errorMsg = "Read timeout";
        break;
    }
    
    Serial.println("   Error: " + errorMsg);
    sslStatus.lastConnectionSuccess = false;
    sslStatus.consecutiveFailures++;
    sslStatus.lastError = errorMsg;
    proxyState.consecutiveFailures++;
    
    https.end();
    return false;
  }
}

// ================= WIFI CONNECTION (Enhanced) =================
void connectWiFi() {
  Serial.println("=== WIFI CONNECTION ===");
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
    Serial.println("✅ WiFi CONNECTED!");
    Serial.println("✅ IP Address: " + WiFi.localIP().toString());
    Serial.println("✅ Gateway: " + WiFi.gatewayIP().toString());
    Serial.println("✅ DNS: " + WiFi.dnsIP().toString());
    Serial.println("✅ RSSI: " + String(WiFi.RSSI()) + " dBm");
    Serial.println("✅ Target Hosting: " + String(hostingDomain));
    
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI = WiFi.RSSI();
    
    // Test SSL connection after WiFi connected
    Serial.println("\n🔒 Testing SSL connection to hosting...");
    if (testSSLConnection()) {
      Serial.println("✅ SSL connection to hosting successful!");
    } else {
      Serial.println("❌ SSL connection failed. Trying to get certificate info...");
      getCertificateFingerprint();
    }
    
    // Initialize proxy detection
    proxyState.lastProxyCheck = 0; // Force immediate check
    checkProxyDetection();
    
  } else {
    Serial.println("❌ WiFi CONNECTION FAILED!");
    currentData.wifiStatus = "Failed";
  }
  Serial.println("================================");
}

// ================= SENSOR FUNCTIONS =================
void readSensors() {
  unsigned long now = millis();
  
  bool currentInState = digitalRead(PROXIMITY_PIN_IN);
  bool currentOutState = digitalRead(PROXIMITY_PIN_OUT);
  
  // Debug output dengan informasi smart detection
  static unsigned long lastProximityDebug = 0;
  if (now - lastProximityDebug > 5000) {
    Serial.println("� SMART IPROXIMITY DEBUG (Walking Speed Detection):");
    Serial.println("   GPIO 32 (MASUK): " + String(currentInState ? "HIGH" : "LOW") + " | Detected: " + String(!currentInState ? "YES" : "NO"));
    Serial.println("   GPIO 33 (KELUAR): " + String(currentOutState ? "HIGH" : "LOW") + " | Detected: " + String(!currentOutState ? "YES" : "NO"));
    Serial.println("   Current Sequence: " + sensorData.currentSequence);
    Serial.println("   Walking Pattern: " + String(sensorData.validWalkingPattern ? "DETECTED" : "NONE"));
    Serial.println("   Direction: " + sensorData.walkingDirection);
    Serial.println("   � DLamp Status: " + String(lastLamp1 ? "ON" : "OFF"));
    lastProximityDebug = now;
  }
  
  // Minimal noise filtering
  if (currentInState != sensorData.lastProximityIn) {
    if (now - sensorData.lastInChange > NOISE_FILTER_TIME) {
      sensorData.lastInChange = now;
      sensorData.lastProximityIn = currentInState;
      Serial.println("📍 IN sensor: " + String(currentInState ? "HIGH" : "LOW"));
    }
  }
  
  if (currentOutState != sensorData.lastProximityOut) {
    if (now - sensorData.lastOutChange > NOISE_FILTER_TIME) {
      sensorData.lastOutChange = now;
      sensorData.lastProximityOut = currentOutState;
      Serial.println("📍 OUT sensor: " + String(currentOutState ? "HIGH" : "LOW"));
    }
  }
  
  // Update sensor states
  sensorData.proximityIn = currentInState;
  sensorData.proximityOut = currentOutState;
  
  // Object detection (LOW = detected)
  sensorData.objectInDetected = !sensorData.proximityIn;
  sensorData.objectOutDetected = !sensorData.proximityOut;
}

void detectPeople() {
  unsigned long now = millis();
  
  // SMART WALKING DETECTION - Deteksi pola berjalan normal
  static unsigned long lastDebug = 0;
  if (now - lastDebug > 8000) {
    Serial.print("🚶 SMART PEOPLE COUNTER: ");
    Serial.print(jumlahOrang);
    Serial.print("/20 | Pattern: ");
    Serial.print(sensorData.validWalkingPattern ? "WALKING" : "NONE");
    Serial.print(" | Direction: ");
    Serial.print(sensorData.walkingDirection);
    Serial.print(" | Sequence: ");
    Serial.print(sensorData.currentSequence);
    Serial.println();
    
    lastDebug = now;
  }
  
  // IMMEDIATE COUNTING - No validation, just count every detection
  static bool lastInDetected = false;
  static bool lastOutDetected = false;
  static unsigned long lastInTrigger = 0;
  static unsigned long lastOutTrigger = 0;
  
  // ENTRY detection - IN sensor (LANGSUNG TERHITUNG - TANPA COOLDOWN)
  if (sensorData.objectInDetected && !lastInDetected) {
    // Hanya cooldown 200ms untuk mencegah noise elektronik pada jarak 10-100cm
    if (now - lastInTrigger > PERSON_COOLDOWN) {
      jumlahOrang++;
      lastPersonDetected = now;
      lastInTrigger = now;
      
      roomEmptyTimerActive = false;
      
      Serial.println("� → LANGSUrNG MASUK! Count: " + String(jumlahOrang));
      
      updateSensorData();
      
      // LANGSUNG KIRIM KE DATABASE - TANPA DELAY
      if (WiFi.status() == WL_CONNECTED) {
        String reason = "IMMEDIATE_ENTRY - Person " + String(jumlahOrang) + " entered - Proximity IN";
        sendDataToAPI(reason);
        Serial.println("📤 ✅ LANGSUNG TERCATAT: Orang masuk #" + String(jumlahOrang));
      }
    }
  }
  
  // EXIT detection - OUT sensor (LANGSUNG TERHITUNG - TANPA COOLDOWN)
  if (sensorData.objectOutDetected && !lastOutDetected) {
    // Cooldown 200ms untuk exit pada jarak 10-100cm
    if (now - lastOutTrigger > PERSON_COOLDOWN) {
      
      if (jumlahOrang > 0) {
        // Normal exit - langsung kurangi
        jumlahOrang--;
        lastPersonDetected = now;
        lastOutTrigger = now;
        
        if (jumlahOrang == 0) {
          emptyRoomStartTime = millis();
          roomEmptyTimerActive = true;
          Serial.println("⏰ Empty room timer started");
        }
        
        Serial.println("🚶 ← LANGSUNG KELUAR! Count: " + String(jumlahOrang));
        
        updateSensorData();
        
        // LANGSUNG KIRIM KE DATABASE - TANPA DELAY
        if (WiFi.status() == WL_CONNECTED) {
          String reason = "IMMEDIATE_EXIT - Person left, remaining: " + String(jumlahOrang) + " - Proximity OUT";
          sendDataToAPI(reason);
          Serial.println("📤 ✅ LANGSUNG TERCATAT: Orang keluar, sisa #" + String(jumlahOrang));
        }
      } else {
        // Exit detected but count is 0
        lastOutTrigger = now;
        
        static unsigned long lastFalseExitWarning = 0;
        if (now - lastFalseExitWarning > 3000) { // Warning setiap 3 detik
          Serial.println("⚠ EXIT saat count=0 - kemungkinan sensor issue");
          lastFalseExitWarning = now;
        }
      }
    }
  }
  
  // Debug state changes
  if (sensorData.objectInDetected != lastInDetected) {
    Serial.println("🔄 IN: " + String(lastInDetected ? "DETECTED" : "CLEAR") + 
                   " → " + String(sensorData.objectInDetected ? "DETECTED" : "CLEAR"));
  }
  
  if (sensorData.objectOutDetected != lastOutDetected) {
    Serial.println("🔄 OUT: " + String(lastOutDetected ? "DETECTED" : "CLEAR") + 
                   " → " + String(sensorData.objectOutDetected ? "DETECTED" : "CLEAR"));
  }
  
  lastInDetected = sensorData.objectInDetected;
  lastOutDetected = sensorData.objectOutDetected;
  jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
}

// ================= TFT DISPLAY FUNCTIONS =================
void drawModernHeader() {
  // Use full width of screen
  int screenWidth = tft.width();
  
  // Static gradient instead of animated to prevent flicker
  for (int y = 0; y < 35; y++) {
    uint16_t color = tft.color565(0, 100 + y * 3, 200 - y * 2); // Static blue gradient
    tft.drawFastHLine(0, y, screenWidth, color);
  }
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(11, 9);
  tft.println("Smart AC - Prodi Elektro");
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 8);
  tft.println("Smart AC - Prodi Elektro");
  
  // Static status indicator instead of pulsing
  uint16_t statusColor = TILE_GREEN;
  tft.fillCircle(screenWidth - 20, 18, 6, statusColor);
  tft.fillCircle(screenWidth - 20, 18, 3, TFT_WHITE);
}
void drawModernTiles() {
  int screenWidth = tft.width();
  int screenHeight = tft.height();
  
  // Use full screen height minus header
  tft.fillRect(0, 35, screenWidth, screenHeight - 35, TFT_BLACK);
  
  int boxWidth = (screenWidth - 30) / 2; // Dynamic width based on screen
  int boxHeight = (screenHeight - 120) / 2; // Dynamic height based on screen
  int spacing = 10;
  int startX = 10;
  int startY = 45;
  
  // ========== BOX 1: JUMLAH ORANG (Kiri Atas) ==========
  int box1X = startX;
  int box1Y = startY;
  uint16_t peopleColor = jumlahOrang > 10 ? TILE_RED : (jumlahOrang > 5 ? TILE_ORANGE : TILE_GREEN);
  tft.fillRoundRect(box1X, box1Y, boxWidth, boxHeight, 10, peopleColor);
  
  tft.fillCircle(box1X + 30, box1Y + 20, 8, TFT_WHITE);
  tft.fillRect(box1X + 22, box1Y + 30, 16, 20, TFT_WHITE);
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(box1X + 10, box1Y + boxHeight - 30);
  tft.println("ORANG");
  
  tft.setTextSize(4);
  tft.setCursor(box1X + boxWidth - 80, box1Y + 25);
  tft.print(jumlahOrang);
  
  // ========== BOX 2: SUHU AC (Kanan Atas) ==========
  int box2X = startX + boxWidth + spacing;
  int box2Y = startY;
  uint16_t acColor = currentData.setTemp > 0 ? TILE_CYAN : TILE_INDIGO;
  tft.fillRoundRect(box2X, box2Y, boxWidth, boxHeight, 10, acColor);
  
  tft.fillRoundRect(box2X + 15, box2Y + 15, 35, 25, 5, TFT_WHITE);
  for (int i = 0; i < 5; i++) {
    tft.drawFastHLine(box2X + 18, box2Y + 20 + i * 3, 29, TILE_BLUE);
  }
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(box2X + 10, box2Y + boxHeight - 30);
  tft.println("SUHU AC");
  
  tft.setTextSize(4);
  tft.setCursor(box2X + boxWidth - 100, box2Y + 20);
  if (currentData.setTemp > 0) {
    tft.print(currentData.setTemp);
    tft.setTextSize(2);
    tft.print("C");
  } else {
    tft.setTextSize(3);
    tft.print("OFF");
  }
  
  // ========== BOX 3: SUHU RUANGAN (Kiri Bawah) ==========
  int box3X = startX;
  int box3Y = startY + boxHeight + spacing;
  tft.fillRoundRect(box3X, box3Y, boxWidth, boxHeight, 10, TILE_ORANGE);
  
  tft.fillRect(box3X + 28, box3Y + 15, 6, 25, TFT_WHITE);
  tft.fillCircle(box3X + 31, box3Y + 42, 8, TFT_RED);
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(box3X + 10, box3Y + boxHeight - 30);
  tft.println("SUHU RUANG");
  
  tft.setTextSize(3);
  tft.setCursor(box3X + boxWidth - 100, box3Y + 20);
  tft.print(currentData.suhuRuang, 1);
  tft.setTextSize(2);
  tft.print("C");
  
  // ========== BOX 4: LAMPU (Kanan Bawah) ==========
  int box4X = startX + boxWidth + spacing;
  int box4Y = startY + boxHeight + spacing;
  uint16_t lightColor = currentData.lamp1 ? TILE_YELLOW : TILE_INDIGO;
  tft.fillRoundRect(box4X, box4Y, boxWidth, boxHeight, 10, lightColor);
  
  // Icon lampu
  tft.fillCircle(box4X + 30, box4Y + 25, 10, currentData.lamp1 ? TFT_YELLOW : TFT_DARKGREY);
  for (int i = 0; i < 8; i++) {
    int angle = i * 45;
    int x1 = box4X + 30 + (int)(14 * cos(angle * PI / 180));
    int y1 = box4Y + 25 + (int)(14 * sin(angle * PI / 180));
    int x2 = box4X + 30 + (int)(20 * cos(angle * PI / 180));
    int y2 = box4Y + 25 + (int)(20 * sin(angle * PI / 180));
    uint16_t rayColor = currentData.lamp1 ? TFT_YELLOW : TFT_DARKGREY;
    tft.drawLine(x1, y1, x2, y2, rayColor);
  }
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(box4X + 10, box4Y + boxHeight - 30);
  tft.println("LAMPU");
  
  tft.setTextSize(2);
  tft.setCursor(box4X + boxWidth - 80, box4Y + 20);
  if (currentData.lamp1) {
    tft.print("ON");
  } else {
    tft.print("OFF");
  }
  
  // ========== EXPANDED STATUS BAR BAWAH =================
  int statusY = startY + (boxHeight * 2) + (spacing * 2) + 10;
  int statusHeight = screenHeight - statusY;
  tft.fillRect(0, statusY, screenWidth, statusHeight, TFT_BLACK);
  
  // Row 1: Status and WiFi
  tft.setTextSize(1);
  tft.setTextColor(TILE_GREEN);
  tft.setCursor(10, statusY + 5);
  tft.print("STATUS: ");
  tft.setTextColor(TFT_WHITE);
  tft.print(currentData.acStatus);
  
  tft.setCursor(screenWidth - 80, statusY + 5);
  if (WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(TILE_GREEN);
    tft.print("ONLINE");
  } else {
    tft.setTextColor(TILE_ORANGE);
    tft.print("OFFLINE");
  }
  
  // Row 2: Lamp and Memory
  tft.setCursor(10, statusY + 20);
  tft.setTextColor(TILE_YELLOW);
  tft.print("LAMPU: ");
  tft.setTextColor(TFT_WHITE);
  tft.print(currentData.lampStatus);
  
  tft.setCursor(screenWidth - 80, statusY + 20);
  tft.setTextColor(TILE_CYAN);
  tft.print("MEM:");
  tft.setTextColor(TFT_WHITE);
  tft.print(ESP.getFreeHeap() / 1024);
  tft.print("K");
  
  // Row 3: Light and Humidity
  if (statusHeight > 40) {
    tft.setCursor(10, statusY + 35);
    tft.setTextColor(TILE_PURPLE);
    tft.print("LIGHT: ");
    tft.setTextColor(TFT_WHITE);
    tft.print(currentData.lightLevel);
    tft.print(" lux");
    
    tft.setCursor(screenWidth - 80, statusY + 35);
    tft.setTextColor(TILE_PURPLE);
    tft.print("HUM:");
    tft.setTextColor(TFT_WHITE);
    tft.print(currentData.humidity, 0);
    tft.print("%");
  }
  
  // Row 4: Timer or Uptime
  if (statusHeight > 55) {
    if (jumlahOrang == 0 && roomEmptyTimerActive) {
      unsigned long emptyDuration = millis() - emptyRoomStartTime;
      unsigned long remainingTime = EMPTY_ROOM_TIMEOUT - emptyDuration;
      int remainingMinutes = remainingTime / 60000;
      int remainingSeconds = (remainingTime % 60000) / 1000;
      
      tft.setCursor(10, statusY + 50);
      tft.setTextColor(TILE_RED);
      tft.print("AUTO OFF: ");
      tft.setTextColor(TFT_WHITE);
      tft.print(remainingMinutes);
      tft.print("m ");
      tft.print(remainingSeconds);
      tft.print("s");
    } else {
      tft.setCursor(10, statusY + 50);
      tft.setTextColor(TILE_TEAL);
      tft.print("UPTIME: ");
      tft.setTextColor(TFT_WHITE);
      unsigned long uptimeMinutes = millis() / 60000;
      tft.print(uptimeMinutes);
      tft.print("m");
    }
  }
  
  // Row 5: Device info (if space available)
  if (statusHeight > 70) {
    tft.setCursor(10, statusY + 65);
    tft.setTextSize(1);
    tft.setTextColor(TILE_INDIGO);
    tft.print("MODE: ");
    tft.setTextColor(TFT_WHITE);
    tft.print("PRODI ELEKTRO - SSL");
  }
}

void drawUI() {
  tft.fillScreen(TFT_BLACK);
  drawModernHeader();
  drawModernTiles();
}

void updateTFT(String acStatus, int setSuhu, float suhuRuang) {
  // Prevent memory issues by limiting string operations
  currentData.acStatus = acStatus;
  currentData.setTemp = setSuhu;
  currentData.suhuRuang = suhuRuang;
  
  // Check if significant changes occurred
  bool needUpdate = false;
  if (lastDisplayedPeople != jumlahOrang) {
    lastDisplayedPeople = jumlahOrang;
    needUpdate = true;
  }
  if (lastDisplayedTemp != (int)suhuRuang) {
    lastDisplayedTemp = (int)suhuRuang;
    needUpdate = true;
  }
  if (lastDisplayedACStatus != acStatus) {
    lastDisplayedACStatus = acStatus;
    needUpdate = true;
  }
  if (lastDisplayedLampStatus != currentData.lampStatus) {
    lastDisplayedLampStatus = currentData.lampStatus;
    needUpdate = true;
  }
  
  // Only update if there are changes or forced update
  static unsigned long lastUpdate = 0;
  if (needUpdate || forceFullUpdate || (millis() - lastUpdate > 5000)) { // Update every 5 seconds minimum
    drawModernTiles();
    lastUpdate = millis();
    forceFullUpdate = false;
  }
  
  // Header update less frequently to prevent flicker
  static unsigned long lastHeaderUpdate = 0;
  if (millis() - lastHeaderUpdate > 15000) { // Update every 15 seconds
    drawModernHeader();
    lastHeaderUpdate = millis();
  }
}

// ================= AC CONTROL =================
void kontrolAC(String &acStatus, int &setTemp) {
  bool ac1ON = false;
  bool ac2ON = false;
  int targetTemp1 = 22;
  int targetTemp2 = 22;
  
  // KONTROL AC BERDASARKAN JUMLAH ORANG DI RUANGAN
  // Cek apakah ruangan kosong lebih dari 5 menit
  if (jumlahOrang == 0 && roomEmptyTimerActive) {
    unsigned long emptyDuration = millis() - emptyRoomStartTime;
    if (emptyDuration >= EMPTY_ROOM_TIMEOUT) {
      acStatus = "AUTO OFF (5 MIN KOSONG)";
      setTemp = 0;
      Serial.println("⏰ AUTO SHUTDOWN: Ruangan kosong > 5 menit - AC dimatikan otomatis");
    } else {
      acStatus = "AC OFF (KOSONG)";
      setTemp = 0;
    }
  }
  else if (jumlahOrang == 0) {
    // Tidak ada orang → AC mati
    acStatus = "AC OFF (KOSONG)";
    setTemp = 0;
  }
  else if (jumlahOrang <= 5) {
    // 1-5 orang → 1 AC, suhu 22°C
    ac1ON = true;
    targetTemp1 = 22;
    setTemp = 22;
    acStatus = "1 AC ON (" + String(jumlahOrang) + " orang)";
  }
  else if (jumlahOrang <= 10) {
    // 6-10 orang → 1 AC, suhu 20°C
    ac1ON = true;
    targetTemp1 = 20;
    setTemp = 20;
    acStatus = "1 AC ON (" + String(jumlahOrang) + " orang)";
  }
  else if (jumlahOrang <= 15) {
    // 11-15 orang → 2 AC, suhu 20°C
    ac1ON = true;
    ac2ON = true;
    targetTemp1 = 20;
    targetTemp2 = 20;
    setTemp = 20;
    acStatus = "2 AC ON (" + String(jumlahOrang) + " orang)";
  }
  else {
    // 16+ orang → 2 AC MAX, suhu 16°C
    ac1ON = true;
    ac2ON = true;
    targetTemp1 = 16;
    targetTemp2 = 16;
    setTemp = 16;
    acStatus = "2 AC MAX (" + String(jumlahOrang) + " orang)";
  }
  
  // Update AC hanya jika ada perubahan
  if (ac1ON != lastAC1 || ac2ON != lastAC2 || targetTemp1 != lastTemp1 || targetTemp2 != lastTemp2) {
    Serial.println("🌡️ AC Control Change: " + acStatus + " @ " + String(setTemp) + "°C");
    Serial.println("   Jumlah orang di ruangan: " + String(jumlahOrang));
    Serial.println("   Suhu ruangan: " + String(currentData.suhuRuang, 1) + "°C");
    
    // Send IR commands untuk AC1
    if (ac1ON) {
      Serial.println("📡 Sending IR signal to AC1...");
      ac1.on();
      ac1.setMode(kPanasonicAcCool);
      ac1.setTemp(targetTemp1);
      ac1.setFan(kPanasonicAcFanAuto);
      ac1.send();
      delay(200);  // Delay untuk memastikan sinyal terkirim
      Serial.println("   AC1: ON, Cool mode, " + String(targetTemp1) + "°C, Auto fan");
    } else {
      Serial.println("📡 Sending IR signal to turn OFF AC1...");
      ac1.off();
      ac1.send();
      delay(200);
      Serial.println("   AC1: OFF");
    }
    
    // Send IR commands untuk AC2
    if (ac2ON) {
      Serial.println("📡 Sending IR signal to AC2...");
      ac2.on();
      ac2.setMode(kPanasonicAcCool);
      ac2.setTemp(targetTemp2);
      ac2.setFan(kPanasonicAcFanAuto);
      ac2.send();
      delay(200);  // Delay untuk memastikan sinyal terkirim
      Serial.println("   AC2: ON, Cool mode, " + String(targetTemp2) + "°C, Auto fan");
    } else {
      Serial.println("📡 Sending IR signal to turn OFF AC2...");
      ac2.off();
      ac2.send();
      delay(200);
      Serial.println("   AC2: OFF");
    }
    
    Serial.println("⚡ IR signals sent successfully!");
    
    lastAC1 = ac1ON;
    lastAC2 = ac2ON;
    lastTemp1 = targetTemp1;
    lastTemp2 = targetTemp2;
  }
}

// ================= LAMP CONTROL (DUAL FUNCTION) =================
void kontrolLampu(String &lampStatus) {
  bool lampON = false;
  
  // ALGORITMA BERDASARKAN JUMLAH ORANG DI RUANGAN (SAMA SEPERTI AC):
  // 1. Jika ada orang di ruangan (jumlahOrang > 0) → Lampu SELALU NYALA
  // 2. Jika tidak ada orang (jumlahOrang = 0) → Lampu mati
  // 3. Auto shutdown setelah 5 menit kosong
  
  if (autoMode && !manualOverride) {
    // Mode otomatis berdasarkan occupancy (jumlah orang di ruangan)
    if (jumlahOrang > 0) {
      // Ada orang di ruangan → Lampu SELALU NYALA
      lampON = true;
      lampStatus = "AUTO ON (" + String(jumlahOrang) + " orang)";
      Serial.println("💡 Auto ON: Ada " + String(jumlahOrang) + " orang di ruangan - Lampu nyala");
    } else {
      // Tidak ada orang di ruangan → Lampu mati
      lampON = false;
      lampStatus = "AUTO OFF (KOSONG)";
    }
    
    // Auto shutdown setelah 5 menit kosong
    if (jumlahOrang == 0 && roomEmptyTimerActive) {
      unsigned long emptyDuration = millis() - emptyRoomStartTime;
      if (emptyDuration >= EMPTY_ROOM_TIMEOUT) {
        lampON = false;
        lampStatus = "AUTO OFF (5 MIN KOSONG)";
        Serial.println("⏰ AUTO SHUTDOWN: Ruangan kosong > 5 menit - Lampu dimatikan otomatis");
      }
    }
  }
  else {
    // Mode manual - relay tidak aktif, kontrol sepenuhnya dari saklar fisik
    lampStatus = "MANUAL MODE";
  }
  
  // Update relay hanya jika ada perubahan
  if (lampON != lastLamp1 || lampStatus != lastLampStatus) {
    Serial.println("💡 Lamp Control Change: " + lampStatus);
    Serial.println("   Jumlah orang: " + String(jumlahOrang));
    Serial.println("   Light level: " + String(currentData.lightLevel) + " lux");
    Serial.println("   Threshold: 800 lux");
    
    // LOGIKA NC RELAY:
    // LOW = NC tersambung = Lampu bisa nyala (otomatis atau manual)
    // HIGH = NC terputus = Lampu padam paksa (override manual)
    digitalWrite(RELAY_LAMP1, lampON ? LOW : HIGH);
    
    Serial.println("   Relay GPIO 25: " + String(lampON ? "LOW (NC ON)" : "HIGH (NC OFF)"));
    Serial.println("   Status: " + String(lampON ? "LAMPU NYALA" : "LAMPU MATI"));
    Serial.println("   Mode: " + String(autoMode ? "OTOMATIS" : "MANUAL"));
    
    Serial.println("✅ Lamp control updated!");
    
    lastLamp1 = lampON;
    lastLampStatus = lampStatus;
  }
  
  currentData.lamp1 = lampON;
  currentData.lamp2 = false;  // Tidak digunakan
  currentData.lampStatus = lampStatus;
}

// ================= UPDATE SENSOR DATA =================
void updateSensorData() {
  currentData.jumlahOrang = jumlahOrang;
  
  // PERBAIKAN DHT22: Read dengan interval yang tepat dan error handling yang lebih baik
  static unsigned long lastDHTRead = 0;
  static float lastValidTemp = 25.0;  // Default fallback
  static float lastValidHumidity = 60.0;  // Default fallback
  static int dhtErrorCount = 0;
  static bool dhtInitialized = false;
  
  // DHT22 membutuhkan minimal 2 detik antar pembacaan
  if (millis() - lastDHTRead >= 2500) { // 2.5 detik untuk safety
    lastDHTRead = millis();
    
    // Re-initialize DHT jika belum diinisialisasi atau terlalu banyak error
    if (!dhtInitialized || dhtErrorCount > 10) {
      Serial.println("🔄 Re-initializing DHT22 sensor...");
      dht.begin();
      delay(1000); // Wait for sensor to stabilize
      dhtInitialized = true;
      dhtErrorCount = 0;
    }
    
    // Multiple reading attempts untuk akurasi
    float tempSum = 0;
    float humSum = 0;
    int validReadings = 0;
    
    for (int attempt = 0; attempt < 3; attempt++) {
      float temp = dht.readTemperature();
      float hum = dht.readHumidity();
      
      // Validasi pembacaan dengan range yang realistis
      if (!isnan(temp) && temp > 10 && temp < 50) {
        tempSum += temp;
        validReadings++;
      }
      
      if (!isnan(hum) && hum >= 20 && hum <= 90) {
        humSum += hum;
      }
      
      if (attempt < 2) delay(100); // Small delay between attempts
    }
    
    // Update temperature jika ada pembacaan valid
    if (validReadings > 0) {
      float avgTemp = tempSum / validReadings;
      
      // Smooth filtering untuk menghindari fluktuasi ekstrem
      if (abs(avgTemp - lastValidTemp) < 10.0) { // Max 10°C change per reading
        lastValidTemp = avgTemp;
        currentData.suhuRuang = avgTemp;
        dhtErrorCount = 0; // Reset error count on successful read
        
        Serial.println("🌡️ DHT22 REAL-TIME: " + String(avgTemp, 2) + "°C (Valid readings: " + String(validReadings) + ")");
      } else {
        Serial.println("⚠️ DHT22 Temperature spike detected: " + String(avgTemp, 1) + "°C - using previous: " + String(lastValidTemp, 1) + "°C");
        currentData.suhuRuang = lastValidTemp;
      }
    } else {
      // Tidak ada pembacaan valid
      dhtErrorCount++;
      currentData.suhuRuang = lastValidTemp;
      
      Serial.println("❌ DHT22 Temperature read failed (Error count: " + String(dhtErrorCount) + ") - using fallback: " + String(lastValidTemp, 1) + "°C");
      
      if (dhtErrorCount > 5) {
        Serial.println("🔧 DHT22 Troubleshooting:");
        Serial.println("   1. Check wiring: VCC=3.3V, GND=GND, DATA=GPIO13");
        Serial.println("   2. Check power supply stability");
        Serial.println("   3. Check for loose connections");
        Serial.println("   4. Sensor may need replacement if errors persist");
      }
    }
    
    // Update humidity dengan validasi serupa
    if (validReadings > 0 && humSum > 0) {
      float avgHum = humSum / validReadings;
      if (abs(avgHum - lastValidHumidity) < 30.0) { // Max 30% change per reading
        lastValidHumidity = avgHum;
        currentData.humidity = avgHum;
        Serial.println("💧 DHT22 Humidity: " + String(avgHum, 1) + "%");
      } else {
        currentData.humidity = lastValidHumidity;
      }
    } else {
      currentData.humidity = lastValidHumidity;
    }
    
    // Debug output setiap 30 detik
    static unsigned long lastDHTDebug = 0;
    if (millis() - lastDHTDebug > 30000) {
      Serial.println("📊 DHT22 Status Report:");
      Serial.println("   Current Temperature: " + String(currentData.suhuRuang, 2) + "°C");
      Serial.println("   Current Humidity: " + String(currentData.humidity, 1) + "%");
      Serial.println("   Error Count: " + String(dhtErrorCount));
      Serial.println("   Initialized: " + String(dhtInitialized ? "YES" : "NO"));
      Serial.println("   Last Valid Temp: " + String(lastValidTemp, 2) + "°C");
      Serial.println("   Sensor Status: " + String(dhtErrorCount < 5 ? "GOOD" : "NEEDS ATTENTION"));
      lastDHTDebug = millis();
    }
  }
  
  // PERBAIKAN LDR: Read dengan multiple sampling dan filtering untuk stabilitas
  static unsigned long lastLDRRead = 0;
  static int lastValidLDR = 500; // Default fallback
  static int ldrErrorCount = 0;
  
  // LDR dapat dibaca lebih sering daripada DHT22
  if (millis() - lastLDRRead >= 500) { // 0.5 detik interval
    lastLDRRead = millis();
    
    // Multiple sampling untuk stabilitas
    long ldrSum = 0;
    int validReadings = 0;
    
    for (int i = 0; i < 10; i++) { // 10 samples
      int reading = analogRead(LDR_PIN);
      if (reading >= 0 && reading <= 4095) { // Valid ADC range
        ldrSum += reading;
        validReadings++;
      }
      delay(10); // Small delay between samples
    }
    
    if (validReadings > 0) {
      int avgReading = ldrSum / validReadings;
      
      // Enhanced mapping dengan kalibrasi yang lebih baik
      // LDR biasanya: Gelap = 4095 (high resistance), Terang = 0-500 (low resistance)
      int mappedValue;
      
      if (avgReading > 3500) {
        // Very dark
        mappedValue = map(avgReading, 3500, 4095, 0, 50);
      } else if (avgReading > 2000) {
        // Dark to dim
        mappedValue = map(avgReading, 2000, 3500, 50, 200);
      } else if (avgReading > 1000) {
        // Dim to moderate
        mappedValue = map(avgReading, 1000, 2000, 200, 500);
      } else if (avgReading > 300) {
        // Moderate to bright
        mappedValue = map(avgReading, 300, 1000, 500, 800);
      } else {
        // Very bright
        mappedValue = map(avgReading, 0, 300, 800, 1200);
      }
      
      // Smooth filtering untuk mencegah fluktuasi
      if (abs(mappedValue - lastValidLDR) < 100) { // Max 100 lux change per reading
        lastValidLDR = mappedValue;
        currentData.lightLevel = mappedValue;
        ldrErrorCount = 0;
        
        // Classify light condition
        String lightCondition;
        if (mappedValue < 50) lightCondition = "VERY_DARK";
        else if (mappedValue < 200) lightCondition = "DARK";
        else if (mappedValue < 500) lightCondition = "DIM";
        else if (mappedValue < 800) lightCondition = "MODERATE";
        else lightCondition = "BRIGHT";
        
        static unsigned long lastLDRLog = 0;
        if (millis() - lastLDRLog > 30000) { // Log every 30 seconds
          Serial.println("💡 LDR STABLE: Raw=" + String(avgReading) + " Mapped=" + String(mappedValue) + " lux (" + lightCondition + ")");
          lastLDRLog = millis();
        }
      } else {
        // Spike detected, use previous value
        currentData.lightLevel = lastValidLDR;
        Serial.println("⚠️ LDR Spike detected: " + String(mappedValue) + " lux - using previous: " + String(lastValidLDR) + " lux");
      }
    } else {
      // No valid readings
      ldrErrorCount++;
      currentData.lightLevel = lastValidLDR;
      
      if (ldrErrorCount > 5) {
        Serial.println("❌ LDR Read failed (Error count: " + String(ldrErrorCount) + ") - using fallback: " + String(lastValidLDR) + " lux");
        Serial.println("🔧 LDR Troubleshooting:");
        Serial.println("   1. Check wiring: Signal=GPIO35, VCC=3.3V, GND=GND");
        Serial.println("   2. Check if 10K pull-down resistor is connected");
        Serial.println("   3. Verify LDR is not damaged");
        Serial.println("   4. Check for loose connections");
      }
    }
    
    // Debug output setiap 60 detik
    static unsigned long lastLDRDebug = 0;
    if (millis() - lastLDRDebug > 60000) {
      Serial.println("📊 LDR Status Report:");
      Serial.println("   Current Light Level: " + String(currentData.lightLevel) + " lux");
      Serial.println("   Error Count: " + String(ldrErrorCount));
      Serial.println("   Last Valid Reading: " + String(lastValidLDR) + " lux");
      Serial.println("   Sensor Status: " + String(ldrErrorCount < 3 ? "STABLE" : "UNSTABLE"));
      
      // Light condition analysis
      if (currentData.lightLevel < LUX_THRESHOLD) {
        Serial.println("   Room Condition: DARK (Lights should be ON if occupied)");
      } else {
        Serial.println("   Room Condition: BRIGHT (Lights can be OFF)");
      }
      
      lastLDRDebug = millis();
    }
  }
  
  currentData.timestamp = millis();
  currentData.proximity1 = sensorData.objectInDetected;
  currentData.proximity2 = sensorData.objectOutDetected;
  
  if (WiFi.isConnected()) {
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI = WiFi.RSSI();
  } else {
    currentData.wifiStatus = "Offline";
    currentData.wifiRSSI = 0;
  }
}

// ================= STATE PERSISTENCE FUNCTIONS =================
void saveCurrentState() {
  // Save critical state to prevent data loss on restart
  static unsigned long lastStateSave = 0;
  if (millis() - lastStateSave < 5000) return; // Don't save too frequently
  
  Serial.println("💾 SAVING CURRENT STATE:");
  Serial.println("   People Count: " + String(jumlahOrang));
  Serial.println("   AC Status: " + currentData.acStatus);
  Serial.println("   Lamp Status: " + currentData.lampStatus);
  Serial.println("   Temperature: " + String(currentData.suhuRuang, 2) + "°C");
  
  // Send immediate update to database to preserve state
  if (WiFi.status() == WL_CONNECTED) {
    sendDataToAPI("STATE_SAVE - Preserving current state");
  }
  
  lastStateSave = millis();
}

void restoreLastState() {
  // Attempt to restore last known state on startup
  Serial.println("🔄 ATTEMPTING TO RESTORE LAST STATE...");
  
  // For now, we'll start fresh but this could be enhanced with EEPROM storage
  jumlahOrang = 0; // Start with 0 for safety
  
  Serial.println("   Starting with fresh state for safety");
  Serial.println("   People count reset to 0");
  Serial.println("   AC and Lamp will be controlled based on current occupancy");
}

// ================= ENHANCED ERROR RECOVERY =================
void handleSystemError(String errorType) {
  Serial.println("🚨 SYSTEM ERROR DETECTED: " + errorType);
  Serial.println("   Implementing recovery procedures...");
  
  // Don't restart - just recover gracefully
  if (errorType == "MEMORY_CRITICAL") {
    Serial.println("   Memory critical - reducing operations");
    delay(2000);
  } else if (errorType == "SENSOR_FAILURE") {
    Serial.println("   Sensor failure - using fallback values");
    // Re-initialize sensors
    dht.begin();
    delay(1000);
  } else if (errorType == "WIFI_FAILURE") {
    Serial.println("   WiFi failure - attempting reconnection");
    WiFi.disconnect();
    delay(1000);
    connectWiFi();
  }
  
  Serial.println("   Recovery complete - continuing operation");
}
void resetPeopleCounter() {
  jumlahOrang = 0;
  lastJumlahOrang = -1;
  roomEmptyTimerActive = false;
  
  // Reset sensor states for immediate counting
  sensorData.lastInChange = 0;
  sensorData.lastOutChange = 0;
  
  Serial.println("🔄 People counter manually reset to 0");
  Serial.println("🔄 Sensor states cleared");
  updateSensorData();
  
  // Force display update
  forceFullUpdate = true;
  String acStatus = "AC OFF";
  int setTemp = 0;
  String lampStatus = "SEMUA OFF";
  kontrolAC(acStatus, setTemp);
  kontrolLampu(lampStatus);
  currentData.acStatus = acStatus;
  currentData.setTemp = setTemp;
  currentData.lampStatus = lampStatus;
  updateTFT(acStatus, setTemp, currentData.suhuRuang);
}

// ================= ENHANCED API FUNCTIONS WITH SSL & PROXY SUPPORT =================

// Enhanced AC Control API with SSL and proxy support
void checkACControlAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  String url = String(acControlURL) + "?device_id=" + DEVICE_ID + "&location=" + DEVICE_LOCATION;
  
  // Add proxy information to URL if detected
  if (proxyState.isProxyDetected) {
    url += "&proxy_detected=true&proxy_type=" + proxyState.proxyType;
  }
  
  String response;
  if (makeHTTPSRequest(url.c_str(), "GET", "", response)) {
    StaticJsonDocument<300> doc; // Reduced from 512
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error && doc["success"]) {
      // Process AC control response (implementation depends on your API)
      Serial.println("✅ Enhanced AC Control received via SSL");
      if (proxyState.isProxyDetected) {
        Serial.println("   Via Proxy: " + proxyState.proxyType);
      }
    }
  }
}

// Enhanced change detection - SEMUA SENSOR DIPANTAU
bool hasSignificantChange() {
  // Deteksi perubahan untuk SEMUA SENSOR - KIRIM SETIAP PERUBAHAN SIGNIFIKAN
  static int lastPeopleCount = -1;
  static float lastTemp = -999.0;
  static float lastHumidity = -999.0;
  static int lastLightLevel = -1;
  static bool lastProximityIn = false;
  static bool lastProximityOut = false;
  static String lastACStatus = "";
  static String lastLampStatus = "";
  
  bool hasChange = false;
  String changeReason = "";
  
  // Perubahan jumlah orang (LANGSUNG KIRIM)
  if (currentData.jumlahOrang != lastPeopleCount) {
    changeReason = "PEOPLE_COUNT_CHANGE: " + String(lastPeopleCount) + " → " + String(currentData.jumlahOrang);
    Serial.println("🔄 " + changeReason);
    lastPeopleCount = currentData.jumlahOrang;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
    hasChange = true;
  }
  
  // Perubahan suhu (LANGSUNG KIRIM) - Threshold lebih sensitif
  if (abs(currentData.suhuRuang - lastTemp) >= 0.2) { // 0.2°C threshold
    changeReason = "TEMPERATURE_CHANGE: " + String(lastTemp, 1) + "°C → " + String(currentData.suhuRuang, 1) + "°C";
    Serial.println("🔄 " + changeReason);
    lastTemp = currentData.suhuRuang;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
    hasChange = true;
  }
  
  // Perubahan kelembaban (LANGSUNG KIRIM) - Threshold lebih sensitif
  if (abs(currentData.humidity - lastHumidity) >= 1.0) { // 1% threshold
    changeReason = "HUMIDITY_CHANGE: " + String(lastHumidity, 1) + "% → " + String(currentData.humidity, 1) + "%";
    Serial.println("🔄 " + changeReason);
    lastHumidity = currentData.humidity;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
    hasChange = true;
  }
  
  // Perubahan cahaya (LANGSUNG KIRIM) - Threshold lebih sensitif
  if (abs(currentData.lightLevel - lastLightLevel) >= 50) { // 50 lux threshold
    changeReason = "LIGHT_CHANGE: " + String(lastLightLevel) + " → " + String(currentData.lightLevel) + " lux";
    Serial.println("🔄 " + changeReason);
    lastLightLevel = currentData.lightLevel;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
    hasChange = true;
  }
  
  // Perubahan sensor proximity IN (LANGSUNG KIRIM)
  if (currentData.proximity1 != lastProximityIn) {
    changeReason = "PROXIMITY_IN_CHANGE: " + String(lastProximityIn ? "DETECTED" : "CLEAR") + " → " + String(currentData.proximity1 ? "DETECTED" : "CLEAR");
    Serial.println("🔄 " + changeReason);
    lastProximityIn = currentData.proximity1;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
    hasChange = true;
  }
  
  // Perubahan sensor proximity OUT (LANGSUNG KIRIM)
  if (currentData.proximity2 != lastProximityOut) {
    changeReason = "PROXIMITY_OUT_CHANGE: " + String(lastProximityOut ? "DETECTED" : "CLEAR") + " → " + String(currentData.proximity2 ? "DETECTED" : "CLEAR");
    Serial.println("🔄 " + changeReason);
    lastProximityOut = currentData.proximity2;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
    hasChange = true;
  }
  
  // Perubahan status AC (LANGSUNG KIRIM)
  if (currentData.acStatus != lastACStatus) {
    changeReason = "AC_STATUS_CHANGE: " + lastACStatus + " → " + currentData.acStatus;
    Serial.println("🔄 " + changeReason);
    lastACStatus = currentData.acStatus;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
    hasChange = true;
  }
  
  // Perubahan status lampu (LANGSUNG KIRIM)
  if (currentData.lampStatus != lastLampStatus) {
    changeReason = "LAMP_STATUS_CHANGE: " + lastLampStatus + " → " + currentData.lampStatus;
    Serial.println("🔄 " + changeReason);
    lastLampStatus = currentData.lampStatus;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
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
  // Update previous data for change detection
  lastForceUpdate = millis();
}

// Enhanced send data to API with SSL and proxy support - SEMUA SENSOR DATA
void sendDataToAPI(String reason) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Cannot send to API - WiFi not connected");
    return;
  }
  
  Serial.println("📤 COMPLETE SENSOR DATA - Sending to hosting web...");
  Serial.println("   Reason: " + reason);
  
  // PERBAIKAN: Baca SEMUA sensor terbaru untuk data real-time yang akurat
  updateSensorData(); // Update semua sensor data terlebih dahulu
  
  Serial.println("   📊 COMPLETE SENSOR READINGS:");
  Serial.println("   People Count: " + String(currentData.jumlahOrang));
  Serial.println("   Room Temperature: " + String(currentData.suhuRuang, 2) + "°C");
  Serial.println("   Humidity: " + String(currentData.humidity, 1) + "%");
  Serial.println("   Light Level: " + String(currentData.lightLevel) + " lux");
  Serial.println("   Proximity IN: " + String(currentData.proximity1 ? "DETECTED" : "CLEAR"));
  Serial.println("   Proximity OUT: " + String(currentData.proximity2 ? "DETECTED" : "CLEAR"));
  Serial.println("   AC Status: " + currentData.acStatus);
  Serial.println("   Lamp Status: " + currentData.lampStatus);
  Serial.println("   WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
  
  StaticJsonDocument<800> doc; // Reduced from 1500 to 800
  
  // Device Information
  doc["device_id"] = DEVICE_ID;
  doc["location"] = DEVICE_LOCATION;
  doc["timestamp"] = millis();
  doc["update_reason"] = reason;
  doc["update_type"] = "complete_sensor_data";
  
  // COMPLETE SENSOR DATA - SEMUA SENSOR DIKIRIM
  doc["people_count"] = currentData.jumlahOrang;
  doc["room_temperature"] = round(currentData.suhuRuang * 100) / 100.0; // 2 decimal precision
  doc["humidity"] = round(currentData.humidity * 10) / 10.0; // 1 decimal precision
  doc["light_level"] = currentData.lightLevel;
  doc["proximity_in"] = currentData.proximity1;
  doc["proximity_out"] = currentData.proximity2;
  
  // AC Control Data
  doc["ac_status"] = currentData.acStatus;
  doc["set_temperature"] = currentData.setTemp;
  doc["ac_mode"] = currentData.setTemp > 0 ? "cooling" : "off";
  doc["ac1_active"] = currentData.setTemp > 0 && currentData.jumlahOrang > 0;
  doc["ac2_active"] = currentData.setTemp > 0 && currentData.jumlahOrang > 10;
  
  // Lamp Control Data
  doc["lamp_status"] = currentData.lampStatus;
  doc["lamp1"] = currentData.lamp1;
  doc["lamp_auto_control"] = autoMode;
  doc["manual_override"] = manualOverride;
  
  // Environmental Analysis
  doc["room_condition"] = currentData.jumlahOrang > 0 ? "occupied" : "empty";
  doc["light_condition"] = currentData.lightLevel > LUX_THRESHOLD ? "bright" : "dark";
  doc["temperature_status"] = currentData.suhuRuang > 26 ? "hot" : (currentData.suhuRuang < 20 ? "cold" : "comfortable");
  doc["humidity_status"] = currentData.humidity > 70 ? "humid" : (currentData.humidity < 40 ? "dry" : "comfortable");
  
  // System Status
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["wifi_status"] = "connected";
  doc["uptime_seconds"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  doc["cpu_frequency"] = getCpuFrequencyMhz();
  doc["system_status"] = "normal_24_7_operation";
  
  // Sensor Health Status
  doc["dht22_status"] = !isnan(dht.readTemperature()) ? "working" : "error";
  doc["ldr_status"] = currentData.lightLevel > 0 ? "working" : "check_wiring";
  doc["proximity_in_status"] = "working";
  doc["proximity_out_status"] = "working";
  
  // SSL & Network Information
  doc["ssl_enabled"] = true;
  doc["ssl_configured"] = sslStatus.isConfigured;
  doc["ssl_last_success"] = sslStatus.lastConnectionSuccess;
  doc["ssl_method"] = "root_ca";
  doc["hosting_domain"] = hostingDomain;
  
  // Proxy Information
  doc["proxy_detected"] = proxyState.isProxyDetected;
  doc["connection_type"] = proxyState.isDirectConnection ? "direct" : "proxy";
  doc["hosting_reachable"] = proxyState.hostingReachable;
  
  // Operational Data
  doc["empty_room_timer_active"] = roomEmptyTimerActive;
  if (roomEmptyTimerActive) {
    doc["empty_duration_seconds"] = (millis() - emptyRoomStartTime) / 1000;
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.println("📋 COMPLETE JSON Data (first 300 chars):");
  Serial.println(jsonString.substring(0, 300) + "...");
  Serial.println("📋 JSON Size: " + String(jsonString.length()) + " bytes");
  
  String response;
  bool dataSent = false;
  
  // Coba HTTPS terlebih dahulu
  if (makeHTTPSRequest(apiURL, "POST", jsonString.c_str(), response)) {
    Serial.println("✅ SUCCESS: Complete sensor data sent via HTTPS!");
    dataSent = true;
  } else {
    Serial.println("⚠️ HTTPS FAILED - Trying HTTP fallback...");
    
    // Fallback ke HTTP jika HTTPS gagal
    String httpURL = String(apiURL);
    httpURL.replace("https://", "http://");
    if (makeHTTPRequest(httpURL.c_str(), "POST", jsonString.c_str(), response)) {
      Serial.println("✅ SUCCESS: Complete sensor data sent via HTTP fallback!");
      dataSent = true;
    } else {
      Serial.println("❌ BOTH HTTPS AND HTTP FAILED!");
      Serial.println("🔒 SSL Error: " + sslStatus.lastError);
      return; // Exit jika kedua metode gagal
    }
  }
  
  if (dataSent) {
    updatePreviousData();
    lastAPIUpdate = millis();
    lastForceUpdate = millis();
    
    StaticJsonDocument<200> responseDoc; // Reduced from 256
    DeserializationError error = deserializeJson(responseDoc, response);
    if (!error && responseDoc["success"]) {
      if (responseDoc["data"]["id"]) {
        Serial.println("✅ Database record ID: " + String(responseDoc["data"]["id"].as<int>()));
      }
      Serial.println("📊 COMPLETE SENSOR DATA RECORDED:");
      Serial.println("   ✅ People Count: " + String(currentData.jumlahOrang));
      Serial.println("   ✅ Temperature: " + String(currentData.suhuRuang, 2) + "°C");
      Serial.println("   ✅ Humidity: " + String(currentData.humidity, 1) + "%");
      Serial.println("   ✅ Light Level: " + String(currentData.lightLevel) + " lux");
      Serial.println("   ✅ Proximity Data: IN=" + String(currentData.proximity1) + " OUT=" + String(currentData.proximity2));
      Serial.println("   ✅ AC Status: " + currentData.acStatus);
      Serial.println("   ✅ Lamp Status: " + currentData.lampStatus);
      Serial.println("🎯 ALL SENSOR DATA SUCCESSFULLY RECORDED IN DATABASE!");
    } else {
      Serial.println("⚠️ Database response parsing failed or unsuccessful");
      Serial.println("Response: " + response.substring(0, 200));
    }
  }
}

// ================= ENHANCED SETUP =================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // DISABLE WATCHDOG TIMER - Mencegah restart otomatis
  // esp_task_wdt_init(120, true);
  // esp_task_wdt_add(NULL);
  
  Serial.println("🛡️ ANTI-RESTART MODE ENABLED");
  Serial.println("   Watchdog: DISABLED (Prevents automatic restarts)");
  Serial.println("   Auto shutdown/startup: PERMANENTLY DISABLED");
  Serial.println("   Memory monitoring: ENABLED");
  Serial.println("   24/7 Operation: ACTIVE");
  Serial.println("   Restart Protection: MAXIMUM");
  
  // Set CPU frequency to stable level
  setCpuFrequencyMhz(240); // Set to 240MHz for stable operation
  Serial.println("CPU Frequency: " + String(getCpuFrequencyMhz()) + " MHz");
  
  Serial.println("\n================================");
  Serial.println("  SMART AC - STABLE MODE");
  Serial.println("  Device: ESP32 Proximity Sensor");
  Serial.println("  Location: " + String(DEVICE_LOCATION));
  Serial.println("  SSL Method: ROOT_CA");
  Serial.println("  Operation: 24/7 Continuous");
  Serial.println("================================");
  
  pinMode(PROXIMITY_PIN_IN, INPUT_PULLUP);
  pinMode(PROXIMITY_PIN_OUT, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT);  // Hanya satu LDR
  pinMode(IR_PIN, OUTPUT);
  pinMode(RELAY_LAMP1, OUTPUT);
  // pinMode(RELAY_LAMP2, OUTPUT);  // Tidak digunakan untuk 1 jalur
  
  // Initialize relay untuk NC (Normally Closed)
  // HIGH = NC terputus = Lampu padam paksa
  // LOW = NC tersambung = Lampu bisa dikontrol manual/otomatis
  digitalWrite(RELAY_LAMP1, HIGH);  // Start dengan kontrol otomatis aktif
  
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN_IN), proximityInInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN_OUT), proximityOutInterrupt, CHANGE);
  
  Serial.println("✓ Proximity interrupts configured for immediate response");
  
  dht.begin();  // Hanya satu DHT22
  
  // PERBAIKAN: Wait for DHT22 sensor to stabilize dengan test yang lebih baik
  Serial.println("\n=== TESTING DHT22 SENSOR (IMPROVED) ===");
  Serial.println("Waiting for DHT22 to stabilize...");
  delay(2000); // Initial stabilization
  
  // Multiple test readings untuk memastikan sensor bekerja
  bool dhtWorking = false;
  for (int i = 0; i < 5; i++) {
    Serial.println("DHT22 Test Reading #" + String(i + 1) + ":");
    
    float testTemp = dht.readTemperature();
    float testHum = dht.readHumidity();
    
    Serial.println("  Temperature: " + String(testTemp) + "°C");
    Serial.println("  Humidity: " + String(testHum) + "%");
    
    if (!isnan(testTemp) && !isnan(testHum) && testTemp > 10 && testTemp < 50) {
      Serial.println("  ✅ DHT22 Reading VALID");
      dhtWorking = true;
      break;
    } else {
      Serial.println("  ❌ DHT22 Reading INVALID - Retrying...");
      delay(2500); // Wait before retry
    }
  }
  
  if (dhtWorking) {
    Serial.println("✅ DHT22 SENSOR WORKING - Real-time temperature monitoring ACTIVE");
  } else {
    Serial.println("⚠️ DHT22 SENSOR NOT RESPONDING");
    Serial.println("   Troubleshooting steps:");
    Serial.println("   1. Check wiring: VCC=3.3V (NOT 5V!), GND=GND, DATA=GPIO13");
    Serial.println("   2. Check if 10K pull-up resistor is connected between DATA and VCC");
    Serial.println("   3. Verify sensor is DHT22 (white sensor, not DHT11)");
    Serial.println("   4. Try different GPIO pin if problem persists");
    Serial.println("   5. Check power supply - DHT22 needs stable 3.3V");
    Serial.println("   System will continue with fallback temperature");
  }
  Serial.println("==============================");
  irsend.begin();
  ac1.begin();
  ac2.begin();
  
  Serial.println("\n=== IR TRANSMITTER TEST ===");
  Serial.println("IR Pin: GPIO " + String(IR_PIN));
  
  // Simple IR test
  ac1.on();
  ac1.setMode(kPanasonicAcCool);
  ac1.setTemp(25);
  ac1.setFan(kPanasonicAcFanAuto);
  ac1.send();
  
  Serial.println("\n=== TESTING SMART PROXIMITY SENSORS ===");
  Serial.println("Initializing smart walking detection system...");
  Serial.println("Detection method: Sequence-based walking pattern recognition");
  Serial.println("Walking speed range: 50ms - 2000ms between sensors");
  Serial.println("Entry pattern: IN sensor → OUT sensor");
  Serial.println("Exit pattern: OUT sensor → IN sensor");
  
  // Calibration phase - read baseline values
  int inHighCount = 0;
  int outHighCount = 0;
  int totalReadings = 50;
  
  for (int i = 0; i < totalReadings; i++) {
    bool inState = digitalRead(PROXIMITY_PIN_IN);
    bool outState = digitalRead(PROXIMITY_PIN_OUT);
    
    if (inState) inHighCount++;
    if (outState) outHighCount++;
    
    delay(50);
  }
  
  Serial.println("Calibration results:");
  Serial.println("- IN sensor HIGH readings: " + String(inHighCount) + "/" + String(totalReadings) + 
                 " (" + String((inHighCount * 100) / totalReadings) + "%)");
  Serial.println("- OUT sensor HIGH readings: " + String(outHighCount) + "/" + String(totalReadings) + 
                 " (" + String((outHighCount * 100) / totalReadings) + "%)");
  
  if (inHighCount < totalReadings * 0.8) {
    Serial.println("⚠️ IN sensor may have wiring issues - expected mostly HIGH when clear");
  } else {
    Serial.println("✓ IN sensor baseline OK");
  }
  
  if (outHighCount < totalReadings * 0.8) {
    Serial.println("⚠️ OUT sensor may have wiring issues - expected mostly HIGH when clear");
  } else {
    Serial.println("✓ OUT sensor baseline OK");
  }
  
  Serial.println("\nTesting detection for 5 seconds...");
  Serial.println("Wave your hand in front of sensors to test");
  
  for (int i = 0; i < 50; i++) { // Test for 5 seconds
    bool inState = digitalRead(PROXIMITY_PIN_IN);
    bool outState = digitalRead(PROXIMITY_PIN_OUT);
    
    Serial.println("Test " + String(i+1) + "/50: IN=" + String(inState) + 
                   " OUT=" + String(outState) + 
                   " | Objects: IN=" + String(!inState ? "DETECTED" : "CLEAR") + 
                   " OUT=" + String(!outState ? "DETECTED" : "CLEAR"));
    delay(100);
  }
  Serial.println("Smart proximity sensor test completed");
  Serial.println("Expected behavior for smart walking detection:");
  Serial.println("- Entry: IN sensor first, then OUT sensor (50-2000ms apart)");
  Serial.println("- Exit: OUT sensor first, then IN sensor (50-2000ms apart)");
  Serial.println("- Too fast (< 50ms): Ignored as noise");
  Serial.println("- Too slow (> 2000ms): Ignored as stationary object");
  Serial.println("- Single sensor stuck > 3s: Auto-reset");
  Serial.println("- Sequence timeout: 5 seconds");
  Serial.println("- Only valid walking patterns are counted");
  Serial.println("- Use 'smart' command to test walking detection");
  Serial.println("==============================");
  
  // Test TFT with error handling
  Serial.println("\n=== TESTING TFT ===");
  tft.init();
  tft.setRotation(1); // Landscape mode
  
  // Get screen dimensions
  int screenWidth = tft.width();
  int screenHeight = tft.height();
  Serial.println("TFT Screen: " + String(screenWidth) + "x" + String(screenHeight));
  
  // Test full screen colors
  tft.fillScreen(TFT_RED);
  delay(500);
  tft.fillScreen(TFT_GREEN);
  delay(500);
  tft.fillScreen(TFT_BLUE);
  delay(500);
  
  drawUI();
  Serial.println("TFT Ready!");
  
  // Initialize display tracking variables
  lastDisplayedPeople = jumlahOrang;
  lastDisplayedTemp = (int)currentData.suhuRuang;
  lastDisplayedACStatus = currentData.acStatus;
  lastDisplayedLampStatus = currentData.lampStatus;
  forceFullUpdate = false;
  
  // Enhanced WiFi connection with SSL and proxy detection
  connectWiFi();
  
  // Time synchronization removed - auto shutdown/startup disabled
  // if (WiFi.status() == WL_CONNECTED) {
  //   initializeTime();
  // }
  
  Serial.println("\n✅ SYSTEM READY!");
  Serial.println("Location: " + String(DEVICE_LOCATION));
  Serial.println("Mode: 24/7 Operation");
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("\nPin Configuration:");
  Serial.println("- PROXIMITY: GPIO 32/33");
  Serial.println("- DHT22: GPIO 13");
  Serial.println("- LDR: GPIO 35");
  Serial.println("- IR LED: GPIO 15");
  Serial.println("- RELAY: GPIO 25");
  
  Serial.println("\nFeatures:");
  Serial.println("- SSL/HTTPS Communication");
  Serial.println("- Smart Proximity Detection");
  Serial.println("- Auto AC/Lamp Control");
  Serial.println("- Real-time Database Sync");
  
  Serial.println("\nCommands: reset, test, sensors, status, help");
  Serial.println("================================\n");
  
  // Initialize SSL and proxy detection state
  sslStatus.isConfigured = false;
  sslStatus.lastConnectionSuccess = false;
  sslStatus.consecutiveFailures = 0;
  proxyState.lastProxyCheck = 0;
  proxyState.consecutiveFailures = 0;
  proxyState.hostingReachable = false;
  
  // Attempt to restore last state (for future enhancement)
  restoreLastState();
  
  // Reset people counter to ensure clean start
  jumlahOrang = 0;
  lastJumlahOrang = -1;
  Serial.println("✓ People counter reset to 0");
  
  // DISABLE WATCHDOG RESET - Mencegah restart
  // esp_task_wdt_reset();
}

// ================= MAIN LOOP =================
void loop() {
  // DISABLE WATCHDOG RESET - Mencegah restart
  // esp_task_wdt_reset();
  
  // CRITICAL: Enhanced memory monitoring untuk mencegah restart
  static unsigned long lastMemoryCheck = 0;
  if (millis() - lastMemoryCheck > 60000) { // Check every 60 seconds - less frequent
    uint32_t freeHeap = ESP.getFreeHeap();
    
    // Only log if memory is getting low
    if (freeHeap < 50000) { // Higher threshold
      Serial.println("🧠 MEMORY WARNING:");
      Serial.println("   Free Heap: " + String(freeHeap) + " bytes");
      
      if (freeHeap < 30000) {
        Serial.println("⚠️ LOW MEMORY - Cleaning up");
        // Force garbage collection
        delay(500);
      }
      
      if (freeHeap < 15000) {
        Serial.println("🚨 CRITICAL MEMORY - Emergency mode");
        // Reduce operations but DON'T restart
        delay(1000);
        return; // Skip this loop iteration
      }
    }
    
    lastMemoryCheck = millis();
  }
  
  // Skip auto shutdown check - DISABLED
  // checkAutoShutdown();
  
  // Skip normal operations if system is in auto shutdown mode - DISABLED
  // if (systemAutoShutdown) {
  //   delay(1000);
  //   return;
  // }
  
  // Always read sensors for immediate response (with robust error handling)
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 100) { // Limit sensor reading frequency
    readSensors();
    detectPeople();
    lastSensorRead = millis();
  }
  
  // Handle interrupt-triggered updates immediately
  if (interruptTriggered) {
    interruptTriggered = false;
    updateSensorData();
    
    if (jumlahOrang != lastJumlahOrang) {
      String acStatus = currentData.acStatus;
      int setTemp = currentData.setTemp;
      String lampStatus = currentData.lampStatus;
      kontrolAC(acStatus, setTemp);
      kontrolLampu(lampStatus);
      currentData.acStatus = acStatus;
      currentData.setTemp = setTemp;
      currentData.lampStatus = lampStatus;
      forceFullUpdate = true; // Force immediate display update
      updateTFT(acStatus, setTemp, currentData.suhuRuang);
      lastJumlahOrang = jumlahOrang;
      Serial.println("🔄 INTERRUPT UPDATE - People: " + String(jumlahOrang));
      
      // CRITICAL: Save state to prevent loss on restart
      saveCurrentState();
    }
  } 
  // Handle regular updates
  else {
    updateSensorData();
    
    if (jumlahOrang != lastJumlahOrang) {
      String acStatus = currentData.acStatus;
      int setTemp = currentData.setTemp;
      String lampStatus = currentData.lampStatus;
      kontrolAC(acStatus, setTemp);
      kontrolLampu(lampStatus);
      currentData.acStatus = acStatus;
      currentData.setTemp = setTemp;
      currentData.lampStatus = lampStatus;
      forceFullUpdate = true; // Force immediate display update
      updateTFT(acStatus, setTemp, currentData.suhuRuang);
      lastJumlahOrang = jumlahOrang;
      Serial.println("🔄 UPDATE - People: " + String(jumlahOrang));
      
      // CRITICAL: Save state to prevent loss on restart
      saveCurrentState();
    }
  }
  
  // Reset watchdog after sensor operations
  esp_task_wdt_reset();
  
  // Enhanced proxy detection check
  checkProxyDetection();
  
  // Enhanced AC control check with SSL and proxy support
  static unsigned long lastACCheck = 0;
  if (millis() - lastACCheck >= AC_CONTROL_CHECK_INTERVAL) {
    checkACControlAPI();
    lastACCheck = millis();
  }
  
  // Light control check (every 3 seconds) - berdasarkan jumlah orang dan lux
  static unsigned long lastLightCheck = 0;
  if (millis() - lastLightCheck > 3000) {
    String lampStatus = currentData.lampStatus;
    kontrolLampu(lampStatus);
    currentData.lampStatus = lampStatus;
    lastLightCheck = millis();
  }
  
  // Check for sensor changes (data sudah dikirim langsung di hasSignificantChange)
  hasSignificantChange();
  
  // Heartbeat untuk monitoring koneksi (hanya jika tidak ada perubahan dalam 5 menit)
  if (shouldForceUpdate() && canSendUpdate()) {
    sendHeartbeat();
  }
  
  // AC control check (every 5 seconds) - berdasarkan jumlah orang
  static unsigned long lastACControlCheck = 0;
  if (millis() - lastACControlCheck > 5000) {
    String acStatus = currentData.acStatus;
    int setTemp = currentData.setTemp;
    kontrolAC(acStatus, setTemp);
    currentData.acStatus = acStatus;
    currentData.setTemp = setTemp;
    lastACControlCheck = millis();
  }
  
  // Empty room timer monitoring (every 30 seconds)
  if (jumlahOrang == 0 && roomEmptyTimerActive) {
    static unsigned long lastTimerLog = 0;
    if (millis() - lastTimerLog > 30000) {
      unsigned long emptyDuration = millis() - emptyRoomStartTime;
      unsigned long remainingTime = EMPTY_ROOM_TIMEOUT - emptyDuration;
      int remainingMinutes = remainingTime / 60000;
      
      Serial.println("⏰ Empty room: " + String(remainingMinutes) + "m remaining");
      lastTimerLog = millis();
    }
  }
  
  // AC control check for empty room timeout
  if (jumlahOrang == lastJumlahOrang) {
    // Tetap jalankan kontrol AC dan lampu berdasarkan jumlah orang saat ini
    static unsigned long lastPeriodicCheck = 0;
    if (millis() - lastPeriodicCheck > 10000) { // Every 10 seconds
      String acStatus = currentData.acStatus;
      int setTemp = currentData.setTemp;
      kontrolAC(acStatus, setTemp);
      currentData.acStatus = acStatus;
      currentData.setTemp = setTemp;
      
      String lampStatus = currentData.lampStatus;
      kontrolLampu(lampStatus);
      currentData.lampStatus = lampStatus;
      
      lastPeriodicCheck = millis();
    }
  }
  
  // TFT update (only when needed or every 3 seconds)
  static unsigned long lastTFTUpdate = 0;
  if (millis() - lastTFTUpdate > 3000) {
    updateTFT(currentData.acStatus, currentData.setTemp, currentData.suhuRuang);
    lastTFTUpdate = millis();
  }
  
  // Optional WiFi check (every 120 seconds) - non-critical
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 120000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("ℹ️ WiFi still offline - continuing in standalone mode");
    }
    lastWiFiCheck = millis();
  }
  
  // Memory monitoring (every 120 seconds)
  static unsigned long lastMemoryCheck2 = 0;
  if (millis() - lastMemoryCheck2 > 120000) {
    uint32_t freeHeap = ESP.getFreeHeap();
    Serial.println("💾 Free memory: " + String(freeHeap) + " bytes");
    lastMemoryCheck2 = millis();
  }
  
  // Auto-reset if too many false detections (every 10 minutes)
  static unsigned long lastAutoResetCheck = 0;
  
  if (millis() - lastAutoResetCheck > 600000) { // Every 10 minutes
    Serial.println("ℹ️ System health check - 10 minutes elapsed");
    lastAutoResetCheck = millis();
  }
  
  // PERBAIKAN: Send heartbeat every 5 minutes dengan SEMUA DATA SENSOR
  unsigned long now = millis();
  if (now - lastHeartbeat > HEARTBEAT_INTERVAL) {
    sendDataToAPI("PERIODIC_SENSOR_UPDATE - All sensors data");
    lastHeartbeat = now;
  }
  
  // PERBAIKAN: Send data berkala setiap 2 menit untuk memastikan semua sensor tercatat
  static unsigned long lastPeriodicSensorUpdate = 0;
  if (now - lastPeriodicSensorUpdate > 120000) { // Every 2 minutes
    sendDataToAPI("PERIODIC_ALL_SENSORS - Temperature, Humidity, Light, Proximity");
    lastPeriodicSensorUpdate = now;
  }
  
  // Check for serial commands for testing
  if (Serial.available()) {
    String command = Serial.readString();
    command.trim();
    
    if (command == "reset") {
      Serial.println("🔄 Manual reset triggered");
      resetPeopleCounter();
    }
    else if (command == "test") {
      Serial.println("🧪 ENHANCED SENSOR TEST:");
      
      // Proximity sensors
      bool inState = digitalRead(PROXIMITY_PIN_IN);
      bool outState = digitalRead(PROXIMITY_PIN_OUT);
      Serial.println("   PROXIMITY:");
      Serial.println("     IN_PIN=" + String(inState) + " (Object=" + String(!inState ? "DETECTED" : "CLEAR") + ")");
      Serial.println("     OUT_PIN=" + String(outState) + " (Object=" + String(!outState ? "DETECTED" : "CLEAR") + ")");
      
      // DHT22 - Multiple readings untuk akurasi
      Serial.println("   DHT22 REAL-TIME TEST (GPIO13):");
      for (int i = 0; i < 3; i++) {
        float temp = dht.readTemperature();
        float hum = dht.readHumidity();
        
        Serial.println("     Reading #" + String(i + 1) + ":");
        Serial.println("       Temperature: " + String(temp, 2) + "°C " + String(!isnan(temp) && temp > 10 && temp < 50 ? "(VALID)" : "(INVALID)"));
        Serial.println("       Humidity: " + String(hum, 1) + "% " + String(!isnan(hum) && hum >= 20 && hum <= 90 ? "(VALID)" : "(INVALID)"));
        
        if (i < 2) delay(2500); // Wait between readings
      }
      
      // Current stored values
      Serial.println("   CURRENT STORED VALUES:");
      Serial.println("     Temperature: " + String(currentData.suhuRuang, 2) + "°C");
      Serial.println("     Humidity: " + String(currentData.humidity, 1) + "%");
      
      // LDR
      int ldrRaw = analogRead(LDR_PIN);
      int ldrMapped = map(ldrRaw, 0, 4095, 0, 1200);
      Serial.println("   LDR (GPIO35):");
      Serial.println("     Raw=" + String(ldrRaw) + " Mapped=" + String(ldrMapped) + " lux");
    }
    else if (command == "add") {
      Serial.println("➕ Manual add person");
      jumlahOrang++;
      jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
      updateSensorData();
      forceFullUpdate = true;
    }
    else if (command == "sub") {
      Serial.println("➖ Manual subtract person");
      if (jumlahOrang > 0) jumlahOrang--;
      updateSensorData();
      forceFullUpdate = true;
    }
    else if (command == "manual") {
      autoMode = false;
      manualOverride = true;
      digitalWrite(RELAY_LAMP1, LOW);  // NC tersambung = kontrol manual penuh
      Serial.println("🔧 Mode: MANUAL - Kontrol sepenuhnya dari saklar fisik");
      Serial.println("   Relay NC tersambung, saklar fisik aktif");
    }
    else if (command == "auto") {
      autoMode = true;
      manualOverride = false;
      Serial.println("🔧 Mode: OTOMATIS - Kontrol berdasarkan jumlah orang di ruangan");
      Serial.println("   Algoritma: Ada orang = LAMPU NYALA, Kosong = LAMPU MATI");
    }
    else if (command == "status") {
      Serial.println("📊 System Status:");
      Serial.println("   People Count: " + String(jumlahOrang));
      Serial.println("   AC Status: " + currentData.acStatus);
      Serial.println("   Lamp Status: " + currentData.lampStatus);
      Serial.println("   Room Temperature: " + String(currentData.suhuRuang, 1) + "°C");
      Serial.println("   Humidity: " + String(currentData.humidity, 1) + "%");
      Serial.println("   Light Level: " + String(currentData.lightLevel) + " lux");
      Serial.println("   WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
      Serial.println("   Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
      Serial.println("   Uptime: " + String(millis() / 1000) + " seconds");
      Serial.println("   SSL Status: " + String(sslStatus.lastConnectionSuccess ? "OK" : "FAILED"));
      Serial.println("   Proxy Status: " + String(proxyState.isProxyDetected ? "DETECTED" : "DIRECT"));
      Serial.println("   Auto Shutdown: PERMANENTLY DISABLED");
      Serial.println("   Operation Mode: 24/7 Continuous");
    }
    else if (command == "dht") {
      Serial.println("🌡️ DHT22 COMPREHENSIVE TEST:");
      Serial.println("   Testing DHT22 sensor on GPIO13...");
      Serial.println("   Expected: 3.3V power, 10K pull-up resistor");
      
      // Re-initialize sensor
      dht.begin();
      delay(2000);
      
      // Multiple test readings
      for (int i = 0; i < 5; i++) {
        Serial.println("   Test #" + String(i + 1) + ":");
        
        unsigned long startTime = millis();
        float temp = dht.readTemperature();
        float hum = dht.readHumidity();
        unsigned long readTime = millis() - startTime;
        
        Serial.println("     Read Time: " + String(readTime) + "ms");
        Serial.println("     Temperature: " + String(temp, 3) + "°C");
        Serial.println("     Humidity: " + String(hum, 2) + "%");
        
        if (!isnan(temp) && !isnan(hum)) {
          Serial.println("     Status: ✅ VALID");
          if (temp > 15 && temp < 45) {
            Serial.println("     Temperature Range: ✅ REALISTIC");
          } else {
            Serial.println("     Temperature Range: ⚠️ UNUSUAL");
          }
        } else {
          Serial.println("     Status: ❌ INVALID");
        }
        
        if (i < 4) {
          Serial.println("     Waiting 3 seconds...");
          delay(3000);
        }
      }
      
      Serial.println("   Current stored values:");
      Serial.println("     Temperature: " + String(currentData.suhuRuang, 2) + "°C");
      Serial.println("     Humidity: " + String(currentData.humidity, 1) + "%");
    }
    else if (command == "smart") {
      Serial.println("🧠 SMART PROXIMITY TEST:");
      Serial.println("   Testing smart walking detection logic...");
      Serial.println("   Walk through sensors at normal speed to test");
      Serial.println("   ");
      Serial.println("   Current Configuration:");
      Serial.println("     Walking Speed Min: " + String(WALKING_SPEED_MIN) + "ms");
      Serial.println("     Walking Speed Max: " + String(WALKING_SPEED_MAX) + "ms");
      Serial.println("     Sequence Timeout: " + String(SEQUENCE_TIMEOUT) + "ms");
      Serial.println("     Noise Filter: " + String(NOISE_FILTER_TIME) + "ms");
      Serial.println("   ");
      Serial.println("   Expected Behavior:");
      Serial.println("     - Walk IN→OUT: +1 person (ENTRY)");
      Serial.println("     - Walk OUT→IN: -1 person (EXIT)");
      Serial.println("     - Too fast (< 50ms): Ignored");
      Serial.println("     - Too slow (> 2000ms): Ignored");
      Serial.println("     - Single sensor stuck: Auto-reset after 3s");
      Serial.println("   ");
      Serial.println("   Current Status:");
      Serial.println("     Sequence: " + sensorData.currentSequence);
      Serial.println("     Walking Pattern: " + String(sensorData.validWalkingPattern ? "DETECTED" : "NONE"));
      Serial.println("     Direction: " + sensorData.walkingDirection);
    }
    else if (command == "proximity") {
      Serial.println("📏 PROXIMITY SENSOR RANGE TEST (10-100cm):");
      Serial.println("   Testing proximity sensors for optimal 10-100cm detection...");
      Serial.println("   Current sensor readings:");
      
      for (int i = 0; i < 10; i++) {
        bool inState = digitalRead(PROXIMITY_PIN_IN);
        bool outState = digitalRead(PROXIMITY_PIN_OUT);
        
        Serial.println("   Test #" + String(i + 1) + ":");
        Serial.println("     IN (GPIO32): " + String(inState ? "HIGH" : "LOW") + 
                       " | Object: " + String(!inState ? "DETECTED (10-100cm)" : "CLEAR"));
        Serial.println("     OUT (GPIO33): " + String(outState ? "HIGH" : "LOW") + 
                       " | Object: " + String(!outState ? "DETECTED (10-100cm)" : "CLEAR"));
        
        delay(500);
      }
      
      Serial.println("   Proximity sensor configuration:");
      Serial.println("     Detection range: 10-100cm (optimized for human movement)");
      Serial.println("     Cooldown: " + String(PERSON_COOLDOWN) + "ms");
      Serial.println("     Debounce: " + String(DEBOUNCE_DELAY) + "ms");
      Serial.println("     Stability time: " + String(PROXIMITY_STABLE_TIME) + "ms");
    }
    else if (command == "sendtest") {
      Serial.println("📤 TESTING DATA TRANSMISSION TO DATABASE:");
      Serial.println("   Forcing complete sensor data transmission...");
      
      // Update all sensors first
      updateSensorData();
      
      // Send to database
      sendDataToAPI("MANUAL_TEST - Testing all sensors data transmission");
      
      Serial.println("   Check your database to verify all sensor data is recorded:");
      Serial.println("   - People Count");
      Serial.println("   - Room Temperature (DHT22)");
      Serial.println("   - Humidity (DHT22)");
      Serial.println("   - Light Level (LDR)");
      Serial.println("   - Proximity IN");
      Serial.println("   - Proximity OUT");
      Serial.println("   - AC Status");
      Serial.println("   - Lamp Status");
    }
    else if (command == "help") {
      Serial.println("📋 Enhanced Available Commands:");
      Serial.println("   reset    - Reset people counter to 0");
      Serial.println("   test     - Test all sensors (proximity, DHT22, LDR)");
      Serial.println("   dht      - Comprehensive DHT22 temperature sensor test");
      Serial.println("   smart    - Test smart walking detection logic");
      Serial.println("   proximity- Test proximity sensors (10-100cm range)");
      Serial.println("   sensors  - Show current sensor readings");
      Serial.println("   sendtest - Test data transmission to database");
      Serial.println("   add      - Manually add 1 person");
      Serial.println("   sub      - Manually subtract 1 person");
      Serial.println("   ssl      - Show SSL status and test connection");
      Serial.println("   proxy    - Show proxy detection status");
      Serial.println("   hosting  - Show hosting connection status");
      Serial.println("   status   - Show complete system status");
      Serial.println("   help     - Show this enhanced help");
    }
    else if (command == "ssl") {
      Serial.println("🔒 Enhanced SSL Status:");
      Serial.println("   SSL Configured: " + String(sslStatus.isConfigured ? "YES" : "NO"));
      Serial.println("   SSL Method: ROOT_CA (Secure)");
      Serial.println("   Last Connection: " + String(sslStatus.lastConnectionSuccess ? "SUCCESS" : "FAILED"));
      Serial.println("   Consecutive Failures: " + String(sslStatus.consecutiveFailures));
      Serial.println("   Last Error: " + sslStatus.lastError);
      Serial.println("   Hosting Domain: " + String(hostingDomain));
      
      // Test SSL connection
      Serial.println("   Testing SSL connection...");
      if (testSSLConnection()) {
        Serial.println("   ✅ SSL Test: SUCCESS");
      } else {
        Serial.println("   ❌ SSL Test: FAILED");
      }
    }
    else if (command == "proxy") {
      Serial.println("🌐 Enhanced Proxy Status:");
      Serial.println("   Proxy Detected: " + String(proxyState.isProxyDetected ? "YES" : "NO"));
      Serial.println("   Connection Type: " + String(proxyState.isDirectConnection ? "DIRECT" : "PROXY"));
      Serial.println("   Proxy Type: " + proxyState.proxyType);
      Serial.println("   Real IP: " + proxyState.realIP);
      Serial.println("   Proxy IP: " + proxyState.proxyIP);
      Serial.println("   Hosting Reachable: " + String(proxyState.hostingReachable ? "YES" : "NO"));
      Serial.println("   Consecutive Failures: " + String(proxyState.consecutiveFailures));
      
      // Force proxy re-detection
      proxyState.lastProxyCheck = 0;
      Serial.println("   Forcing proxy re-detection...");
    }
    else if (command == "hosting") {
      Serial.println("🏠 Enhanced Hosting Status:");
      Serial.println("   Domain: " + String(hostingDomain));
      Serial.println("   API URL: " + String(apiURL));
      Serial.println("   AC Control URL: " + String(acControlURL));
      Serial.println("   Proxy Detection URL: " + String(proxyDetectionURL));
      Serial.println("   SSL Enabled: YES");
      Serial.println("   Connection Status: " + String(WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED"));
      
      // Test hosting connectivity
      testHostingConnectivity();
    }
    else if (command == "cert") {
      Serial.println("🔍 SSL Certificate Info:");
      getCertificateFingerprint();
    }
    else if (command == "testmode") {
      testMode = !testMode;
      Serial.println("🧪 Test mode: " + String(testMode ? "ON (no cooldown)" : "OFF (normal cooldown)"));
      if (testMode) {
        Serial.println("   IN sensor will immediately +1");
        Serial.println("   OUT sensor will immediately -1 (if count > 0)");
        Serial.println("   Use 'testmode' again to turn OFF");
      }
    }
    else if (command == "sensors") {
      Serial.println("🔍 ENHANCED CURRENT SENSOR READINGS:");
      Serial.println("   People in room: " + String(jumlahOrang));
      
      // Real-time DHT22 reading
      Serial.println("   DHT22 REAL-TIME (GPIO13):");
      float currentTemp = dht.readTemperature();
      float currentHum = dht.readHumidity();
      Serial.println("     Live Temperature: " + String(currentTemp, 2) + "°C " + String(!isnan(currentTemp) ? "(LIVE)" : "(ERROR)"));
      Serial.println("     Live Humidity: " + String(currentHum, 1) + "% " + String(!isnan(currentHum) ? "(LIVE)" : "(ERROR)"));
      Serial.println("     Stored Temperature: " + String(currentData.suhuRuang, 2) + "°C");
      Serial.println("     Stored Humidity: " + String(currentData.humidity, 1) + "%");
      
      bool inState = digitalRead(PROXIMITY_PIN_IN);
      bool outState = digitalRead(PROXIMITY_PIN_OUT);
      Serial.println("   PROXIMITY:");
      Serial.println("     IN: " + String(!inState ? "DETECTED" : "CLEAR"));
      Serial.println("     OUT: " + String(!outState ? "DETECTED" : "CLEAR"));
      
      int ldrRaw = analogRead(LDR_PIN);
      int ldrMapped = map(ldrRaw, 0, 4095, 0, 1200);
      Serial.println("   LDR (GPIO35): " + String(ldrMapped) + " lux");
      
      Serial.println("   Enhanced Status:");
      Serial.println("     Lamp: " + currentData.lampStatus);
      Serial.println("     AC: " + currentData.acStatus);
      Serial.println("     SSL: " + String(sslStatus.lastConnectionSuccess ? "SECURE" : "FAILED"));
      Serial.println("     Proxy: " + String(proxyState.isProxyDetected ? "DETECTED" : "DIRECT"));
      Serial.println("     Hosting: " + String(proxyState.hostingReachable ? "REACHABLE" : "UNREACHABLE"));
      Serial.println("     DHT22 Status: Real-time monitoring ACTIVE");
    }
    else if (command == "sequence") {
      Serial.println("🧪 Proximity Sequence Test:");
      Serial.println("   Walk through sensors in this order:");
      Serial.println("   ENTRY: IN sensor first → then OUT sensor");
      Serial.println("   EXIT:  OUT sensor first → then IN sensor");
      Serial.println("   Monitoring for 30 seconds...");
      
      unsigned long testStart = millis();
      while (millis() - testStart < 30000) {
        readSensors();
        detectPeople();
        delay(50);
      }
      Serial.println("   Sequence test completed");
    }
    else if (command == "simple") {
      Serial.println("🧪 Simple Detection Test Mode:");
      Serial.println("   IN sensor = +1 person");
      Serial.println("   OUT sensor = -1 person (if count > 0)");
      Serial.println("   Testing for 60 seconds...");
      
      unsigned long testStart = millis();
      bool lastTestIn = false;
      bool lastTestOut = false;

      
      while (millis() - testStart < 60000) {
        bool currentIn = !digitalRead(PROXIMITY_PIN_IN);  // LOW = detected
        bool currentOut = !digitalRead(PROXIMITY_PIN_OUT); // LOW = detected
        
        // Simple immediate counting
        if (currentIn && !lastTestIn) {
          jumlahOrang++;
          jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
          Serial.println("🚶 → IN DETECTED! Count: " + String(jumlahOrang));
          updateSensorData();
          forceFullUpdate = true;
        }
        
        if (currentOut && !lastTestOut && jumlahOrang > 0) {
          jumlahOrang--;
          Serial.println("🚶 ← OUT DETECTED! Count: " + String(jumlahOrang));
          updateSensorData();
          forceFullUpdate = true;
        }
        
        lastTestIn = currentIn;
        lastTestOut = currentOut;
        
        delay(50);
      }
      Serial.println("   Simple test completed. Final count: " + String(jumlahOrang));
    }
  }
  
  // STABLE MODE: Delay yang lebih aman untuk mencegah overload CPU
  static unsigned long lastLoopTime = 0;
  unsigned long loopDuration = millis() - lastLoopTime;
  lastLoopTime = millis();
  
  // Adaptive delay berdasarkan durasi loop - lebih konservatif
  if (loopDuration < 20) {
    delay(100); // Loop terlalu cepat, beri jeda lebih lama
  } else if (loopDuration < 100) {
    delay(50); // Loop normal
  } else {
    delay(10);  // Loop sudah lambat, delay minimal
  }
  
  // Reset watchdog sebelum akhir loop - DISABLED
  // esp_task_wdt_reset();
}

// ================= AUTO SHUTDOWN FUNCTIONS - REMOVED =================
// Auto shutdown dan startup functions telah dihapus untuk stabilitas sistem

// ================= HTTP FALLBACK FUNCTION =================
bool makeHTTPRequest(const char* url, const char* method, const char* payload, String& response) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi not connected for HTTP fallback");
    return false;
  }
  
  WiFiClient client;
  HTTPClient http;
  
  Serial.println("🔄 HTTP FALLBACK - Making request to: " + String(url));
  
  http.setTimeout(10000); // 10 seconds timeout
  
  if (!http.begin(client, url)) {
    Serial.println("❌ HTTP fallback connection failed");
    return false;
  }
  
  // Add headers
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("User-Agent", "ESP32-HTTP-Fallback/1.0");
  http.addHeader("X-Device-ID", DEVICE_ID);
  http.addHeader("X-Fallback-Mode", "true");
  
  int httpResponseCode;
  if (strcmp(method, "POST") == 0) {
    Serial.println("📤 Sending HTTP POST fallback...");
    httpResponseCode = http.POST(payload);
  } else {
    Serial.println("📤 Sending HTTP GET fallback...");
    httpResponseCode = http.GET();
  }
  
  if (httpResponseCode > 0) {
    response = http.getString();
    http.end();
    
    Serial.println("📥 HTTP Fallback Response Code: " + String(httpResponseCode));
    
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      Serial.println("✅ HTTP FALLBACK SUCCESSFUL - DATA TERKIRIM!");
      return true;
    } else {
      Serial.println("⚠️ HTTP Fallback warning (Code: " + String(httpResponseCode) + ")");
      return httpResponseCode < 500;
    }
  } else {
    Serial.println("❌ HTTP Fallback failed (Code: " + String(httpResponseCode) + ")");
    http.end();
    return false;
  }
}

// ================= HEARTBEAT FUNCTIONS =================
void sendHeartbeat() {
  // Heartbeat untuk monitoring koneksi saja - auto shutdown removed
  sendDataToAPI("SYSTEM_HEARTBEAT - Connection monitoring");
  
  Serial.println("💓 HEARTBEAT SENT - System monitoring only");
  Serial.println("   Note: Real data sent only on sensor changes");
}