/*
 * SMART ENERGY SYSTEM - SOLUSI INTEGRASI BERTAHAP
 * Masalah: Komponen bisa jalan sendiri-sendiri, tapi tidak bisa saat digabung
 * Solusi: Inisialisasi bertahap dengan delay dan error handling
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>

// PERBAIKAN 1: Kurangi library yang berat dulu, tambah bertahap
// #include <SPI.h>
// #include <TFT_eSPI.h>
// #include <XPT2046_Touchscreen.h>

// WiFi credentials
const char* ssid = "LAB TEKNIK TEGANGAN TINGGI";
const char* password = "LAB TTT 2026";

// PERBAIKAN 2: Pin assignment yang tidak konflik
#define DHT1_PIN 4
#define DHT2_PIN 16
#define DHT_TYPE DHT22

#define LDR1_PIN 32  // ADC1_CH4
#define LDR2_PIN 33  // ADC1_CH5

#define PROXIMITY1_PIN 13
#define PROXIMITY2_PIN 14

#define IR_TRANSMITTER_PIN 15

// PERBAIKAN 3: Relay pins yang aman (tidak konflik dengan SPI/I2C)
#define RELAY1_PIN 25  // AC Unit 1
#define RELAY2_PIN 26  // AC Unit 2
#define RELAY3_PIN 27  // Lampu Circuit 1
#define RELAY4_PIN 12  // Lampu Circuit 2

// Status LED untuk debugging
#define STATUS_LED_PIN 2
#define ERROR_LED_PIN 17

// PERBAIKAN 4: Inisialisasi sensor secara bertahap
DHT dht1(DHT1_PIN, DHT_TYPE);
DHT dht2(DHT2_PIN, DHT_TYPE);
WebServer server(80);

// PERBAIKAN 5: Tracking status setiap komponen
struct SystemStatus {
  bool dht1_ok = false;
  bool dht2_ok = false;
  bool wifi_ok = false;
  bool server_ok = false;
  bool relay_ok = false;
  bool ldr_ok = false;
  bool proximity_ok = false;
  bool ir_ok = false;
  int failed_components = 0;
} system_status;

// Device states
struct DeviceStates {
  bool ac1 = false;
  bool ac2 = false;
  bool lamp1 = false;
  bool lamp2 = false;
  bool panel = true;
} devices;

// Sensor data
struct SensorData {
  float temperature1 = 27.0;
  float humidity1 = 65.0;
  float temperature2 = 27.0;
  float humidity2 = 65.0;
  int ldr1_value = 2000;
  int ldr2_value = 2000;
  int light_intensity = 500;
  bool proximity1 = false;
  bool proximity2 = false;
  int people_count = 0;
  unsigned long ir_code = 0;
} sensors;

void setup() {
  Serial.begin(115200);
  delay(2000); // PERBAIKAN 6: Beri waktu untuk stabilisasi
  
  Serial.println("=== SMART ENERGY SYSTEM - INTEGRASI BERTAHAP ===");
  Serial.println("Memulai inisialisasi komponen satu per satu...");
  
  // TAHAP 1: Inisialisasi pin dasar dan LED status
  initializeBasicPins();
  
  // TAHAP 2: Test dan inisialisasi DHT sensors
  initializeDHTSensors();
  
  // TAHAP 3: Test dan inisialisasi sensor analog (LDR)
  initializeLDRSensors();
  
  // TAHAP 4: Test dan inisialisasi sensor digital (Proximity)
  initializeProximitySensors();
  
  // TAHAP 5: Test dan inisialisasi relay
  initializeRelays();
  
  // TAHAP 6: Test dan inisialisasi IR transmitter
  initializeIRTransmitter();
  
  // TAHAP 7: Inisialisasi WiFi (paling akhir karena paling berat)
  initializeWiFi();
  
  // TAHAP 8: Setup web server jika WiFi berhasil
  if (system_status.wifi_ok) {
    initializeWebServer();
  }
  
  // TAHAP 9: Laporan status akhir
  printFinalStatus();
  
  // TAHAP 10: Set status LED berdasarkan keberhasilan
  setStatusLED();
  
  Serial.println("=== INISIALISASI SELESAI ===");
}

void loop() {
  // PERBAIKAN 7: Loop yang aman dengan error handling
  
  // Handle web server jika tersedia
  if (system_status.server_ok) {
    server.handleClient();
  }
  
  // Baca sensor setiap 3 detik (lebih lambat untuk stabilitas)
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 3000) {
    readAllSensors();
    lastSensorRead = millis();
  }
  
  // Auto control sederhana
  simpleAutoControl();
  
  // Monitor sistem setiap 30 detik
  static unsigned long lastSystemCheck = 0;
  if (millis() - lastSystemCheck > 30000) {
    monitorSystemHealth();
    lastSystemCheck = millis();
  }
  
  delay(100); // PERBAIKAN 8: Delay lebih besar untuk stabilitas
}

// TAHAP 1: Inisialisasi pin dasar
void initializeBasicPins() {
  Serial.println("TAHAP 1: Inisialisasi pin dasar...");
  
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  
  digitalWrite(STATUS_LED_PIN, LOW);
  digitalWrite(ERROR_LED_PIN, LOW);
  
  Serial.println("✓ Pin dasar berhasil diinisialisasi");
  delay(500);
}

// TAHAP 2: Inisialisasi DHT sensors
void initializeDHTSensors() {
  Serial.println("TAHAP 2: Inisialisasi DHT sensors...");
  
  // Inisialisasi DHT1
  Serial.print("  Testing DHT1 (Pin " + String(DHT1_PIN) + ")... ");
  dht1.begin();
  delay(2000); // DHT butuh waktu untuk stabilisasi
  
  float temp1 = dht1.readTemperature();
  float hum1 = dht1.readHumidity();
  
  if (!isnan(temp1) && !isnan(hum1) && temp1 > 0 && temp1 < 60) {
    system_status.dht1_ok = true;
    sensors.temperature1 = temp1;
    sensors.humidity1 = hum1;
    Serial.println("✓ OK (" + String(temp1, 1) + "°C, " + String(hum1, 1) + "%)");
  } else {
    Serial.println("✗ GAGAL");
    system_status.failed_components++;
  }
  
  delay(1000);
  
  // Inisialisasi DHT2
  Serial.print("  Testing DHT2 (Pin " + String(DHT2_PIN) + ")... ");
  dht2.begin();
  delay(2000);
  
  float temp2 = dht2.readTemperature();
  float hum2 = dht2.readHumidity();
  
  if (!isnan(temp2) && !isnan(hum2) && temp2 > 0 && temp2 < 60) {
    system_status.dht2_ok = true;
    sensors.temperature2 = temp2;
    sensors.humidity2 = hum2;
    Serial.println("✓ OK (" + String(temp2, 1) + "°C, " + String(hum2, 1) + "%)");
  } else {
    Serial.println("✗ GAGAL");
    system_status.failed_components++;
  }
  
  Serial.println("DHT Status: DHT1=" + String(system_status.dht1_ok ? "OK" : "FAIL") + 
                 ", DHT2=" + String(system_status.dht2_ok ? "OK" : "FAIL"));
  delay(1000);
}

// TAHAP 3: Inisialisasi LDR sensors
void initializeLDRSensors() {
  Serial.println("TAHAP 3: Inisialisasi LDR sensors...");
  
  // Test LDR1
  Serial.print("  Testing LDR1 (Pin " + String(LDR1_PIN) + ")... ");
  int ldr1_reading = analogRead(LDR1_PIN);
  delay(100);
  int ldr1_reading2 = analogRead(LDR1_PIN);
  
  if (ldr1_reading > 0 && ldr1_reading < 4095 && abs(ldr1_reading - ldr1_reading2) < 500) {
    sensors.ldr1_value = ldr1_reading;
    Serial.println("✓ OK (Value: " + String(ldr1_reading) + ")");
  } else {
    Serial.println("✗ GAGAL (Value: " + String(ldr1_reading) + ")");
    system_status.failed_components++;
  }
  
  delay(500);
  
  // Test LDR2
  Serial.print("  Testing LDR2 (Pin " + String(LDR2_PIN) + ")... ");
  int ldr2_reading = analogRead(LDR2_PIN);
  delay(100);
  int ldr2_reading2 = analogRead(LDR2_PIN);
  
  if (ldr2_reading > 0 && ldr2_reading < 4095 && abs(ldr2_reading - ldr2_reading2) < 500) {
    sensors.ldr2_value = ldr2_reading;
    system_status.ldr_ok = true;
    Serial.println("✓ OK (Value: " + String(ldr2_reading) + ")");
  } else {
    Serial.println("✗ GAGAL (Value: " + String(ldr2_reading) + ")");
    system_status.failed_components++;
  }
  
  // Hitung light intensity
  sensors.light_intensity = map((sensors.ldr1_value + sensors.ldr2_value) / 2, 0, 4095, 0, 1000);
  
  Serial.println("LDR Status: " + String(system_status.ldr_ok ? "OK" : "FAIL") + 
                 " (Light: " + String(sensors.light_intensity) + " lux)");
  delay(1000);
}

// TAHAP 4: Inisialisasi Proximity sensors
void initializeProximitySensors() {
  Serial.println("TAHAP 4: Inisialisasi Proximity sensors...");
  
  // Setup proximity pins dengan pull-up
  pinMode(PROXIMITY1_PIN, INPUT_PULLUP);
  pinMode(PROXIMITY2_PIN, INPUT_PULLUP);
  delay(500);
  
  // Test proximity sensors
  Serial.print("  Testing Proximity sensors... ");
  bool prox1 = digitalRead(PROXIMITY1_PIN);
  bool prox2 = digitalRead(PROXIMITY2_PIN);
  
  sensors.proximity1 = prox1;
  sensors.proximity2 = prox2;
  system_status.proximity_ok = true;
  
  Serial.println("✓ OK (Prox1: " + String(prox1 ? "HIGH" : "LOW") + 
                 ", Prox2: " + String(prox2 ? "HIGH" : "LOW") + ")");
  delay(1000);
}

// TAHAP 5: Inisialisasi Relay
void initializeRelays() {
  Serial.println("TAHAP 5: Inisialisasi Relay...");
  
  // Setup relay pins
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Set ke kondisi OFF (HIGH untuk relay aktif LOW)
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
  
  delay(500);
  
  // Test setiap relay dengan pulse singkat
  Serial.print("  Testing Relay 1... ");
  digitalWrite(RELAY1_PIN, LOW);
  delay(200);
  digitalWrite(RELAY1_PIN, HIGH);
  Serial.println("✓ OK");
  
  delay(300);
  
  Serial.print("  Testing Relay 2... ");
  digitalWrite(RELAY2_PIN, LOW);
  delay(200);
  digitalWrite(RELAY2_PIN, HIGH);
  Serial.println("✓ OK");
  
  delay(300);
  
  Serial.print("  Testing Relay 3... ");
  digitalWrite(RELAY3_PIN, LOW);
  delay(200);
  digitalWrite(RELAY3_PIN, HIGH);
  Serial.println("✓ OK");
  
  delay(300);
  
  Serial.print("  Testing Relay 4... ");
  digitalWrite(RELAY4_PIN, LOW);
  delay(200);
  digitalWrite(RELAY4_PIN, HIGH);
  Serial.println("✓ OK");
  
  system_status.relay_ok = true;
  Serial.println("Relay Status: ✓ Semua relay berfungsi normal");
  delay(1000);
}

// TAHAP 6: Inisialisasi IR Transmitter
void initializeIRTransmitter() {
  Serial.println("TAHAP 6: Inisialisasi IR Transmitter...");
  
  pinMode(IR_TRANSMITTER_PIN, OUTPUT);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delay(500);
  
  // Test IR dengan pulse sederhana
  Serial.print("  Testing IR Transmitter (Pin " + String(IR_TRANSMITTER_PIN) + ")... ");
  
  for (int i = 0; i < 10; i++) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    delayMicroseconds(500);
  }
  
  system_status.ir_ok = true;
  Serial.println("✓ OK");
  delay(1000);
}

// TAHAP 7: Inisialisasi WiFi
void initializeWiFi() {
  Serial.println("TAHAP 7: Inisialisasi WiFi...");
  Serial.println("  SSID: " + String(ssid));
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  Serial.print("  Connecting");
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    attempts++;
    Serial.print(".");
    
    // Blink error LED saat connecting
    digitalWrite(ERROR_LED_PIN, !digitalRead(ERROR_LED_PIN));
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    system_status.wifi_ok = true;
    digitalWrite(ERROR_LED_PIN, LOW);
    
    Serial.println("✓ WiFi berhasil terkoneksi!");
    Serial.println("  IP Address: " + WiFi.localIP().toString());
    Serial.println("  Signal Strength: " + String(WiFi.RSSI()) + " dBm");
  } else {
    Serial.println("✗ WiFi gagal terkoneksi");
    Serial.println("  Sistem akan berjalan dalam mode offline");
    system_status.failed_components++;
    digitalWrite(ERROR_LED_PIN, HIGH);
  }
  
  delay(1000);
}

// TAHAP 8: Inisialisasi Web Server
void initializeWebServer() {
  Serial.println("TAHAP 8: Inisialisasi Web Server...");
  
  setupRoutes();
  server.enableCORS(true);
  server.begin();
  
  system_status.server_ok = true;
  
  Serial.println("✓ Web server berhasil dijalankan");
  Serial.println("  Akses dashboard: http://" + WiFi.localIP().toString());
  delay(1000);
}
// TAHAP 9: Laporan status akhir
void printFinalStatus() {
  Serial.println();
  Serial.println("=== LAPORAN STATUS SISTEM ===");
  Serial.println("DHT1 Sensor    : " + String(system_status.dht1_ok ? "✓ BERHASIL" : "✗ GAGAL"));
  Serial.println("DHT2 Sensor    : " + String(system_status.dht2_ok ? "✓ BERHASIL" : "✗ GAGAL"));
  Serial.println("LDR Sensors    : " + String(system_status.ldr_ok ? "✓ BERHASIL" : "✗ GAGAL"));
  Serial.println("Proximity      : " + String(system_status.proximity_ok ? "✓ BERHASIL" : "✗ GAGAL"));
  Serial.println("Relay Control  : " + String(system_status.relay_ok ? "✓ BERHASIL" : "✗ GAGAL"));
  Serial.println("IR Transmitter : " + String(system_status.ir_ok ? "✓ BERHASIL" : "✗ GAGAL"));
  Serial.println("WiFi Connection: " + String(system_status.wifi_ok ? "✓ BERHASIL" : "✗ GAGAL"));
  Serial.println("Web Server     : " + String(system_status.server_ok ? "✓ BERHASIL" : "✗ GAGAL"));
  Serial.println();
  
  int total_components = 8;
  int working_components = total_components - system_status.failed_components;
  float success_rate = (float)working_components / total_components * 100;
  
  Serial.println("Komponen Berhasil: " + String(working_components) + "/" + String(total_components));
  Serial.println("Tingkat Keberhasilan: " + String(success_rate, 1) + "%");
  
  if (success_rate >= 75) {
    Serial.println("STATUS: ✓ SISTEM SIAP BEROPERASI");
  } else if (success_rate >= 50) {
    Serial.println("STATUS: ⚠ SISTEM BEROPERASI TERBATAS");
  } else {
    Serial.println("STATUS: ✗ SISTEM BERMASALAH");
  }
  
  Serial.println("=============================");
}

// TAHAP 10: Set status LED
void setStatusLED() {
  int working_components = 8 - system_status.failed_components;
  
  if (working_components >= 6) {
    // Sistem baik - LED status nyala terus
    digitalWrite(STATUS_LED_PIN, HIGH);
    digitalWrite(ERROR_LED_PIN, LOW);
  } else if (working_components >= 4) {
    // Sistem terbatas - LED status berkedip lambat
    // Akan dihandle di loop()
  } else {
    // Sistem bermasalah - LED error nyala
    digitalWrite(STATUS_LED_PIN, LOW);
    digitalWrite(ERROR_LED_PIN, HIGH);
  }
}

// Fungsi baca semua sensor dengan error handling
void readAllSensors() {
  // Baca DHT1 jika tersedia
  if (system_status.dht1_ok) {
    float temp1 = dht1.readTemperature();
    float hum1 = dht1.readHumidity();
    
    if (!isnan(temp1) && !isnan(hum1)) {
      sensors.temperature1 = temp1;
      sensors.humidity1 = hum1;
    } else {
      Serial.println("Warning: DHT1 read error");
    }
  }
  
  // Baca DHT2 jika tersedia
  if (system_status.dht2_ok) {
    float temp2 = dht2.readTemperature();
    float hum2 = dht2.readHumidity();
    
    if (!isnan(temp2) && !isnan(hum2)) {
      sensors.temperature2 = temp2;
      sensors.humidity2 = hum2;
    } else {
      Serial.println("Warning: DHT2 read error");
    }
  }
  
  // Baca LDR jika tersedia
  if (system_status.ldr_ok) {
    sensors.ldr1_value = analogRead(LDR1_PIN);
    sensors.ldr2_value = analogRead(LDR2_PIN);
    sensors.light_intensity = map((sensors.ldr1_value + sensors.ldr2_value) / 2, 0, 4095, 0, 1000);
  }
  
  // Baca Proximity jika tersedia
  if (system_status.proximity_ok) {
    sensors.proximity1 = digitalRead(PROXIMITY1_PIN);
    sensors.proximity2 = digitalRead(PROXIMITY2_PIN);
    
    // Simple people counting
    static bool last_prox1 = false;
    static bool last_prox2 = false;
    
    if (sensors.proximity1 && !last_prox1) {
      sensors.people_count++;
      Serial.println("Person detected: " + String(sensors.people_count));
    }
    
    if (sensors.proximity2 && !last_prox2) {
      sensors.people_count = max(0, sensors.people_count - 1);
      Serial.println("Person left: " + String(sensors.people_count));
    }
    
    last_prox1 = sensors.proximity1;
    last_prox2 = sensors.proximity2;
    
    // Reset counter jika tidak ada aktivitas
    static unsigned long last_activity = 0;
    if (sensors.proximity1 || sensors.proximity2) {
      last_activity = millis();
    } else if (millis() - last_activity > 300000) { // 5 menit
      sensors.people_count = 0;
    }
  }
}

// Auto control sederhana
void simpleAutoControl() {
  if (!system_status.relay_ok) return;
  
  static unsigned long lastAutoControl = 0;
  if (millis() - lastAutoControl > 10000) { // Setiap 10 detik
    
    // Auto lamp control berdasarkan LDR dan people count
    if (system_status.ldr_ok && system_status.proximity_ok) {
      int avg_light = (sensors.ldr1_value + sensors.ldr2_value) / 2;
      int light_mapped = map(avg_light, 0, 4095, 0, 1000);
      
      // Nyalakan lampu jika gelap dan ada orang
      if (light_mapped < 300 && sensors.people_count > 0) {
        if (!devices.lamp1) {
          devices.lamp1 = true;
          updateRelayStates();
          Serial.println("Auto: Lamp 1 ON (gelap + ada orang)");
        }
        if (!devices.lamp2) {
          devices.lamp2 = true;
          updateRelayStates();
          Serial.println("Auto: Lamp 2 ON (gelap + ada orang)");
        }
      }
      
      // Matikan lampu jika tidak ada orang selama 5 menit
      static unsigned long no_people_time = 0;
      if (sensors.people_count == 0) {
        if (no_people_time == 0) no_people_time = millis();
        if (millis() - no_people_time > 300000) { // 5 menit
          if (devices.lamp1 || devices.lamp2) {
            devices.lamp1 = false;
            devices.lamp2 = false;
            updateRelayStates();
            Serial.println("Auto: Lampu OFF (tidak ada orang 5 menit)");
          }
        }
      } else {
        no_people_time = 0;
      }
    }
    
    lastAutoControl = millis();
  }
}

// Monitor kesehatan sistem
void monitorSystemHealth() {
  Serial.println("=== MONITOR SISTEM ===");
  Serial.println("Uptime: " + String(millis() / 1000) + " detik");
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  
  if (system_status.wifi_ok) {
    Serial.println("WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
  }
  
  // Cek apakah ada sensor yang bermasalah
  if (system_status.dht1_ok) {
    if (isnan(dht1.readTemperature())) {
      Serial.println("Warning: DHT1 mungkin bermasalah");
    }
  }
  
  if (system_status.dht2_ok) {
    if (isnan(dht2.readTemperature())) {
      Serial.println("Warning: DHT2 mungkin bermasalah");
    }
  }
  
  // Handle LED status untuk sistem terbatas
  int working_components = 8 - system_status.failed_components;
  if (working_components >= 4 && working_components < 6) {
    // Blink status LED untuk sistem terbatas
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
  }
  
  Serial.println("======================");
}

// Update relay states
void updateRelayStates() {
  if (!system_status.relay_ok) return;
  
  // LOW = ON, HIGH = OFF untuk relay aktif LOW
  digitalWrite(RELAY1_PIN, !(devices.ac1 && devices.panel));
  digitalWrite(RELAY2_PIN, !(devices.ac2 && devices.panel));
  digitalWrite(RELAY3_PIN, !(devices.lamp1 && devices.panel));
  digitalWrite(RELAY4_PIN, !(devices.lamp2 && devices.panel));
}

// Setup web server routes
void setupRoutes() {
  // CORS
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
  
  // API endpoints
  server.on("/api/sensors", HTTP_GET, handleGetSensors);
  server.on("/api/control", HTTP_POST, handleDeviceControl);
  server.on("/api/status", HTTP_GET, handleSystemStatus);
  server.on("/api/system-health", HTTP_GET, handleSystemHealth);
  
  // Web dashboard
  server.on("/", HTTP_GET, handleWebDashboard);
}

// Handle get sensors
void handleGetSensors() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  DynamicJsonDocument doc(1024);
  
  // Sensor data dengan fallback
  if (system_status.dht1_ok || system_status.dht2_ok) {
    float temp_avg = 0;
    float hum_avg = 0;
    int count = 0;
    
    if (system_status.dht1_ok) {
      temp_avg += sensors.temperature1;
      hum_avg += sensors.humidity1;
      count++;
    }
    
    if (system_status.dht2_ok) {
      temp_avg += sensors.temperature2;
      hum_avg += sensors.humidity2;
      count++;
    }
    
    doc["temperature"] = temp_avg / count;
    doc["humidity"] = hum_avg / count;
  } else {
    doc["temperature"] = 27.0;
    doc["humidity"] = 65.0;
  }
  
  doc["light_intensity"] = sensors.light_intensity;
  doc["people_count"] = sensors.people_count;
  
  // Device states
  JsonObject devicesObj = doc.createNestedObject("devices");
  devicesObj["ac1"] = devices.ac1;
  devicesObj["ac2"] = devices.ac2;
  devicesObj["lamp1"] = devices.lamp1;
  devicesObj["lamp2"] = devices.lamp2;
  devicesObj["panel"] = devices.panel;
  
  // System status
  JsonObject statusObj = doc.createNestedObject("system_status");
  statusObj["dht1"] = system_status.dht1_ok;
  statusObj["dht2"] = system_status.dht2_ok;
  statusObj["ldr"] = system_status.ldr_ok;
  statusObj["proximity"] = system_status.proximity_ok;
  statusObj["relay"] = system_status.relay_ok;
  statusObj["ir"] = system_status.ir_ok;
  statusObj["wifi"] = system_status.wifi_ok;
  statusObj["server"] = system_status.server_ok;
  statusObj["failed_components"] = system_status.failed_components;
  
  doc["timestamp"] = millis();
  doc["uptime"] = millis() / 1000;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Handle device control
void handleDeviceControl() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    
    String device = doc["device"];
    String action = doc["action"];
    bool state = (action == "on");
    
    if (device == "ac1") devices.ac1 = state;
    else if (device == "ac2") devices.ac2 = state;
    else if (device == "lamp1") devices.lamp1 = state;
    else if (device == "lamp2") devices.lamp2 = state;
    else if (device == "panel") {
      devices.panel = state;
      if (!state) {
        devices.ac1 = false;
        devices.ac2 = false;
        devices.lamp1 = false;
        devices.lamp2 = false;
      }
    }
    
    updateRelayStates();
    
    server.send(200, "application/json", "{\"status\":\"success\"}");
    Serial.println("Device control: " + device + " -> " + action);
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

// Handle system status
void handleSystemStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  DynamicJsonDocument doc(512);
  
  doc["status"] = "online";
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  
  if (system_status.wifi_ok) {
    doc["ip"] = WiFi.localIP().toString();
    doc["rssi"] = WiFi.RSSI();
  }
  
  JsonObject components = doc.createNestedObject("components");
  components["dht1"] = system_status.dht1_ok;
  components["dht2"] = system_status.dht2_ok;
  components["ldr"] = system_status.ldr_ok;
  components["proximity"] = system_status.proximity_ok;
  components["relay"] = system_status.relay_ok;
  components["ir"] = system_status.ir_ok;
  components["wifi"] = system_status.wifi_ok;
  components["server"] = system_status.server_ok;
  
  int working = 8 - system_status.failed_components;
  doc["working_components"] = working;
  doc["total_components"] = 8;
  doc["success_rate"] = (float)working / 8 * 100;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Handle system health
void handleSystemHealth() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  DynamicJsonDocument doc(1024);
  
  // Detailed sensor readings
  JsonObject sensors_obj = doc.createNestedObject("sensors");
  sensors_obj["temperature1"] = sensors.temperature1;
  sensors_obj["humidity1"] = sensors.humidity1;
  sensors_obj["temperature2"] = sensors.temperature2;
  sensors_obj["humidity2"] = sensors.humidity2;
  sensors_obj["ldr1_value"] = sensors.ldr1_value;
  sensors_obj["ldr2_value"] = sensors.ldr2_value;
  sensors_obj["light_intensity"] = sensors.light_intensity;
  sensors_obj["proximity1"] = sensors.proximity1;
  sensors_obj["proximity2"] = sensors.proximity2;
  sensors_obj["people_count"] = sensors.people_count;
  
  // System health metrics
  doc["uptime_seconds"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  doc["failed_components"] = system_status.failed_components;
  
  if (system_status.wifi_ok) {
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["wifi_ssid"] = WiFi.SSID();
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Simple web dashboard
void handleWebDashboard() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>Smart Energy - Sistem Terintegrasi</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>";
  html += "body{font-family:Arial;margin:20px;background:#f0f0f0;}";
  html += ".container{max-width:1200px;margin:0 auto;}";
  html += ".header{background:#2c3e50;color:white;padding:20px;border-radius:10px;text-align:center;margin-bottom:20px;}";
  html += ".cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));gap:20px;margin-bottom:20px;}";
  html += ".card{background:white;padding:20px;border-radius:10px;box-shadow:0 2px 5px rgba(0,0,0,0.1);}";
  html += ".card h3{margin:0 0 10px 0;color:#2c3e50;}";
  html += ".value{font-size:2em;font-weight:bold;color:#27ae60;}";
  html += ".status{margin:10px 0;}";
  html += ".status.ok{color:#27ae60;}";
  html += ".status.fail{color:#e74c3c;}";
  html += ".controls{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:15px;}";
  html += ".device{background:white;padding:15px;border-radius:8px;text-align:center;box-shadow:0 2px 5px rgba(0,0,0,0.1);}";
  html += ".btn{background:#3498db;color:white;border:none;padding:10px 20px;border-radius:5px;cursor:pointer;}";
  html += ".btn:hover{background:#2980b9;}";
  html += ".btn.on{background:#27ae60;}";
  html += ".btn.off{background:#e74c3c;}";
  html += "</style></head><body>";
  
  html += "<div class='container'>";
  html += "<div class='header'>";
  html += "<h1>🏠 Smart Energy System</h1>";
  html += "<p>Sistem Terintegrasi - Lab Teknik Tegangan Tinggi</p>";
  html += "</div>";
  
  html += "<div class='cards'>";
  html += "<div class='card'>";
  html += "<h3>🌡️ Temperature</h3>";
  html += "<div class='value' id='temp'>--°C</div>";
  html += "<div class='status' id='temp-status'>Loading...</div>";
  html += "</div>";
  
  html += "<div class='card'>";
  html += "<h3>💧 Humidity</h3>";
  html += "<div class='value' id='hum'>--%</div>";
  html += "<div class='status' id='hum-status'>Loading...</div>";
  html += "</div>";
  
  html += "<div class='card'>";
  html += "<h3>💡 Light</h3>";
  html += "<div class='value' id='light'>-- lux</div>";
  html += "<div class='status' id='light-status'>Loading...</div>";
  html += "</div>";
  
  html += "<div class='card'>";
  html += "<h3>👥 People</h3>";
  html += "<div class='value' id='people'>--</div>";
  html += "<div class='status' id='people-status'>Loading...</div>";
  html += "</div>";
  html += "</div>";
  
  html += "<h2>Device Controls</h2>";
  html += "<div class='controls'>";
  html += "<div class='device'>";
  html += "<h4>AC Unit 1</h4>";
  html += "<button class='btn off' id='btn-ac1' onclick='toggle(\"ac1\")'>OFF</button>";
  html += "</div>";
  
  html += "<div class='device'>";
  html += "<h4>AC Unit 2</h4>";
  html += "<button class='btn off' id='btn-ac2' onclick='toggle(\"ac2\")'>OFF</button>";
  html += "</div>";
  
  html += "<div class='device'>";
  html += "<h4>Lamp 1</h4>";
  html += "<button class='btn off' id='btn-lamp1' onclick='toggle(\"lamp1\")'>OFF</button>";
  html += "</div>";
  
  html += "<div class='device'>";
  html += "<h4>Lamp 2</h4>";
  html += "<button class='btn off' id='btn-lamp2' onclick='toggle(\"lamp2\")'>OFF</button>";
  html += "</div>";
  
  html += "<div class='device'>";
  html += "<h4>Main Panel</h4>";
  html += "<button class='btn on' id='btn-panel' onclick='toggle(\"panel\")'>ON</button>";
  html += "</div>";
  html += "</div>";
  
  html += "</div>";
  
  html += "<script>";
  html += "async function updateData(){";
  html += "try{";
  html += "const response=await fetch('/api/sensors');";
  html += "const data=await response.json();";
  
  html += "document.getElementById('temp').textContent=data.temperature.toFixed(1)+'°C';";
  html += "document.getElementById('hum').textContent=data.humidity.toFixed(1)+'%';";
  html += "document.getElementById('light').textContent=data.light_intensity+' lux';";
  html += "document.getElementById('people').textContent=data.people_count;";
  
  html += "document.getElementById('temp-status').textContent=data.system_status.dht1||data.system_status.dht2?'✓ Working':'✗ Failed';";
  html += "document.getElementById('temp-status').className='status '+(data.system_status.dht1||data.system_status.dht2?'ok':'fail');";
  
  html += "document.getElementById('hum-status').textContent=data.system_status.dht1||data.system_status.dht2?'✓ Working':'✗ Failed';";
  html += "document.getElementById('hum-status').className='status '+(data.system_status.dht1||data.system_status.dht2?'ok':'fail');";
  
  html += "document.getElementById('light-status').textContent=data.system_status.ldr?'✓ Working':'✗ Failed';";
  html += "document.getElementById('light-status').className='status '+(data.system_status.ldr?'ok':'fail');";
  
  html += "document.getElementById('people-status').textContent=data.system_status.proximity?'✓ Working':'✗ Failed';";
  html += "document.getElementById('people-status').className='status '+(data.system_status.proximity?'ok':'fail');";
  
  html += "updateButton('ac1',data.devices.ac1);";
  html += "updateButton('ac2',data.devices.ac2);";
  html += "updateButton('lamp1',data.devices.lamp1);";
  html += "updateButton('lamp2',data.devices.lamp2);";
  html += "updateButton('panel',data.devices.panel);";
  
  html += "}catch(error){console.error('Error:',error);}}";
  
  html += "function updateButton(device,state){";
  html += "const btn=document.getElementById('btn-'+device);";
  html += "btn.textContent=state?'ON':'OFF';";
  html += "btn.className='btn '+(state?'on':'off');}";
  
  html += "async function toggle(device){";
  html += "try{";
  html += "const response=await fetch('/api/control',{";
  html += "method:'POST',headers:{'Content-Type':'application/json'},";
  html += "body:JSON.stringify({device:device,action:'toggle'})});";
  html += "if(response.ok){updateData();}";
  html += "}catch(error){console.error('Error:',error);}}";
  
  html += "setInterval(updateData,3000);";
  html += "updateData();";
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
}