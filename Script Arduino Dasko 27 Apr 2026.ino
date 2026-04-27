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

// ================= HOSTING WEB CONFIGURATION WITH SSL =================
const char* ssid = "Elektro";
const char* password = "PakManab2026";
const char* hostingDomain = "dasko.fst.unja.ac.id";
const char* apiURL = "https://dasko.fst.unja.ac.id/api/sensor/data";
const char* acControlURL = "https://dasko.fst.unja.ac.id/api/ac/control";
const char* proxyDetectionURL = "https://dasko.fst.unja.ac.id/api/proxy/detection";

// ================= SSL CERTIFICATE CONFIGURATION =================
// Root CA Certificate untuk HTTPS (Let's Encrypt - umum digunakan hosting)
const char* rootCACertificate = 
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

// Alternative: Certificate fingerprint method (backup) - NOT USED
// const char* certificateFingerprint = "AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD";

// ================= SSL & PROXY DETECTION CONFIGURATION =================
#define SSL_METHOD_ROOT_CA 1        // Menggunakan Root CA Certificate (Recommended)
#define SSL_METHOD_FINGERPRINT 2    // Menggunakan Certificate Fingerprint
#define SSL_METHOD_INSECURE 3       // Skip SSL verification (HANYA untuk testing)

#define SSL_METHOD SSL_METHOD_INSECURE  // Gunakan insecure karena Root CA sering mismatch

// Jika server TIDAK support HTTPS (Connection Refused di port 443),
// set true agar langsung pakai HTTP tanpa buang waktu retry HTTPS.
#define PREFER_HTTP true

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
#define IR_PIN            4
#define PROXIMITY_PIN_IN  32
#define PROXIMITY_PIN_OUT 14
#define DHTPIN            13  // Hanya menggunakan DHT di pin 13
#define DHTTYPE           DHT22
#define LDR_PIN           34  // Hanya menggunakan LDR di pin 35
#define LDR_ENABLED       true // Set true jika sensor LDR terpasang, false = selalu 0 lux
#define RELAY_LAMP1       25  // Relay untuk lampu (menggunakan NC untuk dual fungsi)
// #define RELAY_LAMP2       26  // Tidak digunakan - hanya 1 jalur lampure
#define MAX_PEOPLE 20
#define PERSON_COOLDOWN 700   // Cooldown lebih cepat, tetap menahan double count satu orang
#define DEBOUNCE_DELAY 20      // Debounce cepat untuk response proximity tanpa noise singkat
#define MIN_PRESENCE_TIME 90   // Objek harus stabil minimal 90ms agar valid sebagai orang
#define DOOR_FILTER_TIME 60    // Durasi di bawah ini dianggap noise/pantulan sensor
#define SEQUENCE_TIMEOUT 1800  // Maks waktu antar sensor IN/OUT untuk dianggap satu orang lewat

// ================= LIGHT CONTROL THRESHOLDS =================
#define LUX_THRESHOLD     800  // Threshold cahaya untuk kontrol otomatis (diubah dari 600 ke 800)

// ================= AUTO SHUTDOWN CONFIGURATION =================khhhhhhhhhhhh
#define AUTO_SHUTDOWN_HOUR    17  // Jam 5 sore (17:00)
#define AUTO_SHUTDOWN_MINUTE  0   // Menit 0
#define AUTO_STARTUP_HOUR     6   // Jam 6 pagi (06:00)
#define AUTO_STARTUP_MINUTE   0   // Menit 0
#define TIMEZONE_OFFSET_HOURS 7   // WIB (UTC+7)

// NTP Server configuration
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = TIMEZONE_OFFSET_HOURS * 3600;
const int daylightOffset_sec = 0;

// Auto shutdown/startup variables
bool autoShutdownEnabled = false;  // DISABLED untuk sementara
bool autoStartupEnabled = false;   // DISABLED untuk sementara
bool autoShutdownTriggered = false;
bool autoStartupTriggered = false;
bool systemAutoShutdown = false;  // Flag to track if system is in auto shutdown state
unsigned long lastTimeCheck = 0;
unsigned long lastAutoShutdownLog = 0;

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
// Simplified configuration for immediate counting
struct SensorData {
  bool proximityIn = false;
  bool proximityOut = false;
  bool objectInDetected = false;
  bool objectOutDetected = false;
  bool lastProximityIn = false;
  bool lastProximityOut = false;
  unsigned long lastInChange = 0;
  unsigned long lastOutChange = 0;
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
void readDHTRealtime();
void readLDRRealtime();
String normalizeACStatus();
String normalizeLampStatus();
void updateSensorData();
void connectWiFi();
bool makeHTTPSRequest(const char* url, const char* method, const char* payload, String& response);
bool makeHTTPRequest(const char* url, const char* method, const char* payload, String& response); // Fallback HTTP
void checkACControlAPI();
void kontrolAC(String &acStatus, int &setTemp);
void kontrolLampu(String &lampStatus);
void syncDeviceControlsAfterPeopleChange(String reason);
void detectPeople();
void detectPeopleSequenced();
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

// Auto shutdown functions
void initializeTime();
void checkAutoShutdown();
void performAutoShutdown();
void performAutoStartup();
String getCurrentTimeString();
bool isAutoShutdownTime();
bool isAutoStartupTime();
void sendHeartbeat();  // Heartbeat saja, bukan data utama

void IRAM_ATTR proximityInInterrupt();
void IRAM_ATTR proximityOutInterrupt();

// ================= INTERRUPT HANDLERS =================
void IRAM_ATTR proximityInInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 10) { // Faster interrupt response
    interruptTriggered = true;
    lastInterruptTime = now;
  }
}

void IRAM_ATTR proximityOutInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 10) { // Faster interrupt response
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
  
  Serial.println("🔍 ENHANCED PROXY DETECTION CHECK (Non-blocking)");
  
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
#if PREFER_HTTP
  WiFiClient client;
#else
  WiFiClientSecure client;
  client.setInsecure();
#endif
  HTTPClient http;
  
  http.setTimeout(PROXY_TIMEOUT_MS);
  
  String proxyURL = String(proxyDetectionURL);
#if PREFER_HTTP
  proxyURL.replace("https://", "http://");
#endif
  
  if (!http.begin(client, proxyURL)) {
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
  
  StaticJsonDocument<300> requestDoc;
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
    
    StaticJsonDocument<512> responseDoc;
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
#if PREFER_HTTP
  WiFiClient client;
  HTTPClient http;
  http.setTimeout(2000);
  
  if (http.begin(client, "http://dasko.fst.unja.ac.id/api/health")) {
#else
  WiFiClientSecure client;
  HTTPClient http;
  client.setInsecure();
  http.setTimeout(2000);
  
  if (http.begin(client, "https://dasko.fst.unja.ac.id/api/health")) {
#endif
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
      Serial.println("   Response Body: " + response.substring(0, 300)); // Cetak body untuk debug
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
  Serial.println("=== ENHANCED WIFI CONNECTION WITH SSL & PROXY SUPPORT ===");
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

  // ── DHT22 real-time (min 2.5s antar baca, hasil di-cache) ──
  readDHTRealtime();

  // ── LDR real-time setiap loop ──
  readLDRRealtime();

  bool currentInState  = digitalRead(PROXIMITY_PIN_IN);
  bool currentOutState = digitalRead(PROXIMITY_PIN_OUT);
  
  // Debug output with SSL and proxy status
  static unsigned long lastProximityDebug = 0;
  if (now - lastProximityDebug > 5000) {
    Serial.println("🔍 ENHANCED PROXIMITY DEBUG (SSL + Proxy):");
    Serial.println("   GPIO 32 (MASUK): " + String(currentInState ? "HIGH" : "LOW") + " | Detected: " + String(!currentInState ? "YES" : "NO"));
    Serial.println("   GPIO 14 (KELUAR): " + String(currentOutState ? "HIGH" : "LOW") + " | Detected: " + String(!currentOutState ? "YES" : "NO"));
    Serial.println("   💡 Lamp Status: " + String(lastLamp1 ? "ON" : "OFF"));
    Serial.println("   🔒 SSL Status: " + String(sslStatus.lastConnectionSuccess ? "OK" : "FAIL"));
    Serial.println("   🌐 Proxy Status: " + String(proxyState.isProxyDetected ? "DETECTED" : "DIRECT"));
    lastProximityDebug = now;
  }
  
  // Simple state change tracking
  if (currentInState != sensorData.lastProximityIn) {
    sensorData.lastInChange = now;
    sensorData.lastProximityIn = currentInState;
    Serial.println("📍 IN sensor: " + String(currentInState ? "HIGH" : "LOW"));
  }
  
  if (currentOutState != sensorData.lastProximityOut) {
    sensorData.lastOutChange = now;
    sensorData.lastProximityOut = currentOutState;
    Serial.println("📍 OUT sensor: " + String(currentOutState ? "HIGH" : "LOW"));
  }
  
  // Simple debounce
  if ((now - sensorData.lastInChange) > DEBOUNCE_DELAY) {
    sensorData.proximityIn = currentInState;
  }
  
  if ((now - sensorData.lastOutChange) > DEBOUNCE_DELAY) {
    sensorData.proximityOut = currentOutState;
  }
  
  // Immediate object detection (LOW = detected)
  sensorData.objectInDetected = !sensorData.proximityIn;
  sensorData.objectOutDetected = !sensorData.proximityOut;
}

void detectPeople() {
  unsigned long now = millis();

  // ================================================================
  // DEBUG PERIODIK
  // ================================================================
  static unsigned long lastDebug = 0;
  if (now - lastDebug > 8000) {
    Serial.print("📊 People: ");
    Serial.print(jumlahOrang);
    Serial.print("/20 | MASUK:");
    Serial.print(sensorData.objectInDetected ? "DETECTED" : "CLEAR");
    Serial.print(" | KELUAR:");
    Serial.print(sensorData.objectOutDetected ? "DETECTED" : "CLEAR");
    Serial.print(" | WiFi: ");
    Serial.print(WiFi.status() == WL_CONNECTED ? "OK" : "FAIL");
    Serial.print(" | SSL: ");
    Serial.print(sslStatus.lastConnectionSuccess ? "OK" : "FAIL");
    Serial.print(" | Proxy: ");
    Serial.println(proxyState.isProxyDetected ? "YES" : "NO");

    // JANGAN memanggil dht.readTemperature() di sini, karena menyalahi interval 2.5 detik DHT22 (membuat sensor blank/NaN)!
    if (!isnan(currentData.suhuRuang)) {
      Serial.println("🌡️ Temp: " + String(currentData.suhuRuang, 1) + "°C");
    }
    lastDebug = now;
  }

  // ================================================================
  // STATE TRACKING
  // Variabel static untuk melacak status per-siklus
  // ================================================================
  static bool lastInDetected  = false;
  static bool lastOutDetected = false;

  // Waktu terakhir trigger yang SAH (setelah melewati semua filter)
  static unsigned long lastInTrigger  = 0;
  static unsigned long lastOutTrigger = 0;

  // Waktu pertama kali sensor menjadi HIGH (untuk MIN_PRESENCE_TIME)
  static unsigned long inFirstDetected  = 0;
  static unsigned long outFirstDetected = 0;

  // Flag "sedang menunggu konfirmasi presence"
  static bool inWaiting  = false;
  static bool outWaiting = false;

  // ================================================================
  // FILTER PINTU / NOISE BERBASIS MINIMUM PRESENCE TIME
  //
  // Prinsip kerja:
  //  1. Saat sensor pertama kali HIGH  → catat waktu, set flag "waiting"
  //  2. Selama sensor masih HIGH       → cek apakah durasi >= MIN_PRESENCE_TIME
  //                                      DAN cooldown antar orang sudah terpenuhi
  //  3. Jika kedua syarat terpenuhi    → hitung sebagai orang (masuk/keluar)
  //  4. Jika sensor kembali LOW
  //     sebelum MIN_PRESENCE_TIME      → batalkan (dianggap gerakan pintu/noise)
  //
  // Hasil: Gerakan pintu (cepat, <180ms) diabaikan otomatis.
  //        Orang lewat (>220ms) tetap terhitung dengan tepat.
  // ================================================================

  // ---------- ENTRY (sensor IN / PROXIMITY_PIN_IN) ----------
  if (sensorData.objectInDetected && !lastInDetected) {
    // Rising edge: objek baru terdeteksi
    inFirstDetected = now;
    inWaiting = true;
    Serial.println("🔵 IN rising-edge → menunggu konfirmasi presence...");
  }

  if (inWaiting) {
    if (sensorData.objectInDetected) {
      // Objek masih terdeteksi – cek dua syarat:
      //   a) Sudah hadir minimal MIN_PRESENCE_TIME
      //   b) Cooldown antar orang sudah lewat
      unsigned long presenceDuration = now - inFirstDetected;
      if (presenceDuration >= MIN_PRESENCE_TIME && (now - lastInTrigger) >= PERSON_COOLDOWN) {
        // ✅ Konfirmasi: INI ORANG, bukan pintu
        jumlahOrang++;
        lastPersonDetected = now;
        lastInTrigger = now;
        inWaiting = false;
        roomEmptyTimerActive = false;

        Serial.println("🚶 → MASUK TERKONFIRMASI! (presence=" + String(presenceDuration) + "ms) Count: " + String(jumlahOrang));
        syncDeviceControlsAfterPeopleChange("ENTRY_CONFIRMED");

        if (WiFi.status() == WL_CONNECTED) {
          String reason = "ENTRY_CONFIRMED - Person " + String(jumlahOrang) +
                          " entered (presence " + String(presenceDuration) + "ms) - Proximity IN";
          sendDataToAPI(reason);
          Serial.println("📤 ✅ TERCATAT: Orang masuk #" + String(jumlahOrang));
        }
      }
      // Belum memenuhi syarat → lanjut tunggu
    } else {
      // Falling edge sebelum syarat terpenuhi → pintu / noise → abaikan
      unsigned long duration = now - inFirstDetected;
      inWaiting = false;
      if (duration < DOOR_FILTER_TIME) {
        Serial.println("🚪 IN: Diabaikan – durasi " + String(duration) + "ms < " +
                       String(DOOR_FILTER_TIME) + "ms (kemungkinan gerakan pintu/noise)");
      } else {
        // Durasi cukup tapi cooldown belum lewat → masih dalam cooldown
        Serial.println("⏸️ IN: Diabaikan – masih dalam cooldown antar orang (" +
                       String(now - lastInTrigger) + "ms < " + String(PERSON_COOLDOWN) + "ms)");
      }
    }
  }

  // ---------- EXIT (sensor OUT / PROXIMITY_PIN_OUT) ----------
  if (sensorData.objectOutDetected && !lastOutDetected) {
    // Rising edge: objek baru terdeteksi
    outFirstDetected = now;
    outWaiting = true;
    Serial.println("🔴 OUT rising-edge → menunggu konfirmasi presence...");
  }

  if (outWaiting) {
    if (sensorData.objectOutDetected) {
      unsigned long presenceDuration = now - outFirstDetected;
      if (presenceDuration >= MIN_PRESENCE_TIME && (now - lastOutTrigger) >= PERSON_COOLDOWN) {
        // ✅ Konfirmasi: INI ORANG yang keluar
        if (jumlahOrang > 0) {
          jumlahOrang--;
          lastPersonDetected = now;
          lastOutTrigger = now;
          outWaiting = false;

          if (jumlahOrang == 0) {
            emptyRoomStartTime = millis();
            roomEmptyTimerActive = true;
            Serial.println("⏰ Empty room timer started");
          }

          Serial.println("🚶 ← KELUAR TERKONFIRMASI! (presence=" + String(presenceDuration) + "ms) Count: " + String(jumlahOrang));
          syncDeviceControlsAfterPeopleChange("EXIT_CONFIRMED");

          if (WiFi.status() == WL_CONNECTED) {
            String reason = "EXIT_CONFIRMED - Person left (presence " + String(presenceDuration) +
                            "ms), remaining: " + String(jumlahOrang) + " - Proximity OUT";
            sendDataToAPI(reason);
            Serial.println("📤 ✅ TERCATAT: Orang keluar, sisa #" + String(jumlahOrang));
          }
        } else {
          // Count sudah 0, batalkan
          outWaiting = false;
          lastOutTrigger = now;
          static unsigned long lastFalseExitWarning = 0;
          if (now - lastFalseExitWarning > 3000) {
            Serial.println("⚠️ EXIT saat count=0 – kemungkinan sensor issue");
            lastFalseExitWarning = now;
          }
        }
      }
    } else {
      // Falling edge sebelum syarat terpenuhi → pintu / noise → abaikan
      unsigned long duration = now - outFirstDetected;
      outWaiting = false;
      if (duration < DOOR_FILTER_TIME) {
        Serial.println("🚪 OUT: Diabaikan – durasi " + String(duration) + "ms < " +
                       String(DOOR_FILTER_TIME) + "ms (kemungkinan gerakan pintu/noise)");
      } else {
        Serial.println("⏸️ OUT: Diabaikan – masih dalam cooldown antar orang (" +
                       String(now - lastOutTrigger) + "ms < " + String(PERSON_COOLDOWN) + "ms)");
      }
    }
  }

  // ================================================================
  // DEBUG STATE CHANGES
  // ================================================================
  if (sensorData.objectInDetected != lastInDetected) {
    Serial.println("🔄 IN: " + String(lastInDetected ? "DETECTED" : "CLEAR") +
                   " → " + String(sensorData.objectInDetected ? "DETECTED" : "CLEAR"));
  }
  if (sensorData.objectOutDetected != lastOutDetected) {
    Serial.println("🔄 OUT: " + String(lastOutDetected ? "DETECTED" : "CLEAR") +
                   " → " + String(sensorData.objectOutDetected ? "DETECTED" : "CLEAR"));
  }

  lastInDetected  = sensorData.objectInDetected;
  lastOutDetected = sensorData.objectOutDetected;
  jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
}

void detectPeopleSequenced() {
  unsigned long now = millis();

  static unsigned long lastDebug = 0;
  if (now - lastDebug > 8000) {
    Serial.print("People: ");
    Serial.print(jumlahOrang);
    Serial.print("/20 | IN:");
    Serial.print(sensorData.objectInDetected ? "DETECTED" : "CLEAR");
    Serial.print(" | OUT:");
    Serial.print(sensorData.objectOutDetected ? "DETECTED" : "CLEAR");
    Serial.print(" | WiFi: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "OK" : "FAIL");
    lastDebug = now;
  }

  static bool lastInDetected = false;
  static bool lastOutDetected = false;
  static bool inWaiting = false;
  static bool outWaiting = false;
  static unsigned long inFirstDetected = 0;
  static unsigned long outFirstDetected = 0;
  static byte sequenceState = 0; // 0 idle, 1 IN first, 2 OUT first
  static unsigned long sequenceStart = 0;

  bool inStableEvent = false;
  bool outStableEvent = false;

  if (sequenceState != 0 && (now - sequenceStart) > SEQUENCE_TIMEOUT) {
    Serial.println("Sequence timeout - second sensor did not follow, ignored");
    sequenceState = 0;
  }

  if (sensorData.objectInDetected && !lastInDetected) {
    inFirstDetected = now;
    inWaiting = true;
    Serial.println("IN edge - waiting stable");
  }

  if (sensorData.objectOutDetected && !lastOutDetected) {
    outFirstDetected = now;
    outWaiting = true;
    Serial.println("OUT edge - waiting stable");
  }

  if (inWaiting) {
    if (sensorData.objectInDetected) {
      if ((now - inFirstDetected) >= MIN_PRESENCE_TIME) {
        inStableEvent = true;
        inWaiting = false;
      }
    } else {
      unsigned long duration = now - inFirstDetected;
      inWaiting = false;
      if (duration < DOOR_FILTER_TIME) {
        Serial.println("IN ignored - short noise " + String(duration) + "ms");
      }
    }
  }

  if (outWaiting) {
    if (sensorData.objectOutDetected) {
      if ((now - outFirstDetected) >= MIN_PRESENCE_TIME) {
        outStableEvent = true;
        outWaiting = false;
      }
    } else {
      unsigned long duration = now - outFirstDetected;
      outWaiting = false;
      if (duration < DOOR_FILTER_TIME) {
        Serial.println("OUT ignored - short noise " + String(duration) + "ms");
      }
    }
  }

  bool cooldownReady = testMode || (now - lastPersonDetected) >= PERSON_COOLDOWN;
  bool sequenceCompleted = false;

  if (inStableEvent && outStableEvent && sequenceState == 0) {
    Serial.println("IN and OUT stable at the same time - direction unclear, ignored");
  } else {
    if (inStableEvent) {
      if (sequenceState == 2) {
        if (!cooldownReady) {
          Serial.println("OUT-IN ignored - still in person cooldown");
          sequenceState = 0;
          sequenceCompleted = true;
        } else if (jumlahOrang > 0) {
          jumlahOrang--;
          lastPersonDetected = now;
          sequenceState = 0;
          sequenceCompleted = true;

          if (jumlahOrang == 0) {
            emptyRoomStartTime = millis();
            roomEmptyTimerActive = true;
            Serial.println("Empty room timer started");
          }

          Serial.println("KELUAR TERKONFIRMASI: OUT->IN. Count: " + String(jumlahOrang));
          syncDeviceControlsAfterPeopleChange("EXIT_CONFIRMED_OUT_IN");

          if (WiFi.status() == WL_CONNECTED) {
            String reason = "EXIT_CONFIRMED - Person left by OUT-IN sequence, remaining: " +
                            String(jumlahOrang) + " - E18-D80NK";
            sendDataToAPI(reason);
          }
        } else {
          sequenceState = 0;
          sequenceCompleted = true;
          static unsigned long lastFalseExitWarning = 0;
          if (now - lastFalseExitWarning > 3000) {
            Serial.println("EXIT ignored - count already 0");
            lastFalseExitWarning = now;
          }
        }
      } else if (sequenceState == 0) {
        sequenceState = 1;
        sequenceStart = now;
        Serial.println("Sequence start: IN valid, waiting OUT");
      }
    }

    if (outStableEvent && !sequenceCompleted) {
      if (sequenceState == 1) {
        if (!cooldownReady) {
          Serial.println("IN-OUT ignored - still in person cooldown");
          sequenceState = 0;
          sequenceCompleted = true;
        } else {
          jumlahOrang++;
          lastPersonDetected = now;
          sequenceState = 0;
          sequenceCompleted = true;
          roomEmptyTimerActive = false;

          Serial.println("MASUK TERKONFIRMASI: IN->OUT. Count: " + String(jumlahOrang));
          syncDeviceControlsAfterPeopleChange("ENTRY_CONFIRMED_IN_OUT");

          if (WiFi.status() == WL_CONNECTED) {
            String reason = "ENTRY_CONFIRMED - Person " + String(jumlahOrang) +
                            " entered by IN-OUT sequence - E18-D80NK";
            sendDataToAPI(reason);
          }
        }
      } else if (sequenceState == 0) {
        sequenceState = 2;
        sequenceStart = now;
        Serial.println("Sequence start: OUT valid, waiting IN");
      }
    }
  }

  if (sensorData.objectInDetected != lastInDetected) {
    Serial.println("IN: " + String(lastInDetected ? "DETECTED" : "CLEAR") +
                   " -> " + String(sensorData.objectInDetected ? "DETECTED" : "CLEAR"));
  }
  if (sensorData.objectOutDetected != lastOutDetected) {
    Serial.println("OUT: " + String(lastOutDetected ? "DETECTED" : "CLEAR") +
                   " -> " + String(sensorData.objectOutDetected ? "DETECTED" : "CLEAR"));
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
  jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);

  // Validasi utama lampu:
  // - jumlahOrang > 0  => lampu wajib ON
  // - jumlahOrang == 0 => lampu wajib OFF
  // LDR tidak lagi dipakai sebagai syarat mematikan lampu saat ruangan berisi orang.
  bool lampON = jumlahOrang > 0;
  lampStatus = lampON
    ? "AUTO ON (" + String(jumlahOrang) + " orang)"
    : "AUTO OFF (KOSONG)";
  
  // Update relay hanya jika ada perubahan
  if (lampON != lastLamp1 || lampStatus != lastLampStatus) {
    Serial.println("💡 Lamp Control Change: " + lampStatus);
    Serial.println("   Jumlah orang: " + String(jumlahOrang));
    Serial.println("   Rule: jumlahOrang > 0 = LAMPU ON, jumlahOrang = 0 = LAMPU OFF");
    
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

void syncDeviceControlsAfterPeopleChange(String reason) {
  updateSensorData();

  String lampStatus = currentData.lampStatus;
  kontrolLampu(lampStatus);
  currentData.lampStatus = lampStatus;

  String acStatus = currentData.acStatus;
  int setTemp = currentData.setTemp;
  kontrolAC(acStatus, setTemp);
  currentData.acStatus = acStatus;
  currentData.setTemp = setTemp;

  forceFullUpdate = true;
  updateTFT(acStatus, setTemp, currentData.suhuRuang);
  lastJumlahOrang = jumlahOrang;

  Serial.println("🔄 DEVICE SYNC AFTER PEOPLE CHANGE: " + reason);
  Serial.println("   People: " + String(jumlahOrang));
  Serial.println("   Lamp  : " + normalizeLampStatus());
  Serial.println("   AC    : " + normalizeACStatus());
}

// ================= UPDATE SENSOR DATA =================
// Fungsi ini menyinkronkan currentData dari nilai cache sensor terbaru
void updateSensorData() {
  currentData.jumlahOrang = jumlahOrang;

  // ── Gunakan cache DHT dari readDHTRealtime() — tidak baca ulang langsung ──
  // (readDHTRealtime sudah dipanggil di readSensors() setiap loop)
  // currentData.suhuRuang dan currentData.humidity sudah diupdate di sana

  // Validasi fallback jika cache masih NAN (misal pertama kali boot)
  if (isnan(currentData.suhuRuang) || currentData.suhuRuang < -40 || currentData.suhuRuang > 80) {
    currentData.suhuRuang = 25.0; // Fallback default
    Serial.println("⚠️ DHT22: Cache suhu tidak valid, pakai default 25.0°C");
  }
  if (isnan(currentData.humidity) || currentData.humidity < 0 || currentData.humidity > 100) {
    currentData.humidity = 60.0; // Fallback default
    Serial.println("⚠️ DHT22: Cache humidity tidak valid, pakai default 60.0%");
  }

  // ── LDR: gunakan readLDRRealtime() yang sudah dipanggil di readSensors() ──
  // currentData.lightLevel sudah diupdate di sana (mapping 0-4095 → 0-1500 lux)

  currentData.timestamp  = millis();
  currentData.proximity1 = sensorData.objectInDetected;
  currentData.proximity2 = sensorData.objectOutDetected;

  if (WiFi.isConnected()) {
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI   = WiFi.RSSI();
  } else {
    currentData.wifiStatus = "Offline";
    currentData.wifiRSSI   = 0;
  }

  // ── Debug periodik setiap 30 detik ──
  static unsigned long lastSensorDebug = 0;
  if (millis() - lastSensorDebug > 30000) {
    Serial.println("📊 Sensor Update:");
    Serial.println("   Suhu     : " + String(currentData.suhuRuang, 1) + "°C");
    Serial.println("   Humidity : " + String(currentData.humidity, 1) + "%");
    Serial.println("   Cahaya   : " + String(currentData.lightLevel) + " lux");
    Serial.println("   Orang    : " + String(currentData.jumlahOrang));
    Serial.println("   WiFi RSSI: " + String(currentData.wifiRSSI) + " dBm");
    lastSensorDebug = millis();
  }
}

// ================= RESET FUNCTION =================
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

// Enhanced AC Control API — GET /ac/control
// Sesuai API docs: query params hanya device_id dan location
void checkACControlAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  // URL-encode spasi dengan %20 agar query string valid
  String loc = String(DEVICE_LOCATION);
  loc.replace(" ", "%20");

  // Hanya kirim query param yang didukung API (device_id & location)
  String queryParams = "?device_id=" + String(DEVICE_ID) + "&location=" + loc;

  String response;
  bool requestSuccess = false;

#if PREFER_HTTP
  // Langsung pakai HTTP
  String httpACURL = String(acControlURL);
  httpACURL.replace("https://", "http://");
  httpACURL += queryParams;
  requestSuccess = makeHTTPRequest(httpACURL.c_str(), "GET", "", response);
#else
  // Coba HTTPS dulu, fallback HTTP
  String httpsURL = String(acControlURL) + queryParams;
  requestSuccess = makeHTTPSRequest(httpsURL.c_str(), "GET", "", response);
  if (!requestSuccess) {
    String httpACURL = String(acControlURL);
    httpACURL.replace("https://", "http://");
    httpACURL += queryParams;
    requestSuccess = makeHTTPRequest(httpACURL.c_str(), "GET", "", response);
  }
#endif

  if (requestSuccess && response.length() > 0) {
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (!error && doc["success"]) {
      Serial.println("✅ AC Control data diterima dari API");

      // Ambil data control dari response sesuai struktur API docs
      JsonObject data = doc["data"];
      if (!data.isNull()) {
        bool manualOverrideAPI = data["manual_override"] | false;
        String controlMode     = data["control_mode"]   | "auto";
        bool ac1               = data["ac1_status"]     | false;
        bool ac2               = data["ac2_status"]     | false;
        int  temp1             = data["ac1_temperature"] | 24;
        int  temp2             = data["ac2_temperature"] | 24;

        Serial.println("   Control Mode : " + controlMode);
        Serial.println("   Manual       : " + String(manualOverrideAPI ? "YES" : "NO"));
        Serial.println("   AC1          : " + String(ac1 ? "ON" : "OFF") + " @ " + String(temp1) + "°C");
        Serial.println("   AC2          : " + String(ac2 ? "ON" : "OFF") + " @ " + String(temp2) + "°C");

        // Jika server override manual, terapkan ke hardware
        if (manualOverrideAPI && controlMode == "manual") {
          manualOverride = true;
          autoMode = false;
          Serial.println("⚙️ Manual override dari dashboard diterapkan");
        } else if (controlMode == "auto") {
          manualOverride = false;
          autoMode = true;
          Serial.println("⚙️ Mode otomatis aktif dari dashboard");
        }
      }
    } else {
      Serial.println("⚠️ AC Control API response tidak valid");
      if (response.length() > 0) {
        Serial.println("   Response: " + response.substring(0, 200));
      }
    }
  } else {
    Serial.println("⚠️ AC Control API tidak dapat dijangkau");
  }
}

// Enhanced change detection
bool hasSignificantChange() {
  // Deteksi perubahan LANGSUNG untuk semua sensor - KIRIM SETIAP PERUBAHAN
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
  
  // Perubahan suhu (LANGSUNG KIRIM)
  if (abs(currentData.suhuRuang - lastTemp) >= TEMP_CHANGE_THRESHOLD) {
    changeReason = "TEMPERATURE_CHANGE: " + String(lastTemp, 1) + "°C → " + String(currentData.suhuRuang, 1) + "°C";
    Serial.println("🔄 " + changeReason);
    lastTemp = currentData.suhuRuang;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
    hasChange = true;
  }
  
  // Perubahan kelembaban (LANGSUNG KIRIM)
  if (abs(currentData.humidity - lastHumidity) >= HUMIDITY_CHANGE_THRESHOLD) {
    changeReason = "HUMIDITY_CHANGE: " + String(lastHumidity, 1) + "% → " + String(currentData.humidity, 1) + "%";
    Serial.println("🔄 " + changeReason);
    lastHumidity = currentData.humidity;
    
    // LANGSUNG KIRIM KE DATABASE
    if (canSendUpdate()) {
      sendDataToAPI(changeReason);
    }
    hasChange = true;
  }
  
  // Perubahan cahaya (LANGSUNG KIRIM)
  if (abs(currentData.lightLevel - lastLightLevel) >= LIGHT_CHANGE_THRESHOLD) {
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

// ================= DHT REAL-TIME DENGAN CACHE (min 2s antar baca) =================
// DHT22 membutuhkan minimal 2 detik antar pembacaan — jika dibaca lebih cepat hasilnya NaN
static float dhtCacheTemp = 0.0;
static float dhtCacheHum  = 0.0;
static unsigned long lastDHTRead = 0;

void readDHTRealtime() {
  unsigned long now = millis();
  // Baca hanya jika sudah lewat 2.5 detik dari pembacaan terakhir
  if (now - lastDHTRead < 2500) return;
  lastDHTRead = now;

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t) && t > -40.0 && t < 80.0) {
    dhtCacheTemp = t;
    currentData.suhuRuang = t;
  } else {
    Serial.println("⚠️ DHT22: Pembacaan suhu gagal, pakai nilai cache: " + String(dhtCacheTemp, 1) + "°C");
    Serial.println("   -> ALASAN ERROR KEMUNGKINAN DARI HARDWARE ATAU WIRING: ");
    Serial.println("      1. Kabel DATA (GPIO" + String(DHTPIN) + "), VCC (3.3V), atau GND kendor/copot.");
    Serial.println("      2. DHT22 perlu Resistor Pull-up (4.7k-10k Ohm) antara VCC dan DATA (Jika bukan modul DHT siap pakai).");
    Serial.println("      3. Sensor mengalami 'Lockup/Hang' (Solusi: cabut-pasang kabel VCC DHT22 dari ESP32).");
  }

  if (!isnan(h) && h >= 0.0 && h <= 100.0) {
    dhtCacheHum = h;
    currentData.humidity = h;
  } else {
    Serial.println("⚠️ DHT22: Pembacaan humidity gagal, pakai nilai cache: " + String(dhtCacheHum, 1) + "%");
  }
}

// ================= LDR REAL-TIME DENGAN CACHE (seperti DHT) =================
static int ldrCache = 0;
static unsigned long lastLDRRead = 0;

void readLDRRealtime() {
#if !LDR_ENABLED
  // Sensor LDR tidak terpasang — selalu 0 lux
  currentData.lightLevel = 0;
  return;
#endif

  unsigned long now = millis();
  // Baca hanya setiap 1 detik (menghindari noise dari floating pin)
  if (now - lastLDRRead < 1000) return;
  lastLDRRead = now;

  // Ambil beberapa sampel lalu rata-ratakan (filter noise)
  long total = 0;
  const int samples = 5;
  for (int i = 0; i < samples; i++) {
    total += analogRead(LDR_PIN);
    delayMicroseconds(200);
  }
  int raw = total / samples;

  // Validasi: noise dari floating pin biasanya < 50 atau sangat fluktuatif
  // Jika raw terlalu rendah dan sebelumnya juga rendah, kemungkinan tidak ada sensor
  if (raw < 30) {
    // Nilai terlalu rendah — kemungkinan tidak ada sensor terpasang
    currentData.lightLevel = 0;
    return;
  }

  // Map 0-4095 → 0-1000 lux (API docs: light_level max 1000)
  int mapped = map(raw, 0, 4095, 0, 1000);
  mapped = constrain(mapped, 0, 1000);

  ldrCache = mapped;
  currentData.lightLevel = mapped;
}

// ================= API FIELD NORMALIZER =================
// ac_status: API hanya terima "ON", "OFF", "AC 1+2"
// Derivasi dari state hardware (lastAC1, lastAC2) agar tidak parsing string
String normalizeACStatus() {
  if (lastAC1 && lastAC2) return "AC 1+2";
  if (lastAC1 || lastAC2) return "ON";
  return "OFF";
}

// lamp_status: API hanya terima "ON" atau "OFF"
String normalizeLampStatus() {
  return currentData.lamp1 ? "ON" : "OFF";
}

// ================= SEND DATA TO API =================
// JSON sesuai dokumentasi API /api/sensor/data (POST)
void sendDataToAPI(String reason) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Tidak bisa kirim — WiFi tidak terhubung");
    return;
  }

  // ── Baca sensor real-time sebelum kirim ──
  readDHTRealtime();
  readLDRRealtime();

  // ── Sinkronisasi state ──
  currentData.proximity1  = sensorData.objectInDetected;
  currentData.proximity2  = sensorData.objectOutDetected;
  currentData.jumlahOrang = jumlahOrang;

  // ── Normalisasi nilai agar sesuai validasi API ──
  String apiACStatus   = normalizeACStatus();          // "ON" / "OFF" / "AC 1+2"
  String apiLampStatus = normalizeLampStatus();         // "ON" / "OFF"
  int    apiLightLevel = constrain((int)currentData.lightLevel, 0, 1000); // max 1000
  int    apiPeopleCount = constrain((int)currentData.jumlahOrang, 0, 100);// max 100

  // ── Validasi suhu & humidity (range API: suhu -10~60, humidity 0-100) ──
  float apiTemp = currentData.suhuRuang;
  if (isnan(apiTemp) || apiTemp < -10.0 || apiTemp > 60.0) apiTemp = 25.0;

  float apiHumidity = currentData.humidity;
  if (isnan(apiHumidity) || apiHumidity < 0.0 || apiHumidity > 100.0) apiHumidity = 60.0;

  // ── Bangun JSON — field PERSIS sesuai API docs /api/sensor/data ──
  StaticJsonDocument<512> doc;

  // Optional
  doc["device_id"]    = DEVICE_ID;
  doc["location"]     = DEVICE_LOCATION;
  // Required
  doc["people_count"] = apiPeopleCount;  // integer, 0-100, WAJIB
  doc["ac_status"]    = apiACStatus;     // "ON"/"OFF"/"AC 1+2", WAJIB
  // Optional sensor
  if (currentData.setTemp > 0) {
    // Kirim set_temperature hanya saat AC ON (null saat OFF, sesuai DB default null)
    doc["set_temperature"] = constrain((int)currentData.setTemp, 16, 30);
  }
  doc["room_temperature"] = round(apiTemp * 100.0) / 100.0;   // decimal(5,2)
  doc["humidity"]         = round(apiHumidity * 100.0) / 100.0; // decimal(5,2)
  doc["light_level"]      = apiLightLevel;                       // integer, 0-1000
  doc["lamp_status"]      = apiLampStatus;                       // "ON"/"OFF"
  doc["proximity_in"]     = currentData.proximity1;              // boolean
  doc["proximity_out"]    = currentData.proximity2;              // boolean
  doc["wifi_rssi"]        = (int)WiFi.RSSI();                   // integer, -100~0
  doc["status"]           = "active";                            // string
  doc["timestamp"]        = (unsigned long)millis();             // millis() → kolom device_timestamp di DB

  String jsonString;
  serializeJson(doc, jsonString);

  // ── Log ke Serial ──
  Serial.println("📤 Kirim ke API...");
  Serial.println("   Reason  : " + reason);
  Serial.println("   AC      : " + apiACStatus + " | Lamp: " + apiLampStatus +
                 " | People: " + String(apiPeopleCount));
  Serial.println("   Temp    : " + String(apiTemp, 1) + "°C | Hum: " +
                 String(apiHumidity, 1) + "% | Light: " + String(apiLightLevel) + " lux");
  Serial.println("   Payload : " + jsonString);

  String response;
  bool   dataSent = false;

#if PREFER_HTTP
  // ── Langsung pakai HTTP (server tidak support HTTPS) ──
  String httpURL = String(apiURL);
  httpURL.replace("https://", "http://");
  if (makeHTTPRequest(httpURL.c_str(), "POST", jsonString.c_str(), response)) {
    Serial.println("✅ Terkirim via HTTP!");
    dataSent = true;
  } else {
    Serial.println("❌ Gagal kirim via HTTP");
    if (response.length() > 0) Serial.println("   Response  : " + response.substring(0, 300));
    return;
  }
#else
  // ── Coba HTTPS dulu ──
  if (makeHTTPSRequest(apiURL, "POST", jsonString.c_str(), response)) {
    Serial.println("✅ Terkirim via HTTPS!");
    dataSent = true;
  } else {
    // ── Fallback ke HTTP ──
    Serial.println("⚠️ HTTPS gagal → coba HTTP...");
    String httpURL = String(apiURL);
    httpURL.replace("https://", "http://");
    if (makeHTTPRequest(httpURL.c_str(), "POST", jsonString.c_str(), response)) {
      Serial.println("✅ Terkirim via HTTP fallback!");
      dataSent = true;
    } else {
      Serial.println("❌ Gagal kirim — HTTPS & HTTP keduanya error");
      Serial.println("   SSL Error : " + sslStatus.lastError);
      if (response.length() > 0) Serial.println("   Response  : " + response.substring(0, 300));
      return;
    }
  }
#endif

  if (dataSent) {
    updatePreviousData();
    lastAPIUpdate   = millis();
    lastForceUpdate = millis();

    Serial.println("   Server Response: " + response.substring(0, 300));

    // ── Parse response ──
    StaticJsonDocument<512> resDoc;
    DeserializationError err = deserializeJson(resDoc, response);
    if (!err) {
      bool success = resDoc["success"] | false;
      if (success) {
        Serial.println("✅ Server: data tersimpan — ID: " +
                       String(resDoc["data"]["id"] | 0));
      } else {
        // 422 atau error lain: cetak detail validasi
        Serial.println("⚠️ Server error: " + resDoc["message"].as<String>());
        if (resDoc.containsKey("errors")) {
          JsonObject errors = resDoc["errors"].as<JsonObject>();
          for (JsonPair kv : errors) {
            Serial.println("   [" + String(kv.key().c_str()) + "] " +
                           kv.value()[0].as<String>());
          }
        }
      }
    }
  }
}

// ================= ENHANCED SETUP =================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Initialize watchdog timer (60 seconds timeout - lebih longgar untuk mencegah restart)
  esp_task_wdt_init(60, true);
  esp_task_wdt_add(NULL);
  
  Serial.println("⚠️ ANTI-RESTART MODE ENABLED");
  Serial.println("   Watchdog: 60s timeout (extended)");
  Serial.println("   Auto shutdown/startup: DISABLED");
  Serial.println("   Memory monitoring: ENABLED");
  
  // Set CPU frequency to stable level
  setCpuFrequencyMhz(240); // Set to 240MHz for stable operation
  Serial.println("CPU Frequency: " + String(getCpuFrequencyMhz()) + " MHz");
  
  Serial.println("\n================================");
  Serial.println("  SMART AC - ENHANCED SSL + PROXY HOSTING MODE");
  Serial.println("  Device: ESP32 Enhanced Proximity Sensor - Prodi Elektro");
  Serial.println("  Location: " + String(DEVICE_LOCATION));
  Serial.println("  Hosting: " + String(hostingDomain));
  Serial.println("  Features: SSL + Proxy Detection + Enhanced API");
  Serial.println("  SSL Method: ROOT_CA (Secure)");
  Serial.println("  Watchdog: 30s timeout enabled");
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
  digitalWrite(RELAY_LAMP1, HIGH);  // Start OFF; kontrolLampu() akan ON jika jumlahOrang > 0
  
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN_IN), proximityInInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN_OUT), proximityOutInterrupt, CHANGE);
  
  Serial.println("✓ Proximity interrupts configured for immediate response");
  
  dht.begin();  // Hanya satu DHT22
  
  // Wait for DHT22 sensor to stabilize
  delay(2000);
  
  // Test DHT22 sensor
  Serial.println("\n=== TESTING DHT22 SENSOR ===");
  float testTemp = dht.readTemperature();
  float testHum = dht.readHumidity();
  
  Serial.println("DHT22 (GPIO13): T=" + String(testTemp) + "°C H=" + String(testHum) + "%");
  
  if (isnan(testTemp) || isnan(testHum)) {
    Serial.println("⚠️ DHT22 not responding - check wiring");
    Serial.println("   Expected: VCC=3.3V, GND=GND, DATA=GPIO13");
  } else {
    Serial.println("✓ DHT22 OK");
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
  
  Serial.println("\n=== TESTING PROXIMITY SENSORS ===");
  Serial.println("Calibrating proximity sensors...");
  Serial.println("Please ensure no objects are near the sensors");

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
  Serial.println("Proximity sensor test completed");
  Serial.println("Expected behavior for immediate counting:");
  Serial.println("- Sensors read HIGH (1) when clear");
  Serial.println("- Sensors read LOW (0) when object detected");
  Serial.println("- Every detection is counted immediately");
  Serial.println("- Only cooldown prevents double counting (200ms)");
  Serial.println("- IN sensor = +1 person");
  Serial.println("- OUT sensor = -1 person (if count > 0)");
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
  
  // Initialize time synchronization for auto shutdown
  if (WiFi.status() == WL_CONNECTED) {
    initializeTime();
  }
  
  Serial.println("\n✅ ENHANCED SYSTEM READY - ANTI-RESTART MODE!");
  Serial.println("Location: Ruang Prodi Dosen Teknik Elektro - UNJA");
  Serial.println("Auto Shutdown: " + String(autoShutdownEnabled ? "ENABLED" : "DISABLED (SAFE MODE)"));
  Serial.println("Auto Startup: " + String(autoStartupEnabled ? "ENABLED" : "DISABLED (SAFE MODE)"));
  Serial.println("Watchdog Timer: 60 seconds (Extended for stability)");
  Serial.println("Memory Monitoring: ACTIVE");
  Serial.println("System Mode: ANTI-RESTART PROTECTION");
  Serial.println("WiFi Network: Teknik Elektro");
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("\nEnhanced Pin Configuration:");
  Serial.println("- PROXIMITY_IN: GPIO 32 (Enhanced with SSL logging)");
  Serial.println("- PROXIMITY_OUT: GPIO 14 (Enhanced with SSL logging)");
  Serial.println("- DHT22: GPIO 13 (Enhanced error handling)");
  Serial.println("- LDR: GPIO 34 (Enhanced mapping)");
  Serial.println("- IR LED: GPIO 4 (Enhanced IR control)");
  Serial.println("- RELAY LAMP: GPIO 25 (Enhanced NC control)");
  Serial.println("\nEnhanced Features:");
  Serial.println("- 🔒 SSL/HTTPS: Secure communication with hosting");
  Serial.println("- 🌐 PROXY DETECTION: Automatic proxy detection and handling");
  Serial.println("- 📡 ENHANCED API: JSON data with SSL and proxy info");
  Serial.println("- ⚡ FAST RESPONSE: Optimized debouncing and detection");
  Serial.println("- 🔄 AUTO RECOVERY: Enhanced error handling and retry logic");
  Serial.println("- 📊 DETAILED LOGGING: SSL and proxy status monitoring");
  Serial.println("- 💡 SMART LAMP: Based on people count");
  Serial.println("- 🌡️ SMART AC: Based on people count with enhanced IR");
  Serial.println("- 📱 ENHANCED TFT: SSL and proxy status indicators");
  Serial.println("\nLocation Configuration:");
  Serial.println("- WiFi Network: Teknik Elektro");
  Serial.println("- Location: Ruang Prodi Dosen Teknik Elektro");
  Serial.println("- University: Universitas Jambi (UNJA)");
  Serial.println("- Department: Teknik Elektro");
  Serial.println("\nEnhanced Serial Commands:");
  Serial.println("  * reset, test, add, sub, sensors");
  Serial.println("  * ssl - Show SSL status and test connection");
  Serial.println("  * proxy - Show proxy detection status");
  Serial.println("  * hosting - Test hosting connectivity");
  Serial.println("  * cert - Get certificate fingerprint");
  Serial.println("  * help - Show all enhanced commands");
  Serial.println("================================\n");
  
  // Initialize SSL and proxy detection state
  sslStatus.isConfigured = false;
  sslStatus.lastConnectionSuccess = false;
  sslStatus.consecutiveFailures = 0;
  proxyState.lastProxyCheck = 0;
  proxyState.consecutiveFailures = 0;
  proxyState.hostingReachable = false;
  
  // Reset people counter to ensure clean start
  jumlahOrang = 0;
  lastJumlahOrang = -1;
  Serial.println("✓ People counter reset to 0");
  
  // Reset watchdog after successful setup
  esp_task_wdt_reset();
}

// ================= MAIN LOOP =================
void loop() {
  // Reset watchdog timer at start of each loop
  esp_task_wdt_reset();
  
  // Enhanced memory monitoring untuk mencegah restart
  static unsigned long lastMemoryCheck = 0;
  if (millis() - lastMemoryCheck > 10000) { // Check every 10 seconds
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t minFreeHeap = ESP.getMinFreeHeap();
    
    Serial.println("🧠 MEMORY STATUS:");
    Serial.println("   Free Heap: " + String(freeHeap) + " bytes");
    Serial.println("   Min Free Heap: " + String(minFreeHeap) + " bytes");
    Serial.println("   Heap Fragmentation: " + String(100 - (ESP.getMaxAllocHeap() * 100) / freeHeap) + "%");
    
    if (freeHeap < 20000) {
      Serial.println("⚠️ LOW MEMORY WARNING - Reducing operations");
      delay(2000); // Give system time to recover
      esp_task_wdt_reset(); // Reset watchdog after delay
      return;
    }
    
    if (freeHeap < 10000) {
      Serial.println("🚨 CRITICAL MEMORY - Emergency cleanup");
      // Force garbage collection
      delay(5000);
      esp_task_wdt_reset();
      return;
    }
    
    lastMemoryCheck = millis();
  }
  
  // Skip auto shutdown check (DISABLED)
  // checkAutoShutdown();
  
  // Skip normal operations if system is in auto shutdown mode (DISABLED)
  // if (systemAutoShutdown) {
  //   delay(1000);
  //   return;
  // }
  
  // Always read sensors for immediate response
  readSensors();
  detectPeopleSequenced();
  
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
  
  // Auto-reset if too many false detections (every 10 minutes)
  static unsigned long lastAutoResetCheck = 0;
  
  if (millis() - lastAutoResetCheck > 600000) { // Every 10 minutes
    Serial.println("ℹ️ System health check - 10 minutes elapsed");
    lastAutoResetCheck = millis();
  }
  
  // Send heartbeat every 5 minutes (hanya untuk monitoring koneksi)
  unsigned long now = millis();
  if (now - lastHeartbeat > HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    lastHeartbeat = now;
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
      Serial.println("🧪 Manual sensor test:");
      bool inState = digitalRead(PROXIMITY_PIN_IN);
      bool outState = digitalRead(PROXIMITY_PIN_OUT);
      Serial.println("   PROXIMITY:");
      Serial.println("     IN_PIN=" + String(inState) + " (Object=" + String(!inState ? "DETECTED" : "CLEAR") + ")");
      Serial.println("     OUT_PIN=" + String(outState) + " (Object=" + String(!outState ? "DETECTED" : "CLEAR") + ")");
      
      // Mengambil dari cache. Pemanggilan DHT langsung akan memicu error NaN karena melanggar ritme 2 detik.
      Serial.println("   DHT22 (GPIO" + String(DHTPIN) + "):");
      Serial.println("     Temperature=" + String(currentData.suhuRuang) + "°C");
      Serial.println("     Humidity=" + String(currentData.humidity) + "%");
      
      int ldrRaw = analogRead(LDR_PIN);
      int ldrMapped = map(ldrRaw, 0, 4095, 0, 1000);
      Serial.println("   LDR (GPIO35):");
      Serial.println("     Raw=" + String(ldrRaw) + " Mapped=" + String(ldrMapped) + " lux");
    }
    else if (command == "add") {
      Serial.println("➕ Manual add person");
      jumlahOrang++;
      jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
      syncDeviceControlsAfterPeopleChange("MANUAL_ADD");
    }
    else if (command == "sub") {
      Serial.println("➖ Manual subtract person");
      if (jumlahOrang > 0) jumlahOrang--;
      syncDeviceControlsAfterPeopleChange("MANUAL_SUB");
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
    else if (command == "help") {
      Serial.println("📋 Enhanced Available Commands:");
      Serial.println("   reset    - Reset people counter to 0");
      Serial.println("   test     - Test all sensors (proximity, DHT22, LDR)");
      Serial.println("   sensors  - Show current sensor readings");
      Serial.println("   add      - Manually add 1 person");
      Serial.println("   sub      - Manually subtract 1 person");
      Serial.println("   ssl      - Show SSL status and test connection");
      Serial.println("   proxy    - Show proxy detection status");
      Serial.println("   hosting  - Show hosting connection status");
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
      Serial.println("🔍 Enhanced Current Sensor Readings:");
      Serial.println("   People in room: " + String(jumlahOrang));
      
      // Mengambil dari cache.
      Serial.println("   DHT22 (GPIO" + String(DHTPIN) + "): T=" + String(currentData.suhuRuang, 1) + "°C H=" + String(currentData.humidity, 1) + "%");
      
      int ldrRaw = analogRead(LDR_PIN);
      int ldrMapped = map(ldrRaw, 0, 4095, 0, 1000);
      Serial.println("   LDR (GPIO35): Raw=" + String(ldrRaw) + " Mapped=" + String(ldrMapped) + " lux");
      
      Serial.println("   Enhanced Status:");
      Serial.println("     Lamp: " + currentData.lampStatus);
      Serial.println("     AC: " + currentData.acStatus);
      Serial.println("     SSL: " + String(sslStatus.lastConnectionSuccess ? "SECURE" : "FAILED"));
      Serial.println("     Proxy: " + String(proxyState.isProxyDetected ? "DETECTED" : "DIRECT"));
      Serial.println("     Hosting: " + String(proxyState.hostingReachable ? "REACHABLE" : "UNREACHABLE"));
      Serial.println("     Logic: Enhanced SSL + Proxy aware communication");
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
        detectPeopleSequenced();
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
          syncDeviceControlsAfterPeopleChange("SIMPLE_TEST_IN");
        }
        
        if (currentOut && !lastTestOut && jumlahOrang > 0) {
          jumlahOrang--;
          Serial.println("🚶 ← OUT DETECTED! Count: " + String(jumlahOrang));
          syncDeviceControlsAfterPeopleChange("SIMPLE_TEST_OUT");
        }
        
        lastTestIn = currentIn;
        lastTestOut = currentOut;
        
        delay(50);
      }
      Serial.println("   Simple test completed. Final count: " + String(jumlahOrang));
    }
  }
  
  // Jaga loop tetap responsif untuk proximity tanpa menahan polling sensor.
  yield();
  
  // Reset watchdog sebelum akhir loop
  esp_task_wdt_reset();
}

// ================= AUTO SHUTDOWN FUNCTIONS =================

void initializeTime() {
  Serial.println("🕐 INITIALIZING TIME SYNCHRONIZATION");
  Serial.println("   NTP Server: " + String(ntpServer));
  Serial.println("   Timezone: WIB (UTC+" + String(TIMEZONE_OFFSET_HOURS) + ")");
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // Wait for time synchronization
  int attempts = 0;
  while (!time(nullptr) && attempts < 10) {
    Serial.print(".");
    delay(1000);
    attempts++;
  }
  
  if (time(nullptr)) {
    Serial.println("\n✅ TIME SYNCHRONIZED");
    Serial.println("   Current time: " + getCurrentTimeString());
    Serial.println("   Auto shutdown: " + String(AUTO_SHUTDOWN_HOUR) + ":00 WIB daily");
    Serial.println("   Auto startup: " + String(AUTO_STARTUP_HOUR) + ":00 WIB daily");
    
    // Reset auto shutdown/startup triggers for new day
    autoShutdownTriggered = false;
    autoStartupTriggered = false;
  } else {
    Serial.println("\n❌ TIME SYNC FAILED - Auto shutdown/startup disabled");
    autoShutdownEnabled = false;
    autoStartupEnabled = false;
  }
}

void checkAutoShutdown() {
  if (!autoShutdownEnabled && !autoStartupEnabled) return;
  
  unsigned long now = millis();
  
  // Check time every 30 seconds
  if (now - lastTimeCheck < 30000) return;
  lastTimeCheck = now;
  
  // Check for auto shutdown (17:00)
  if (isAutoShutdownTime() && !autoShutdownTriggered && !systemAutoShutdown) {
    Serial.println("🕐 AUTO SHUTDOWN TIME REACHED: " + getCurrentTimeString());
    performAutoShutdown();
    autoShutdownTriggered = true;
    systemAutoShutdown = true;
  }
  
  // Check for auto startup (06:00)
  if (isAutoStartupTime() && !autoStartupTriggered && systemAutoShutdown) {
    Serial.println("🕐 AUTO STARTUP TIME REACHED: " + getCurrentTimeString());
    performAutoStartup();
    autoStartupTriggered = true;
    systemAutoShutdown = false;
  }
  
  // Reset triggers at midnight (00:00)
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
      if (autoShutdownTriggered || autoStartupTriggered) {
        autoShutdownTriggered = false;
        autoStartupTriggered = false;
        Serial.println("🔄 Auto shutdown/startup triggers reset for new day");
      }
    }
  }
  
  // Log current time every 10 minutes for monitoring
  if (now - lastAutoShutdownLog > 600000) { // 10 minutes
    lastAutoShutdownLog = now;
    String systemStatus = systemAutoShutdown ? "SHUTDOWN MODE" : "NORMAL MODE";
    Serial.println("🕐 Current time: " + getCurrentTimeString() + 
                   " | System: " + systemStatus);
    Serial.println("   Auto shutdown: " + String(autoShutdownEnabled ? "ENABLED" : "DISABLED") +
                   " at " + String(AUTO_SHUTDOWN_HOUR) + ":00 WIB");
    Serial.println("   Auto startup: " + String(autoStartupEnabled ? "ENABLED" : "DISABLED") +
                   " at " + String(AUTO_STARTUP_HOUR) + ":00 WIB");
  }
}

void performAutoShutdown() {
  Serial.println("🔴 PERFORMING AUTO SHUTDOWN AT 17:00");
  Serial.println("   Shutting down all systems...");
  
  // Force people count to 0
  jumlahOrang = 0;
  
  // Turn off AC
  currentData.acStatus = "AUTO SHUTDOWN 17:00";
  currentData.setTemp = 0;
  
  // Turn off lights
  currentData.lampStatus = "AUTO SHUTDOWN 17:00";
  currentData.lamp1 = false;
  digitalWrite(RELAY_LAMP1, HIGH); // NC relay - HIGH = force off
  
  // Send IR commands to turn off AC
  ac1.off();
  ac1.send();
  delay(500);
  ac2.off();
  ac2.send();
  
  // Update display
  forceFullUpdate = true;
  updateTFT(currentData.acStatus, currentData.setTemp, currentData.suhuRuang);
  
  // Send final status to API
  if (WiFi.status() == WL_CONNECTED) {
    sendDataToAPI("AUTO SHUTDOWN 17:00 - All systems turned off");
  }
  
  // Display shutdown message on TFT
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_RED);
  tft.setCursor(50, 80);
  tft.println("AUTO SHUTDOWN");
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(80, 120);
  tft.println("17:00 WIB");
  tft.setCursor(60, 150);
  tft.println("All systems OFF");
  tft.setTextSize(1);
  tft.setCursor(70, 180);
  tft.println("Prodi Teknik Elektro - UNJA");
  
  Serial.println("✅ AUTO SHUTDOWN COMPLETED");
  Serial.println("   AC: OFF");
  Serial.println("   Lights: OFF");
  Serial.println("   People count: 0");
  Serial.println("   System will restart automatically at 06:00 tomorrow");
  Serial.println("   Next auto shutdown: Tomorrow 17:00");
}

String getCurrentTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Time not available";
  }
  
  char timeString[64];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S WIB", &timeinfo);
  return String(timeString);
}

bool isAutoShutdownTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return false;
  }
  
  // Check if current time is 17:00 (5 PM)
  return (timeinfo.tm_hour == AUTO_SHUTDOWN_HOUR && 
          timeinfo.tm_min >= AUTO_SHUTDOWN_MINUTE && 
          timeinfo.tm_min < AUTO_SHUTDOWN_MINUTE + 1);
}

void performAutoStartup() {
  Serial.println("🟢 PERFORMING AUTO STARTUP AT 06:00");
  Serial.println("   Starting up all systems...");
  
  // Reset people count to 0 (fresh start)
  jumlahOrang = 0;
  
  // Enable AC control (but don't turn on immediately - wait for people)
  currentData.acStatus = "READY - AUTO STARTUP 06:00";
  currentData.setTemp = 0;  // Will be set when people enter
  
  // Enable lights control (but don't turn on immediately - wait for low light)
  currentData.lampStatus = "READY - AUTO STARTUP 06:00";
  currentData.lamp1 = false;
  digitalWrite(RELAY_LAMP1, LOW); // NC relay - LOW = allow normal control
  
  // Update display
  forceFullUpdate = true;
  updateTFT(currentData.acStatus, currentData.setTemp, currentData.suhuRuang);
  
  // Send startup status to API
  if (WiFi.status() == WL_CONNECTED) {
    sendDataToAPI("AUTO STARTUP 06:00 - Systems ready for operation");
  }
  
  // Display startup message on TFT
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(50, 80);
  tft.println("AUTO STARTUP");
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(80, 120);
  tft.println("06:00 WIB");
  tft.setCursor(60, 150);
  tft.println("Systems READY");
  tft.setTextSize(1);
  tft.setCursor(70, 180);
  tft.println("Prodi Teknik Elektro - UNJA");
  
  // Wait 3 seconds to show message, then return to normal display
  delay(3000);
  drawUI();
  
  Serial.println("✅ AUTO STARTUP COMPLETED");
  Serial.println("   AC: READY (will activate when people enter)");
  Serial.println("   Lights: READY (will activate based on light level)");
  Serial.println("   People count: 0");
  Serial.println("   Next auto shutdown: Today 17:00");
}

bool isAutoStartupTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return false;
  }
  
  // Check if current time is 06:00 (6 AM)
  return (timeinfo.tm_hour == AUTO_STARTUP_HOUR && 
          timeinfo.tm_min >= AUTO_STARTUP_MINUTE && 
          timeinfo.tm_min < AUTO_STARTUP_MINUTE + 1);
}

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
      Serial.println("   Response Body: " + response.substring(0, 300)); // Cetak body untuk debug
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
  if (systemAutoShutdown) {
    // Kirim status shutdown
    sendDataToAPI("SYSTEM_HEARTBEAT - Auto shutdown mode");
    return;
  }
  
  // Heartbeat untuk monitoring koneksi saja
  sendDataToAPI("SYSTEM_HEARTBEAT - Connection monitoring");
  
  Serial.println("💓 HEARTBEAT SENT - System monitoring only");
  Serial.println("   Note: Real data sent only on sensor changes");
}
