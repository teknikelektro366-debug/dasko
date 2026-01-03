#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ================= TFT =================
TFT_eSPI tft;

// ================= CONFIGURATION =================
// WiFi Configuration
const char* ssid = "LAB TEKNIK TEGANGAN TINGGI";
const char* password = "LABTTT2026";

// Production Hosting Configuration
const char* hostingDomain = "dasko.fst.unja.ac.id";
const char* apiURL = "https://dasko.fst.unja.ac.id/api/sensor/simple";

// Device Information
#define DEVICE_ID "ESP32_Ultrasonic_Simple"
#define DEVICE_LOCATION "Lab_Teknik_Tegangan_Tinggi"

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

// AC Hardware State
bool lastAC1 = false;
bool lastAC2 = false;
int lastTemp1 = -1;
int lastTemp2 = -1;

// Timing variables
unsigned long lastAPIUpdate = 0;
#define API_UPDATE_INTERVAL 2000  // Update setiap 2 detik

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
    Serial.println("✓ Simple API: " + String(apiURL));
    Serial.println("✓ Domain: " + String(hostingDomain));
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI = WiFi.RSSI();
  } else {
    Serial.println("✗ WiFi FAILED!");
    currentData.wifiStatus = "Failed";
  }
  Serial.println("========================");
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
    sendDataToAPI("Ultrasonic_entry_Count_" + String(jumlahOrang));
  }
  
  // Deteksi KELUAR: Sensor DALAM aktif (dari tidak aktif ke aktif)  
  if (sensorData.objectOutDetected && !lastOutDetected && jumlahOrang > 0) {
    jumlahOrang--;
    lastPersonDetected = now;
    Serial.println("🚶 ← ULTRASONIC DALAM AKTIF - ORANG KELUAR!");
    Serial.println("📊 Jumlah orang: " + String(jumlahOrang) + "/20");
    
    // FORCE IMMEDIATE UPDATE TO WEBSITE
    updateSensorData();
    sendDataToAPI("Ultrasonic_exit_Count_" + String(jumlahOrang));
  } else if (sensorData.objectOutDetected && !lastOutDetected && jumlahOrang == 0) {
    Serial.println("⚠ Ultrasonic DALAM aktif tapi count sudah 0");
    // STILL SEND UPDATE TO WEBSITE FOR LOGGING
    updateSensorData();
    sendDataToAPI("Ultrasonic_exit_attempt_when_count_is_0");
  }
  
  // Update state terakhir
  lastInDetected = sensorData.objectInDetected;
  lastOutDetected = sensorData.objectOutDetected;
  
  // Safety limits
  jumlahOrang = constrain(jumlahOrang, 0, MAX_PEOPLE);
}

// ================= SEND DATA TO HOSTING API - SIMPLE =================
void sendDataToAPI(String reason) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("✗ Cannot send to API - WiFi not connected");
    return;
  }
  
  Serial.println("📤 Sending simple data to website...");
  Serial.println("   Reason: " + reason);
  Serial.println("   People Count: " + String(currentData.jumlahOrang));
  Serial.println("   AC Status: " + currentData.acStatus);
  
  // Buat data string sederhana (tanpa JSON)
  String postData = "device_id=" + String(DEVICE_ID);
  postData += "&location=" + String(DEVICE_LOCATION);
  postData += "&people_count=" + String(currentData.jumlahOrang);
  postData += "&ac_status=" + currentData.acStatus;
  postData += "&set_temperature=" + String(currentData.setTemp);
  postData += "&room_temperature=" + String(currentData.suhuRuang);
  postData += "&humidity=" + String(currentData.humidity);
  postData += "&light_level=" + String(currentData.lightLevel);
  postData += "&ultrasonic_in=" + String(currentData.ultrasonic1 ? 1 : 0);
  postData += "&ultrasonic_out=" + String(currentData.ultrasonic2 ? 1 : 0);
  postData += "&wifi_rssi=" + String(WiFi.RSSI());
  postData += "&uptime=" + String(millis() / 1000);
  postData += "&free_heap=" + String(ESP.getFreeHeap());
  postData += "&timestamp=" + String(millis());
  postData += "&status=active";
  postData += "&update_reason=" + reason;
  postData += "&update_type=ultrasonic_detection";
  postData += "&sensor_type=HC-SR04_Ultrasonic";
  
  WiFiClientSecure client;
  HTTPClient https;
  
  // Skip SSL verification untuk simplicity
  client.setInsecure();
  
  // Set timeout
  client.setTimeout(5000);
  https.setTimeout(5000);
  
  if (https.begin(client, apiURL)) {
    // Set headers
    https.addHeader("Content-Type", "application/x-www-form-urlencoded");
    https.addHeader("User-Agent", "ESP32-Ultrasonic-Simple/1.0");
    
    int httpResponseCode = https.POST(postData);
    
    if (httpResponseCode > 0) {
      String response = https.getString();
      Serial.println("✅ SUCCESS: Simple data sent to website!");
      Serial.println("   Response Code: " + String(httpResponseCode));
      Serial.println("   Response: " + response.substring(0, 100) + "...");
      
      lastAPIUpdate = millis();
    } else {
      Serial.println("❌ FAILED: HTTP Error " + String(httpResponseCode));
    }
    
    https.end();
  } else {
    Serial.println("❌ FAILED: Could not connect to " + String(apiURL));
  }
}

// Overload tanpa parameter (default reason)
void sendDataToAPI() {
  sendDataToAPI("Change_detected");
}

// ================= TFT DISPLAY =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ULTRASONIC - SIMPLE");
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
  tft.println("Mode: Simple (No JSON)");
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
    
    // Mode status
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 250);
    tft.println("Mode: Simple (No JSON)");
    
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
    
    // WiFi status
    tft.setCursor(10, 280);
    tft.print("WiFi: ");
    tft.setTextColor(WiFi.isConnected() ? TFT_GREEN : TFT_RED);
    tft.println(currentData.wifiStatus);
    
    lastTftUpdate = millis();
  }
}

// ================= AC CONTROL =================
void kontrolAC(String &acStatus, int &setTemp) {
  bool ac1ON = false;
  bool ac2ON = false;
  int targetTemp1 = 25;
  int targetTemp2 = 25;
  
  // Auto mode berdasarkan jumlah orang
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
  Serial.println("  ULTRASONIC - SIMPLE HOSTING");
  Serial.println("  Sensor: HC-SR04 Ultrasonic");
  Serial.println("  Domain: dasko.fst.unja.ac.id");
  Serial.println("  Mode: Simple (No JSON)");
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
  
  Serial.println("ULTRASONIC SIMPLE SYSTEM READY!");
  Serial.println("Features:");
  Serial.println("- Simple HTTP communication (No JSON)");
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
  
  // Control AC (auto mode only)
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
  
  // Send data to hosting (regular interval)
  if (millis() - lastAPIUpdate >= API_UPDATE_INTERVAL) {
    sendDataToAPI("Regular_update");
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