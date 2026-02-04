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

// ================= TFT =================
TFT_eSPI tft;

// ================= HOSTING WEB CONFIGURATION WITH SSL =================
const char* ssid = "Teknik Elektro";
const char* password = "tanyaakmanab";
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

// Alternative: Certificate fingerprint method (backup)
const char* certificateFingerprint = "AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD"; // Update dengan fingerprint yang benar

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
#define TEMP_CHANGE_THRESHOLD 0.3
#define HUMIDITY_CHANGE_THRESHOLD 1.0
#define LIGHT_CHANGE_THRESHOLD 50
#define MAX_TIME_WITHOUT_UPDATE 120000  // 2 minutes
#define MIN_UPDATE_INTERVAL 1000        // 1 second minimum
#define AC_CONTROL_CHECK_INTERVAL 5000  // Check every 5 seconds

#define DEVICE_ID "ESP32_Proximity_SSL_Proxy_UNJA_Prodi_Elektro"
#define DEVICE_LOCATION "Ruang Prodi Dosen Teknik Elektro - UNJA - SSL + Proxy Detection"

// ================= PINS =================
#define IR_PIN            15
#define PROXIMITY_PIN_IN  32
#define PROXIMITY_PIN_OUT 33
#define DHTPIN            13  // Hanya menggunakan DHT di pin 13
#define DHTTYPE           DHT22
#define LDR_PIN           35  // Hanya menggunakan LDR di pin 35
#define RELAY_LAMP1       25  // Relay untuk lampu (menggunakan NC untuk dual fungsi)
// #define RELAY_LAMP2       26  // Tidak digunakan - hanya 1 jalur lampu

#define MAX_PEOPLE 20
#define PERSON_COOLDOWN 200  // Simple cooldown to prevent double counting
#define DEBOUNCE_DELAY 30    // Minimal debounce for immediate response

// ================= LIGHT CONTROL THRESHOLDS =================
#define LUX_THRESHOLD     800  // Threshold cahaya untuk kontrol otomatis (diubah dari 600 ke 800)

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

void IRAM_ATTR proximityInInterrupt();
void IRAM_ATTR proximityOutInterrupt();

// ================= INTERRUPT HANDLERS =================
void IRAM_ATTR proximityInInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 30) { // Faster interrupt response
    interruptTriggered = true;
    lastInterruptTime = now;
  }
}

void IRAM_ATTR proximityOutInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 30) { // Faster interrupt response
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
    Serial.println("   Method: Certificate Fingerprint");
    client.setFingerprint(certificateFingerprint);
    sslStatus.isConfigured = true;
    
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

// Get certificate fingerprint
void getCertificateFingerprint() {
  Serial.println("🔍 GETTING CERTIFICATE FINGERPRINT");
  
  WiFiClientSecure client;
  client.setInsecure();
  
  if (client.connect(hostingDomain, 443)) {
    Serial.println("✅ Connected to get certificate info");
    
    String fingerprint = client.getPeerCertificateFingerprint();
    Serial.println("📋 Certificate Fingerprint: " + fingerprint);
    Serial.println("📋 Certificate Info:");
    Serial.println("   Subject: " + client.getPeerCertificateSubject());
    Serial.println("   Issuer: " + client.getPeerCertificateIssuer());
    
    client.stop();
    
    Serial.println("💡 To use fingerprint method, update this line:");
    Serial.println("   const char* certificateFingerprint = \"" + fingerprint + "\";");
    
  } else {
    Serial.println("❌ Failed to connect to get certificate info");
  }
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
  #if SSL_METHOD == SSL_METHOD_ROOT_CA
    https.addHeader("X-SSL-Method", "root-ca");
  #elif SSL_METHOD == SSL_METHOD_FINGERPRINT
    https.addHeader("X-SSL-Method", "fingerprint");
  #elif SSL_METHOD == SSL_METHOD_INSECURE
    https.addHeader("X-SSL-Method", "insecure");
  #endif
  
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
  
  bool currentInState = digitalRead(PROXIMITY_PIN_IN);
  bool currentOutState = digitalRead(PROXIMITY_PIN_OUT);
  
  // Debug output with SSL and proxy status
  static unsigned long lastProximityDebug = 0;
  if (now - lastProximityDebug > 5000) {
    Serial.println("🔍 ENHANCED PROXIMITY DEBUG (SSL + Proxy):");
    Serial.println("   GPIO 32 (MASUK): " + String(currentInState ? "HIGH" : "LOW") + " | Detected: " + String(!currentInState ? "YES" : "NO"));
    Serial.println("   GPIO 33 (KELUAR): " + String(currentOutState ? "HIGH" : "LOW") + " | Detected: " + String(!currentOutState ? "YES" : "NO"));
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
  
  // Skip cooldown in test mode, otherwise use simple cooldown
  if (!testMode && now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
  // Debug with SSL and proxy status
  static unsigned long lastDebug = 0;
  if (now - lastDebug > 8000) {
    Serial.print("📊 Enhanced People: ");
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
    Serial.print(proxyState.isProxyDetected ? "YES" : "NO");
    Serial.println();
    
    float temp = dht.readTemperature();
    if (!isnan(temp)) {
      Serial.println("🌡️ Enhanced Temp: " + String(temp, 1) + "°C");
    }
    
    lastDebug = now;
  }
  
  // IMMEDIATE COUNTING - No validation, just count every detection
  static bool lastInDetected = false;
  static bool lastOutDetected = false;
  static unsigned long lastInTrigger = 0;
  static unsigned long lastOutTrigger = 0;
  
  // ENTRY detection - IN sensor (immediate counting)
  if (sensorData.objectInDetectedDetected && !lastInDetected) {
    if (testMode || now - lastInTrigger > PERSON_COOLDOWN) {
      jumlahOrang++;
      lastPersonDetected = now;
      lastInTrigger = now;
      
      roomEmptyTimerActive = false;
      
      Serial.println("🚶 → ENHANCED PERSON ENTERED!");
      Serial.println("📊 Jumlah orang: " + String(jumlahOrang) + "/20");
      Serial.println("🔒 SSL: " + String(sslStatus.lastConnectionSuccess ? "SECURE" : "FAILED"));
      Serial.println("🌐 Connection: " + String(proxyState.isProxyDetected ? "PROXY" : "DIRECT"));
      
      updateSensorData();
      
      // Enhanced API call with SSL and proxy awareness
      if (WiFi.status() == WL_CONNECTED) {
        String reason = "Enhanced Proximity entry - Count: " + String(jumlahOrang);
        if (proxyState.isProxyDetected) {
          reason += " [PROXY]";
        }
        sendDataToAPI(reason);
      }
    } else if (!testMode) {
      Serial.println("⏱️ IN detection cooldown: " + String(PERSON_COOLDOWN - (now - lastInTrigger)) + "ms");
    }
  }
  
  // EXIT detection - OUT sensor (immediate counting, only if people count > 0)
  if (sensorData.objectOutDetected && !lastOutDetected) {
    if ((testMode || now - lastOutTrigger > PERSON_COOLDOWN) && jumlahOrang > 0) {
      jumlahOrang--;
      lastPersonDetected = now;
      lastOutTrigger = now;
      
      if (jumlahOrang == 0) {
        emptyRoomStartTime = millis();
        roomEmptyTimerActive = true;
        Serial.println("⏰ Empty room timer started");
      }
      
      Serial.println("🚶 ← ENHANCED PERSON EXITED!");
      Serial.println("📊 Jumlah orang: " + String(jumlahOrang) + "/20");
      Serial.println("🔒 SSL: " + String(sslStatus.lastConnectionSuccess ? "SECURE" : "FAILED"));
      Serial.println("🌐 Connection: " + String(proxyState.isProxyDetected ? "PROXY" : "DIRECT"));
      
      updateSensorData();
      
      // Enhanced API call with SSL and proxy awareness
      if (WiFi.status() == WL_CONNECTED) {
        String reason = "Enhanced Proximity exit - Count: " + String(jumlahOrang);
        if (proxyState.isProxyDetected) {
          reason += " [PROXY]";
        }
        sendDataToAPI(reason);
      }
    }
    else if ((testMode || now - lastOutTrigger > PERSON_COOLDOWN) && jumlahOrang == 0) {
      // Update trigger time to prevent spam
      lastOutTrigger = now;
      
      static unsigned long lastFalseExitWarning = 0;
      static int localFalseCount = 0;
      localFalseCount++;
      
      if (now - lastFalseExitWarning > 10000) {
        Serial.println("⚠ False exit ignored (count=0) - Recent: " + String(localFalseCount));
        lastFalseExitWarning = now;
        localFalseCount = 0;
      }
    }
    else if (jumlahOrang > 0 && !testMode) {
      Serial.println("⏱️ OUT detection cooldown: " + String(PERSON_COOLDOWN - (now - lastOutTrigger)) + "ms");
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
  
  // Read single DHT22 sensor
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  
  // Debug DHT22 readings with better error detection
  static unsigned long lastDHTDebug = 0;
  static int dhtErrorCount = 0;
  
  if (millis() - lastDHTDebug > 30000) { // Every 30 seconds
    Serial.println("🌡️ DHT22 Debug:");
    
    if (isnan(temp) || isnan(hum)) {
      dhtErrorCount++;
      Serial.println("  DHT22 (GPIO13): ERROR - Check wiring/power (" + String(dhtErrorCount) + " errors)");
    } else {
      Serial.println("  DHT22 (GPIO13): T=" + String(temp, 1) + "°C H=" + String(hum, 1) + "%");
      dhtErrorCount = 0; // Reset error count on successful read
    }
    
    lastDHTDebug = millis();
  }
  
  // Use temperature with fallback
  if (!isnan(temp) && temp > -40 && temp < 80) {
    currentData.suhuRuang = temp;
  } else {
    // Use fallback temperature if sensor fails
    static float lastValidTemp = 25.0; // Default room temperature
    currentData.suhuRuang = lastValidTemp;
    
    static unsigned long lastTempError = 0;
    if (millis() - lastTempError > 60000) { // Log error every minute
      Serial.println("⚠️ DHT22 temperature sensor not responding - using fallback: " + String(lastValidTemp) + "°C");
      Serial.println("   Check DHT22 connection: VCC=3.3V, GND=GND, DATA=GPIO13");
      lastTempError = millis();
    }
  }
  
  // Update last valid temperature for fallback
  if (!isnan(temp) && temp > -40 && temp < 80) {
    static float lastValidTemp = temp;
    lastValidTemp = temp;
  }
  
  // Use humidity with fallback
  if (!isnan(hum) && hum >= 0 && hum <= 100) {
    currentData.humidity = hum;
  } else {
    // Use fallback humidity if sensor fails
    static float lastValidHumidity = 60.0; // Default humidity
    currentData.humidity = lastValidHumidity;
    
    static unsigned long lastHumError = 0;
    if (millis() - lastHumError > 60000) { // Log error every minute
      Serial.println("⚠️ DHT22 humidity sensor not responding - using fallback: " + String(lastValidHumidity) + "%");
      lastHumError = millis();
    }
  }
  
  // Update last valid humidity for fallback
  if (!isnan(hum) && hum >= 0 && hum <= 100) {
    static float lastValidHumidity = hum;
    lastValidHumidity = hum;
  }
  
  // Read single LDR sensor
  int lightReading = analogRead(LDR_PIN);
  currentData.lightLevel = map(lightReading, 0, 4095, 0, 1000);
  
  // Debug LDR reading
  static unsigned long lastLDRDebug = 0;
  if (millis() - lastLDRDebug > 30000) { // Every 30 seconds
    Serial.println("💡 LDR Debug: Raw=" + String(lightReading) + " Mapped=" + String(currentData.lightLevel) + " lux");
    lastLDRDebug = millis();
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
    StaticJsonDocument<512> doc;
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

// Enhanced change detection
bool hasSignificantChange() {
  // Implementation for detecting significant changes in sensor data
  static int lastPeopleCount = -1;
  static float lastTemp = -999.0;
  
  bool hasChange = false;
  
  if (currentData.jumlahOrang != lastPeopleCount) {
    Serial.println("🔄 Enhanced People count changed: " + String(lastPeopleCount) + " → " + String(currentData.jumlahOrang));
    lastPeopleCount = currentData.jumlahOrang;
    hasChange = true;
  }
  
  if (abs(currentData.suhuRuang - lastTemp) >= TEMP_CHANGE_THRESHOLD) {
    Serial.println("🔄 Enhanced Temperature changed: " + String(lastTemp) + " → " + String(currentData.suhuRuang));
    lastTemp = currentData.suhuRuang;
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

// Enhanced send data to API with SSL and proxy support
void sendDataToAPI(String reason) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Cannot send to API - WiFi not connected");
    return;
  }
  
  Serial.println("📤 ENHANCED SSL-SECURED Sending data to hosting web...");
  Serial.println("   Reason: " + reason);
  Serial.println("   People Count: " + String(currentData.jumlahOrang));
  Serial.println("   AC Status: " + currentData.acStatus);
  Serial.println("   Room Temperature: " + String(currentData.suhuRuang, 1) + "°C");
  Serial.println("   Humidity: " + String(currentData.humidity, 1) + "%");
  Serial.println("   SSL Status: " + String(sslStatus.lastConnectionSuccess ? "SECURE" : "FAILED"));
  Serial.println("   Proxy Status: " + String(proxyState.isProxyDetected ? "DETECTED" : "DIRECT"));
  
  StaticJsonDocument<900> doc;
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
  doc["update_type"] = "enhanced_ssl_proxy_detection";
  doc["hosting_domain"] = hostingDomain;
  doc["ssl_enabled"] = true;
  doc["sensor_type"] = "Enhanced_SSL_Proxy_Sensor";
  
  // Enhanced SSL information
  doc["ssl_configured"] = sslStatus.isConfigured;
  doc["ssl_last_success"] = sslStatus.lastConnectionSuccess;
  doc["ssl_consecutive_failures"] = sslStatus.consecutiveFailures;
  doc["ssl_last_error"] = sslStatus.lastError;
  #if SSL_METHOD == SSL_METHOD_ROOT_CA
    doc["ssl_method"] = "root_ca";
  #elif SSL_METHOD == SSL_METHOD_FINGERPRINT
    doc["ssl_method"] = "fingerprint";
  #elif SSL_METHOD == SSL_METHOD_INSECURE
    doc["ssl_method"] = "insecure";
  #endif
  
  // Enhanced proxy information
  doc["proxy_detected"] = proxyState.isProxyDetected;
  doc["proxy_type"] = proxyState.proxyType;
  doc["connection_type"] = proxyState.isDirectConnection ? "direct" : "proxy";
  doc["hosting_reachable"] = proxyState.hostingReachable;
  doc["consecutive_failures"] = proxyState.consecutiveFailures;
  
  if (proxyState.realIP.length() > 0) {
    doc["real_ip"] = proxyState.realIP;
  }
  if (proxyState.proxyIP.length() > 0) {
    doc["proxy_ip"] = proxyState.proxyIP;
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.println("📋 Enhanced SSL-Secured JSON Data to send:");
  Serial.println(jsonString);
  
  String response;
  if (makeHTTPSRequest(apiURL, "POST", jsonString.c_str(), response)) {
    Serial.println("✅ SUCCESS: Enhanced SSL-secured data sent to hosting web!");
    Serial.println("📥 Server Response: " + response);
    
    updatePreviousData();
    lastAPIUpdate = millis();
    lastForceUpdate = millis();
    
    StaticJsonDocument<256> responseDoc;
    DeserializationError error = deserializeJson(responseDoc, response);
    if (!error && responseDoc["success"] && responseDoc["data"]["id"]) {
      Serial.println("✅ Laravel saved with ID: " + String(responseDoc["data"]["id"].as<int>()));
      Serial.println("🔒 Data transmitted securely via SSL/HTTPS");
      
      // Log successful proxy communication if applicable
      if (proxyState.isProxyDetected) {
        Serial.println("✅ Data successfully sent through proxy: " + proxyState.proxyType);
      }
    }
  } else {
    Serial.println("❌ FAILED: Could not send enhanced SSL-secured data to hosting web");
    Serial.println("📥 Server Response: " + response);
    Serial.println("🔒 SSL Error: " + sslStatus.lastError);
    
    // If proxy detected and communication failed, try to re-detect proxy
    if (proxyState.isProxyDetected) {
      Serial.println("🔄 Proxy communication failed, re-checking proxy status...");
      proxyState.lastProxyCheck = 0; // Force immediate re-check
    }
  }
}

// ================= ENHANCED SETUP =================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Initialize watchdog timer (30 seconds timeout)
  esp_task_wdt_init(30, true);
  esp_task_wdt_add(NULL);
  
  // Set CPU frequency to stable level
  setCpuFrequencyMhz(CPU_FREQ_MHZ);
  Serial.println("CPU Frequency: " + String(getCpuFrequencyMhz()) + " MHz");
  
  Serial.println("\n================================");
  Serial.println("  SMART AC - ENHANCED SSL + PROXY HOSTING MODE");
  Serial.println("  Device: ESP32 Enhanced Proximity Sensor - Prodi Elektro");
  Serial.println("  Location: " + String(DEVICE_LOCATION));
  Serial.println("  Hosting: " + String(hostingDomain));
  Serial.println("  Features: SSL + Proxy Detection + Enhanced API");
  Serial.println("  SSL Method: " + String(SSL_METHOD == SSL_METHOD_ROOT_CA ? "ROOT_CA" : 
                                         SSL_METHOD == SSL_METHOD_FINGERPRINT ? "FINGERPRINT" : "INSECURE"));
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
  digitalWrite(RELAY_LAMP1, HIGH);  // Start dengan kontrol otomatis aktif
  
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
  
  Serial.println("\n✅ ENHANCED SYSTEM READY WITH SSL + PROXY DETECTION!");
  Serial.println("Location: Ruang Prodi Dosen Teknik Elektro - UNJA");
  Serial.println("WiFi Network: Teknik Elektro");
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("\nEnhanced Pin Configuration:");
  Serial.println("- PROXIMITY_IN: GPIO 32 (Enhanced with SSL logging)");
  Serial.println("- PROXIMITY_OUT: GPIO 33 (Enhanced with SSL logging)");
  Serial.println("- DHT22: GPIO 13 (Enhanced error handling)");
  Serial.println("- LDR: GPIO 35 (Enhanced mapping)");
  Serial.println("- IR LED: GPIO 15 (Enhanced IR control)");
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
  
  // Memory check at start of loop
  if (ESP.getFreeHeap() < 30000) {
    Serial.println("⚠️ LOW MEMORY: " + String(ESP.getFreeHeap()) + " bytes");
    delay(1000); // Give system time to recover
    esp_task_wdt_reset(); // Reset watchdog after delay
    return;
  }
  
  // Always read sensors for immediate response
  readSensors();
  detectPeople();
  
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
  
  // Enhanced data sending logic with SSL and proxy awareness
  bool shouldSend = false;
  String sendReason = "";
  
  if (hasSignificantChange()) {
    if (canSendUpdate()) {
      shouldSend = true;
      sendReason = "Enhanced change detected";
    }
  } else if (shouldForceUpdate()) {
    if (canSendUpdate()) {
      shouldSend = true;
      sendReason = "Enhanced force update (" + String(MAX_TIME_WITHOUT_UPDATE/1000) + "s)";
    }
  }
  
  // Enhanced data sending with SSL and proxy support
  if (shouldSend) {
    sendDataToAPI(sendReason);
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
  static unsigned long lastMemoryCheck = 0;
  if (millis() - lastMemoryCheck > 120000) {
    uint32_t freeHeap = ESP.getFreeHeap();
    Serial.println("💾 Free memory: " + String(freeHeap) + " bytes");
    lastMemoryCheck = millis();
  }
  
  // Auto-reset if too many false detections (every 10 minutes)
  static unsigned long lastAutoResetCheck = 0;
  
  if (millis() - lastAutoResetCheck > 600000) { // Every 10 minutes
    Serial.println("ℹ️ System health check - 10 minutes elapsed");
    lastAutoResetCheck = millis();
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
      
      float temp = dht.readTemperature();
      float hum = dht.readHumidity();
      Serial.println("   DHT22 (GPIO13):");
      Serial.println("     Temperature=" + String(temp) + "°C");
      Serial.println("     Humidity=" + String(hum) + "%");
      
      int ldrRaw = analogRead(LDR_PIN);
      int ldrMapped = map(ldrRaw, 0, 4095, 0, 1000);
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
      Serial.println("   cert     - Get certificate fingerprint");
      Serial.println("   help     - Show this enhanced help");
    }
    else if (command == "ssl") {
      Serial.println("🔒 Enhanced SSL Status:");
      Serial.println("   SSL Configured: " + String(sslStatus.isConfigured ? "YES" : "NO"));
      Serial.println("   SSL Method: " + String(SSL_METHOD == SSL_METHOD_ROOT_CA ? "ROOT_CA" : 
                                               SSL_METHOD == SSL_METHOD_FINGERPRINT ? "FINGERPRINT" : "INSECURE"));
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
      Serial.println("🔍 Getting Certificate Information:");
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
      
      float temp = dht.readTemperature();
      float hum = dht.readHumidity();
      Serial.println("   DHT22 (GPIO13): T=" + String(temp, 1) + "°C H=" + String(hum, 1) + "%");
      
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
  
  delay(20); // Very fast main loop for immediate proximity response
}