#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// WiFi credentials
const char* ssid = "LAB TEKNIK TEGANGAN TINGGI";
const char* password = "LAB TTT 2026";

// Pin definitions berdasarkan datasheet
// DHT22 Sensors
#define DHT1_PIN 4
#define DHT2_PIN 16
#define DHT_TYPE DHT22

// LDR Sensors (ADC Pins) - 2 sensors untuk 2 sakelar lampu
#define LDR1_PIN 32  // ADC1_CH4 - untuk Lampu Circuit 1
#define LDR2_PIN 33  // ADC1_CH5 - untuk Lampu Circuit 2

// Proximity Sensors
#define PROXIMITY1_PIN 13
#define PROXIMITY2_PIN 14

// IR Transmitter - untuk mengirim sinyal kontrol ke AC dan TV
#define IR_TRANSMITTER_PIN 15

// Relay 4 Channel
#define RELAY1_PIN 25  // AC Unit 1
#define RELAY2_PIN 26  // AC Unit 2
#define RELAY3_PIN 27  // Lampu Circuit 1
#define RELAY4_PIN 12  // Lampu Circuit 2 (atau panel utama)

// TFT LCD SPI (ILI9341)
#define TFT_SCK  18
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_CS   5
#define TFT_DC   21
#define TFT_RST  22

// Touchscreen pins
#define TOUCH_CS 2
#define TOUCH_IRQ 255

// Sensors
DHT dht1(DHT1_PIN, DHT_TYPE);
DHT dht2(DHT2_PIN, DHT_TYPE);
WebServer server(80);
TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

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
  bool ac1 = false;      // Relay 1
  bool ac2 = false;      // Relay 2
  bool lamp1 = false;    // Relay 3
  bool lamp2 = false;    // Relay 4
  bool computer = false;
  bool tv = false;
  bool dispenser = false;
  bool kettle = false;
  bool coffee = false;
  bool fridge = true;    // Always on
  bool router = true;    // Always on
  bool panel = true;     // Main panel
} devices;

// Sensor data
struct SensorData {
  float temperature1 = 0;    // DHT22 #1
  float humidity1 = 0;       // DHT22 #1
  float temperature2 = 0;    // DHT22 #2
  float humidity2 = 0;       // DHT22 #2
  int ldr1_value = 0;        // LDR 1 - untuk Lampu Circuit 1
  int ldr2_value = 0;        // LDR 2 - untuk Lampu Circuit 2
  int light_intensity = 0;   // Average LDR
  bool proximity1 = false;   // Proximity sensor 1
  bool proximity2 = false;   // Proximity sensor 2
  int people_count = 0;      // Calculated from proximity
  unsigned long ir_code = 0; // IR transmitter status
} sensors;

void setup() {
  Serial.begin(115200);
  
  // Initialize DHT sensors
  dht1.begin();
  dht2.begin();
  
  // Initialize TFT LCD
  tft.init();
  tft.setRotation(1); // Landscape mode
  tft.fillScreen(TFT_BLACK);
  
  // Initialize touchscreen
  touch.begin();
  touch.setRotation(1);
  
  // Show splash screen
  showSplashScreen();
  
  // Initialize digital input pins
  pinMode(PROXIMITY1_PIN, INPUT);
  pinMode(PROXIMITY2_PIN, INPUT);
  
  // Initialize IR transmitter pin
  pinMode(IR_TRANSMITTER_PIN, OUTPUT);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  // Initialize analog pins (LDR) - hanya 2 sensor
  pinMode(LDR1_PIN, INPUT);
  pinMode(LDR2_PIN, INPUT);
  
  // Initialize relay pins
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Set initial relay states (LOW = OFF for most relay modules)
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
  
  // Update LCD
  tft.drawString("Connecting to WiFi...", 10, 60);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_attempts < 20) {
    delay(1000);
    wifi_attempts++;
    Serial.println("Connecting to WiFi...");
    tft.drawString("Attempt: " + String(wifi_attempts), 10, 80);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Setup web server routes
    setupRoutes();
    
    // Enable CORS
    server.enableCORS(true);
    
    // Start server
    server.begin();
    Serial.println("HTTP server started");
    
    // Show success and switch to home page
    delay(2000);
    currentPage = PAGE_HOME;
    screenNeedsUpdate = true;
  } else {
    Serial.println("WiFi connection failed!");
    showErrorScreen("WiFi Connection Failed!");
  }
}

void loop() {
  server.handleClient();
  
  // Handle touch input
  handleTouch();
  
  // Read sensors every 2 seconds
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 2000) {
    readSensors();
    screenNeedsUpdate = true;
    lastSensorRead = millis();
  }
  
  // Update display if needed
  if (screenNeedsUpdate) {
    updateDisplay();
    screenNeedsUpdate = false;
  }
  
  // Auto control based on sensors
  autoControl();
  
  delay(50);
}

void autoControl() {
  // Auto lamp control based on individual LDR sensors
  static unsigned long lastAutoControl = 0;
  if (millis() - lastAutoControl > 10000) { // Check every 10 seconds
    
    // Threshold untuk LDR (nilai rendah = gelap)
    const int LIGHT_THRESHOLD = 1500; // Adjust sesuai kondisi ruangan
    
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

// ===== TOUCH HANDLING =====

void handleTouch() {
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
    
    // Handle device control touches (only on devices page)
    if (currentPage == PAGE_DEVICES && y >= 60 && y <= 150) {
      handleDeviceTouch(x, y);
    }
    
    // Debounce
    delay(200);
  }
}

void handleDeviceTouch(int x, int y) {
  // Device button areas
  if (y >= 60 && y <= 100) {
    // First row
    if (x >= 10 && x <= 100) {
      // AC 1
      devices.ac1 = !devices.ac1;
      updateRelayStates();
      screenNeedsUpdate = true;
      Serial.println("Touch: AC1 toggled to " + String(devices.ac1 ? "ON" : "OFF"));
    } else if (x >= 110 && x <= 200) {
      // AC 2
      devices.ac2 = !devices.ac2;
      updateRelayStates();
      screenNeedsUpdate = true;
      Serial.println("Touch: AC2 toggled to " + String(devices.ac2 ? "ON" : "OFF"));
    } else if (x >= 210 && x <= 300) {
      // Lamp 1
      devices.lamp1 = !devices.lamp1;
      updateRelayStates();
      screenNeedsUpdate = true;
      Serial.println("Touch: Lamp1 toggled to " + String(devices.lamp1 ? "ON" : "OFF"));
    }
  } else if (y >= 110 && y <= 150) {
    // Second row
    if (x >= 10 && x <= 100) {
      // Lamp 2
      devices.lamp2 = !devices.lamp2;
      updateRelayStates();
      screenNeedsUpdate = true;
      Serial.println("Touch: Lamp2 toggled to " + String(devices.lamp2 ? "ON" : "OFF"));
    } else if (x >= 110 && x <= 200) {
      // Panel
      devices.panel = !devices.panel;
      if (!devices.panel) {
        // Turn off all devices when panel is off
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
  
  // Get sensor data
  server.on("/api/sensors", HTTP_GET, handleGetSensors);
  
  // Control devices
  server.on("/api/control", HTTP_POST, handleDeviceControl);
  
  // IR control endpoint
  server.on("/api/ir", HTTP_POST, handleIRControl);
  
  // Status check
  server.on("/api/status", HTTP_GET, handleStatus);
  
  // Serve web dashboard
  server.on("/", HTTP_GET, handleWebDashboard);
  server.on("/dashboard", HTTP_GET, handleWebDashboard);
}

void handleGetSensors() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  DynamicJsonDocument doc(2048);
  
  // Sensor data - use average of both DHT sensors
  doc["temperature"] = (sensors.temperature1 + sensors.temperature2) / 2.0;
  doc["humidity"] = (sensors.humidity1 + sensors.humidity2) / 2.0;
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
  sensorsObj["ir_last_transmission"] = sensors.ir_code; // Timestamp of last IR transmission
  
  // LDR values (hanya 2 sensor)
  sensorsObj["ldr1_value"] = sensors.ldr1_value;
  sensorsObj["ldr2_value"] = sensors.ldr2_value;
  
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
  
  doc["timestamp"] = millis();
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  
  String response;
  serializeJson(doc, response);
  
  server.send(200, "application/json", response);
}

void handleDeviceControl() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(512);
    deserializeJson(doc, server.arg("plain"));
    
    String device = doc["device"];
    String action = doc["action"];
    bool state = (action == "on");
    
    // Control devices
    if (device == "ac1") devices.ac1 = state;
    else if (device == "ac2") devices.ac2 = state;
    else if (device == "lamp1") devices.lamp1 = state;
    else if (device == "lamp2") devices.lamp2 = state;
    else if (device == "computer") devices.computer = state;
    else if (device == "tv") devices.tv = state;
    else if (device == "dispenser") devices.dispenser = state;
    else if (device == "kettle") devices.kettle = state;
    else if (device == "coffee") devices.coffee = state;
    else if (device == "panel") {
      devices.panel = state;
      if (!state) {
        // Turn off all devices when panel is off
        devices.ac1 = false;
        devices.ac2 = false;
        devices.lamp1 = false;
        devices.lamp2 = false;
        devices.computer = false;
        devices.tv = false;
        devices.dispenser = false;
        devices.kettle = false;
        devices.coffee = false;
      }
    }
    
    updateRelayStates();
    
    server.send(200, "application/json", "{\"status\":\"success\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

void handleIRControl() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    
    String device = doc["device"];
    String command = doc["command"];
    
    // Send IR command
    sendIRCommand(device, command);
    
    DynamicJsonDocument response(128);
    response["status"] = "success";
    response["message"] = "IR command sent to " + device;
    response["timestamp"] = millis();
    
    String responseStr;
    serializeJson(response, responseStr);
    
    server.send(200, "application/json", responseStr);
  } else {
    server.send(400, "application/json", "{\"error\":\"No IR command data received\"}");
  }
}

void handleStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  DynamicJsonDocument doc(256);
  doc["status"] = "online";
  doc["ip"] = WiFi.localIP().toString();
  doc["uptime"] = millis();
  doc["ir_transmitter"] = "enabled";
  doc["last_ir_transmission"] = sensors.ir_code;
  
  String response;
  serializeJson(doc, response);
  
  server.send(200, "application/json", response);
}

void handleWebDashboard() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Smart Energy Dashboard</title>";
  html += "<style>";
  html += "body{font-family:Arial,sans-serif;margin:0;padding:20px;background:#1a1a1a;color:white;}";
  html += ".header{text-align:center;margin-bottom:30px;background:rgba(255,255,255,0.1);padding:20px;border-radius:15px;}";
  html += ".cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:20px;margin-bottom:30px;}";
  html += ".card{background:rgba(255,255,255,0.1);padding:20px;border-radius:15px;text-align:center;}";
  html += ".card h3{margin:0 0 10px 0;color:#4facfe;}";
  html += ".card .value{font-size:2em;font-weight:bold;}";
  html += ".devices{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:15px;}";
  html += ".device{background:rgba(255,255,255,0.1);padding:15px;border-radius:10px;text-align:center;}";
  html += ".device.on{background:rgba(40,167,69,0.3);}";
  html += ".device.off{background:rgba(220,53,69,0.3);}";
  html += ".btn{background:#4facfe;color:white;border:none;padding:10px 20px;border-radius:5px;cursor:pointer;margin:5px;}";
  html += ".btn:hover{background:#3d8bfd;}";
  html += ".status{font-weight:bold;}";
  html += ".on{color:#28a745;}";
  html += ".off{color:#dc3545;}";
  html += "</style></head><body>";
  
  html += "<div class='header'>";
  html += "<h1>Smart Energy Management System</h1>";
  html += "<p>Lab Teknik Tegangan Tinggi</p>";
  html += "<p id='lastUpdate'>Last Update: --</p>";
  html += "</div>";
  
  html += "<div class='cards'>";
  html += "<div class='card'><h3>Temperature</h3><div class='value' id='temperature'>--°C</div></div>";
  html += "<div class='card'><h3>Humidity</h3><div class='value' id='humidity'>--%</div></div>";
  html += "<div class='card'><h3>Light</h3><div class='value' id='light'>-- lux</div></div>";
  html += "<div class='card'><h3>People</h3><div class='value' id='people'>--</div></div>";
  html += "<div class='card'><h3>IR Status</h3><div class='value' id='irStatus'>Ready</div></div>";
  html += "</div>";
  
  html += "<h2>Device Control</h2>";
  html += "<div class='devices'>";
  html += "<div class='device' id='device-ac1'><h4>AC Unit 1</h4><div class='status' id='status-ac1'>OFF</div><button class='btn' onclick='toggleDevice(\"ac1\")'>Toggle</button></div>";
  html += "<div class='device' id='device-ac2'><h4>AC Unit 2</h4><div class='status' id='status-ac2'>OFF</div><button class='btn' onclick='toggleDevice(\"ac2\")'>Toggle</button></div>";
  html += "<div class='device' id='device-lamp1'><h4>Lamp Circuit 1</h4><div class='status' id='status-lamp1'>OFF</div><button class='btn' onclick='toggleDevice(\"lamp1\")'>Toggle</button></div>";
  html += "<div class='device' id='device-lamp2'><h4>Lamp Circuit 2</h4><div class='status' id='status-lamp2'>OFF</div><button class='btn' onclick='toggleDevice(\"lamp2\")'>Toggle</button></div>";
  html += "<div class='device' id='device-panel'><h4>Main Panel</h4><div class='status' id='status-panel'>ON</div><button class='btn' onclick='toggleDevice(\"panel\")'>Toggle</button></div>";
  html += "</div>";
  
  html += "<h2>IR Remote Control</h2>";
  html += "<div class='devices'>";
  html += "<div class='device'><h4>AC IR Control</h4><button class='btn' onclick='sendIRCommand(\"ac1\",\"on\")'>AC ON</button><button class='btn' onclick='sendIRCommand(\"ac1\",\"off\")'>AC OFF</button></div>";
  html += "<div class='device'><h4>TV IR Control</h4><button class='btn' onclick='sendIRCommand(\"tv\",\"on\")'>TV ON</button><button class='btn' onclick='sendIRCommand(\"tv\",\"off\")'>TV OFF</button></div>";
  html += "</div>";
  
  html += "<script>";
  html += "async function fetchData(){try{const response=await fetch('/api/sensors');const data=await response.json();";
  html += "document.getElementById('temperature').textContent=data.temperature.toFixed(1)+'°C';";
  html += "document.getElementById('humidity').textContent=data.humidity.toFixed(1)+'%';";
  html += "document.getElementById('light').textContent=data.light_intensity+' lux';";
  html += "document.getElementById('people').textContent=data.people_count;";
  html += "const irStatus=document.getElementById('irStatus');";
  html += "if(data.sensors_detail&&data.sensors_detail.ir_last_transmission>0){";
  html += "const timeSince=(Date.now()-data.sensors_detail.ir_last_transmission)/1000;";
  html += "if(timeSince<5){irStatus.textContent='Active';irStatus.style.color='#28a745';}";
  html += "else{irStatus.textContent='Ready';irStatus.style.color='#17a2b8';}}";
  html += "else{irStatus.textContent='Ready';irStatus.style.color='#17a2b8';}";
  html += "document.getElementById('lastUpdate').textContent='Last Update: '+new Date().toLocaleTimeString();";
  html += "updateDeviceStatus('ac1',data.devices.ac1);updateDeviceStatus('ac2',data.devices.ac2);";
  html += "updateDeviceStatus('lamp1',data.devices.lamp1);updateDeviceStatus('lamp2',data.devices.lamp2);";
  html += "updateDeviceStatus('panel',data.devices.panel);}catch(error){console.error('Error:',error);}}";
  html += "function updateDeviceStatus(device,status){";
  html += "const deviceEl=document.getElementById('device-'+device);";
  html += "const statusEl=document.getElementById('status-'+device);";
  html += "deviceEl.className='device '+(status?'on':'off');";
  html += "statusEl.textContent=status?'ON':'OFF';statusEl.className='status '+(status?'on':'off');}";
  html += "async function toggleDevice(device){try{const response=await fetch('/api/control',{";
  html += "method:'POST',headers:{'Content-Type':'application/json'},";
  html += "body:JSON.stringify({device:device,action:'toggle'})});";
  html += "if(response.ok){fetchData();}}catch(error){console.error('Error:',error);}}";
  html += "async function sendIRCommand(device,command){try{const response=await fetch('/api/ir',{";
  html += "method:'POST',headers:{'Content-Type':'application/json'},";
  html += "body:JSON.stringify({device:device,command:command})});";
  html += "if(response.ok){const result=await response.json();console.log('IR sent:',result.message);fetchData();}";
  html += "else{console.error('Failed to send IR');}}catch(error){console.error('Error:',error);}}";
  html += "setInterval(fetchData,2000);fetchData();";
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
}

void readSensors() {
  // Read DHT22 sensors
  sensors.temperature1 = dht1.readTemperature();
  sensors.humidity1 = dht1.readHumidity();
  sensors.temperature2 = dht2.readTemperature();
  sensors.humidity2 = dht2.readHumidity();
  
  // Check for DHT sensor errors
  if (isnan(sensors.temperature1)) sensors.temperature1 = 27.0;
  if (isnan(sensors.humidity1)) sensors.humidity1 = 65.0;
  if (isnan(sensors.temperature2)) sensors.temperature2 = 27.0;
  if (isnan(sensors.humidity2)) sensors.humidity2 = 65.0;
  
  // Read LDR sensors (hanya 2 sensor)
  sensors.ldr1_value = analogRead(LDR1_PIN);
  sensors.ldr2_value = analogRead(LDR2_PIN);
  
  // Calculate average light intensity dari 2 sensor
  sensors.light_intensity = map((sensors.ldr1_value + sensors.ldr2_value) / 2, 0, 4095, 0, 1000); // ESP32 ADC is 12-bit (0-4095)
  
  // Read proximity sensors
  sensors.proximity1 = digitalRead(PROXIMITY1_PIN);
  sensors.proximity2 = digitalRead(PROXIMITY2_PIN);
  
  // Calculate people count based on proximity sensors
  static bool last_prox1 = false;
  static bool last_prox2 = false;
  static int people_counter = 0;
  
  // Simple people counting logic
  if (sensors.proximity1 && !last_prox1) {
    people_counter++;
  }
  if (sensors.proximity2 && !last_prox2) {
    people_counter++;
  }
  
  // Reset counter if no proximity detected for a while
  static unsigned long last_detection = 0;
  if (sensors.proximity1 || sensors.proximity2) {
    last_detection = millis();
  } else if (millis() - last_detection > 30000) { // 30 seconds timeout
    people_counter = 0;
  }
  
  sensors.people_count = constrain(people_counter, 0, 20);
  
  last_prox1 = sensors.proximity1;
  last_prox2 = sensors.proximity2;
  
  // IR transmitter status (always 0 when not transmitting)
  sensors.ir_code = 0;
}

void updateRelayStates() {
  // Update relay states (LOW = ON for most relay modules, HIGH = OFF)
  // Only control devices if main panel is on
  digitalWrite(RELAY1_PIN, !(devices.ac1 && devices.panel));      // AC Unit 1
  digitalWrite(RELAY2_PIN, !(devices.ac2 && devices.panel));      // AC Unit 2  
  digitalWrite(RELAY3_PIN, !(devices.lamp1 && devices.panel));    // Lampu Circuit 1
  digitalWrite(RELAY4_PIN, !(devices.lamp2 && devices.panel));    // Lampu Circuit 2
  
  // Send IR commands for AC and TV control
  static bool last_ac1_state = false;
  static bool last_ac2_state = false;
  static bool last_tv_state = false;
  static unsigned long last_ir_time = 0;
  
  // Send IR command when AC1 state changes (with delay to prevent interference)
  if (devices.ac1 != last_ac1_state && (millis() - last_ir_time > 1000)) {
    sendIRCommand("ac1", devices.ac1 ? "on" : "off");
    last_ac1_state = devices.ac1;
    last_ir_time = millis();
  }
  
  // Send IR command when AC2 state changes
  if (devices.ac2 != last_ac2_state && (millis() - last_ir_time > 1000)) {
    sendIRCommand("ac2", devices.ac2 ? "on" : "off");
    last_ac2_state = devices.ac2;
    last_ir_time = millis();
  }
  
  // Send IR command when TV state changes
  if (devices.tv != last_tv_state && (millis() - last_ir_time > 1000)) {
    sendIRCommand("tv", devices.tv ? "on" : "off");
    last_tv_state = devices.tv;
    last_ir_time = millis();
  }
}

// ===== IR TRANSMITTER FUNCTIONS =====
// CATATAN PENTING: Kode IR di bawah ini adalah contoh implementasi
// Untuk penggunaan nyata, Anda perlu:
// 1. Menggunakan library IRremote untuk protokol yang lebih akurat
// 2. Merekam kode IR dari remote asli menggunakan IR receiver
// 3. Menyesuaikan timing dan protokol sesuai dengan perangkat target

void sendIRCommand(String device, String command) {
  // Fungsi untuk mengirim sinyal IR ke perangkat
  // Implementasi tergantung pada protokol IR yang digunakan
  
  Serial.println("Sending IR command: " + device + " - " + command);
  
  // Contoh implementasi sederhana untuk AC dan TV
  if (device == "ac1" && command == "on") {
    sendACOnCommand();
  } else if (device == "ac1" && command == "off") {
    sendACOffCommand();
  } else if (device == "ac2" && command == "on") {
    sendACOnCommand(); // Same command for both AC units
  } else if (device == "ac2" && command == "off") {
    sendACOffCommand();
  } else if (device == "tv" && command == "on") {
    sendTVOnCommand();
  } else if (device == "tv" && command == "off") {
    sendTVOffCommand();
  }
  
  // Update IR status
  sensors.ir_code = millis(); // Timestamp of last IR transmission
}

void sendACOnCommand() {
  // Kirim sinyal IR untuk menyalakan AC Panasonic
  // Protokol NEC standard untuk AC
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(9000); // Header pulse
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delayMicroseconds(4500); // Header space
  
  // Kirim data bit untuk AC ON (contoh kode untuk Panasonic AC)
  // Kode ini perlu disesuaikan dengan remote AC yang sebenarnya
  uint32_t ac_on_code = 0x02020DF0; // Contoh kode untuk AC ON
  
  for (int i = 31; i >= 0; i--) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(560);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    
    if (ac_on_code & (1UL << i)) {
      delayMicroseconds(1690); // Bit 1
    } else {
      delayMicroseconds(560);  // Bit 0
    }
  }
  
  // Stop bit
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(560);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  Serial.println("IR: AC ON command sent");
}

void sendACOffCommand() {
  // Kirim sinyal IR untuk mematikan AC Panasonic
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(9000);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delayMicroseconds(4500);
  
  // Kode untuk AC OFF
  uint32_t ac_off_code = 0x02020DF1; // Contoh kode untuk AC OFF
  
  for (int i = 31; i >= 0; i--) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(560);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    
    if (ac_off_code & (1UL << i)) {
      delayMicroseconds(1690); // Bit 1
    } else {
      delayMicroseconds(560);  // Bit 0
    }
  }
  
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(560);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  Serial.println("IR: AC OFF command sent");
}

void sendTVOnCommand() {
  // Kirim sinyal IR untuk menyalakan Smart TV Horizon
  // Protokol berbeda untuk TV (biasanya Sony atau Samsung)
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(2400); // TV header pulse (berbeda dengan AC)
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delayMicroseconds(600);
  
  // Kode untuk TV ON
  uint32_t tv_on_code = 0xA90; // Contoh kode untuk TV ON (12 bit)
  
  for (int i = 11; i >= 0; i--) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    
    if (tv_on_code & (1UL << i)) {
      delayMicroseconds(1200); // Bit 1
    } else {
      delayMicroseconds(600);  // Bit 0
    }
  }
  
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  Serial.println("IR: TV ON command sent");
}

void sendTVOffCommand() {
  // Kirim sinyal IR untuk mematikan Smart TV Horizon
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(2400);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delayMicroseconds(600);
  
  // Kode untuk TV OFF (biasanya sama dengan ON untuk toggle)
  uint32_t tv_off_code = 0xA90; // Same code for toggle
  
  for (int i = 11; i >= 0; i--) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    
    if (tv_off_code & (1UL << i)) {
      delayMicroseconds(1200); // Bit 1
    } else {
      delayMicroseconds(600);  // Bit 0
    }
  }
  
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  Serial.println("IR: TV OFF command sent");
}

// ===== UI FUNCTIONS =====

void showSplashScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  // Title
  tft.setTextSize(3);
  tft.drawString("Smart Energy", 50, 50);
  tft.setTextSize(2);
  tft.drawString("Management System", 60, 90);
  
  // Lab info
  tft.setTextSize(1);
  tft.drawString("Lab Teknik Tegangan Tinggi", 80, 130);
  tft.drawString("Initializing...", 120, 150);
  
  // Progress bar
  for (int i = 0; i < 200; i += 10) {
    tft.fillRect(60 + i, 180, 8, 10, TFT_BLUE);
    delay(100);
  }
}

void showErrorScreen(String message) {
  tft.fillScreen(TFT_RED);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setTextSize(2);
  tft.drawString("ERROR", 120, 80);
  tft.setTextSize(1);
  tft.drawString(message, 50, 120);
}

void updateDisplay() {
  switch (currentPage) {
    case PAGE_HOME:
      drawHomePage();
      break;
    case PAGE_MONITORING:
      drawMonitoringPage();
      break;
    case PAGE_DEVICES:
      drawDevicesPage();
      break;
    case PAGE_SETTINGS:
      drawSettingsPage();
      break;
  }
}

void drawHomePage() {
  tft.fillScreen(TFT_BLACK);
  
  // Header
  drawHeader("Smart Energy Dashboard");
  
  // Navigation buttons
  drawNavButtons();
  
  // Main info cards
  drawInfoCard(10, 60, 90, 50, "Temp", String((sensors.temperature1 + sensors.temperature2)/2, 1) + "°C", TFT_BLUE);
  drawInfoCard(110, 60, 90, 50, "Humidity", String((sensors.humidity1 + sensors.humidity2)/2, 1) + "%", TFT_GREEN);
  drawInfoCard(210, 60, 90, 50, "People", String(sensors.people_count), TFT_ORANGE);
  
  // Device status
  drawDeviceStatus(10, 120);
  
  // IP Address
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("IP: " + WiFi.localIP().toString(), 10, 220);
}

void drawMonitoringPage() {
  tft.fillScreen(TFT_BLACK);
  drawHeader("Sensor Monitoring");
  drawNavButtons();
  
  int y = 60;
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  tft.drawString("DHT22 #1: " + String(sensors.temperature1, 1) + "°C, " + String(sensors.humidity1, 1) + "%", 10, y);
  y += 15;
  tft.drawString("DHT22 #2: " + String(sensors.temperature2, 1) + "°C, " + String(sensors.humidity2, 1) + "%", 10, y);
  y += 15;
  tft.drawString("LDR #1: " + String(map(sensors.ldr1_value, 0, 4095, 0, 1000)) + " (Raw: " + String(sensors.ldr1_value) + ")", 10, y);
  y += 15;
  tft.drawString("LDR #2: " + String(map(sensors.ldr2_value, 0, 4095, 0, 1000)) + " (Raw: " + String(sensors.ldr2_value) + ")", 10, y);
  y += 15;
  tft.drawString("Proximity #1: " + String(sensors.proximity1 ? "DETECTED" : "CLEAR"), 10, y);
  y += 15;
  tft.drawString("Proximity #2: " + String(sensors.proximity2 ? "DETECTED" : "CLEAR"), 10, y);
  y += 15;
  tft.drawString("People Count: " + String(sensors.people_count), 10, y);
  y += 15;
  tft.drawString("Light Intensity: " + String(sensors.light_intensity), 10, y);
  y += 15;
  tft.drawString("IR Transmitter: " + String(sensors.ir_code > 0 ? "Active" : "Ready"), 10, y);
  y += 15;
  if (sensors.ir_code > 0) {
    unsigned long timeSince = (millis() - sensors.ir_code) / 1000;
    tft.drawString("Last IR: " + String(timeSince) + "s ago", 10, y);
  }
}

void drawDevicesPage() {
  tft.fillScreen(TFT_BLACK);
  drawHeader("Device Control");
  drawNavButtons();
  
  // Device control buttons
  drawDeviceButton(10, 60, 90, 40, "AC 1", devices.ac1, 1);
  drawDeviceButton(110, 60, 90, 40, "AC 2", devices.ac2, 2);
  drawDeviceButton(210, 60, 90, 40, "Lamp 1", devices.lamp1, 3);
  drawDeviceButton(10, 110, 90, 40, "Lamp 2", devices.lamp2, 4);
  drawDeviceButton(110, 110, 90, 40, "Panel", devices.panel, 5);
}

void drawSettingsPage() {
  tft.fillScreen(TFT_BLACK);
  drawHeader("System Settings");
  drawNavButtons();
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
  int y = 60;
  tft.drawString("System Information:", 10, y);
  y += 20;
  tft.drawString("IP Address: " + WiFi.localIP().toString(), 10, y);
  y += 15;
  tft.drawString("MAC Address: " + WiFi.macAddress(), 10, y);
  y += 15;
  tft.drawString("Free Heap: " + String(ESP.getFreeHeap()) + " bytes", 10, y);
  y += 15;
  tft.drawString("Uptime: " + String(millis() / 1000) + " seconds", 10, y);
  y += 15;
  tft.drawString("WiFi RSSI: " + String(WiFi.RSSI()) + " dBm", 10, y);
  y += 15;
  tft.drawString("IR Transmitter: GPIO " + String(IR_TRANSMITTER_PIN), 10, y);
  y += 15;
  tft.drawString("IR Status: " + String(sensors.ir_code > 0 ? "Recently Used" : "Ready"), 10, y);
}

void drawHeader(String title) {
  tft.fillRect(0, 0, 320, 30, TFT_BLUE);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.drawString(title, 10, 8);
}

void drawNavButtons() {
  // Navigation bar
  tft.fillRect(0, 200, 320, 40, TFT_DARKGREY);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.drawString("Home", 20, 215);
  tft.drawString("Monitor", 80, 215);
  tft.drawString("Devices", 140, 215);
  tft.drawString("Settings", 200, 215);
  
  // Highlight current page
  int x = 0;
  switch (currentPage) {
    case PAGE_HOME: x = 10; break;
    case PAGE_MONITORING: x = 70; break;
    case PAGE_DEVICES: x = 130; break;
    case PAGE_SETTINGS: x = 190; break;
  }
  tft.drawRect(x, 210, 60, 20, TFT_WHITE);
}

void drawInfoCard(int x, int y, int w, int h, String title, String value, uint16_t color) {
  tft.fillRect(x, y, w, h, TFT_DARKGREY);
  tft.drawRect(x, y, w, h, color);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.drawString(title, x + 5, y + 5);
  
  tft.setTextSize(2);
  tft.setTextColor(color, TFT_DARKGREY);
  tft.drawString(value, x + 5, y + 20);
}

void drawDeviceStatus(int x, int y) {
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Device Status:", x, y);
  
  y += 15;
  drawStatusIndicator(x, y, "AC1", devices.ac1);
  drawStatusIndicator(x + 60, y, "AC2", devices.ac2);
  drawStatusIndicator(x + 120, y, "L1", devices.lamp1);
  drawStatusIndicator(x + 160, y, "L2", devices.lamp2);
  drawStatusIndicator(x + 200, y, "Panel", devices.panel);
}

void drawStatusIndicator(int x, int y, String name, bool status) {
  uint16_t color = status ? TFT_GREEN : TFT_RED;
  tft.fillCircle(x + 5, y + 5, 5, color);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(name, x - 5, y + 15);
}

void drawDeviceButton(int x, int y, int w, int h, String name, bool status, int deviceId) {
  uint16_t bgColor = status ? TFT_GREEN : TFT_RED;
  uint16_t textColor = TFT_WHITE;
  
  tft.fillRect(x, y, w, h, bgColor);
  tft.drawRect(x, y, w, h, TFT_WHITE);
  
  tft.setTextSize(1);
  tft.setTextColor(textColor, bgColor);
  
  // Center text
  int textX = x + (w - name.length() * 6) / 2;
  int textY = y + (h - 8) / 2;
  tft.drawString(name, textX, textY);
  
  // Status text
  String statusText = status ? "ON" : "OFF";
  int statusX = x + (w - statusText.length() * 6) / 2;
  tft.drawString(statusText, statusX, textY + 12);
}