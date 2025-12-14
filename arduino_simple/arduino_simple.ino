// VERSI SEDERHANA TANPA TFT - UNTUK TESTING AWAL
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "LAB TEKNIK TEGANGAN TINGGI";
const char* password = "LAB TTT 2026";

// Login credentials
const char* login_username = "admin";
const char* login_password = "elektro2024";

// Session management
String sessionToken = "";
unsigned long sessionExpiry = 0;
const unsigned long SESSION_DURATION = 3600000; // 1 hour in milliseconds

// Pin definitions
#define DHT1_PIN 4
#define DHT2_PIN 16
#define DHT_TYPE DHT22

#define LDR1_PIN 32
#define LDR2_PIN 33

#define PROXIMITY1_PIN 13
#define PROXIMITY2_PIN 14

#define IR_TRANSMITTER_PIN 15

#define RELAY1_PIN 25  // AC Unit 1
#define RELAY2_PIN 26  // AC Unit 2
#define RELAY3_PIN 27  // Lampu Circuit 1
#define RELAY4_PIN 12  // Lampu Circuit 2

// Sensors
DHT dht1(DHT1_PIN, DHT_TYPE);
DHT dht2(DHT2_PIN, DHT_TYPE);
WebServer server(80);

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
  bool fridge = true;
  bool router = true;
  bool panel = true;
} devices;

// AC Automation System - Proximity sebagai sensor utama, DHT sebagai pendamping
struct ACAutomationRule {
  int minPeople;
  int maxPeople;
  float minOutdoorTemp;
  float maxOutdoorTemp;
  int acUnitsNeeded;
  int targetTemp;
  String description;
};

// Tabel 9: Estimasi Suhu Untuk Dua AC Berdasarkan Kondisi - IMPLEMENTASI PERSIS
ACAutomationRule acRules[] = {
  {0, 0, 0.0, 50.0, 0, 28, "Tidak ada orang - Mode gradual 15 menit ke 28°C"},
  {5, 10, 30.0, 32.0, 1, 25, "5-10 orang, 30-32°C - 1 AC, target 25°C (beban ringan)"},
  {5, 10, 32.1, 35.0, 1, 26, "5-10 orang, 32-35°C - 1 AC, target 26°C (suhu luar tinggi)"},
  {10, 13, 30.0, 32.0, 2, 24, "10-13 orang, 30-32°C - 2 AC, target 24°C (beban meningkat)"},
  {10, 20, 32.1, 35.0, 2, 25, "10-20 orang, 32-35°C - 2 AC, target 25°C (kondisi luar panas)"}
};

const int NUM_AC_RULES = sizeof(acRules) / sizeof(acRules[0]);

// Temperature adjustment based on outdoor conditions (DHT sensor)
struct TemperatureAdjustment {
  float minOutdoorTemp;
  float maxOutdoorTemp;
  int tempAdjustment;
  String condition;
};

// Penyesuaian suhu berdasarkan kondisi udara luar (sesuai Tabel 9)
TemperatureAdjustment tempAdjustments[] = {
  {0.0, 29.9, 0, "Udara luar sejuk - suhu AC standar"},
  {30.0, 32.0, 0, "Udara luar normal (30-32°C) - target 25-26°C atau 24-25°C"},
  {32.1, 35.0, +1, "Udara luar panas (32-35°C) - target 26-27°C atau 25-26°C"},
  {35.1, 40.0, +2, "Udara luar sangat panas - suhu AC dinaikkan"},
  {40.1, 50.0, +3, "Udara luar ekstrem panas - suhu AC dinaikkan maksimal"}
};

const int NUM_TEMP_ADJUSTMENTS = sizeof(tempAdjustments) / sizeof(tempAdjustments[0]);

// AC Control Variables
int currentTargetTemp = 25;
int baseTargetTemp = 25;
int temperatureAdjustment = 0;
bool autoACEnabled = true;
unsigned long lastACCheck = 0;
const unsigned long AC_CHECK_INTERVAL = 30000; // Check every 30 seconds

// AC Gradual Temperature Control (when no people detected)
unsigned long acGradualStartTime = 0;
bool acGradualMode = false;
int acOriginalTemp = 25;
const unsigned long AC_GRADUAL_DURATION = 15 * 60 * 1000; // 15 minutes
const int AC_ROOM_TEMPERATURE = 28; // Target room temperature when no people

// Working Hours System (sesuai dengan HTML command)
bool workingDaysEnabled = true; // Senin-Jumat
int workingStartHour = 7;  // 07:00 WIB
int workingEndHour = 17;   // 17:00 WIB
bool scheduledShutdownEnabled = true;
bool autoShutdownEnabled = true;
bool presenceDetectionEnabled = true;
bool autoLampControlEnabled = true;

// Time management (simulasi - dalam implementasi nyata gunakan RTC)
struct TimeInfo {
  int hour = 8;        // Default jam 8 pagi
  int minute = 0;
  int dayOfWeek = 2;   // Default Selasa (1=Senin, 7=Minggu)
} currentTime;

// Energy Saving System
unsigned long lastPersonDetected = 0;
bool energySavingMode = false;
bool panelAutoShutdownEnabled = true;
const unsigned long ENERGY_SAVING_DELAY = 15 * 60 * 1000; // 15 menit
const unsigned long PANEL_SHUTDOWN_DELAY = 30 * 60 * 1000; // 30 menit total
const int ENERGY_SAVING_TEMP = 28; // Suhu hemat energi

enum EnergyState {
  NORMAL_OPERATION,     // Ada orang, operasi normal
  ENERGY_SAVING,        // 15 menit tanpa orang, AC naik ke 28°C
  PANEL_SHUTDOWN        // 30 menit tanpa orang, panel mati
};

EnergyState currentEnergyState = NORMAL_OPERATION;

// Proximity sensor variables (sensor utama)
int proximityPeopleCount = 0;
bool lastProximity1 = false;
bool lastProximity2 = false;
unsigned long lastProximityChange = 0;

// Sensor data
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
} sensors;

void setup() {
  Serial.begin(115200);
  
  // Initialize DHT sensors
  dht1.begin();
  dht2.begin();
  
  // Initialize pins
  pinMode(PROXIMITY1_PIN, INPUT);
  pinMode(PROXIMITY2_PIN, INPUT);
  pinMode(IR_TRANSMITTER_PIN, OUTPUT);
  pinMode(LDR1_PIN, INPUT);
  pinMode(LDR2_PIN, INPUT);
  
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Set initial relay states
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup web server
  setupRoutes();
  server.enableCORS(true);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("=================================");
  Serial.println("LOGIN CREDENTIALS:");
  Serial.println("Username: admin");
  Serial.println("Password: elektro2024");
  Serial.println("=================================");
  Serial.print("Access dashboard at: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
  
  // Update simulated time
  updateSimulatedTime();
  
  // Working hours check
  checkWorkingHours();
  
  // Read sensors every 2 seconds (hanya saat jam kerja)
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 2000) {
    readSensors();
    lastSensorRead = millis();
  }
  
  // Energy Saving System check (hanya saat jam kerja)
  if (isWorkingHours()) {
    checkEnergySavingSystem();
  }
  
  // AC Automation check (hanya jika panel menyala dan jam kerja)
  if (autoACEnabled && devices.panel && isWorkingHours() && millis() - lastACCheck > AC_CHECK_INTERVAL) {
    checkACAutomation();
    lastACCheck = millis();
  }
  
  // Scheduled shutdown check
  if (scheduledShutdownEnabled) {
    checkScheduledShutdown();
  }
  
  // Clean expired sessions
  if (sessionExpiry > 0 && millis() > sessionExpiry) {
    sessionToken = "";
    sessionExpiry = 0;
    Serial.println("Session expired");
  }
  
  delay(50);
}

// Authentication functions
bool isAuthenticated() {
  if (sessionToken == "" || sessionExpiry == 0) {
    return false;
  }
  
  if (millis() > sessionExpiry) {
    sessionToken = "";
    sessionExpiry = 0;
    return false;
  }
  
  String authHeader = server.header("Authorization");
  return (authHeader == "Bearer " + sessionToken);
}

String generateSessionToken() {
  String token = "";
  for (int i = 0; i < 16; i++) {
    token += String(random(0, 16), HEX);
  }
  return token;
}

// ===== WORKING HOURS SYSTEM =====
bool isWorkingDay() {
  if (!workingDaysEnabled) return true; // Jika disabled, anggap setiap hari adalah hari kerja
  
  // dayOfWeek: 1=Senin, 2=Selasa, ..., 6=Sabtu, 7=Minggu
  return currentTime.dayOfWeek >= 1 && currentTime.dayOfWeek <= 5; // Senin-Jumat
}

bool isWorkingHours() {
  // Jam operasional: 07:00 - 17:00
  bool isOperatingTime = currentTime.hour >= workingStartHour && currentTime.hour < workingEndHour;
  bool isWorkingDayToday = isWorkingDay();
  
  return isOperatingTime && isWorkingDayToday;
}

String getWorkingHoursStatus() {
  if (isWorkingHours()) {
    return "Jam Kerja Aktif";
  } else {
    if (!isWorkingDay() && workingDaysEnabled) {
      return getDayName(currentTime.dayOfWeek) + " (Hari Libur)";
    } else {
      return "Di Luar Jam Kerja";
    }
  }
}

String getDayName(int dayOfWeek) {
  String days[] = {"", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
  if (dayOfWeek >= 1 && dayOfWeek <= 7) {
    return days[dayOfWeek];
  }
  return "Unknown";
}

void updateSimulatedTime() {
  // Simulasi waktu - dalam implementasi nyata, gunakan RTC atau NTP
  static unsigned long lastTimeUpdate = 0;
  if (millis() - lastTimeUpdate > 60000) { // Update setiap menit
    currentTime.minute++;
    if (currentTime.minute >= 60) {
      currentTime.minute = 0;
      currentTime.hour++;
      if (currentTime.hour >= 24) {
        currentTime.hour = 0;
        currentTime.dayOfWeek++;
        if (currentTime.dayOfWeek > 7) {
          currentTime.dayOfWeek = 1;
        }
      }
    }
    lastTimeUpdate = millis();
  }
}

void checkWorkingHours() {
  static bool lastWorkingHoursState = false;
  static unsigned long lastWorkingHoursCheck = 0;
  
  // Check setiap 30 detik
  if (millis() - lastWorkingHoursCheck < 30000) {
    return;
  }
  lastWorkingHoursCheck = millis();
  
  bool currentWorkingHours = isWorkingHours();
  
  // Log perubahan status jam kerja
  if (currentWorkingHours != lastWorkingHoursState) {
    Serial.println("=== WORKING HOURS STATUS CHANGE ===");
    Serial.println("Time: " + String(currentTime.hour) + ":" + 
                   (currentTime.minute < 10 ? "0" : "") + String(currentTime.minute));
    Serial.println("Day: " + getDayName(currentTime.dayOfWeek));
    Serial.println("Working Hours: " + String(currentWorkingHours ? "ACTIVE" : "INACTIVE"));
    Serial.println("Status: " + getWorkingHoursStatus());
    
    if (!currentWorkingHours && lastWorkingHoursState) {
      // Jam kerja berakhir - lakukan shutdown otomatis
      Serial.println("Working hours ended - initiating automatic shutdown");
      executeWorkingHoursShutdown();
    } else if (currentWorkingHours && !lastWorkingHoursState) {
      // Jam kerja dimulai - reset energy saving state
      Serial.println("Working hours started - resetting energy saving state");
      resetEnergyState();
    }
    
    Serial.println("===================================");
    lastWorkingHoursState = currentWorkingHours;
  }
  
  // Jika di luar jam kerja, pastikan semua perangkat mati (kecuali always-on)
  if (!currentWorkingHours) {
    static unsigned long lastOutsideHoursCheck = 0;
    if (millis() - lastOutsideHoursCheck > 120000) { // Check setiap 2 menit
      checkOutsideHoursDevices();
      lastOutsideHoursCheck = millis();
    }
  }
}

void checkScheduledShutdown() {
  if (!scheduledShutdownEnabled) {
    return;
  }
  
  static unsigned long lastScheduledCheck = 0;
  
  // Check setiap menit
  if (millis() - lastScheduledCheck < 60000) {
    return;
  }
  lastScheduledCheck = millis();
  
  // Cek apakah waktu adalah 17:00 (akhir jam kerja)
  if (currentTime.hour == workingEndHour && currentTime.minute == 0) {
    if (isWorkingDay() || !workingDaysEnabled) {
      Serial.println("=== SCHEDULED SHUTDOWN ===");
      Serial.println("Time: 17:00 - End of working hours");
      Serial.println("Executing scheduled shutdown");
      executeWorkingHoursShutdown();
      Serial.println("==========================");
    }
  }
  
  // Cek apakah di luar jam operasional dan ada perangkat yang masih menyala
  if (!isWorkingHours()) {
    static unsigned long lastOutsideHoursShutdown = 0;
    
    // Hanya check setiap 5 menit untuk menghindari spam
    if (millis() - lastOutsideHoursShutdown > 300000) {
      bool anyDeviceOn = devices.ac1 || devices.ac2 || devices.lamp1 || devices.lamp2 || 
                         devices.computer || devices.tv || devices.dispenser || 
                         devices.kettle || devices.coffee;
      
      if (anyDeviceOn) {
        Serial.println("=== OUTSIDE HOURS SHUTDOWN ===");
        Serial.println("Devices still on outside working hours - shutting down");
        executeWorkingHoursShutdown();
        lastOutsideHoursShutdown = millis();
        Serial.println("==============================");
      }
    }
  }
}

void executeWorkingHoursShutdown() {
  Serial.println("=== EXECUTING WORKING HOURS SHUTDOWN ===");
  
  // Matikan AC units
  if (devices.ac1) {
    devices.ac1 = false;
    sendIRCommand("ac1", "off");
    Serial.println("AC1 OFF - Working hours shutdown");
  }
  
  if (devices.ac2) {
    devices.ac2 = false;
    sendIRCommand("ac2", "off");
    Serial.println("AC2 OFF - Working hours shutdown");
  }
  
  // Matikan lampu
  if (devices.lamp1) {
    devices.lamp1 = false;
    Serial.println("Lamp Circuit 1 OFF - Working hours shutdown");
  }
  
  if (devices.lamp2) {
    devices.lamp2 = false;
    Serial.println("Lamp Circuit 2 OFF - Working hours shutdown");
  }
  
  // Matikan perangkat lainnya (kecuali always-on)
  devices.computer = false;
  devices.tv = false;
  devices.dispenser = false;
  devices.kettle = false;
  devices.coffee = false;
  
  Serial.println("Computer OFF - Working hours shutdown");
  Serial.println("TV OFF - Working hours shutdown");
  Serial.println("Dispenser OFF - Working hours shutdown");
  Serial.println("Kettle OFF - Working hours shutdown");
  Serial.println("Coffee Machine OFF - Working hours shutdown");
  
  // Update relay states
  updateRelayStates();
  
  // Reset energy saving state
  currentEnergyState = NORMAL_OPERATION;
  energySavingMode = false;
  
  Serial.println("All non-essential devices shut down for working hours compliance");
  Serial.println("Always-on devices (fridge, router, panel) remain active");
  Serial.println("========================================");
}

void checkOutsideHoursDevices() {
  bool anyDeviceOn = devices.ac1 || devices.ac2 || devices.lamp1 || devices.lamp2 || 
                     devices.computer || devices.tv || devices.dispenser || 
                     devices.kettle || devices.coffee;
  
  if (anyDeviceOn) {
    Serial.println("=== OUTSIDE HOURS DEVICE CHECK ===");
    Serial.println("Warning: Devices detected ON outside working hours");
    Serial.println("Current time: " + String(currentTime.hour) + ":" + 
                   (currentTime.minute < 10 ? "0" : "") + String(currentTime.minute));
    Serial.println("Working hours: " + String(workingStartHour) + ":00 - " + 
                   String(workingEndHour) + ":00");
    Serial.println("Day: " + getDayName(currentTime.dayOfWeek));
    
    if (scheduledShutdownEnabled) {
      Serial.println("Scheduled shutdown enabled - turning off devices");
      executeWorkingHoursShutdown();
    } else {
      Serial.println("Scheduled shutdown disabled - devices remain on");
    }
    Serial.println("==================================");
  }
}

void resetEnergyState() {
  currentEnergyState = NORMAL_OPERATION;
  energySavingMode = false;
  lastPersonDetected = millis();
  
  Serial.println("Energy saving state reset - normal operation resumed");
}

// ===== AC AUTOMATION SYSTEM =====
void checkACAutomation() {
  if (!devices.panel) {
    Serial.println("AC Automation: Panel is OFF, skipping automation");
    return;
  }
  
  // SENSOR UTAMA: Proximity untuk jumlah orang
  int peopleCount = proximityPeopleCount;
  
  // SENSOR PENDAMPING: DHT untuk kondisi udara luar
  float outdoorTemp = (sensors.temperature1 + sensors.temperature2) / 2.0;
  
  Serial.println("=== AC AUTOMATION CHECK ===");
  Serial.println("PROXIMITY (Main): People Count = " + String(peopleCount));
  Serial.println("DHT (Support): Outdoor Temperature = " + String(outdoorTemp, 1) + "°C");
  
  // STEP 1: Cari aturan yang sesuai berdasarkan jumlah orang DAN suhu luar (TABEL 9 PERSIS)
  ACAutomationRule* matchedRule = nullptr;
  
  // Jika tidak ada orang, langsung ke mode gradual
  if (peopleCount == 0) {
    Serial.println("=== NO PEOPLE DETECTED - AC GRADUAL MODE ===");
    Serial.println("Tabel 9: 0 orang -> Mode gradual 15 menit ke suhu ruang (28°C)");
    
    // Jika AC menyala dan tidak ada orang, mulai gradual temperature increase
    if (devices.ac1 || devices.ac2) {
      if (!acGradualMode) {
        // Mulai mode gradual temperature increase
        acGradualMode = true;
        acGradualStartTime = millis();
        acOriginalTemp = currentTargetTemp;
        Serial.println("AC GRADUAL MODE: Started - will increase temperature gradually over 15 minutes");
        Serial.println("Original Temperature: " + String(acOriginalTemp) + "°C -> Room Temperature: " + String(AC_ROOM_TEMPERATURE) + "°C");
        Serial.println("Purpose: Prevent AC from working too hard when restarting");
      }
      
      // Hitung progress gradual (0.0 - 1.0)
      unsigned long elapsedTime = millis() - acGradualStartTime;
      float progress = min(1.0f, (float)elapsedTime / (float)AC_GRADUAL_DURATION);
      
      // Hitung suhu saat ini berdasarkan progress (linear interpolation)
      int gradualTemp = acOriginalTemp + (int)((AC_ROOM_TEMPERATURE - acOriginalTemp) * progress);
      currentTargetTemp = gradualTemp;
      
      // Set temperature ke AC
      sendACTemperatureCommand(currentTargetTemp);
      
      Serial.println("AC GRADUAL MODE: Progress " + String(progress * 100, 1) + "% - Current Temp: " + String(currentTargetTemp) + "°C");
      Serial.println("Time elapsed: " + String(elapsedTime / 60000) + " minutes / 15 minutes");
      Serial.println("Remaining time: " + String((AC_GRADUAL_DURATION - elapsedTime) / 60000) + " minutes");
      
      // Setelah 15 menit, matikan AC
      if (progress >= 1.0f) {
        Serial.println("AC GRADUAL MODE: 15 minutes completed - turning off AC to save energy");
        Serial.println("Final temperature reached: " + String(AC_ROOM_TEMPERATURE) + "°C (room temperature)");
        
        if (devices.ac1) {
          devices.ac1 = false;
          sendIRCommand("ac1", "off");
          Serial.println("AC1 OFF - Gradual mode completed, energy saved");
        }
        
        if (devices.ac2) {
          devices.ac2 = false;
          sendIRCommand("ac2", "off");
          Serial.println("AC2 OFF - Gradual mode completed, energy saved");
        }
        
        updateRelayStates();
        acGradualMode = false;
        Serial.println("AC units turned off gently after gradual temperature increase");
      }
    } else {
      // AC sudah mati, reset gradual mode
      if (acGradualMode) {
        acGradualMode = false;
        Serial.println("AC GRADUAL MODE: Reset - AC already off");
      }
    }
    
    Serial.println("========================================");
    return;
  }
  
  
  // Cari aturan yang cocok berdasarkan jumlah orang DAN suhu luar
  for (int i = 0; i < NUM_AC_RULES; i++) {
    if (peopleCount >= acRules[i].minPeople && peopleCount <= acRules[i].maxPeople &&
        outdoorTemp >= acRules[i].minOutdoorTemp && outdoorTemp <= acRules[i].maxOutdoorTemp) {
      matchedRule = &acRules[i];
      break;
    }
  }
  
  // Jika tidak ada aturan yang cocok, coba cari berdasarkan jumlah orang saja (fallback)
  if (matchedRule == nullptr) {
    if (peopleCount >= 1 && peopleCount <= 4) {
      // 1-4 orang - AC mati
      Serial.println("TABEL 9: 1-4 orang -> AC MATI (di bawah threshold)");
      if (devices.ac1) {
        devices.ac1 = false;
        sendIRCommand("ac1", "off");
        Serial.println("AC1 OFF - Below threshold (1-4 people)");
      }
      if (devices.ac2) {
        devices.ac2 = false;
        sendIRCommand("ac2", "off");
        Serial.println("AC2 OFF - Below threshold (1-4 people)");
      }
      updateRelayStates();
      Serial.println("========================================");
      return;
    } else if (peopleCount >= 21) {
      // 21+ orang - 2 AC dengan suhu agresif (di atas tabel)
      currentTargetTemp = 23;
      devices.ac1 = true;
      devices.ac2 = true;
      Serial.println("TABEL 9: 21+ orang (di atas tabel) -> 2 AC, Target 23°C");
    } else {
      // Fallback untuk kondisi lain
      Serial.println("TABEL 9: Tidak ada aturan yang cocok -> AC mati");
      if (devices.ac1) {
        devices.ac1 = false;
        sendIRCommand("ac1", "off");
      }
      if (devices.ac2) {
        devices.ac2 = false;
        sendIRCommand("ac2", "off");
      }
      updateRelayStates();
      Serial.println("========================================");
      return;
    }
  } else {
    // Reset gradual mode jika ada orang terdeteksi
    if (acGradualMode) {
      acGradualMode = false;
      Serial.println("AC GRADUAL MODE: Reset - people detected, returning to normal operation");
    }
    
    Serial.println("TABEL 9 MATCH: " + matchedRule->description);
    Serial.println("People: " + String(peopleCount) + ", Outdoor Temp: " + String(outdoorTemp, 1) + "°C");
    Serial.println("AC Units: " + String(matchedRule->acUnitsNeeded) + ", Target Temp: " + String(matchedRule->targetTemp) + "°C");
    
    // Set target temperature dan AC units sesuai aturan
    currentTargetTemp = matchedRule->targetTemp;
    
    // Kontrol AC units
    bool needAC1 = matchedRule->acUnitsNeeded >= 1;
    bool needAC2 = matchedRule->acUnitsNeeded >= 2;
    
    if (needAC1 != devices.ac1) {
      devices.ac1 = needAC1;
      sendIRCommand("ac1", needAC1 ? "on" : "off");
      Serial.println("AC1 " + String(needAC1 ? "ON" : "OFF") + " - Tabel 9 rule");
    }
    
    if (needAC2 != devices.ac2) {
      devices.ac2 = needAC2;
      sendIRCommand("ac2", needAC2 ? "on" : "off");
      Serial.println("AC2 " + String(needAC2 ? "ON" : "OFF") + " - Tabel 9 rule");
    }
  }
  
  // Override jika dalam mode energy saving
  if (energySavingMode && currentEnergyState == ENERGY_SAVING) {
    currentTargetTemp = ENERGY_SAVING_TEMP;
    Serial.println("ENERGY SAVING MODE: Temperature overridden to " + String(ENERGY_SAVING_TEMP) + "°C");
  }
  
  // Batasi range suhu
  currentTargetTemp = constrain(currentTargetTemp, 18, 30);
  
  // Update relay states
  updateRelayStates();
  
  // Set temperature jika ada AC yang menyala
  if (devices.ac1 || devices.ac2) {
    sendACTemperatureCommand(currentTargetTemp);
    Serial.println("Temperature set to " + String(currentTargetTemp) + "°C via IR");
  }
  
  Serial.println("Final AC Status: AC1=" + String(devices.ac1 ? "ON" : "OFF") + 
                 ", AC2=" + String(devices.ac2 ? "ON" : "OFF"));
  Serial.println("========================");
}

// ===== ENERGY SAVING SYSTEM =====
void checkEnergySavingSystem() {
  if (!panelAutoShutdownEnabled || !devices.panel) {
    return;
  }
  
  unsigned long timeSinceLastPerson = millis() - lastPersonDetected;
  
  // Cek state berdasarkan waktu tanpa orang
  if (proximityPeopleCount == 0) {
    if (timeSinceLastPerson >= PANEL_SHUTDOWN_DELAY) {
      // TAHAP 2: 30 menit tanpa orang - Matikan panel listrik
      if (currentEnergyState != PANEL_SHUTDOWN) {
        currentEnergyState = PANEL_SHUTDOWN;
        executeFullShutdown();
      }
    } else if (timeSinceLastPerson >= ENERGY_SAVING_DELAY) {
      // TAHAP 1: 15 menit tanpa orang - Mode hemat energi
      if (currentEnergyState != ENERGY_SAVING) {
        currentEnergyState = ENERGY_SAVING;
        executeEnergySaving();
      }
    }
  }
  
  // Log status setiap 30 detik jika tidak ada orang
  static unsigned long lastStatusLog = 0;
  if (proximityPeopleCount == 0 && millis() - lastStatusLog > 30000) {
    int minutesWithoutPeople = timeSinceLastPerson / (60 * 1000);
    Serial.println("=== ENERGY SAVING STATUS ===");
    Serial.println("Minutes without people: " + String(minutesWithoutPeople));
    Serial.println("Current state: " + getEnergyStateString());
    Serial.println("============================");
    lastStatusLog = millis();
  }
}

void executeEnergySaving() {
  Serial.println("=== ENERGY SAVING MODE ACTIVATED ===");
  Serial.println("15 minutes without people detected");
  Serial.println("Raising AC temperature to " + String(ENERGY_SAVING_TEMP) + "°C for energy saving");
  
  energySavingMode = true;
  
  // Naikkan suhu AC ke mode hemat energi jika AC menyala
  if (devices.ac1 || devices.ac2) {
    currentTargetTemp = ENERGY_SAVING_TEMP;
    sendACTemperatureCommand(currentTargetTemp);
    Serial.println("AC temperature raised to " + String(ENERGY_SAVING_TEMP) + "°C via IR");
  }
  
  // Matikan lampu untuk hemat energi
  if (devices.lamp1 || devices.lamp2) {
    if (devices.lamp1) {
      devices.lamp1 = false;
      Serial.println("Lamp Circuit 1 OFF - Energy saving");
    }
    if (devices.lamp2) {
      devices.lamp2 = false;
      Serial.println("Lamp Circuit 2 OFF - Energy saving");
    }
    updateRelayStates();
  }
  
  Serial.println("Energy saving measures applied");
  Serial.println("=====================================");
}

void executeFullShutdown() {
  Serial.println("=== FULL PANEL SHUTDOWN ===");
  Serial.println("30 minutes without people detected");
  Serial.println("Shutting down all electrical systems");
  
  // Matikan semua perangkat
  if (devices.ac1) {
    devices.ac1 = false;
    sendIRCommand("ac1", "off");
    Serial.println("AC1 OFF - Full shutdown");
  }
  
  if (devices.ac2) {
    devices.ac2 = false;
    sendIRCommand("ac2", "off");
    Serial.println("AC2 OFF - Full shutdown");
  }
  
  if (devices.lamp1) {
    devices.lamp1 = false;
    Serial.println("Lamp Circuit 1 OFF - Full shutdown");
  }
  
  if (devices.lamp2) {
    devices.lamp2 = false;
    Serial.println("Lamp Circuit 2 OFF - Full shutdown");
  }
  
  // Matikan perangkat lainnya (kecuali yang always-on)
  devices.computer = false;
  devices.tv = false;
  devices.dispenser = false;
  devices.kettle = false;
  devices.coffee = false;
  
  // Update relay states
  updateRelayStates();
  
  // Delay sebelum matikan panel utama
  delay(2000);
  
  // Matikan panel listrik
  devices.panel = false;
  updateRelayStates();
  
  Serial.println("PANEL LISTRIK DIMATIKAN - Full energy saving shutdown");
  Serial.println("System will remain off until manual restart");
  Serial.println("===============================");
}

String getEnergyStateString() {
  switch (currentEnergyState) {
    case NORMAL_OPERATION:
      return "Normal Operation";
    case ENERGY_SAVING:
      return "Energy Saving Mode (AC 28°C)";
    case PANEL_SHUTDOWN:
      return "Panel Shutdown";
    default:
      return "Unknown";
  }
}

void setupRoutes() {
  // CORS
  server.on("/api/sensors", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    server.send(200);
  });
  
  server.on("/api/control", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    server.send(200);
  });
  
  server.on("/api/ir", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    server.send(200);
  });
  
  server.on("/api/login", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200);
  });
  
  server.on("/api/ac-auto", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    server.send(200);
  });
  
  server.on("/api/energy-saving", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    server.send(200);
  });
  
  server.on("/api/working-hours", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    server.send(200);
  });
  
  server.on("/api/status", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    server.send(200);
  });
  
  // Routes
  server.on("/api/login", HTTP_POST, handleLogin);
  server.on("/api/logout", HTTP_POST, handleLogout);
  server.on("/api/sensors", HTTP_GET, handleGetSensors);
  server.on("/api/control", HTTP_POST, handleDeviceControl);
  server.on("/api/ir", HTTP_POST, handleIRControl);
  server.on("/api/ac-auto", HTTP_POST, handleACAutomation);
  server.on("/api/energy-saving", HTTP_POST, handleEnergySaving);
  server.on("/api/working-hours", HTTP_POST, handleWorkingHours);
  server.on("/api/status", HTTP_GET, handleSystemStatus);
  server.on("/login", HTTP_GET, handleLoginPage);
  server.on("/", HTTP_GET, handleWebDashboard);
  
  // Handle 404
  server.onNotFound([]() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(404, "text/html", "<h1>404 - Page Not Found</h1><p><a href='/login'>Go to Login</a></p>");
  });
}

void handleLogin() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    
    String username = doc["username"];
    String password = doc["password"];
    
    if (username == login_username && password == login_password) {
      sessionToken = generateSessionToken();
      sessionExpiry = millis() + SESSION_DURATION;
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["message"] = "Login successful";
      response["token"] = sessionToken;
      response["expires_in"] = SESSION_DURATION / 1000; // seconds
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
      Serial.println("User logged in successfully");
    } else {
      server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Invalid credentials\"}");
      Serial.println("Failed login attempt");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No login data received\"}");
  }
}

void handleLogout() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  sessionToken = "";
  sessionExpiry = 0;
  
  server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Logged out successfully\"}");
  Serial.println("User logged out");
}

void handleLoginPage() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>Smart Energy Login</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>";
  html += "body{font-family:Arial;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);margin:0;padding:0;height:100vh;display:flex;align-items:center;justify-content:center;}";
  html += ".login-container{background:rgba(255,255,255,0.95);padding:40px;border-radius:15px;box-shadow:0 15px 35px rgba(0,0,0,0.1);width:100%;max-width:400px;}";
  html += ".login-header{text-align:center;margin-bottom:30px;}";
  html += ".login-header h1{color:#333;margin:0;font-size:28px;}";
  html += ".login-header p{color:#666;margin:10px 0 0 0;}";
  html += ".form-group{margin-bottom:20px;}";
  html += ".form-group label{display:block;margin-bottom:8px;color:#333;font-weight:bold;}";
  html += ".form-group input{width:100%;padding:12px;border:2px solid #ddd;border-radius:8px;font-size:16px;box-sizing:border-box;}";
  html += ".form-group input:focus{outline:none;border-color:#667eea;}";
  html += ".login-btn{width:100%;padding:12px;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white;border:none;border-radius:8px;font-size:16px;font-weight:bold;cursor:pointer;transition:transform 0.2s;}";
  html += ".login-btn:hover{transform:translateY(-2px);}";
  html += ".login-btn:disabled{opacity:0.6;cursor:not-allowed;transform:none;}";
  html += ".error-message{color:#e74c3c;text-align:center;margin-top:15px;display:none;}";
  html += ".success-message{color:#27ae60;text-align:center;margin-top:15px;display:none;}";
  html += "</style></head><body>";
  
  html += "<div class='login-container'>";
  html += "<div class='login-header'>";
  html += "<h1>🏠 Smart Energy</h1>";
  html += "<p>Lab Teknik Tegangan Tinggi</p>";
  html += "</div>";
  
  html += "<form id='loginForm'>";
  html += "<div class='form-group'>";
  html += "<label for='username'>Username:</label>";
  html += "<input type='text' id='username' name='username' required>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='password'>Password:</label>";
  html += "<input type='password' id='password' name='password' required>";
  html += "</div>";
  
  html += "<button type='submit' class='login-btn' id='loginBtn'>Login</button>";
  html += "<div class='error-message' id='errorMessage'></div>";
  html += "<div class='success-message' id='successMessage'></div>";
  html += "</form>";
  html += "</div>";
  
  html += "<script>";
  html += "document.getElementById('loginForm').addEventListener('submit',async function(e){";
  html += "e.preventDefault();";
  html += "const username=document.getElementById('username').value;";
  html += "const password=document.getElementById('password').value;";
  html += "const loginBtn=document.getElementById('loginBtn');";
  html += "const errorMsg=document.getElementById('errorMessage');";
  html += "const successMsg=document.getElementById('successMessage');";
  
  html += "loginBtn.disabled=true;loginBtn.textContent='Logging in...';";
  html += "errorMsg.style.display='none';successMsg.style.display='none';";
  
  html += "try{";
  html += "const response=await fetch('/api/login',{";
  html += "method:'POST',headers:{'Content-Type':'application/json'},";
  html += "body:JSON.stringify({username:username,password:password})});";
  
  html += "const result=await response.json();";
  html += "if(response.ok){";
  html += "localStorage.setItem('authToken',result.token);";
  html += "successMsg.textContent='Login successful! Redirecting...';";
  html += "successMsg.style.display='block';";
  html += "setTimeout(()=>{window.location.href='/';},1500);";
  html += "}else{";
  html += "errorMsg.textContent=result.message||'Login failed';";
  html += "errorMsg.style.display='block';";
  html += "}}catch(error){";
  html += "errorMsg.textContent='Connection error. Please try again.';";
  html += "errorMsg.style.display='block';}";
  
  html += "loginBtn.disabled=false;loginBtn.textContent='Login';";
  html += "});";
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
}

void handleGetSensors() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (!isAuthenticated()) {
    server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Authentication required\"}");
    return;
  }
  
  DynamicJsonDocument doc(1024);
  
  doc["temperature"] = (sensors.temperature1 + sensors.temperature2) / 2.0;
  doc["humidity"] = (sensors.humidity1 + sensors.humidity2) / 2.0;
  doc["light_intensity"] = sensors.light_intensity;
  doc["people_count"] = sensors.people_count;
  
  JsonObject devicesObj = doc.createNestedObject("devices");
  devicesObj["ac1"] = devices.ac1;
  devicesObj["ac2"] = devices.ac2;
  devicesObj["lamp1"] = devices.lamp1;
  devicesObj["lamp2"] = devices.lamp2;
  devicesObj["panel"] = devices.panel;
  
  // AC Automation status
  JsonObject acAutoObj = doc.createNestedObject("ac_automation");
  acAutoObj["enabled"] = autoACEnabled;
  acAutoObj["target_temp"] = currentTargetTemp;
  acAutoObj["base_temp"] = baseTargetTemp;
  acAutoObj["temp_adjustment"] = temperatureAdjustment;
  acAutoObj["proximity_count"] = proximityPeopleCount;
  acAutoObj["outdoor_temp"] = (sensors.temperature1 + sensors.temperature2) / 2.0;
  acAutoObj["last_check"] = (millis() - lastACCheck) / 1000; // seconds ago
  
  // AC Gradual Mode status
  JsonObject acGradualObj = acAutoObj.createNestedObject("gradual_mode");
  acGradualObj["active"] = acGradualMode;
  if (acGradualMode) {
    unsigned long elapsedTime = millis() - acGradualStartTime;
    float progress = min(1.0f, (float)elapsedTime / (float)AC_GRADUAL_DURATION);
    acGradualObj["progress_percent"] = progress * 100;
    acGradualObj["elapsed_minutes"] = elapsedTime / 60000;
    acGradualObj["remaining_minutes"] = max(0, (int)((AC_GRADUAL_DURATION - elapsedTime) / 60000));
    acGradualObj["original_temp"] = acOriginalTemp;
    acGradualObj["target_room_temp"] = AC_ROOM_TEMPERATURE;
  }
  
  // Energy Saving System status
  JsonObject energySavingObj = doc.createNestedObject("energy_saving");
  energySavingObj["enabled"] = panelAutoShutdownEnabled;
  energySavingObj["current_state"] = getEnergyStateString();
  energySavingObj["energy_saving_mode"] = energySavingMode;
  energySavingObj["minutes_without_people"] = (millis() - lastPersonDetected) / (60 * 1000);
  energySavingObj["time_to_energy_saving"] = max(0, (int)((ENERGY_SAVING_DELAY - (millis() - lastPersonDetected)) / (60 * 1000)));
  energySavingObj["time_to_shutdown"] = max(0, (int)((PANEL_SHUTDOWN_DELAY - (millis() - lastPersonDetected)) / (60 * 1000)));
  energySavingObj["energy_saving_temp"] = ENERGY_SAVING_TEMP;
  
  // Working hours status
  JsonObject workingHoursObj = doc.createNestedObject("working_hours");
  workingHoursObj["enabled"] = workingDaysEnabled;
  workingHoursObj["start_hour"] = workingStartHour;
  workingHoursObj["end_hour"] = workingEndHour;
  workingHoursObj["is_working_hours"] = isWorkingHours();
  workingHoursObj["is_working_day"] = isWorkingDay();
  workingHoursObj["status"] = getWorkingHoursStatus();
  workingHoursObj["scheduled_shutdown_enabled"] = scheduledShutdownEnabled;
  
  // Current time
  JsonObject timeObj = doc.createNestedObject("current_time");
  timeObj["hour"] = currentTime.hour;
  timeObj["minute"] = currentTime.minute;
  timeObj["day_of_week"] = currentTime.dayOfWeek;
  timeObj["day_name"] = getDayName(currentTime.dayOfWeek);
  timeObj["formatted"] = String(currentTime.hour) + ":" + (currentTime.minute < 10 ? "0" : "") + String(currentTime.minute);
  
  // Sensor roles
  JsonObject sensorRoles = doc.createNestedObject("sensor_roles");
  sensorRoles["main_sensor"] = "proximity";
  sensorRoles["support_sensor"] = "dht";
  sensorRoles["main_function"] = "people_counting";
  sensorRoles["support_function"] = "outdoor_temperature_adjustment";
  
  doc["timestamp"] = millis();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleDeviceControl() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (!isAuthenticated()) {
    server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Authentication required\"}");
    return;
  }
  
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
    Serial.println("Device control: " + device + " - " + action);
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

void handleIRControl() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (!isAuthenticated()) {
    server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Authentication required\"}");
    return;
  }
  
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    
    String device = doc["device"];
    String command = doc["command"];
    
    sendIRCommand(device, command);
    
    server.send(200, "application/json", "{\"status\":\"IR command sent\"}");
    Serial.println("IR control: " + device + " - " + command);
  } else {
    server.send(400, "application/json", "{\"error\":\"No IR data received\"}");
  }
}

void handleWebDashboard() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Smart Energy Dashboard</title>";
  html += "<style>";
  html += "body{font-family:Arial,sans-serif;margin:0;padding:0;background:linear-gradient(135deg,#1a1a1a 0%,#2d1b2e 100%);color:white;min-height:100vh;display:flex;}";
  
  // Sidebar styles
  html += ".sidebar{width:250px;background:rgba(255,255,255,0.05);backdrop-filter:blur(25px);border-right:1px solid rgba(255,192,203,0.1);padding:0;height:100vh;overflow-y:auto;box-shadow:0 8px 32px rgba(0,0,0,0.3);position:relative;transition:width 0.3s ease;}";
  html += ".sidebar.collapsed{width:70px;}";
  html += ".sidebar-toggle{position:absolute;top:15px;right:15px;width:40px;height:40px;background:rgba(255,255,255,0.1);border-radius:10px;display:flex;align-items:center;justify-content:center;cursor:pointer;transition:all 0.3s ease;z-index:10;backdrop-filter:blur(10px);border:1px solid rgba(255,255,255,0.2);}";
  html += ".sidebar-toggle:hover{background:rgba(255,255,255,0.2);transform:scale(1.05);}";
  html += ".sidebar-content{padding:20px;padding-top:70px;}";
  html += ".sidebar.collapsed .sidebar-content{padding:20px 10px;padding-top:70px;}";
  html += ".sidebar h2{margin-bottom:30px;text-align:center;font-size:18px;font-weight:800;padding:15px;background:rgba(255,255,255,0.12);border-radius:20px;border:1px solid rgba(255,255,255,0.25);backdrop-filter:blur(15px);letter-spacing:-0.03em;text-shadow:0 2px 4px rgba(0,0,0,0.1);position:relative;z-index:1;white-space:nowrap;overflow:hidden;}";
  html += ".sidebar.collapsed h2{text-align:center;padding:15px 8px;}";
  html += ".sidebar ul{list-style:none;padding:0;margin:0;}";
  html += ".sidebar ul li{margin:8px 0;}";
  html += ".sidebar ul li a{color:rgba(255,255,255,0.75);text-decoration:none;font-weight:600;display:flex;align-items:center;padding:14px 18px;border-radius:14px;transition:all 0.4s ease;font-size:14px;letter-spacing:-0.01em;position:relative;z-index:1;white-space:nowrap;overflow:hidden;cursor:pointer;}";
  html += ".sidebar.collapsed ul li a{padding:14px 10px;justify-content:center;}";
  html += ".sidebar ul li a i{margin-right:16px;width:24px;text-align:center;font-size:18px;transition:transform 0.3s ease;}";
  html += ".sidebar ul li a:hover,.sidebar ul li a.active{color:#fff;background:linear-gradient(135deg,#4299e1 0%,#63b3ed 50%,#90cdf4 100%);box-shadow:0 4px 12px rgba(66,153,225,0.2);backdrop-filter:blur(10px);border:1px solid rgba(255,255,255,0.2);}";
  html += ".sidebar-text{transition:opacity 0.3s ease,transform 0.3s ease;}";
  html += ".sidebar.collapsed .sidebar-text{opacity:0;transform:translateX(-10px);pointer-events:none;}";
  
  // Main content styles
  html += ".main-content{flex:1;transition:margin-left 0.3s ease;}";
  html += ".header{display:flex;justify-content:space-between;align-items:center;padding:20px;background:rgba(255,255,255,0.1);backdrop-filter:blur(10px);}";
  html += ".header h1{margin:0;font-size:28px;color:#4facfe;}";
  html += ".user-info{display:flex;align-items:center;gap:15px;background:rgba(255,255,255,0.1);padding:10px 15px;border-radius:25px;}";
  html += ".user-avatar{width:40px;height:40px;background:linear-gradient(135deg,#667eea,#764ba2);border-radius:50%;display:flex;align-items:center;justify-content:center;font-weight:bold;font-size:16px;}";
  html += ".user-details{display:flex;flex-direction:column;}";
  html += ".user-name{font-weight:bold;font-size:14px;}";
  html += ".user-role{font-size:12px;color:#ccc;}";
  html += ".logout-btn{background:#dc3545;color:white;border:none;padding:8px 15px;border-radius:20px;cursor:pointer;font-size:12px;transition:background 0.3s;}";
  html += ".logout-btn:hover{background:#c82333;}";
  html += ".container{padding:20px;}";
  html += ".cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));gap:20px;margin-bottom:30px;}";
  html += ".card{background:rgba(255,255,255,0.1);padding:25px;border-radius:15px;text-align:center;backdrop-filter:blur(10px);border:1px solid rgba(255,255,255,0.2);}";
  html += ".card h3{margin:0 0 15px 0;color:#4facfe;font-size:18px;}";
  html += ".card .value{font-size:2.5em;font-weight:bold;color:#fff;margin:10px 0;}";
  html += ".controls{margin-top:30px;}";
  html += ".controls h2{color:#4facfe;margin-bottom:20px;}";
  html += ".device-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:15px;margin-bottom:30px;}";
  html += ".device{background:rgba(255,255,255,0.1);padding:20px;border-radius:12px;text-align:center;border:1px solid rgba(255,255,255,0.2);}";
  html += ".device h4{margin:0 0 10px 0;color:#fff;}";
  html += ".device-status{font-weight:bold;margin:10px 0;font-size:14px;}";
  html += ".device-status.on{color:#28a745;}";
  html += ".device-status.off{color:#dc3545;}";
  html += ".btn{background:linear-gradient(135deg,#4facfe,#00f2fe);color:white;border:none;padding:10px 20px;border-radius:8px;cursor:pointer;font-weight:bold;transition:transform 0.2s;}";
  html += ".btn:hover{transform:translateY(-2px);}";
  html += ".btn:disabled{opacity:0.6;cursor:not-allowed;transform:none;}";
  html += ".ir-controls{margin-top:20px;}";
  html += ".ir-btn{background:linear-gradient(135deg,#ff6b6b,#ee5a24);margin:5px;}";
  html += ".status-indicator{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:8px;}";
  html += ".status-indicator.online{background:#28a745;box-shadow:0 0 10px #28a745;}";
  html += ".status-indicator.offline{background:#dc3545;}";
  html += ".section{display:none;}";
  html += ".section.active{display:block;}";
  html += ".automation-card{background:rgba(255,255,255,0.1);padding:25px;border-radius:15px;margin-bottom:20px;backdrop-filter:blur(10px);border:1px solid rgba(255,255,255,0.2);}";
  html += ".automation-card h3{color:#4facfe;margin-bottom:15px;}";
  html += ".automation-status{display:grid;grid-template-columns:1fr 1fr;gap:15px;margin:15px 0;}";
  html += ".status-item{display:flex;justify-content:space-between;padding:10px;background:rgba(255,255,255,0.05);border-radius:8px;}";
  html += ".automation-controls{display:flex;gap:10px;margin:15px 0;}";
  html += ".temp-control{margin-top:20px;}";
  html += ".temp-input-group{display:flex;gap:10px;align-items:center;}";
  html += ".temp-input{padding:8px 12px;border:1px solid rgba(255,255,255,0.3);border-radius:6px;background:rgba(255,255,255,0.1);color:white;width:80px;}";
  html += ".sensor-info{display:grid;grid-template-columns:1fr 1fr;gap:20px;margin:20px 0;}";
  html += ".sensor-main,.sensor-support{background:rgba(255,255,255,0.05);padding:15px;border-radius:10px;}";
  html += ".sensor-main h4{color:#4facfe;margin-bottom:10px;}";
  html += ".sensor-support h4{color:#ffc107;margin-bottom:10px;}";
  html += ".sensor-data{display:flex;justify-content:space-between;margin:8px 0;}";
  html += ".sensor-value{font-weight:bold;color:#fff;}";
  html += ".final-temp{font-size:1.2em;color:#28a745;font-weight:bold;}";
  html += ".energy-saving-status{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin:15px 0;}";
  html += ".energy-controls{display:flex;gap:10px;margin:15px 0;flex-wrap:wrap;}";
  html += ".btn-warning{background:linear-gradient(135deg,#ffc107,#ffca2c)!important;color:#000!important;}";
  html += ".btn-danger{background:linear-gradient(135deg,#dc3545,#e55353)!important;}";
  html += ".rules-table{overflow-x:auto;}";
  html += ".rules-table table{width:100%;border-collapse:collapse;margin-top:10px;}";
  html += ".rules-table th,.rules-table td{padding:8px 12px;border:1px solid rgba(255,255,255,0.2);text-align:center;}";
  html += ".rules-table th{background:rgba(255,255,255,0.1);font-weight:bold;}";
  html += "@media (max-width:768px){body{flex-direction:column;}.sidebar{width:100%;height:auto;}.main-content{margin-left:0;}.header{flex-direction:column;gap:15px;}.user-info{order:-1;}.automation-status{grid-template-columns:1fr;}.automation-controls{flex-direction:column;}.temp-input-group{flex-direction:column;align-items:stretch;}}";
  html += "</style>";
  html += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css'>";
  html += "</head><body>";
  
  // Sidebar
  html += "<div class='sidebar' id='sidebar'>";
  html += "<div class='sidebar-toggle' onclick='toggleSidebar()'>";
  html += "<i class='fas fa-bars' id='toggleIcon'></i>";
  html += "</div>";
  html += "<div class='sidebar-content'>";
  html += "<h2><i class='fas fa-home'></i> <span class='sidebar-text'>Smart Energy</span></h2>";
  html += "<ul>";
  html += "<li><a href='#' onclick='showSection(\"monitoring\")' class='active'><i class='fas fa-desktop'></i> <span class='sidebar-text'>Monitoring</span></a></li>";
  html += "<li><a href='#' onclick='showSection(\"analytics\")'><i class='fas fa-chart-line'></i> <span class='sidebar-text'>Analytics</span></a></li>";
  html += "<li><a href='#' onclick='showSection(\"devices\")'><i class='fas fa-microchip'></i> <span class='sidebar-text'>Devices</span></a></li>";
  html += "<li><a href='#' onclick='showSection(\"automation\")'><i class='fas fa-robot'></i> <span class='sidebar-text'>Automation</span></a></li>";
  html += "<li><a href='#' onclick='showSection(\"settings\")'><i class='fas fa-cog'></i> <span class='sidebar-text'>Settings</span></a></li>";
  html += "</ul>";
  html += "</div>";
  html += "</div>";
  
  // Main content
  html += "<div class='main-content'>";
  html += "<div class='header'>";
  html += "<h1>🏠 Smart Energy Dashboard</h1>";
  html += "<div class='user-info'>";
  html += "<div class='user-avatar'>A</div>";
  html += "<div class='user-details'>";
  html += "<div class='user-name'>Administrator</div>";
  html += "<div class='user-role'>Lab Teknik Tegangan Tinggi</div>";
  html += "</div>";
  html += "<button class='logout-btn' onclick='logout()'>Logout</button>";
  html += "</div>";
  html += "</div>";
  
  // Monitoring Section
  html += "<section id='monitoring' class='section active'>";
  html += "<div class='container'>";
  html += "<div class='cards'>";
  html += "<div class='card'>";
  html += "<h3>🌡️ Temperature</h3>";
  html += "<div class='value' id='temp'>--°C</div>";
  html += "<div><span class='status-indicator online'></span>Sensor Active</div>";
  html += "</div>";
  
  html += "<div class='card'>";
  html += "<h3>💧 Humidity</h3>";
  html += "<div class='value' id='hum'>--%</div>";
  html += "<div><span class='status-indicator online'></span>Sensor Active</div>";
  html += "</div>";
  
  html += "<div class='card'>";
  html += "<h3>💡 Light Intensity</h3>";
  html += "<div class='value' id='light'>-- lux</div>";
  html += "<div><span class='status-indicator online'></span>LDR Active</div>";
  html += "</div>";
  
  html += "<div class='card'>";
  html += "<h3>👥 People Count</h3>";
  html += "<div class='value' id='people'>--</div>";
  html += "<div><span class='status-indicator online'></span>Proximity Active</div>";
  html += "</div>";
  html += "</div>";
  html += "</div>";
  html += "</section>";
  
  // Devices Section
  html += "<section id='devices' class='section'>";
  html += "<div class='container'>";
  html += "<div class='controls'>";
  html += "<h2>Device Controls</h2>";
  html += "<div class='device-grid'>";
  
  html += "<div class='device'>";
  html += "<h4>AC Unit 1</h4>";
  html += "<div class='device-status off' id='status-ac1'>OFF</div>";
  html += "<button class='btn' onclick='toggle(\"ac1\")'>Toggle</button>";
  html += "</div>";
  
  html += "<div class='device'>";
  html += "<h4>AC Unit 2</h4>";
  html += "<div class='device-status off' id='status-ac2'>OFF</div>";
  html += "<button class='btn' onclick='toggle(\"ac2\")'>Toggle</button>";
  html += "</div>";
  
  html += "<div class='device'>";
  html += "<h4>Lamp Circuit 1</h4>";
  html += "<div class='device-status off' id='status-lamp1'>OFF</div>";
  html += "<button class='btn' onclick='toggle(\"lamp1\")'>Toggle</button>";
  html += "</div>";
  
  html += "<div class='device'>";
  html += "<h4>Lamp Circuit 2</h4>";
  html += "<div class='device-status off' id='status-lamp2'>OFF</div>";
  html += "<button class='btn' onclick='toggle(\"lamp2\")'>Toggle</button>";
  html += "</div>";
  
  html += "<div class='device'>";
  html += "<h4>Main Panel</h4>";
  html += "<div class='device-status on' id='status-panel'>ON</div>";
  html += "<button class='btn' onclick='toggle(\"panel\")'>Toggle</button>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class='ir-controls'>";
  html += "<h2>IR Remote Controls</h2>";
  html += "<div class='device-grid'>";
  html += "<div class='device'>";
  html += "<h4>AC IR Control</h4>";
  html += "<button class='btn ir-btn' onclick='sendIR(\"ac1\",\"on\")'>AC ON</button>";
  html += "<button class='btn ir-btn' onclick='sendIR(\"ac1\",\"off\")'>AC OFF</button>";
  html += "</div>";
  html += "<div class='device'>";
  html += "<h4>TV IR Control</h4>";
  html += "<button class='btn ir-btn' onclick='sendIR(\"tv\",\"on\")'>TV ON</button>";
  html += "<button class='btn ir-btn' onclick='sendIR(\"tv\",\"off\")'>TV OFF</button>";
  html += "</div>";
  html += "</div>";
  html += "</div>";
  html += "</div>";
  html += "</section>";
  
  // Analytics Section
  html += "<section id='analytics' class='section'>";
  html += "<div class='container'>";
  html += "<h2>Analytics</h2>";
  html += "<p>Energy consumption analytics will be displayed here.</p>";
  html += "</div>";
  html += "</section>";
  
  // Automation Section
  html += "<section id='automation' class='section'>";
  html += "<div class='container'>";
  html += "<h2>AC Automation System</h2>";
  
  html += "<div class='automation-card'>";
  html += "<h3>🤖 Automatic AC Control</h3>";
  html += "<p><strong>Sensor Utama:</strong> Proximity (deteksi jumlah orang)<br>";
  html += "<strong>Sensor Pendamping:</strong> DHT (penyesuaian suhu udara luar)</p>";
  
  html += "<div class='sensor-info'>";
  html += "<div class='sensor-main'>";
  html += "<h4>📍 Proximity Sensor (Utama)</h4>";
  html += "<div class='sensor-data'>";
  html += "<span>Jumlah Orang:</span>";
  html += "<span id='proximityCount' class='sensor-value'>--</span>";
  html += "</div>";
  html += "</div>";
  html += "<div class='sensor-support'>";
  html += "<h4>🌡️ DHT Sensor (Pendamping)</h4>";
  html += "<div class='sensor-data'>";
  html += "<span>Suhu Luar:</span>";
  html += "<span id='outdoorTemp' class='sensor-value'>--°C</span>";
  html += "</div>";
  html += "<div class='sensor-data'>";
  html += "<span>Penyesuaian:</span>";
  html += "<span id='tempAdjustment' class='sensor-value'>--°C</span>";
  html += "</div>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class='automation-status'>";
  html += "<div class='status-item'>";
  html += "<span>Status Otomatis:</span>";
  html += "<span id='autoStatus' class='status-indicator'>Loading...</span>";
  html += "</div>";
  html += "<div class='status-item'>";
  html += "<span>Suhu Base:</span>";
  html += "<span id='baseTemp'>--°C</span>";
  html += "</div>";
  html += "<div class='status-item'>";
  html += "<span>Suhu Final:</span>";
  html += "<span id='targetTemp' class='final-temp'>--°C</span>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class='automation-controls'>";
  html += "<button class='btn' onclick='toggleACAutomation()' id='autoToggleBtn'>Toggle Auto AC</button>";
  html += "<button class='btn' onclick='forceACCheck()'>Force Check</button>";
  html += "</div>";
  
  html += "<div class='temp-control'>";
  html += "<h4>Manual Temperature Control</h4>";
  html += "<div class='temp-input-group'>";
  html += "<input type='number' id='manualTemp' min='16' max='30' value='25' class='temp-input'>";
  html += "<button class='btn' onclick='setManualTemp()'>Set Temperature</button>";
  html += "</div>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class='automation-card'>";
  html += "<h3>⚡ Energy Saving System</h3>";
  html += "<p><strong>Tahap 1 (15 menit tanpa orang):</strong> AC dinaikkan ke 28°C, lampu dimatikan<br>";
  html += "<strong>Tahap 2 (30 menit tanpa orang):</strong> Panel listrik dimatikan sepenuhnya</p>";
  
  html += "<div class='energy-saving-status'>";
  html += "<div class='status-item'>";
  html += "<span>Status Sistem:</span>";
  html += "<span id='energySavingStatus'>Loading...</span>";
  html += "</div>";
  html += "<div class='status-item'>";
  html += "<span>State Saat Ini:</span>";
  html += "<span id='currentEnergyState'>Loading...</span>";
  html += "</div>";
  html += "<div class='status-item'>";
  html += "<span>Menit Tanpa Orang:</span>";
  html += "<span id='minutesWithoutPeople'>--</span>";
  html += "</div>";
  html += "<div class='status-item'>";
  html += "<span>Waktu ke Hemat Energi:</span>";
  html += "<span id='timeToEnergySaving'>--</span>";
  html += "</div>";
  html += "<div class='status-item'>";
  html += "<span>Waktu ke Shutdown:</span>";
  html += "<span id='timeToShutdown'>--</span>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class='energy-controls'>";
  html += "<button class='btn' onclick='toggleEnergySaving()' id='energySavingToggleBtn'>Toggle Energy Saving</button>";
  html += "<button class='btn btn-warning' onclick='resetEnergyTimer()'>Reset Timer</button>";
  html += "<button class='btn btn-danger' onclick='emergencyShutdown()'>Emergency Shutdown</button>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class='automation-card'>";
  html += "<h3>📊 Proximity-Based AC Rules</h3>";
  html += "<div class='rules-table'>";
  html += "<table>";
  html += "<tr><th>Jumlah Orang</th><th>Suhu Luar Ruangan (°C)</th><th>AC Aktif</th><th>Target Suhu Dalam (°C)</th><th>Keterangan</th></tr>";
  html += "<tr><td>0</td><td>--</td><td>Mode Gradual</td><td>28°C (15 menit)</td><td>Tidak ada orang → naikkan suhu bertahap ke suhu ruang, lalu mati</td></tr>";
  html += "<tr><td>5-10</td><td>30-32</td><td>1 AC</td><td>25-26</td><td>Beban ringan → hanya satu AC</td></tr>";
  html += "<tr><td>5-10</td><td>32-35</td><td>1 AC</td><td>26-27</td><td>Suhu luar tinggi → setpoint lebih longgar</td></tr>";
  html += "<tr><td>10-13</td><td>30-32</td><td>2 AC</td><td>24-25</td><td>Saat beban meningkat, aktifkan kedua AC</td></tr>";
  html += "<tr><td>10-20</td><td>32-35</td><td>2 AC</td><td>25-26</td><td>Kondisi luar panas → pendinginan lebih ringan</td></tr>";
  html += "</table>";
  html += "</div>";
  html += "</div>";
  
  html += "</div>";
  html += "</section>";
  
  // Settings Section
  html += "<section id='settings' class='section'>";
  html += "<div class='container'>";
  html += "<h2>Settings</h2>";
  html += "<p>System settings and configuration options.</p>";
  html += "</div>";
  html += "</section>";
  
  html += "</div>"; // Close main-content
  
  html += "<script>";
  html += "const authToken=localStorage.getItem('authToken');";
  html += "if(!authToken){window.location.href='/login';return;}";
  
  html += "const authHeaders={'Content-Type':'application/json','Authorization':'Bearer '+authToken};";
  
  html += "async function toggle(device){";
  html += "try{";
  html += "const response=await fetch('/api/control',{";
  html += "method:'POST',headers:authHeaders,";
  html += "body:JSON.stringify({device:device,action:'toggle'})});";
  html += "if(response.status===401){localStorage.removeItem('authToken');window.location.href='/login';return;}";
  html += "if(response.ok){updateData();}";
  html += "}catch(error){console.error('Control error:',error);}}";
  
  html += "async function sendIR(device,command){";
  html += "try{";
  html += "const response=await fetch('/api/ir',{";
  html += "method:'POST',headers:authHeaders,";
  html += "body:JSON.stringify({device:device,command:command})});";
  html += "if(response.status===401){localStorage.removeItem('authToken');window.location.href='/login';return;}";
  html += "if(response.ok){console.log('IR command sent:',device,command);}";
  html += "}catch(error){console.error('IR error:',error);}}";
  
  html += "async function updateData(){";
  html += "try{";
  html += "const response=await fetch('/api/sensors',{headers:{'Authorization':'Bearer '+authToken}});";
  html += "if(response.status===401){localStorage.removeItem('authToken');window.location.href='/login';return;}";
  html += "if(response.ok){";
  html += "const data=await response.json();";
  html += "document.getElementById('temp').textContent=data.temperature.toFixed(1)+'°C';";
  html += "document.getElementById('hum').textContent=data.humidity.toFixed(1)+'%';";
  html += "document.getElementById('light').textContent=data.light_intensity+' lux';";
  html += "document.getElementById('people').textContent=data.people_count;";
  
  html += "updateDeviceStatus('ac1',data.devices.ac1);";
  html += "updateDeviceStatus('ac2',data.devices.ac2);";
  html += "updateDeviceStatus('lamp1',data.devices.lamp1);";
  html += "updateDeviceStatus('lamp2',data.devices.lamp2);";
  html += "updateDeviceStatus('panel',data.devices.panel);";
  html += "}}catch(error){console.error('Update error:',error);}}";
  
  html += "function updateDeviceStatus(device,status){";
  html += "const statusEl=document.getElementById('status-'+device);";
  html += "if(statusEl){";
  html += "statusEl.textContent=status?'ON':'OFF';";
  html += "statusEl.className='device-status '+(status?'on':'off');";
  html += "}}";
  
  html += "async function logout(){";
  html += "try{";
  html += "await fetch('/api/logout',{method:'POST',headers:authHeaders});";
  html += "}catch(error){console.error('Logout error:',error);}";
  html += "localStorage.removeItem('authToken');";
  html += "window.location.href='/login';}";
  
  // Sidebar functions
  html += "function toggleSidebar(){";
  html += "const sidebar=document.getElementById('sidebar');";
  html += "const toggleIcon=document.getElementById('toggleIcon');";
  html += "sidebar.classList.toggle('collapsed');";
  html += "if(sidebar.classList.contains('collapsed')){";
  html += "toggleIcon.className='fas fa-chevron-right';";
  html += "}else{";
  html += "toggleIcon.className='fas fa-bars';";
  html += "}}";
  
  html += "function showSection(sectionId){";
  html += "document.querySelectorAll('.section').forEach(sec=>sec.classList.remove('active'));";
  html += "document.getElementById(sectionId).classList.add('active');";
  html += "document.querySelectorAll('.sidebar ul li a').forEach(link=>link.classList.remove('active'));";
  html += "event.target.classList.add('active');}";
  
  html += "setInterval(updateData,3000);";
  html += "updateData();";
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
}

void readSensors() {
  sensors.temperature1 = dht1.readTemperature();
  sensors.humidity1 = dht1.readHumidity();
  sensors.temperature2 = dht2.readTemperature();
  sensors.humidity2 = dht2.readHumidity();
  
  if (isnan(sensors.temperature1)) sensors.temperature1 = 27.0;
  if (isnan(sensors.humidity1)) sensors.humidity1 = 65.0;
  if (isnan(sensors.temperature2)) sensors.temperature2 = 27.0;
  if (isnan(sensors.humidity2)) sensors.humidity2 = 65.0;
  
  sensors.ldr1_value = analogRead(LDR1_PIN);
  sensors.ldr2_value = analogRead(LDR2_PIN);
  sensors.light_intensity = map((sensors.ldr1_value + sensors.ldr2_value) / 2, 0, 4095, 0, 1000);
  
  // PROXIMITY SENSOR - SENSOR UTAMA untuk deteksi jumlah orang
  sensors.proximity1 = digitalRead(PROXIMITY1_PIN);
  sensors.proximity2 = digitalRead(PROXIMITY2_PIN);
  
  // Advanced people counting dengan proximity sensor
  static int people_counter = 0;
  static unsigned long last_detection = 0;
  static unsigned long entry_time = 0;
  static unsigned long exit_time = 0;
  
  // Deteksi perubahan status proximity
  bool prox1_triggered = sensors.proximity1 && !lastProximity1;
  bool prox2_triggered = sensors.proximity2 && !lastProximity2;
  bool prox1_released = !sensors.proximity1 && lastProximity1;
  bool prox2_released = !sensors.proximity2 && lastProximity2;
  
  // Logic untuk menghitung orang masuk/keluar
  if (prox1_triggered) {
    entry_time = millis();
    Serial.println("Proximity 1 triggered - Person entering");
  }
  
  if (prox2_triggered) {
    exit_time = millis();
    Serial.println("Proximity 2 triggered - Person exiting");
  }
  
  // Deteksi sequence masuk: Prox1 -> Prox2
  if (prox1_triggered && !sensors.proximity2) {
    // Orang mulai masuk
  } else if (sensors.proximity1 && prox2_triggered) {
    // Orang selesai masuk
    people_counter++;
    Serial.println("Person ENTERED - Count: " + String(people_counter));
    lastProximityChange = millis();
  }
  
  // Deteksi sequence keluar: Prox2 -> Prox1
  if (prox2_triggered && !sensors.proximity1) {
    // Orang mulai keluar
  } else if (sensors.proximity2 && prox1_triggered) {
    // Orang selesai keluar
    people_counter = max(0, people_counter - 1);
    Serial.println("Person EXITED - Count: " + String(people_counter));
    lastProximityChange = millis();
  }
  
  // Update waktu deteksi terakhir
  if (sensors.proximity1 || sensors.proximity2) {
    last_detection = millis();
  }
  
  // Reset counter jika tidak ada aktivitas dalam 5 menit (untuk kalibrasi)
  if (millis() - last_detection > 300000) { // 5 menit
    if (people_counter > 0) {
      Serial.println("No activity for 5 minutes - Resetting people count");
      people_counter = 0;
    }
  }
  
  // Batasi jumlah maksimal orang (sesuai kapasitas ruangan)
  proximityPeopleCount = constrain(people_counter, 0, 25);
  sensors.people_count = proximityPeopleCount;
  
  // Update waktu terakhir ada orang untuk sistem penghematan energi
  if (proximityPeopleCount > 0) {
    lastPersonDetected = millis();
    
    // Reset energy saving mode jika ada orang kembali
    if (currentEnergyState != NORMAL_OPERATION) {
      currentEnergyState = NORMAL_OPERATION;
      energySavingMode = false;
      Serial.println("ENERGY SAVING: People detected - returning to normal operation");
    }
    
    // Reset AC gradual mode jika ada orang kembali
    if (acGradualMode) {
      acGradualMode = false;
      Serial.println("AC GRADUAL MODE: Reset - people detected");
    }
  }
  
  // Update status proximity terakhir
  lastProximity1 = sensors.proximity1;
  lastProximity2 = sensors.proximity2;
}

void updateRelayStates() {
  digitalWrite(RELAY1_PIN, !(devices.ac1 && devices.panel));
  digitalWrite(RELAY2_PIN, !(devices.ac2 && devices.panel));
  digitalWrite(RELAY3_PIN, !(devices.lamp1 && devices.panel));
  digitalWrite(RELAY4_PIN, !(devices.lamp2 && devices.panel));
}

void sendIRCommand(String device, String command) {
  Serial.println("IR Command: " + device + " - " + command);
  
  // Enhanced IR command based on device and command
  if (device.startsWith("ac")) {
    if (command == "on") {
      sendACOnCommand();
    } else if (command == "off") {
      sendACOffCommand();
    }
  } else if (device == "tv") {
    sendTVCommand(command);
  } else {
    // Generic IR pulse
    for (int i = 0; i < 10; i++) {
      digitalWrite(IR_TRANSMITTER_PIN, HIGH);
      delayMicroseconds(500);
      digitalWrite(IR_TRANSMITTER_PIN, LOW);
      delayMicroseconds(500);
    }
  }
  
  sensors.ir_code = millis();
}

void sendACOnCommand() {
  // AC ON command with NEC protocol
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(9000); // Header pulse
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delayMicroseconds(4500); // Header space
  
  // Send AC ON code (example for Panasonic)
  uint32_t ac_on_code = 0x02020DF0;
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
  
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(560);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  Serial.println("IR: AC ON command sent");
}

void sendACOffCommand() {
  // AC OFF command
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(9000);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delayMicroseconds(4500);
  
  uint32_t ac_off_code = 0x02020DF1;
  for (int i = 31; i >= 0; i--) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(560);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    
    if (ac_off_code & (1UL << i)) {
      delayMicroseconds(1690);
    } else {
      delayMicroseconds(560);
    }
  }
  
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(560);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  Serial.println("IR: AC OFF command sent");
}

void sendACTemperatureCommand(int temperature) {
  Serial.println("IR: Setting AC temperature to " + String(temperature) + "°C");
  
  // Temperature command (simplified - needs actual AC remote codes)
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(9000);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delayMicroseconds(4500);
  
  // Temperature code based on target temp (example mapping)
  uint32_t temp_code = 0x02020D00 + (temperature - 16); // Base code + temp offset
  
  for (int i = 31; i >= 0; i--) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(560);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    
    if (temp_code & (1UL << i)) {
      delayMicroseconds(1690);
    } else {
      delayMicroseconds(560);
    }
  }
  
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(560);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  
  Serial.println("IR: AC temperature " + String(temperature) + "°C command sent");
}

void sendTVCommand(String command) {
  // TV IR command (different protocol)
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(2400);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delayMicroseconds(600);
  
  uint32_t tv_code = (command == "on") ? 0xA90 : 0xA90; // Same code for toggle
  
  for (int i = 11; i >= 0; i--) {
    digitalWrite(IR_TRANSMITTER_PIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(IR_TRANSMITTER_PIN, LOW);
    
    if (tv_code & (1UL << i)) {
      delayMicroseconds(1200);
    } else {
      delayMicroseconds(600);
    }
  }
  
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  Serial.println("IR: TV " + command + " command sent");
}

void handleACAutomation() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (!isAuthenticated()) {
    server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Authentication required\"}");
    return;
  }
  
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    
    String action = doc["action"];
    
    if (action == "toggle") {
      autoACEnabled = !autoACEnabled;
      Serial.println("AC Automation " + String(autoACEnabled ? "ENABLED" : "DISABLED"));
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["ac_automation_enabled"] = autoACEnabled;
      response["message"] = String("AC Automation ") + (autoACEnabled ? "enabled" : "disabled");
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
    } else if (action == "set_temp") {
      int newTemp = doc["temperature"];
      if (newTemp >= 16 && newTemp <= 30) {
        currentTargetTemp = newTemp;
        
        // Send temperature command to AC if any AC is on
        if (devices.ac1 || devices.ac2) {
          sendACTemperatureCommand(currentTargetTemp);
        }
        
        DynamicJsonDocument response(256);
        response["status"] = "success";
        response["target_temperature"] = currentTargetTemp;
        response["message"] = "Target temperature set to " + String(currentTargetTemp) + "°C";
        
        String responseStr;
        serializeJson(response, responseStr);
        server.send(200, "application/json", responseStr);
        
        Serial.println("Manual temperature set to: " + String(currentTargetTemp) + "°C");
      } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Temperature must be between 16-30°C\"}");
      }
      
    } else if (action == "force_check") {
      checkACAutomation();
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["message"] = "AC automation check forced";
      response["ac1"] = devices.ac1;
      response["ac2"] = devices.ac2;
      response["target_temp"] = currentTargetTemp;
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
    } else if (action == "start_gradual_mode") {
      if (devices.ac1 || devices.ac2) {
        acGradualMode = true;
        acGradualStartTime = millis();
        acOriginalTemp = currentTargetTemp;
        
        DynamicJsonDocument response(256);
        response["status"] = "success";
        response["message"] = "AC gradual mode started manually";
        response["gradual_mode"] = true;
        response["original_temp"] = acOriginalTemp;
        response["target_temp"] = AC_ROOM_TEMPERATURE;
        
        String responseStr;
        serializeJson(response, responseStr);
        server.send(200, "application/json", responseStr);
        
        Serial.println("AC GRADUAL MODE: Started manually via API");
      } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No AC units are currently running\"}");
      }
      
    } else if (action == "stop_gradual_mode") {
      acGradualMode = false;
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["message"] = "AC gradual mode stopped";
      response["gradual_mode"] = false;
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
      Serial.println("AC GRADUAL MODE: Stopped manually via API");
      
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid action\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No data received\"}");
  }
}

void handleEnergySaving() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (!isAuthenticated()) {
    server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Authentication required\"}");
    return;
  }
  
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    
    String action = doc["action"];
    
    if (action == "toggle") {
      panelAutoShutdownEnabled = !panelAutoShutdownEnabled;
      Serial.println("Energy Saving System " + String(panelAutoShutdownEnabled ? "ENABLED" : "DISABLED"));
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["energy_saving_enabled"] = panelAutoShutdownEnabled;
      response["message"] = String("Energy Saving System ") + (panelAutoShutdownEnabled ? "enabled" : "disabled");
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
    } else if (action == "reset_timer") {
      lastPersonDetected = millis();
      currentEnergyState = NORMAL_OPERATION;
      energySavingMode = false;
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["message"] = "Energy saving timer reset";
      response["current_state"] = getEnergyStateString();
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
      Serial.println("Energy saving timer manually reset");
      
    } else if (action == "force_energy_saving") {
      currentEnergyState = ENERGY_SAVING;
      executeEnergySaving();
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["message"] = "Energy saving mode forced";
      response["current_state"] = getEnergyStateString();
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
    } else if (action == "emergency_shutdown") {
      currentEnergyState = PANEL_SHUTDOWN;
      executeFullShutdown();
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["message"] = "Emergency shutdown executed";
      response["current_state"] = getEnergyStateString();
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid action\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No data received\"}");
  }
}

void handleWorkingHours() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  if (!isAuthenticated()) {
    server.send(401, "application/json", "{\"status\":\"error\",\"message\":\"Authentication required\"}");
    return;
  }
  
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(512);
    deserializeJson(doc, server.arg("plain"));
    
    String action = doc["action"];
    
    if (action == "toggle_working_days") {
      workingDaysEnabled = !workingDaysEnabled;
      Serial.println("Working Days (Mon-Fri) " + String(workingDaysEnabled ? "ENABLED" : "DISABLED"));
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["working_days_enabled"] = workingDaysEnabled;
      response["message"] = String("Working Days ") + (workingDaysEnabled ? "enabled (Mon-Fri)" : "disabled (7 days)");
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
    } else if (action == "toggle_scheduled_shutdown") {
      scheduledShutdownEnabled = !scheduledShutdownEnabled;
      Serial.println("Scheduled Shutdown " + String(scheduledShutdownEnabled ? "ENABLED" : "DISABLED"));
      
      DynamicJsonDocument response(256);
      response["status"] = "success";
      response["scheduled_shutdown_enabled"] = scheduledShutdownEnabled;
      response["message"] = String("Scheduled Shutdown ") + (scheduledShutdownEnabled ? "enabled" : "disabled");
      
      String responseStr;
      serializeJson(response, responseStr);
      server.send(200, "application/json", responseStr);
      
    } else if (action == "set_working_hours") {
      int startHour = doc["start_hour"];
      int endHour = doc["end_hour"];
      
      if (startHour >= 0 && startHour <= 23 && endHour >= 0 && endHour <= 23 && startHour < endHour) {
        workingStartHour = startHour;
        workingEndHour = endHour;
        
        DynamicJsonDocument response(256);
        response["status"] = "success";
        response["start_hour"] = workingStartHour;
        response["end_hour"] = workingEndHour;
        response["message"] = "Working hours updated to " + String(workingStartHour) + ":00 - " + String(workingEndHour) + ":00";
        
        String responseStr;
        serializeJson(response, responseStr);
        server.send(200, "application/json", responseStr);
        
        Serial.println("Working hours updated: " + String(workingStartHour) + ":00 - " + String(workingEndHour) + ":00");
      } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid working hours\"}");
      }
      
    } else if (action == "set_time") {
      int hour = doc["hour"];
      int minute = doc["minute"];
      int dayOfWeek = doc["day_of_week"];
      
      if (hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59 && dayOfWeek >= 1 && dayOfWeek <= 7) {
        currentTime.hour = hour;
        currentTime.minute = minute;
        currentTime.dayOfWeek = dayOfWeek;
        
        DynamicJsonDocument response(256);
        response["status"] = "success";
        response["current_time"] = String(hour) + ":" + (minute < 10 ? "0" : "") + String(minute);
        response["day_of_week"] = getDayName(dayOfWeek);
        response["message"] = "Time updated successfully";
        
        String responseStr;
        serializeJson(response, responseStr);
        server.send(200, "application/json", responseStr);
        
        Serial.println("Time manually set: " + String(hour) + ":" + (minute < 10 ? "0" : "") + String(minute) + " " + getDayName(dayOfWeek));
      } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid time parameters\"}");
      }
      
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid action\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No data received\"}");
  }
}

void handleSystemStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  DynamicJsonDocument doc(1024);
  
  // System status
  doc["status"] = "online";
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  
  // Current time
  JsonObject timeObj = doc.createNestedObject("current_time");
  timeObj["hour"] = currentTime.hour;
  timeObj["minute"] = currentTime.minute;
  timeObj["day_of_week"] = currentTime.dayOfWeek;
  timeObj["day_name"] = getDayName(currentTime.dayOfWeek);
  timeObj["formatted"] = String(currentTime.hour) + ":" + (currentTime.minute < 10 ? "0" : "") + String(currentTime.minute);
  
  // Working hours status
  JsonObject workingHoursObj = doc.createNestedObject("working_hours");
  workingHoursObj["enabled"] = workingDaysEnabled;
  workingHoursObj["start_hour"] = workingStartHour;
  workingHoursObj["end_hour"] = workingEndHour;
  workingHoursObj["is_working_hours"] = isWorkingHours();
  workingHoursObj["is_working_day"] = isWorkingDay();
  workingHoursObj["status"] = getWorkingHoursStatus();
  workingHoursObj["scheduled_shutdown_enabled"] = scheduledShutdownEnabled;
  
  // Energy saving status
  JsonObject energySavingObj = doc.createNestedObject("energy_saving");
  energySavingObj["enabled"] = panelAutoShutdownEnabled;
  energySavingObj["current_state"] = getEnergyStateString();
  energySavingObj["energy_saving_mode"] = energySavingMode;
  energySavingObj["minutes_without_people"] = (millis() - lastPersonDetected) / (60 * 1000);
  energySavingObj["time_to_energy_saving"] = max(0, (int)((ENERGY_SAVING_DELAY - (millis() - lastPersonDetected)) / (60 * 1000)));
  energySavingObj["time_to_shutdown"] = max(0, (int)((PANEL_SHUTDOWN_DELAY - (millis() - lastPersonDetected)) / (60 * 1000)));
  
  // AC automation status
  JsonObject acAutoObj = doc.createNestedObject("ac_automation");
  acAutoObj["enabled"] = autoACEnabled;
  acAutoObj["target_temp"] = currentTargetTemp;
  acAutoObj["base_temp"] = baseTargetTemp;
  acAutoObj["temp_adjustment"] = temperatureAdjustment;
  acAutoObj["proximity_count"] = proximityPeopleCount;
  
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
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}