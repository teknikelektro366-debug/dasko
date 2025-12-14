/****************************************************
 * ESP32 – SENSOR + TFT + IR PANASONIC + JSON API
 * Library JSON: Arduino_JSON (bukan ArduinoJson)
 * WiFi + HTTP POST + HTTP GET
 ****************************************************/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

#include <DHT.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// =============================
// KONFIGURASI WIFI & API
// =============================
const char* WIFI_SSID = "ISI_WIFI_SSID";
const char* WIFI_PASS = "ISI_WIFI_PASS";

// contoh: https://ac.id/api/update.php
String API_UPDATE   = "https://ac.id/api/update.php";
String API_GETCMD   = "https://ac.id/api/getcommand.php";

// =============================
// PIN (SESUAI WIRING PDF)
// =============================
#define IR_TX_PIN   17

#define DHT_DALAM   4
#define DHT_LUAR    16
#define DHTTYPE     DHT22

#define PROX_IN     13
#define PROX_OUT    14

TFT_eSPI tft = TFT_eSPI();

// =============================
// OBJECT
// =============================
IRPanasonicAc ac1(IR_TX_PIN);
IRPanasonicAc ac2(IR_TX_PIN);

DHT dhtIndoor(DHT_DALAM, DHTTYPE);
DHT dhtOutdoor(DHT_LUAR, DHTTYPE);

// =============================
// VARIABLE
// =============================
int jumlahOrang = 0;
bool lastIn = 0, lastOut = 0;

int suhuSetAC1 = 26;
int suhuSetAC2 = 26;

bool ac1_ON = false;
bool ac2_ON = false;

unsigned long lastSend = 0;
unsigned long lastCmd = 0;

// ==================================================================
// JSON BUILDER
// ==================================================================
String createJSON(
  float suhuDalam,
  float suhuLuar,
  float kelembaban,
  int jumlahOrang,
  bool ac1_ON,
  int suhuSetAC1,
  bool ac2_ON,
  int suhuSetAC2
) {
  JsonVar data;

  data["suhu_dalam"]   = suhuDalam;
  data["suhu_luar"]    = suhuLuar;
  data["kelembaban"]   = kelembaban;
  data["jumlah_orang"] = jumlahOrang;

  data["ac1_status"]   = ac1_ON ? "ON" : "OFF";
  data["ac1_temp"]     = suhuSetAC1;

  data["ac2_status"]   = ac2_ON ? "ON" : "OFF";
  data["ac2_temp"]     = suhuSetAC2;

  data["last_update"]  = (unsigned long)(millis() / 1000);

  return JSson.stringify(data);
}

// ==================================================================
// WIFI CONNECT
// ==================================================================
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.println("\nConnecting WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 30) {
    delay(300);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\nConnected. IP = ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi FAILED");
  }
}

// ==================================================================
// POST DATA KE SERVER
// ==================================================================
void sendToAPI(String jsonData) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(API_UPDATE);
  http.addHeader("Content-Type", "application/json");

  int code = http.POST(jsonData);
  Serial.print("POST Response: ");
  Serial.println(code);

  http.end();
}

// ==================================================================
// GET COMMAND DARI SERVER
// ==================================================================
void getCommand() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(API_GETCMD);

  int code = http.GET();
  if (code != 200) {
    Serial.println("GET CMD FAIL");
    http.end();
    return;
  }

  String payload = http.getString();
  http.end();

  JSONVar cmd = Json.parse(payload);

  if (Json.typeof(cmd) == "undefined") {
    Serial.println("JSON CMD ERROR");
    return;
  }

  String ac1_cmd = (const char*)cmd["ac1_cmd"];
  int ac1_temp   = cmd["ac1_set_temp"];

  String ac2_cmd = (const char*)cmd["ac2_cmd"];
  int ac2_temp   = cmd["ac2_set_temp"];

  // AC1 command
  if (ac1_cmd == "ON") {
    ac1.on(); ac1.setMode(kPanasonicAcCool); ac1.setFan(2);
    if (ac1_temp > 0) ac1.setTemp(ac1_temp);
    ac1.send();
    ac1_ON = true;
  } 
  else if (ac1_cmd == "OFF") {
    ac1.off(); ac1.send();
    ac1_ON = false;
  }

  // AC2 command
  if (ac2_cmd == "ON") {
    ac2.on(); ac2.setMode(kPanasonicAcCool); ac2.setFan(2);
    if (ac2_temp > 0) ac2.setTemp(ac2_temp);
    ac2.send();
    ac2_ON = true;
  } 
  else if (ac2_cmd == "OFF") {
    ac2.off(); ac2.send();
    ac2_ON = false;
  }

  Serial.println("Command Applied");
}

// ==================================================================
// HITUNG ORANG
// ==================================================================
void updatePeople() {
  bool masuk = digitalRead(PROX_IN);
  bool keluar = digitalRead(PROX_OUT);

  if (masuk && !lastIn) jumlahOrang++;
  if (keluar && !lastOut && jumlahOrang > 0) jumlahOrang--;

  lastIn = masuk;
  lastOut = keluar;
}

// ==================================================================
// TFT TAMPILKAN SUHU
// ==================================================================
void showOnTFT(float suhuDalam) {
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.setCursor(10, 10);
  tft.println("SUHU RUANGAN");

  tft.setTextSize(5);
  tft.setCursor(10, 70);

  if (isnan(suhuDalam)) {
    tft.println("ERROR");
  } else {
    tft.printf("%.1f C", suhuDalam);
  }
}

// ==================================================================
// SETUP
// ==================================================================
void setup() {
  Serial.begin(115200);

  pinMode(PROX_IN, INPUT);
  pinMode(PROX_OUT, INPUT);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  dhtIndoor.begin();
  dhtOutdoor.begin();

  ac1.begin();
  ac2.begin();

  connectWiFi();
  Serial.println("SYSTEM READY");
}

// ==================================================================
// LOOP
// ==================================================================
void loop() {
  updatePeople();

  float suhuDalam = dhtIndoor.readTemperature();
  float kelembaban = dhtIndoor.readHumidity();
  float suhuLuar  = dhtOutdoor.readTemperature();

  showOnTFT(suhuDalam);

  // Kirim ke server tiap 5 detik
  if (millis() - lastSend > 5000) {
    lastSend = millis();

    String jsonPayload = createJSON(
      suhuDalam,
      suhuLuar,
      kelembaban,
      jumlahOrang,
      ac1_ON,
      suhuSetAC1,
      ac2_ON,
      suhuSetAC2
    );

    Serial.println("SEND JSON:");
    Serial.println(jsonPayload);

    connectWiFi();
    sendToAPI(jsonPayload);
  }

  // Ambil command tiap 3 detik
  if (millis() - lastCmd > 3000) {
    lastCmd = millis();
    connectWiFi();
    getCommand();
  }

  delay(300);
}
