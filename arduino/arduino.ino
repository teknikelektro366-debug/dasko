#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

// ================= TFT =================
TFT_eSPI tft;

// ================= WIFI & WEB =================
const char* ssid = "LAB TEKNIK TEGANGAN TINGGI";        // Ganti dengan nama WiFi Anda
const char* password = "LAB TTT 2026"; // Ganti dengan password WiFi Anda

// PERBAIKAN: URL Laravel API untuk menyimpan data ke database
const char* laravelURL = "https://equilibrious-piddly-felicidad.ngrok-free.dev/api/sensor/data";  // Ganti dengan IP Laravel server Anda
const char* backupURL = "https://dasko.fst.unja.ac.id/dashboard";

// PERBAIKAN: Web Server dan WebSocket untuk realtime monitoring
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

unsigned long lastWebUpdate = 0;
unsigned long lastWebSocketUpdate = 0;
#define WEB_UPDATE_INTERVAL 5000   // Kirim ke Laravel setiap 5 detik
#define WEBSOCKET_UPDATE_INTERVAL 1000  // WebSocket update setiap 1 detik

// ================= IR =================
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

#define IR_PIN 4
IRPanasonicAc ac1(IR_PIN);
IRPanasonicAc ac2(IR_PIN);

// ================= SENSOR =================
#define PROX_IN   32
#define PROX_OUT  33
#define DHTPIN    27
#define DHTTYPE   DHT22
#define LDR_PIN   34

DHT dht(DHTPIN, DHTTYPE);

// ================= VARIABLE =================
int jumlahOrang = 0;
int lastJumlahOrang = -1;

// Proximity timing dengan anti double counting
bool lastIn  = HIGH;
bool lastOut = HIGH;
unsigned long inTime = 0;
unsigned long outTime = 0;
unsigned long lastPersonDetected = 0;
#define MAX_INTERVAL 1000
#define TIMEOUT 2000
#define PERSON_COOLDOWN 1500

// AC State
bool lastAC1 = false;
bool lastAC2 = false;
int  lastTemp = -1;

// PERBAIKAN: Tambah tracking untuk realtime data
struct RealtimeData {
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

// ================= WIFI CONNECTION =================
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("WiFi connected! IP: ");
    Serial.println(WiFi.localIP());
    
    // Update realtime data
    currentData.wifiStatus = "Connected";
    currentData.wifiRSSI = WiFi.RSSI();
    
    // Start web server dan websocket
    setupWebServer();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    
    Serial.println("Web Server started on port 80");
    Serial.println("WebSocket server started on port 81");
    Serial.println("Access realtime dashboard: http://" + WiFi.localIP().toString());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed!");
    currentData.wifiStatus = "Failed";
  }
}

// ================= WEBSOCKET EVENT HANDLER =================
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
      
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      
      // Send current data to new client
      sendRealtimeData();
      break;
    }
    
    case WStype_TEXT:
      Serial.printf("[%u] Received Text: %s\n", num, payload);
      
      // Handle commands from web interface
      String message = String((char*)payload);
      if (message == "GET_DATA") {
        sendRealtimeData();
      } else if (message.startsWith("RESET_COUNT")) {
        jumlahOrang = 0;
        Serial.println("People count reset via WebSocket");
      }
      break;
      
    default:
      break;
  }
}

// ================= SEND REALTIME DATA VIA WEBSOCKET =================
void sendRealtimeData() {
  StaticJsonDocument<500> doc;
  
  doc["jumlahOrang"] = currentData.jumlahOrang;
  doc["acStatus"] = currentData.acStatus;
  doc["setTemp"] = currentData.setTemp;
  doc["suhuRuang"] = currentData.suhuRuang;
  doc["humidity"] = currentData.humidity;
  doc["lightLevel"] = currentData.lightLevel;
  doc["proximity1"] = currentData.proximity1;
  doc["proximity2"] = currentData.proximity2;
  doc["timestamp"] = currentData.timestamp;
  doc["wifiStatus"] = currentData.wifiStatus;
  doc["wifiRSSI"] = currentData.wifiRSSI;
  doc["uptime"] = millis() / 1000;
  doc["freeHeap"] = ESP.getFreeHeap();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  webSocket.broadcastTXT(jsonString);
}

// ================= WEB SERVER SETUP =================
void setupWebServer() {
  // Serve main dashboard page
  server.on("/", HTTP_GET, []() {
    String html = getRealtimeDashboardHTML();
    server.send(200, "text/html", html);
  });
  
  // API endpoint for current data
  server.on("/api/data", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    
    StaticJsonDocument<500> doc;
    doc["jumlahOrang"] = currentData.jumlahOrang;
    doc["acStatus"] = currentData.acStatus;
    doc["setTemp"] = currentData.setTemp;
    doc["suhuRuang"] = currentData.suhuRuang;
    doc["humidity"] = currentData.humidity;
    doc["lightLevel"] = currentData.lightLevel;
    doc["proximity1"] = currentData.proximity1;
    doc["proximity2"] = currentData.proximity2;
    doc["timestamp"] = currentData.timestamp;
    doc["wifiStatus"] = currentData.wifiStatus;
    doc["wifiRSSI"] = currentData.wifiRSSI;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  // Control endpoint
  server.on("/api/control", HTTP_POST, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    
    if (server.hasArg("plain")) {
      StaticJsonDocument<200> doc;
      deserializeJson(doc, server.arg("plain"));
      
      String action = doc["action"];
      
      if (action == "reset_count") {
        jumlahOrang = 0;
        Serial.println("People count reset via API");
        server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Count reset\"}");
      } else if (action == "set_count") {
        int newCount = doc["count"];
        jumlahOrang = constrain(newCount, 0, 50);
        Serial.println("People count set to: " + String(jumlahOrang));
        server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Count updated\"}");
      } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid action\"}");
      }
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No data received\"}");
    }
  });
  
  server.begin();
}

// ================= REALTIME DASHBOARD HTML =================
String getRealtimeDashboardHTML() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Smart Energy - Realtime Monitor</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background: #1a1a1a; color: white; }
        .container { max-width: 1200px; margin: 0 auto; }
        .header { text-align: center; margin-bottom: 30px; background: rgba(255,255,255,0.1); padding: 20px; border-radius: 15px; }
        .cards { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-bottom: 30px; }
        .card { background: rgba(255,255,255,0.1); padding: 20px; border-radius: 15px; text-align: center; }
        .card h3 { margin: 0 0 10px 0; color: #4facfe; }
        .card .value { font-size: 2.5em; font-weight: bold; margin: 10px 0; }
        .card .unit { font-size: 0.8em; color: #ccc; }
        .status { padding: 5px 10px; border-radius: 20px; font-size: 0.9em; }
        .status.connected { background: #28a745; }
        .status.disconnected { background: #dc3545; }
        .controls { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-top: 20px; }
        .btn { background: #4facfe; color: white; border: none; padding: 10px 20px; border-radius: 8px; cursor: pointer; }
        .btn:hover { background: #3d8bfd; }
        .btn.danger { background: #dc3545; }
        .btn.danger:hover { background: #c82333; }
        .realtime-indicator { display: inline-block; width: 10px; height: 10px; background: #28a745; border-radius: 50%; margin-right: 8px; animation: blink 1s infinite; }
        @keyframes blink { 0%, 50% { opacity: 1; } 51%, 100% { opacity: 0.3; } }
        .sensor-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 10px; }
        .sensor-item { background: rgba(255,255,255,0.05); padding: 10px; border-radius: 8px; }
        .log { background: rgba(255,255,255,0.05); padding: 15px; border-radius: 10px; height: 200px; overflow-y: auto; font-family: monospace; font-size: 0.9em; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🏠 Smart Energy System</h1>
            <p>Lab Teknik Tegangan Tinggi - Realtime Monitor</p>
            <p><span class="realtime-indicator"></span>Live Data Stream</p>
            <p>Status: <span id="connectionStatus" class="status disconnected">Connecting...</span></p>
        </div>
        
        <div class="cards">
            <div class="card">
                <h3>👥 Jumlah Orang</h3>
                <div class="value" id="jumlahOrang">--</div>
                <div class="unit">orang</div>
            </div>
            
            <div class="card">
                <h3>❄️ Status AC</h3>
                <div class="value" id="acStatus" style="font-size: 1.5em;">--</div>
                <div class="unit">Set: <span id="setTemp">--</span>°C</div>
            </div>
            
            <div class="card">
                <h3>🌡️ Suhu Ruang</h3>
                <div class="value" id="suhuRuang">--</div>
                <div class="unit">°C</div>
            </div>
            
            <div class="card">
                <h3>💧 Kelembaban</h3>
                <div class="value" id="humidity">--</div>
                <div class="unit">%</div>
            </div>
            
            <div class="card">
                <h3>💡 Cahaya</h3>
                <div class="value" id="lightLevel">--</div>
                <div class="unit">lux</div>
            </div>
            
            <div class="card">
                <h3>📡 WiFi Signal</h3>
                <div class="value" id="wifiRSSI">--</div>
                <div class="unit">dBm</div>
            </div>
        </div>
        
        <div class="cards">
            <div class="card">
                <h3>🔍 Sensor Proximity</h3>
                <div class="sensor-grid">
                    <div class="sensor-item">
                        <strong>IN:</strong> <span id="proximity1">--</span>
                    </div>
                    <div class="sensor-item">
                        <strong>OUT:</strong> <span id="proximity2">--</span>
                    </div>
                </div>
            </div>
            
            <div class="card">
                <h3>⚙️ System Info</h3>
                <div class="sensor-grid">
                    <div class="sensor-item">
                        <strong>Uptime:</strong> <span id="uptime">--</span>
                    </div>
                    <div class="sensor-item">
                        <strong>Free Heap:</strong> <span id="freeHeap">--</span>
                    </div>
                </div>
            </div>
        </div>
        
        <div class="controls">
            <button class="btn" onclick="resetCount()">Reset Jumlah Orang</button>
            <button class="btn" onclick="requestData()">Refresh Data</button>
            <button class="btn danger" onclick="setCount()">Set Manual Count</button>
        </div>
        
        <div style="margin-top: 30px;">
            <h3>📊 Live Log</h3>
            <div id="log" class="log"></div>
        </div>
    </div>

    <script>
        let ws;
        let reconnectInterval;
        
        function connectWebSocket() {
            const wsUrl = `ws://${window.location.hostname}:81`;
            ws = new WebSocket(wsUrl);
            
            ws.onopen = function() {
                console.log('WebSocket connected');
                document.getElementById('connectionStatus').textContent = 'Connected';
                document.getElementById('connectionStatus').className = 'status connected';
                addLog('✓ WebSocket connected');
                
                if (reconnectInterval) {
                    clearInterval(reconnectInterval);
                    reconnectInterval = null;
                }
            };
            
            ws.onmessage = function(event) {
                try {
                    const data = JSON.parse(event.data);
                    updateDisplay(data);
                } catch (e) {
                    console.error('Error parsing WebSocket data:', e);
                }
            };
            
            ws.onclose = function() {
                console.log('WebSocket disconnected');
                document.getElementById('connectionStatus').textContent = 'Disconnected';
                document.getElementById('connectionStatus').className = 'status disconnected';
                addLog('✗ WebSocket disconnected - attempting reconnect...');
                
                // Auto reconnect
                if (!reconnectInterval) {
                    reconnectInterval = setInterval(connectWebSocket, 3000);
                }
            };
            
            ws.onerror = function(error) {
                console.error('WebSocket error:', error);
                addLog('✗ WebSocket error');
            };
        }
        
        function updateDisplay(data) {
            document.getElementById('jumlahOrang').textContent = data.jumlahOrang || '--';
            document.getElementById('acStatus').textContent = data.acStatus || '--';
            document.getElementById('setTemp').textContent = data.setTemp || '--';
            document.getElementById('suhuRuang').textContent = data.suhuRuang ? data.suhuRuang.toFixed(1) : '--';
            document.getElementById('humidity').textContent = data.humidity ? data.humidity.toFixed(1) : '--';
            document.getElementById('lightLevel').textContent = data.lightLevel || '--';
            document.getElementById('wifiRSSI').textContent = data.wifiRSSI || '--';
            document.getElementById('proximity1').textContent = data.proximity1 ? 'AKTIF' : 'OFF';
            document.getElementById('proximity2').textContent = data.proximity2 ? 'AKTIF' : 'OFF';
            document.getElementById('uptime').textContent = data.uptime ? formatTime(data.uptime) : '--';
            document.getElementById('freeHeap').textContent = data.freeHeap ? (data.freeHeap / 1024).toFixed(1) + ' KB' : '--';
            
            // Log significant changes
            static let lastCount = -1;
            if (data.jumlahOrang !== lastCount && lastCount !== -1) {
                addLog(`👥 Jumlah orang: ${lastCount} → ${data.jumlahOrang}`);
            }
            lastCount = data.jumlahOrang;
        }
        
        function addLog(message) {
            const log = document.getElementById('log');
            const time = new Date().toLocaleTimeString();
            log.innerHTML += `[${time}] ${message}<br>`;
            log.scrollTop = log.scrollHeight;
        }
        
        function formatTime(seconds) {
            const hours = Math.floor(seconds / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            const secs = seconds % 60;
            return `${hours}h ${minutes}m ${secs}s`;
        }
        
        function resetCount() {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send('RESET_COUNT');
                addLog('🔄 Reset count command sent');
            }
        }
        
        function requestData() {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send('GET_DATA');
                addLog('📡 Data refresh requested');
            }
        }
        
        function setCount() {
            const count = prompt('Masukkan jumlah orang:');
            if (count !== null && !isNaN(count)) {
                fetch('/api/control', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({action: 'set_count', count: parseInt(count)})
                }).then(response => response.json())
                  .then(data => addLog(`✓ ${data.message}`))
                  .catch(error => addLog(`✗ Error: ${error}`));
            }
        }
        
        // Initialize
        connectWebSocket();
        
        // Fallback: fetch data every 5 seconds if WebSocket fails
        setInterval(() => {
            if (!ws || ws.readyState !== WebSocket.OPEN) {
                fetch('/api/data')
                    .then(response => response.json())
                    .then(data => updateDisplay(data))
                    .catch(error => console.error('Fetch error:', error));
            }
        }, 5000);
    </script>
</body>
</html>
)";
  return html;
}

// ================= SEND DATA TO LARAVEL SERVER =================
void sendDataToLaravel(int jumlahOrang, String acStatus, int setTemp, float suhuRuang, float humidity, int lightLevel) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping Laravel update");
    return;
  }
  
  HTTPClient http;
  http.begin(laravelURL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("User-Agent", "ESP32-SmartEnergy/1.0");
  
  // Buat JSON data untuk Laravel
  StaticJsonDocument<400> doc;
  doc["device_id"] = "ESP32_Smart_Energy";
  doc["location"] = "Lab Teknik Tegangan Tinggi";
  doc["people_count"] = jumlahOrang;
  doc["ac_status"] = acStatus;
  doc["set_temperature"] = setTemp;
  doc["room_temperature"] = suhuRuang;
  doc["humidity"] = humidity;
  doc["light_level"] = lightLevel;
  doc["proximity_in"] = currentData.proximity1;
  doc["proximity_out"] = currentData.proximity2;
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["timestamp"] = millis();
  doc["status"] = "active";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.print("Sending to Laravel: ");
  Serial.println(jsonString);
  
  // Kirim POST request ke Laravel
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Laravel response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200 || httpResponseCode == 201) {
      Serial.println("✓ Data berhasil dikirim ke Laravel");
    } else {
      Serial.println("⚠ Laravel response: " + response);
    }
  } else {
    Serial.print("✗ Error sending to Laravel: ");
    Serial.println(httpResponseCode);
    
    // Fallback ke backup URL jika Laravel gagal
    sendDataToBackup(jumlahOrang, acStatus, setTemp, suhuRuang);
  }
  
  http.end();
}

// ================= SEND DATA TO BACKUP SERVER =================
void sendDataToBackup(int jumlahOrang, String acStatus, int setTemp, float suhuRuang) {
  HTTPClient http;
  http.begin(backupURL);
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<300> doc;
  doc["device_id"] = "ESP32_Room_Monitor";
  doc["location"] = "Ruang Meeting";
  doc["jumlah_orang"] = jumlahOrang;
  doc["ac_status"] = acStatus;
  doc["set_suhu"] = setTemp;
  doc["suhu_ruang"] = suhuRuang;
  doc["timestamp"] = millis();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  if (httpResponseCode > 0) {
    Serial.println("✓ Data sent to backup server");
  } else {
    Serial.println("✗ Backup server also failed");
  }
  
  http.end();
}
// ================= TFT =================
void drawUI() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(10, 10);
  tft.println("SMART ENERGY");
  tft.drawFastHLine(10, 50, 300, TFT_DARKGREY);
  
  tft.setTextSize(2);
  tft.setCursor(10, 70);
  tft.println("Jumlah Orang:");
  tft.setCursor(10, 120);
  tft.println("AC Status:");
  tft.setCursor(10, 170);
  tft.println("Set Suhu AC:");
  tft.setCursor(10, 220);
  tft.println("Suhu Ruang:");
  
  // PERBAIKAN: Tambah indikator WiFi dan realtime
  tft.setTextSize(1);
  tft.setCursor(10, 270);
  tft.println("WiFi: " + String(WiFi.isConnected() ? "Connected" : "Disconnected"));
  tft.setCursor(10, 285);
  tft.println("IP: " + WiFi.localIP().toString());
  tft.setCursor(10, 300);
  tft.println("Realtime: ACTIVE");
}

void updateTFT(String acStatus, int setSuhu, float suhuRuang) {
  tft.fillRect(200, 65, 130, 40, TFT_BLACK);
  tft.setTextSize(4);
  tft.setCursor(200, 65);
  tft.setTextColor(TFT_GREEN);
  tft.print(jumlahOrang);
  
  tft.fillRect(200, 115, 130, 35, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(200, 120);
  tft.setTextColor(TFT_CYAN);
  tft.print(acStatus);
  
  tft.fillRect(200, 165, 130, 35, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(200, 170);
  tft.setTextColor(TFT_YELLOW);
  if (setSuhu == 0) tft.print("-");
  else {
    tft.print(setSuhu);
    tft.print(" C");
  }
  
  tft.fillRect(200, 215, 130, 35, TFT_BLACK);
  tft.setCursor(200, 220);
  tft.print(suhuRuang);
  tft.print(" C");
  
  // PERBAIKAN: Update status realtime di TFT
  static unsigned long lastTftUpdate = 0;
  if (millis() - lastTftUpdate > 5000) {
    tft.fillRect(10, 270, 300, 45, TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 270);
    tft.println("WiFi: " + String(WiFi.isConnected() ? "Connected" : "Disconnected"));
    tft.setCursor(10, 285);
    tft.println("IP: " + WiFi.localIP().toString());
    tft.setCursor(10, 300);
    tft.setTextColor(TFT_GREEN);
    tft.println("Realtime: ACTIVE");
    lastTftUpdate = millis();
  }
}

// ================= PROXIMITY - ANTI SPAM & DELAY =================
void bacaProximity() {
  bool inNow  = digitalRead(PROX_IN);
  bool outNow = digitalRead(PROX_OUT);
  unsigned long now = millis();
  
  // Update realtime data
  currentData.proximity1 = (inNow == LOW);
  currentData.proximity2 = (outNow == LOW);
  
  // Debug status berkala (setiap 3 detik) untuk monitoring
  static unsigned long lastStatusPrint = 0;
  if (now - lastStatusPrint > 3000) {
    Serial.print("Status: IN=");
    Serial.print(inNow == LOW ? "AKTIF" : "OFF");
    Serial.print(" OUT=");
    Serial.print(outNow == LOW ? "AKTIF" : "OFF");
    Serial.print(" Jumlah=");
    Serial.println(jumlahOrang);
    lastStatusPrint = now;
  }
  
  // Cooldown - jangan deteksi terlalu cepat berturut-turut
  if (now - lastPersonDetected < PERSON_COOLDOWN) {
    return;
  }
  
  // Deteksi falling edge (HIGH -> LOW) dengan debouncing
  static unsigned long lastInTrigger = 0;
  static unsigned long lastOutTrigger = 0;
  
  if (lastIn == HIGH && inNow == LOW && inTime == 0) {
    if (now - lastInTrigger > 200) {
      inTime = now;
      lastInTrigger = now;
      Serial.println("✓ Sensor IN triggered");
    }
  }
  
  if (lastOut == HIGH && outNow == LOW && outTime == 0) {
    if (now - lastOutTrigger > 200) {
      outTime = now;
      lastOutTrigger = now;
      Serial.println("✓ Sensor OUT triggered");
    }
  }
  
  // MASUK: IN dulu, baru OUT
  if (inTime > 0 && outTime > 0 && inTime < outTime) {
    unsigned long interval = outTime - inTime;
    if (interval < MAX_INTERVAL && interval > 50) {
      jumlahOrang++;
      lastPersonDetected = now;
      Serial.print(">>> ORANG MASUK! Interval: ");
      Serial.print(interval);
      Serial.print("ms | Total: ");
      Serial.println(jumlahOrang);
    }
    inTime = 0;
    outTime = 0;
  }
  
  // KELUAR: OUT dulu, baru IN
  if (inTime > 0 && outTime > 0 && outTime < inTime) {
    unsigned long interval = inTime - outTime;
    if (interval < MAX_INTERVAL && interval > 50 && jumlahOrang > 0) {
      jumlahOrang--;
      lastPersonDetected = now;
      Serial.print("<<< ORANG KELUAR! Interval: ");
      Serial.print(interval);
      Serial.print("ms | Total: ");
      Serial.println(jumlahOrang);
    } else if (jumlahOrang == 0) {
      Serial.println("⚠ Tidak ada orang - deteksi OUT diabaikan");
    }
    inTime = 0;
    outTime = 0;
  }
  
  // Reset dengan timeout
  if (outTime > 0 && inTime == 0 && (now - outTime) > 800) {
    outTime = 0;
  }
  
  if (inTime > 0 && outTime == 0 && (now - inTime) > TIMEOUT) {
    inTime = 0;
  }
  
  if (outTime > 0 && (now - outTime) > TIMEOUT) {
    outTime = 0;
  }
  
  jumlahOrang = constrain(jumlahOrang, 0, 50);  // Increased max capacity
  
  lastIn = inNow;
  lastOut = outNow;
}

// ================= AC + IR =================
void kontrolAC(String &status, int &temp) {
  bool ac1ON = false;
  bool ac2ON = false;
  
  if (jumlahOrang == 0) {
    status = "AC OFF";
    temp = 0;
  }
  else if (jumlahOrang <= 5) {
    ac1ON = true;
    temp = 25;
    status = "1 AC ON";
  }
  else if (jumlahOrang <= 10) {
    ac1ON = true;
    temp = 22;
    status = "1 AC ON";
  }
  else if (jumlahOrang <= 15) {
    ac1ON = true;
    ac2ON = true;
    temp = 22;
    status = "2 AC ON";
  }
  else {
    ac1ON = true;
    ac2ON = true;
    temp = 20;
    status = "2 AC MAX";
  }
  
  // Kirim IR hanya jika berubah
  if (ac1ON != lastAC1 || ac2ON != lastAC2 || temp != lastTemp) {
    Serial.println("=== AC CONTROL CHANGE ===");
    Serial.println("AC1: " + String(lastAC1 ? "ON" : "OFF") + " -> " + String(ac1ON ? "ON" : "OFF"));
    Serial.println("AC2: " + String(lastAC2 ? "ON" : "OFF") + " -> " + String(ac2ON ? "ON" : "OFF"));
    Serial.println("Temp: " + String(lastTemp) + "°C -> " + String(temp) + "°C");
    
    if (!ac1ON && !ac2ON) {
      ac1.off(); ac1.send();
      ac2.off(); ac2.send();
      Serial.println("IR: Both AC turned OFF");
    } else {
      if (ac1ON) {
        ac1.on();
        ac1.setMode(kPanasonicAcCool);
        ac1.setTemp(temp);
        ac1.send();
        Serial.println("IR: AC1 ON at " + String(temp) + "°C");
      } else {
        ac1.off(); ac1.send();
        Serial.println("IR: AC1 OFF");
      }
      
      if (ac2ON) {
        ac2.on();
        ac2.setMode(kPanasonicAcCool);
        ac2.setTemp(temp);
        ac2.send();
        Serial.println("IR: AC2 ON at " + String(temp) + "°C");
      } else {
        ac2.off(); ac2.send();
        Serial.println("IR: AC2 OFF");
      }
    }
    
    lastAC1 = ac1ON;
    lastAC2 = ac2ON;
    lastTemp = temp;
    Serial.println("========================");
  }
}

// ================= UPDATE REALTIME DATA =================
void updateRealtimeData() {
  currentData.jumlahOrang = jumlahOrang;
  currentData.suhuRuang = dht.readTemperature();
  currentData.humidity = dht.readHumidity();
  currentData.lightLevel = map(analogRead(LDR_PIN), 0, 4095, 0, 1000);
  currentData.timestamp = millis();
  
  // Handle NaN values
  if (isnan(currentData.suhuRuang)) currentData.suhuRuang = 0.0;
  if (isnan(currentData.humidity)) currentData.humidity = 0.0;
  
  // Update WiFi status
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
  
  Serial.println("=== SMART ENERGY SYSTEM - REALTIME VERSION ===");
  
  pinMode(PROX_IN, INPUT);
  pinMode(PROX_OUT, INPUT);
  pinMode(LDR_PIN, INPUT);
  
  dht.begin();
  ac1.begin();
  ac2.begin();
  
  tft.init();
  tft.setRotation(1);
  drawUI();
  
  // Connect to WiFi
  connectWiFi();
  
  Serial.println("SYSTEM READY - Realtime monitoring active");
  Serial.println("Access dashboard: http://" + WiFi.localIP().toString());
}

// ================= LOOP =================
void loop() {
  // Handle web server dan websocket
  server.handleClient();
  webSocket.loop();
  
  // Baca proximity dan update data
  bacaProximity();
  updateRealtimeData();
  
  String acStatus = currentData.acStatus;
  int setTemp = currentData.setTemp;
  kontrolAC(acStatus, setTemp);
  
  // Update current data dengan hasil kontrol AC
  currentData.acStatus = acStatus;
  currentData.setTemp = setTemp;
  
  // Update TFT jika ada perubahan
  if (jumlahOrang != lastJumlahOrang) {
    updateTFT(acStatus, setTemp, currentData.suhuRuang);
    Serial.println("========================");
    Serial.print("Jumlah Orang : "); Serial.println(jumlahOrang);
    Serial.print("AC Status    : "); Serial.println(acStatus);
    Serial.print("Set Suhu AC  : "); Serial.println(setTemp);
    Serial.print("Suhu Ruang   : "); Serial.println(currentData.suhuRuang);
    Serial.print("Humidity     : "); Serial.println(currentData.humidity);
    Serial.print("Light Level  : "); Serial.println(currentData.lightLevel);
    Serial.println("========================");
    lastJumlahOrang = jumlahOrang;
  }
  
  // Kirim data realtime via WebSocket setiap 1 detik
  unsigned long now = millis();
  if (now - lastWebSocketUpdate > WEBSOCKET_UPDATE_INTERVAL) {
    sendRealtimeData();
    lastWebSocketUpdate = now;
  }
  
  // Kirim data ke Laravel server setiap 5 detik
  if (now - lastWebUpdate > WEB_UPDATE_INTERVAL) {
    sendDataToLaravel(jumlahOrang, acStatus, setTemp, currentData.suhuRuang, 
                     currentData.humidity, currentData.lightLevel);
    lastWebUpdate = now;
  }
  
  // Reconnect WiFi jika terputus
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, attempting reconnection...");
    connectWiFi();
  }
  
  delay(100);  // Small delay untuk stabilitas
}