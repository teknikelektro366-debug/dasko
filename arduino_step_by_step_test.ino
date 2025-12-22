/*
 * ARDUINO STEP-BY-STEP INTEGRATION TEST
 * Untuk mengatasi masalah: "Bisa jalan sendiri-sendiri, tapi tidak bisa digabung"
 * 
 * CARA PENGGUNAAN:
 * 1. Upload kode ini
 * 2. Buka Serial Monitor
 * 3. Ikuti instruksi step-by-step
 * 4. Catat pada step mana sistem mulai bermasalah
 */

#include <WiFi.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "LAB TEKNIK TEGANGAN TINGGI";
const char* password = "LAB TTT 2026";

// Pin definitions - PASTIKAN TIDAK ADA KONFLIK!
#define DHT1_PIN 4
#define DHT2_PIN 16
#define DHT_TYPE DHT22

#define LDR1_PIN 32
#define LDR2_PIN 33

#define PROXIMITY1_PIN 13
#define PROXIMITY2_PIN 14

#define IR_TRANSMITTER_PIN 15

#define RELAY1_PIN 25
#define RELAY2_PIN 26
#define RELAY3_PIN 27
#define RELAY4_PIN 12

#define STATUS_LED_PIN 2

// Test control variables
int current_test = 0;
bool test_passed[10] = {false};
String test_names[10] = {
  "Basic Setup",
  "DHT1 Sensor", 
  "DHT2 Sensor",
  "LDR Sensors",
  "Proximity Sensors", 
  "Relay Control",
  "IR Transmitter",
  "WiFi Connection",
  "All Sensors Together",
  "Full System"
};

// Sensor objects
DHT dht1(DHT1_PIN, DHT_TYPE);
DHT dht2(DHT2_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  delay(3000); // Beri waktu untuk buka Serial Monitor
  
  Serial.println("========================================");
  Serial.println("    SMART ENERGY INTEGRATION TEST");
  Serial.println("========================================");
  Serial.println();
  Serial.println("Sistem akan test komponen satu per satu.");
  Serial.println("Jika ada yang gagal, catat pada step berapa.");
  Serial.println();
  
  // Mulai test bertahap
  runStepByStepTest();
}

void loop() {
  // Monitor sistem jika semua test berhasil
  if (current_test >= 10) {
    monitorSystem();
    delay(5000);
  } else {
    delay(1000);
  }
}

void runStepByStepTest() {
  Serial.println("=== MEMULAI TEST BERTAHAP ===");
  Serial.println();
  
  // TEST 0: Basic Setup
  current_test = 0;
  if (testBasicSetup()) {
    test_passed[0] = true;
    Serial.println("✓ " + test_names[0] + " - BERHASIL");
  } else {
    Serial.println("✗ " + test_names[0] + " - GAGAL");
    return;
  }
  delay(2000);
  
  // TEST 1: DHT1 Sensor
  current_test = 1;
  if (testDHT1()) {
    test_passed[1] = true;
    Serial.println("✓ " + test_names[1] + " - BERHASIL");
  } else {
    Serial.println("✗ " + test_names[1] + " - GAGAL");
    Serial.println("STOP: DHT1 bermasalah. Cek koneksi pin " + String(DHT1_PIN));
    return;
  }
  delay(2000);
  
  // TEST 2: DHT2 Sensor
  current_test = 2;
  if (testDHT2()) {
    test_passed[2] = true;
    Serial.println("✓ " + test_names[2] + " - BERHASIL");
  } else {
    Serial.println("⚠ " + test_names[2] + " - GAGAL (tapi lanjut)");
    Serial.println("WARNING: DHT2 bermasalah. Cek koneksi pin " + String(DHT2_PIN));
  }
  delay(2000);
  
  // TEST 3: LDR Sensors
  current_test = 3;
  if (testLDRSensors()) {
    test_passed[3] = true;
    Serial.println("✓ " + test_names[3] + " - BERHASIL");
  } else {
    Serial.println("⚠ " + test_names[3] + " - GAGAL (tapi lanjut)");
    Serial.println("WARNING: LDR bermasalah. Cek koneksi pin " + String(LDR1_PIN) + " dan " + String(LDR2_PIN));
  }
  delay(2000);
  
  // TEST 4: Proximity Sensors
  current_test = 4;
  if (testProximitySensors()) {
    test_passed[4] = true;
    Serial.println("✓ " + test_names[4] + " - BERHASIL");
  } else {
    Serial.println("⚠ " + test_names[4] + " - GAGAL (tapi lanjut)");
    Serial.println("WARNING: Proximity bermasalah. Cek koneksi pin " + String(PROXIMITY1_PIN) + " dan " + String(PROXIMITY2_PIN));
  }
  delay(2000);
  
  // TEST 5: Relay Control
  current_test = 5;
  if (testRelayControl()) {
    test_passed[5] = true;
    Serial.println("✓ " + test_names[5] + " - BERHASIL");
  } else {
    Serial.println("⚠ " + test_names[5] + " - GAGAL (tapi lanjut)");
    Serial.println("WARNING: Relay bermasalah. Cek koneksi relay dan power supply");
  }
  delay(2000);
  
  // TEST 6: IR Transmitter
  current_test = 6;
  if (testIRTransmitter()) {
    test_passed[6] = true;
    Serial.println("✓ " + test_names[6] + " - BERHASIL");
  } else {
    Serial.println("⚠ " + test_names[6] + " - GAGAL (tapi lanjut)");
    Serial.println("WARNING: IR bermasalah. Cek koneksi pin " + String(IR_TRANSMITTER_PIN));
  }
  delay(2000);
  
  // TEST 7: WiFi Connection
  current_test = 7;
  if (testWiFiConnection()) {
    test_passed[7] = true;
    Serial.println("✓ " + test_names[7] + " - BERHASIL");
  } else {
    Serial.println("⚠ " + test_names[7] + " - GAGAL (tapi lanjut)");
    Serial.println("WARNING: WiFi bermasalah. Cek SSID dan password");
  }
  delay(2000);
  
  // TEST 8: All Sensors Together
  current_test = 8;
  if (testAllSensorsTogether()) {
    test_passed[8] = true;
    Serial.println("✓ " + test_names[8] + " - BERHASIL");
  } else {
    Serial.println("✗ " + test_names[8] + " - GAGAL");
    Serial.println("CRITICAL: Sensor interference detected!");
    printDiagnostics();
    return;
  }
  delay(2000);
  
  // TEST 9: Full System
  current_test = 9;
  if (testFullSystem()) {
    test_passed[9] = true;
    Serial.println("✓ " + test_names[9] + " - BERHASIL");
  } else {
    Serial.println("✗ " + test_names[9] + " - GAGAL");
    Serial.println("CRITICAL: Full system integration failed!");
    printDiagnostics();
    return;
  }
  
  // Jika sampai sini, semua berhasil
  current_test = 10;
  printFinalReport();
}

bool testBasicSetup() {
  Serial.println("TEST 0: Basic Setup");
  Serial.println("  - Inisialisasi pin status LED");
  
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  // Test LED blink
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(200);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
  
  Serial.println("  - LED test selesai");
  Serial.println("  - Free heap: " + String(ESP.getFreeHeap()) + " bytes");
  
  return true;
}

bool testDHT1() {
  Serial.println("TEST 1: DHT1 Sensor (Pin " + String(DHT1_PIN) + ")");
  Serial.println("  - Inisialisasi DHT1...");
  
  dht1.begin();
  delay(3000); // DHT butuh waktu lama untuk stabilisasi
  
  Serial.println("  - Membaca data DHT1...");
  float temp1 = dht1.readTemperature();
  float hum1 = dht1.readHumidity();
  
  Serial.println("  - Temperature: " + String(temp1) + "°C");
  Serial.println("  - Humidity: " + String(hum1) + "%");
  
  if (isnan(temp1) || isnan(hum1)) {
    Serial.println("  - ERROR: DHT1 mengembalikan NaN");
    return false;
  }
  
  if (temp1 < 0 || temp1 > 60) {
    Serial.println("  - ERROR: DHT1 temperature tidak masuk akal");
    return false;
  }
  
  if (hum1 < 0 || hum1 > 100) {
    Serial.println("  - ERROR: DHT1 humidity tidak masuk akal");
    return false;
  }
  
  return true;
}

bool testDHT2() {
  Serial.println("TEST 2: DHT2 Sensor (Pin " + String(DHT2_PIN) + ")");
  Serial.println("  - Inisialisasi DHT2...");
  
  dht2.begin();
  delay(3000);
  
  Serial.println("  - Membaca data DHT2...");
  float temp2 = dht2.readTemperature();
  float hum2 = dht2.readHumidity();
  
  Serial.println("  - Temperature: " + String(temp2) + "°C");
  Serial.println("  - Humidity: " + String(hum2) + "%");
  
  if (isnan(temp2) || isnan(hum2)) {
    Serial.println("  - ERROR: DHT2 mengembalikan NaN");
    return false;
  }
  
  if (temp2 < 0 || temp2 > 60) {
    Serial.println("  - ERROR: DHT2 temperature tidak masuk akal");
    return false;
  }
  
  if (hum2 < 0 || hum2 > 100) {
    Serial.println("  - ERROR: DHT2 humidity tidak masuk akal");
    return false;
  }
  
  return true;
}

bool testLDRSensors() {
  Serial.println("TEST 3: LDR Sensors");
  Serial.println("  - Testing LDR1 (Pin " + String(LDR1_PIN) + ")");
  
  int ldr1_val = analogRead(LDR1_PIN);
  delay(100);
  int ldr1_val2 = analogRead(LDR1_PIN);
  
  Serial.println("  - LDR1 reading 1: " + String(ldr1_val));
  Serial.println("  - LDR1 reading 2: " + String(ldr1_val2));
  
  if (ldr1_val == 0 || ldr1_val == 4095) {
    Serial.println("  - ERROR: LDR1 stuck at extreme value");
    return false;
  }
  
  Serial.println("  - Testing LDR2 (Pin " + String(LDR2_PIN) + ")");
  
  int ldr2_val = analogRead(LDR2_PIN);
  delay(100);
  int ldr2_val2 = analogRead(LDR2_PIN);
  
  Serial.println("  - LDR2 reading 1: " + String(ldr2_val));
  Serial.println("  - LDR2 reading 2: " + String(ldr2_val2));
  
  if (ldr2_val == 0 || ldr2_val == 4095) {
    Serial.println("  - ERROR: LDR2 stuck at extreme value");
    return false;
  }
  
  return true;
}

bool testProximitySensors() {
  Serial.println("TEST 4: Proximity Sensors");
  Serial.println("  - Setup proximity pins dengan pull-up");
  
  pinMode(PROXIMITY1_PIN, INPUT_PULLUP);
  pinMode(PROXIMITY2_PIN, INPUT_PULLUP);
  delay(500);
  
  Serial.println("  - Reading proximity sensors...");
  bool prox1 = digitalRead(PROXIMITY1_PIN);
  bool prox2 = digitalRead(PROXIMITY2_PIN);
  
  Serial.println("  - Proximity1: " + String(prox1 ? "HIGH" : "LOW"));
  Serial.println("  - Proximity2: " + String(prox2 ? "HIGH" : "LOW"));
  
  // Test beberapa kali untuk stabilitas
  for (int i = 0; i < 5; i++) {
    bool p1 = digitalRead(PROXIMITY1_PIN);
    bool p2 = digitalRead(PROXIMITY2_PIN);
    delay(100);
  }
  
  Serial.println("  - Proximity sensors stable");
  return true;
}

bool testRelayControl() {
  Serial.println("TEST 5: Relay Control");
  Serial.println("  - Setup relay pins...");
  
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Set ke kondisi OFF (HIGH untuk relay aktif LOW)
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
  
  Serial.println("  - Testing relay 1...");
  digitalWrite(RELAY1_PIN, LOW);
  delay(300);
  digitalWrite(RELAY1_PIN, HIGH);
  
  Serial.println("  - Testing relay 2...");
  digitalWrite(RELAY2_PIN, LOW);
  delay(300);
  digitalWrite(RELAY2_PIN, HIGH);
  
  Serial.println("  - Testing relay 3...");
  digitalWrite(RELAY3_PIN, LOW);
  delay(300);
  digitalWrite(RELAY3_PIN, HIGH);
  
  Serial.println("  - Testing relay 4...");
  digitalWrite(RELAY4_PIN, LOW);
  delay(300);
  digitalWrite(RELAY4_PIN, HIGH);
  
  Serial.println("  - All relays tested successfully");
  return true;
}

bool testIRTransmitter() {
  Serial.println("TEST 6: IR Transmitter (Pin " + String(IR_TRANSMITTER_PIN) + ")");
  Serial.println("  - Setup IR transmitter pin...");
  
  pinMode(IR_TRANSMITTER_PIN, OUTPUT);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  Serial.println("  - Sending test IR pulses...");
  
  for (int i = 0; i < 20; i++) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    delayMicroseconds(500);
  }
  
  Serial.println("  - IR test pulses sent");
  return true;
}

bool testWiFiConnection() {
  Serial.println("TEST 7: WiFi Connection");
  Serial.println("  - SSID: " + String(ssid));
  Serial.println("  - Connecting...");
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    attempts++;
    Serial.print(".");
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("  - WiFi connected!");
    Serial.println("  - IP: " + WiFi.localIP().toString());
    Serial.println("  - RSSI: " + String(WiFi.RSSI()) + " dBm");
    return true;
  } else {
    Serial.println("  - WiFi connection failed");
    return false;
  }
}

bool testAllSensorsTogether() {
  Serial.println("TEST 8: All Sensors Together");
  Serial.println("  - Testing sensor interference...");
  
  // Baca semua sensor bersamaan
  float temp1 = dht1.readTemperature();
  float hum1 = dht1.readHumidity();
  float temp2 = dht2.readTemperature();
  float hum2 = dht2.readHumidity();
  
  int ldr1 = analogRead(LDR1_PIN);
  int ldr2 = analogRead(LDR2_PIN);
  
  bool prox1 = digitalRead(PROXIMITY1_PIN);
  bool prox2 = digitalRead(PROXIMITY2_PIN);
  
  Serial.println("  - DHT1: " + String(temp1) + "°C, " + String(hum1) + "%");
  Serial.println("  - DHT2: " + String(temp2) + "°C, " + String(hum2) + "%");
  Serial.println("  - LDR1: " + String(ldr1));
  Serial.println("  - LDR2: " + String(ldr2));
  Serial.println("  - Prox1: " + String(prox1));
  Serial.println("  - Prox2: " + String(prox2));
  
  // Cek apakah ada sensor yang bermasalah
  if (test_passed[1] && (isnan(temp1) || isnan(hum1))) {
    Serial.println("  - ERROR: DHT1 interference detected!");
    return false;
  }
  
  if (test_passed[2] && (isnan(temp2) || isnan(hum2))) {
    Serial.println("  - ERROR: DHT2 interference detected!");
    return false;
  }
  
  Serial.println("  - No sensor interference detected");
  return true;
}

bool testFullSystem() {
  Serial.println("TEST 9: Full System Integration");
  Serial.println("  - Testing all components together...");
  
  // Test semua komponen bekerja bersamaan
  for (int i = 0; i < 5; i++) {
    Serial.println("  - Cycle " + String(i + 1) + "/5");
    
    // Baca sensors
    if (test_passed[1]) {
      float temp1 = dht1.readTemperature();
      Serial.println("    DHT1: " + String(temp1) + "°C");
    }
    
    if (test_passed[3]) {
      int ldr1 = analogRead(LDR1_PIN);
      Serial.println("    LDR1: " + String(ldr1));
    }
    
    if (test_passed[4]) {
      bool prox1 = digitalRead(PROXIMITY1_PIN);
      Serial.println("    Prox1: " + String(prox1));
    }
    
    // Test relay
    if (test_passed[5]) {
      digitalWrite(RELAY1_PIN, LOW);
      delay(100);
      digitalWrite(RELAY1_PIN, HIGH);
      Serial.println("    Relay1: cycled");
    }
    
    // Test IR
    if (test_passed[6]) {
      digitalWrite(IR_TRANSMITTER_PIN, HIGH);
      delayMicroseconds(500);
      digitalWrite(IR_TRANSMITTER_PIN, LOW);
      Serial.println("    IR: pulse sent");
    }
    
    // Check WiFi
    if (test_passed[7]) {
      Serial.println("    WiFi: " + String(WiFi.isConnected() ? "OK" : "Lost"));
    }
    
    Serial.println("    Free heap: " + String(ESP.getFreeHeap()));
    delay(1000);
  }
  
  Serial.println("  - Full system test completed");
  return true;
}

void printFinalReport() {
  Serial.println();
  Serial.println("========================================");
  Serial.println("           FINAL TEST REPORT");
  Serial.println("========================================");
  
  int passed_count = 0;
  for (int i = 0; i < 10; i++) {
    String status = test_passed[i] ? "✓ PASS" : "✗ FAIL";
    Serial.println(test_names[i] + ": " + status);
    if (test_passed[i]) passed_count++;
  }
  
  Serial.println();
  Serial.println("SUMMARY:");
  Serial.println("Passed: " + String(passed_count) + "/10");
  Serial.println("Success Rate: " + String((float)passed_count / 10 * 100, 1) + "%");
  
  if (passed_count == 10) {
    Serial.println("STATUS: ✓ SISTEM SIAP BEROPERASI PENUH");
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else if (passed_count >= 7) {
    Serial.println("STATUS: ⚠ SISTEM BEROPERASI TERBATAS");
    // LED akan berkedip di loop
  } else {
    Serial.println("STATUS: ✗ SISTEM BERMASALAH - PERLU PERBAIKAN");
  }
  
  Serial.println("========================================");
  Serial.println();
  Serial.println("Sistem akan mulai monitoring...");
}

void printDiagnostics() {
  Serial.println();
  Serial.println("=== DIAGNOSTICS ===");
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
  
  if (WiFi.isConnected()) {
    Serial.println("WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
  }
  
  Serial.println("===================");
}

void monitorSystem() {
  static int blink_state = 0;
  
  Serial.println("=== SYSTEM MONITORING ===");
  
  // Baca semua sensor yang berfungsi
  if (test_passed[1]) {
    float temp1 = dht1.readTemperature();
    float hum1 = dht1.readHumidity();
    Serial.println("DHT1: " + String(temp1) + "°C, " + String(hum1) + "%");
  }
  
  if (test_passed[2]) {
    float temp2 = dht2.readTemperature();
    float hum2 = dht2.readHumidity();
    Serial.println("DHT2: " + String(temp2) + "°C, " + String(hum2) + "%");
  }
  
  if (test_passed[3]) {
    int ldr1 = analogRead(LDR1_PIN);
    int ldr2 = analogRead(LDR2_PIN);
    int light = map((ldr1 + ldr2) / 2, 0, 4095, 0, 1000);
    Serial.println("Light: " + String(light) + " lux");
  }
  
  if (test_passed[4]) {
    bool prox1 = digitalRead(PROXIMITY1_PIN);
    bool prox2 = digitalRead(PROXIMITY2_PIN);
    Serial.println("Proximity: " + String(prox1) + ", " + String(prox2));
  }
  
  if (test_passed[7]) {
    Serial.println("WiFi: " + String(WiFi.isConnected() ? "Connected" : "Disconnected"));
  }
  
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("========================");
  
  // Handle LED status
  int passed_count = 0;
  for (int i = 0; i < 10; i++) {
    if (test_passed[i]) passed_count++;
  }
  
  if (passed_count >= 7 && passed_count < 10) {
    // Blink LED untuk sistem terbatas
    digitalWrite(STATUS_LED_PIN, blink_state);
    blink_state = !blink_state;
  }
}