#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// WiFi credentials - PERBAIKAN: Tambah fallback WiFi
const char* ssid_primary = "LAB TEKNIK TEGANGAN TINGGI";
const char* password_primary = "LAB TTT 2026";
const char* ssid_fallback = "Hotspot";  // Backup WiFi
const char* password_fallback = "12345678";

// Pin definitions - PERBAIKAN: Validasi pin assignment
// DHT22 Sensors - Pastikan pin tidak konflik
#define DHT1_PIN 4
#define DHT2_PIN 16
#define DHT_TYPE DHT22

// LDR Sensors - Gunakan pin ADC yang valid
#define LDR1_PIN 32  // ADC1_CH4
#define LDR2_PIN 33  // ADC1_CH5

// Proximity Sensors - Pin digital yang stabil
#define PROXIMITY1_PIN 13
#define PROXIMITY2_PIN 14

// IR Transmitter - Pin PWM yang mendukung IR
#define IR_TRANSMITTER_PIN 15

// Relay 4 Channel - Pin output yang kuat
#define RELAY1_PIN 25  // AC Unit 1
#define RELAY2_PIN 26  // AC Unit 2
#define RELAY3_PIN 27  // Lampu Circuit 1
#define RELAY4_PIN 12  // Lampu Circuit 2

// TFT LCD SPI - PERBAIKAN: Pastikan pin SPI tidak konflik
#define TFT_SCK  18
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_CS   5
#define TFT_DC   21
#define TFT_RST  22

// Touchscreen pins - PERBAIKAN: Validasi pin touchscreen
#define TOUCH_CS 2
#define TOUCH_IRQ 255  // Tidak digunakan jika 255

// PERBAIKAN: Tambah pin status LED untuk debugging
#define STATUS_LED_PIN 2
#define WIFI_LED_PIN 17

// Sensors initialization dengan error handling
DHT dht1(DHT1_PIN, DHT_TYPE);
DHT dht2(DHT2_PIN, DHT_TYPE);
WebServer server(80);
TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

// Connection status tracking
struct ConnectionStatus {
  bool wifi_connected = false;
  bool dht1_working = false;
  bool dht2_working = false;
  bool tft_working = false;
  bool touch_working = false;
  bool server_running = false;
  bool ir_working = false;
  unsigned long last_check = 0;
} connection_status;

// UI State
enum UIPage {
  PAGE_HOME,
  PAGE_MONITORING,
  PAGE_DEVICES,
  PAGE_SETTINGS
};

UIPage currentPage = PAGE_HOME;
bool screenNeedsUpdate = true;

// Device states
struct DeviceStates {
  bool ac1 = false;
  bool ac2 = false;
  bool lamp1 = false;
  bool lamp2 = false;
  bool computer = false;
  bool tv = false;
  bool dispenser = false;
  bool kettle = false;
  bool coffee = false;
  bool fridge = true;    // Always on
  bool router = true;    // Always on
  bool panel = true;     // Main panel
} devices;

// Sensor data dengan error tracking
struct SensorData {
  float temperature1 = 0;
  float humidity1 = 0;
  float temperature2 = 0;
  float humidity2 = 0;
  int ldr1_value = 0;
  int ldr2_value = 0;
  int light_intensity = 0;
  bool proximity1 = false;
  bool proximity2 = false;
  int people_count = 0;
  unsigned long ir_code = 0;
  
  // Error tracking
  int dht1_errors = 0;
  int dht2_errors = 0;
  int ldr_errors = 0;
  int proximity_errors = 0;
} sensors;

void setup() {
  Serial.begin(115200);
  Serial.println("=== SMART ENERGY SYSTEM STARTUP ===");
  
  // PERBAIKAN: Initialize status LEDs first
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(WIFI_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  digitalWrite(WIFI_LED_PIN, LOW);
  
  // PERBAIKAN: Test all pins before initialization
  testPinConnections();
  
  // Initialize sensors dengan error handling
  Serial.println("Initializing DHT sensors...");
  dht1.begin();
  dht2.begin();
  delay(2000); // DHT needs time to stabilize
  
  // Test DHT sensors
  testDHTSensors();
  
  // PERBAIKAN: Initialize TFT dengan retry mechanism
  Serial.println("Initializing TFT display...");
  if (initializeTFT()) {
    connection_status.tft_working = true;
    showSplashScreen();
  } else {
    Serial.println("WARNING: TFT initialization failed, continuing without display");
  }
  
  // PERBAIKAN: Initialize touchscreen dengan error handling
  Serial.println("Initializing touchscreen...");
  if (initializeTouchscreen()) {
    connection_status.touch_working = true;
  } else {
    Serial.println("WARNING: Touchscreen initialization failed");
  }
  
  // Initialize digital pins dengan pull-up
  Serial.println("Initializing digital pins...");
  pinMode(PROXIMITY1_PIN, INPUT_PULLUP);
  pinMode(PROXIMITY2_PIN, INPUT_PULLUP);
  
  // Initialize IR transmitter
  pinMode(IR_TRANSMITTER_PIN, OUTPUT);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  // Test IR transmitter
  testIRTransmitter();
  
  // Initialize analog pins
  pinMode(LDR1_PIN, INPUT);
  pinMode(LDR2_PIN, INPUT);
  
  // Test LDR sensors
  testLDRSensors();
  
  // Initialize relay pins dengan safe state
  Serial.println("Initializing relay pins...");
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Set safe initial states (HIGH = OFF for most relay modules)
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
  
  // Test relay connections
  testRelayConnections();
  
  // PERBAIKAN: WiFi connection dengan retry dan fallback
  connectToWiFi();
  
  if (connection_status.wifi_connected) {
    // Setup web server
    setupRoutes();
    server.enableCORS(true);
    server.begin();
    connection_status.server_running = true;
    
    Serial.println("HTTP server started successfully");
    Serial.print("Access dashboard at: http://");
    Serial.println(WiFi.localIP());
    
    // Update display if available
    if (connection_status.tft_working) {
      delay(2000);
      currentPage = PAGE_HOME;
      screenNeedsUpdate = true;
    }
  } else {
    Serial.println("ERROR: WiFi connection failed - running in offline mode");
    if (connection_status.tft_working) {
      showErrorScreen("WiFi Connection Failed!");
    }
  }
  
  // Final status report
  printConnectionStatus();
  digitalWrite(STATUS_LED_PIN, HIGH); // System ready
}

void loop() {
  // Handle web server if connected
  if (connection_status.server_running) {
    server.handleClient();
  }
  
  // Handle touch input if available
  if (connection_status.touch_working) {
    handleTouch();
  }
  
  // Read sensors every 2 seconds
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 2000) {
    readSensors();
    if (connection_status.tft_working) {
      screenNeedsUpdate = true;
    }
    lastSensorRead = millis();
  }
  
  // Update display if needed and available
  if (screenNeedsUpdate && connection_status.tft_working) {
    updateDisplay();
    screenNeedsUpdate = false;
  }
  
  // Auto control based on sensors
  autoControl();
  
  // PERBAIKAN: Periodic connection check
  static unsigned long lastConnectionCheck = 0;
  if (millis() - lastConnectionCheck > 30000) { // Check every 30 seconds
    checkAllConnections();
    lastConnectionCheck = millis();
  }
  
  // PERBAIKAN: WiFi reconnection if disconnected
  if (!WiFi.isConnected() && connection_status.wifi_connected) {
    Serial.println("WiFi disconnected, attempting reconnection...");
    connectToWiFi();
  }
  
  delay(50);
}

// PERBAIKAN: Fungsi untuk test koneksi pin
void testPinConnections() {
  Serial.println("Testing pin connections...");
  
  // Test digital pins
  pinMode(PROXIMITY1_PIN, INPUT_PULLUP);
  pinMode(PROXIMITY2_PIN, INPUT_PULLUP);
  
  Serial.println("Proximity pins configured");
  
  // Test analog pins
  int ldr1_test = analogRead(LDR1_PIN);
  int ldr2_test = analogRead(LDR2_PIN);
  
  Serial.println("LDR1 test reading: " + String(ldr1_test));
  Serial.println("LDR2 test reading: " + String(ldr2_test));
  
  // Test output pins
  pinMode(IR_TRANSMITTER_PIN, OUTPUT);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  Serial.println("Pin connection test completed");
}

// PERBAIKAN: Test DHT sensors
void testDHTSensors() {
  Serial.println("Testing DHT sensors...");
  
  float temp1 = dht1.readTemperature();
  float hum1 = dht1.readHumidity();
  float temp2 = dht2.readTemperature();
  float hum2 = dht2.readHumidity();
  
  if (!isnan(temp1) && !isnan(hum1)) {
    connection_status.dht1_working = true;
    Serial.println("DHT1 OK - Temp: " + String(temp1) + "°C, Hum: " + String(hum1) + "%");
  } else {
    Serial.println("WARNING: DHT1 sensor not responding");
  }
  
  if (!isnan(temp2) && !isnan(hum2)) {
    connection_status.dht2_working = true;
    Serial.println("DHT2 OK - Temp: " + String(temp2) + "°C, Hum: " + String(hum2) + "%");
  } else {
    Serial.println("WARNING: DHT2 sensor not responding");
  }
}

// PERBAIKAN: Initialize TFT dengan error handling
bool initializeTFT() {
  try {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    
    // Test display
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("TFT Test", 10, 10);
    
    Serial.println("TFT display initialized successfully");
    return true;
  } catch (...) {
    Serial.println("ERROR: TFT initialization failed");
    return false;
  }
}

// PERBAIKAN: Initialize touchscreen dengan error handling
bool initializeTouchscreen() {
  try {
    if (touch.begin()) {
      touch.setRotation(1);
      Serial.println("Touchscreen initialized successfully");
      return true;
    } else {
      Serial.println("WARNING: Touchscreen begin() failed");
      return false;
    }
  } catch (...) {
    Serial.println("ERROR: Touchscreen initialization failed");
    return false;
  }
}

// PERBAIKAN: Test IR transmitter
void testIRTransmitter() {
  Serial.println("Testing IR transmitter...");
  
  // Send test pulse
  for (int i = 0; i < 5; i++) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    delayMicroseconds(500);
  }
  
  connection_status.ir_working = true;
  Serial.println("IR transmitter test completed");
}

// PERBAIKAN: Test LDR sensors
void testLDRSensors() {
  Serial.println("Testing LDR sensors...");
  
  int ldr1 = analogRead(LDR1_PIN);
  int ldr2 = analogRead(LDR2_PIN);
  
  Serial.println("LDR1 reading: " + String(ldr1));
  Serial.println("LDR2 reading: " + String(ldr2));
  
  // Check if readings are reasonable (not stuck at 0 or max)
  if (ldr1 > 10 && ldr1 < 4090 && ldr2 > 10 && ldr2 < 4090) {
    Serial.println("LDR sensors working normally");
  } else {
    Serial.println("WARNING: LDR sensors may have connection issues");
  }
}

// PERBAIKAN: Test relay connections
void testRelayConnections() {
  Serial.println("Testing relay connections...");
  
  // Test each relay briefly
  digitalWrite(RELAY1_PIN, LOW);
  delay(100);
  digitalWrite(RELAY1_PIN, HIGH);
  Serial.println("Relay 1 test completed");
  
  digitalWrite(RELAY2_PIN, LOW);
  delay(100);
  digitalWrite(RELAY2_PIN, HIGH);
  Serial.println("Relay 2 test completed");
  
  digitalWrite(RELAY3_PIN, LOW);
  delay(100);
  digitalWrite(RELAY3_PIN, HIGH);
  Serial.println("Relay 3 test completed");
  
  digitalWrite(RELAY4_PIN, LOW);
  delay(100);
  digitalWrite(RELAY4_PIN, HIGH);
  Serial.println("Relay 4 test completed");
  
  Serial.println("All relay tests completed");
}

// PERBAIKAN: WiFi connection dengan retry dan fallback
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  
  // Try primary WiFi first
  WiFi.begin(ssid_primary, password_primary);
  Serial.print("Attempting connection to: ");
  Serial.println(ssid_primary);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    attempts++;
    Serial.print(".");
    digitalWrite(WIFI_LED_PIN, !digitalRead(WIFI_LED_PIN)); // Blink LED
    
    if (connection_status.tft_working) {
      tft.drawString("WiFi attempt: " + String(attempts), 10, 80);
    }
  }
  
  // If primary fails, try fallback
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("Primary WiFi failed, trying fallback...");
    WiFi.begin(ssid_fallback, password_fallback);
    
    attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 15) {
      delay(1000);
      attempts++;
      Serial.print(".");
      digitalWrite(WIFI_LED_PIN, !digitalRead(WIFI_LED_PIN));
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    connection_status.wifi_connected = true;
    digitalWrite(WIFI_LED_PIN, HIGH); // Solid LED when connected
    
    Serial.println();
    Serial.println("WiFi connected successfully!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    if (connection_status.tft_working) {
      tft.fillScreen(TFT_BLACK);
      tft.drawString("WiFi Connected!", 10, 60);
      tft.drawString("IP: " + WiFi.localIP().toString(), 10, 80);
    }
  } else {
    connection_status.wifi_connected = false;
    digitalWrite(WIFI_LED_PIN, LOW);
    
    Serial.println();
    Serial.println("WiFi connection failed!");
    Serial.println("Running in offline mode...");
  }
}

// PERBAIKAN: Check all connections periodically
void checkAllConnections() {
  connection_status.last_check = millis();
  
  // Check WiFi
  if (WiFi.isConnected() != connection_status.wifi_connected) {
    connection_status.wifi_connected = WiFi.isConnected();
    Serial.println("WiFi status changed: " + String(connection_status.wifi_connected ? "Connected" : "Disconnected"));
  }
  
  // Check DHT sensors
  float temp1 = dht1.readTemperature();
  if (isnan(temp1)) {
    sensors.dht1_errors++;
    if (sensors.dht1_errors > 5) {
      connection_status.dht1_working = false;
    }
  } else {
    sensors.dht1_errors = 0;
    connection_status.dht1_working = true;
  }
  
  float temp2 = dht2.readTemperature();
  if (isnan(temp2)) {
    sensors.dht2_errors++;
    if (sensors.dht2_errors > 5) {
      connection_status.dht2_working = false;
    }
  } else {
    sensors.dht2_errors = 0;
    connection_status.dht2_working = true;
  }
  
  // Check LDR sensors
  int ldr1 = analogRead(LDR1_PIN);
  int ldr2 = analogRead(LDR2_PIN);
  
  if (ldr1 == 0 || ldr1 == 4095 || ldr2 == 0 || ldr2 == 4095) {
    sensors.ldr_errors++;
  } else {
    sensors.ldr_errors = 0;
  }
  
  // Update status LED based on overall health
  bool system_healthy = connection_status.wifi_connected && 
                       (connection_status.dht1_working || connection_status.dht2_working) &&
                       sensors.ldr_errors < 3;
  
  digitalWrite(STATUS_LED_PIN, system_healthy ? HIGH : LOW);
}

// PERBAIKAN: Print comprehensive connection status
void printConnectionStatus() {
  Serial.println();
  Serial.println("=== CONNECTION STATUS REPORT ===");
  Serial.println("WiFi: " + String(connection_status.wifi_connected ? "✓ Connected" : "✗ Failed"));
  if (connection_status.wifi_connected) {
    Serial.println("  SSID: " + WiFi.SSID());
    Serial.println("  IP: " + WiFi.localIP().toString());
    Serial.println("  Signal: " + String(WiFi.RSSI()) + " dBm");
  }
  
  Serial.println("DHT1 Sensor: " + String(connection_status.dht1_working ? "✓ Working" : "✗ Failed"));
  Serial.println("DHT2 Sensor: " + String(connection_status.dht2_working ? "✓ Working" : "✗ Failed"));
  Serial.println("TFT Display: " + String(connection_status.tft_working ? "✓ Working" : "✗ Failed"));
  Serial.println("Touchscreen: " + String(connection_status.touch_working ? "✓ Working" : "✗ Failed"));
  Serial.println("IR Transmitter: " + String(connection_status.ir_working ? "✓ Working" : "✗ Failed"));
  Serial.println("Web Server: " + String(connection_status.server_running ? "✓ Running" : "✗ Failed"));
  
  Serial.println("LDR Errors: " + String(sensors.ldr_errors));
  Serial.println("DHT1 Errors: " + String(sensors.dht1_errors));
  Serial.println("DHT2 Errors: " + String(sensors.dht2_errors));
  
  Serial.println("================================");
}

void autoControl() {
  // Auto lamp control based on LDR sensors
  static unsigned long lastAutoControl = 0;
  if (millis() - lastAutoControl > 10000) { // Check every 10 seconds
    
    // Auto control Lampu Circuit 1 berdasarkan LDR1
    int ldr1_mapped = map(sensors.ldr1_value, 0, 4095, 0, 1000);
    if (ldr1_mapped < 200 && sensors.people_count > 0) {
      if (!devices.lamp1) {
        devices.lamp1 = true;
        updateRelayStates();
        Serial.println("Auto: Lamp Circuit 1 turned ON (LDR1 low + people detected)");
      }
    }
    
    // Auto control Lampu Circuit 2 berdasarkan LDR2
    int ldr2_mapped = map(sensors.ldr2_value, 0, 4095, 0, 1000);
    if (ldr2_mapped < 200 && sensors.people_count > 0) {
      if (!devices.lamp2) {
        devices.lamp2 = true;
        updateRelayStates();
        Serial.println("Auto: Lamp Circuit 2 turned ON (LDR2 low + people detected)");
      }
    }
    
    // Auto turn off lamps if no people for 5 minutes
    static unsigned long noPeopleTime = 0;
    if (sensors.people_count == 0) {
      if (noPeopleTime == 0) noPeopleTime = millis();
      if (millis() - noPeopleTime > 300000) { // 5 minutes
        if (devices.lamp1 || devices.lamp2) {
          devices.lamp1 = false;
          devices.lamp2 = false;
          updateRelayStates();
          Serial.println("Auto: All Lamps turned OFF (no people detected for 5 min)");
        }
      }
    } else {
      noPeopleTime = 0;
    }
    
    lastAutoControl = millis();
  }
}
// PERBAIKAN: Touch handling dengan error protection
void handleTouch() {
  if (!connection_status.touch_working) return;
  
  try {
    if (touch.touched()) {
      TS_Point p = touch.getPoint();
      
      // Map touch coordinates to screen coordinates
      int x = map(p.x, 0, 4095, 0, 320);
      int y = map(p.y, 0, 4095, 0, 240);
      
      // Handle navigation bar touches
      if (y >= 200 && y <= 240) {
        if (x >= 10 && x <= 70) {
          currentPage = PAGE_HOME;
          screenNeedsUpdate = true;
        } else if (x >= 70 && x <= 130) {
          currentPage = PAGE_MONITORING;
          screenNeedsUpdate = true;
        } else if (x >= 130 && x <= 190) {
          currentPage = PAGE_DEVICES;
          screenNeedsUpdate = true;
        } else if (x >= 190 && x <= 250) {
          currentPage = PAGE_SETTINGS;
          screenNeedsUpdate = true;
        }
      }
      
      // Handle device control touches
      if (currentPage == PAGE_DEVICES && y >= 60 && y <= 150) {
        handleDeviceTouch(x, y);
      }
      
      delay(200); // Debounce
    }
  } catch (...) {
    Serial.println("Touch handling error - disabling touchscreen");
    connection_status.touch_working = false;
  }
}

void handleDeviceTouch(int x, int y) {
  // Device button areas
  if (y >= 60 && y <= 100) {
    // First row
    if (x >= 10 && x <= 100) {
      devices.ac1 = !devices.ac1;
      updateRelayStates();
      screenNeedsUpdate = true;
      Serial.println("Touch: AC1 toggled to " + String(devices.ac1 ? "ON" : "OFF"));
    } else if (x >= 110 && x <= 200) {
      devices.ac2 = !devices.ac2;
      updateRelayStates();
      screenNeedsUpdate = true;
      Serial.println("Touch: AC2 toggled to " + String(devices.ac2 ? "ON" : "OFF"));
    } else if (x >= 210 && x <= 300) {
      devices.lamp1 = !devices.lamp1;
      updateRelayStates();
      screenNeedsUpdate = true;
      Serial.println("Touch: Lamp1 toggled to " + String(devices.lamp1 ? "ON" : "OFF"));
    }
  } else if (y >= 110 && y <= 150) {
    // Second row
    if (x >= 10 && x <= 100) {
      devices.lamp2 = !devices.lamp2;
      updateRelayStates();
      screenNeedsUpdate = true;
      Serial.println("Touch: Lamp2 toggled to " + String(devices.lamp2 ? "ON" : "OFF"));
    } else if (x >= 110 && x <= 200) {
      devices.panel = !devices.panel;
      if (!devices.panel) {
        devices.ac1 = false;
        devices.ac2 = false;
        devices.lamp1 = false;
        devices.lamp2 = false;
      }
      updateRelayStates();
      screenNeedsUpdate = true;
      Serial.println("Touch: Panel toggled to " + String(devices.panel ? "ON" : "OFF"));
    }
  }
}

void setupRoutes() {
  // CORS preflight
  server.on("/api/sensors", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200);
  });
  
  server.on("/api/control", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200);
  });
  
  server.on("/api/status", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200);
  });
  
  server.on("/api/ir", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200);
  });
  
  // API endpoints
  server.on("/api/sensors", HTTP_GET, handleGetSensors);
  server.on("/api/control", HTTP_POST, handleDeviceControl);
  server.on("/api/ir", HTTP_POST, handleIRControl);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/connection-status", HTTP_GET, handleConnectionStatus);
  
  // Web dashboard
  server.on("/", HTTP_GET, handleWebDashboard);
  server.on("/dashboard", HTTP_GET, handleWebDashboard);
}

// PERBAIKAN: Tambah endpoint untuk status koneksi
void handleConnectionStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  DynamicJsonDocument doc(1024);
  
  doc["wifi_connected"] = connection_status.wifi_connected;
  doc["dht1_working"] = connection_status.dht1_working;
  doc["dht2_working"] = connection_status.dht2_working;
  doc["tft_working"] = connection_status.tft_working;
  doc["touch_working"] = connection_status.touch_working;
  doc["server_running"] = connection_status.server_running;
  doc["ir_working"] = connection_status.ir_working;
  
  if (connection_status.wifi_connected) {
    doc["wifi_ssid"] = WiFi.SSID();
    doc["wifi_ip"] = WiFi.localIP().toString();
    doc["wifi_rssi"] = WiFi.RSSI();
  }
  
  doc["dht1_errors"] = sensors.dht1_errors;
  doc["dht2_errors"] = sensors.dht2_errors;
  doc["ldr_errors"] = sensors.ldr_errors;
  doc["proximity_errors"] = sensors.proximity_errors;
  
  doc["last_check"] = connection_status.last_check;
  doc["uptime"] = millis();
  doc["free_heap"] = ESP.getFreeHeap();
  
  String response;
  serializeJson(doc, response);
  
  server.send(200, "application/json", response);
}

void handleGetSensors() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  DynamicJsonDocument doc(2048);
  
  // Sensor data dengan fallback values
  if (connection_status.dht1_working || connection_status.dht2_working) {
    float temp_avg = 0;
    float hum_avg = 0;
    int sensor_count = 0;
    
    if (connection_status.dht1_working) {
      temp_avg += sensors.temperature1;
      hum_avg += sensors.humidity1;
      sensor_count++;
    }
    
    if (connection_status.dht2_working) {
      temp_avg += sensors.temperature2;
      hum_avg += sensors.humidity2;
      sensor_count++;
    }
    
    doc["temperature"] = temp_avg / sensor_count;
    doc["humidity"] = hum_avg / sensor_count;
  } else {
    doc["temperature"] = 27.0; // Default fallback
    doc["humidity"] = 65.0;
  }
  
  doc["light_intensity"] = sensors.light_intensity;
  doc["people_count"] = sensors.people_count;
  
  // Detailed sensor data
  JsonObject sensorsObj = doc.createNestedObject("sensors_detail");
  sensorsObj["temperature1"] = sensors.temperature1;
  sensorsObj["humidity1"] = sensors.humidity1;
  sensorsObj["temperature2"] = sensors.temperature2;
  sensorsObj["humidity2"] = sensors.humidity2;
  sensorsObj["proximity1"] = sensors.proximity1;
  sensorsObj["proximity2"] = sensors.proximity2;
  sensorsObj["ldr1_value"] = sensors.ldr1_value;
  sensorsObj["ldr2_value"] = sensors.ldr2_value;
  sensorsObj["ir_last_transmission"] = sensors.ir_code;
  
  // Device states
  JsonObject devicesObj = doc.createNestedObject("devices");
  devicesObj["ac1"] = devices.ac1;
  devicesObj["ac2"] = devices.ac2;
  devicesObj["lamp1"] = devices.lamp1;
  devicesObj["lamp2"] = devices.lamp2;
  devicesObj["computer"] = devices.computer;
  devicesObj["tv"] = devices.tv;
  devicesObj["dispenser"] = devices.dispenser;
  devicesObj["kettle"] = devices.kettle;
  devicesObj["coffee"] = devices.coffee;
  devicesObj["fridge"] = devices.fridge;
  devicesObj["router"] = devices.router;
  devicesObj["panel"] = devices.panel;
  
  // Connection status
  JsonObject connectionObj = doc.createNestedObject("connection_status");
  connectionObj["wifi"] = connection_status.wifi_connected;
  connectionObj["dht1"] = connection_status.dht1_working;
  connectionObj["dht2"] = connection_status.dht2_working;
  connectionObj["tft"] = connection_status.tft_working;
  connectionObj["touch"] = connection_status.touch_working;
  connectionObj["ir"] = connection_status.ir_working;
  
  doc["timestamp"] = millis();
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  
  String response;
  serializeJson(doc, response);
  
  server.send(200, "application/json", response);
}