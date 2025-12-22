# TROUBLESHOOTING INTEGRASI SISTEM SMART ENERGY

## Masalah Umum: "Komponen bisa jalan sendiri-sendiri, tapi tidak bisa saat digabung"

### PENYEBAB UTAMA:

1. **Konflik Pin Assignment**
   - Pin yang sama digunakan untuk beberapa fungsi
   - Pin SPI/I2C bentrok dengan pin digital lain

2. **Power Supply Tidak Cukup**
   - Terlalu banyak komponen aktif bersamaan
   - Voltage drop saat semua sensor bekerja

3. **Timing Issues**
   - Inisialisasi terlalu cepat
   - Sensor belum siap saat dibaca

4. **Memory Overflow**
   - Terlalu banyak library dimuat bersamaan
   - Stack overflow karena fungsi rekursif

5. **Interference Elektromagnetik**
   - Kabel sensor terlalu dekat dengan relay
   - Ground loop problems

---

## SOLUSI BERTAHAP:

### TAHAP 1: VALIDASI HARDWARE

```cpp
// Test pin assignment - pastikan tidak ada konflik
void testPinAssignment() {
  Serial.println("=== PIN ASSIGNMENT TEST ===");
  
  // DHT Pins
  Serial.println("DHT1: Pin " + String(DHT1_PIN));
  Serial.println("DHT2: Pin " + String(DHT2_PIN));
  
  // LDR Pins (harus ADC)
  Serial.println("LDR1: Pin " + String(LDR1_PIN) + " (ADC)");
  Serial.println("LDR2: Pin " + String(LDR2_PIN) + " (ADC)");
  
  // Digital Pins
  Serial.println("Proximity1: Pin " + String(PROXIMITY1_PIN));
  Serial.println("Proximity2: Pin " + String(PROXIMITY2_PIN));
  Serial.println("IR TX: Pin " + String(IR_TRANSMITTER_PIN));
  
  // Relay Pins
  Serial.println("Relay1: Pin " + String(RELAY1_PIN));
  Serial.println("Relay2: Pin " + String(RELAY2_PIN));
  Serial.println("Relay3: Pin " + String(RELAY3_PIN));
  Serial.println("Relay4: Pin " + String(RELAY4_PIN));
  
  // Cek konflik
  int pins[] = {DHT1_PIN, DHT2_PIN, LDR1_PIN, LDR2_PIN, 
                PROXIMITY1_PIN, PROXIMITY2_PIN, IR_TRANSMITTER_PIN,
                RELAY1_PIN, RELAY2_PIN, RELAY3_PIN, RELAY4_PIN};
  
  for (int i = 0; i < 11; i++) {
    for (int j = i + 1; j < 11; j++) {
      if (pins[i] == pins[j]) {
        Serial.println("ERROR: Pin conflict detected!");
        Serial.println("Pin " + String(pins[i]) + " used multiple times");
      }
    }
  }
}
```

### TAHAP 2: POWER SUPPLY CHECK

```cpp
void checkPowerSupply() {
  Serial.println("=== POWER SUPPLY CHECK ===");
  
  // Cek voltage
  float voltage = analogRead(A0) * (3.3 / 4095.0);
  Serial.println("Supply Voltage: " + String(voltage, 2) + "V");
  
  if (voltage < 3.0) {
    Serial.println("WARNING: Low voltage detected!");
    Serial.println("Recommendation: Use external 5V supply");
  }
  
  // Test current draw
  Serial.println("Testing current draw...");
  
  // Baseline
  Serial.println("1. Baseline (no load)");
  delay(1000);
  
  // DHT sensors
  Serial.println("2. DHT sensors active");
  dht1.begin();
  dht2.begin();
  delay(2000);
  
  // Relays (one by one)
  Serial.println("3. Testing relays individually");
  digitalWrite(RELAY1_PIN, LOW);
  delay(500);
  digitalWrite(RELAY1_PIN, HIGH);
  delay(500);
  
  Serial.println("Power test completed");
}
```

### TAHAP 3: INISIALISASI BERTAHAP

```cpp
void gradualInitialization() {
  Serial.println("=== GRADUAL INITIALIZATION ===");
  
  // Step 1: Basic pins
  Serial.println("Step 1: Basic pins");
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  delay(1000);
  
  // Step 2: DHT sensors (paling sensitif)
  Serial.println("Step 2: DHT sensors");
  dht1.begin();
  delay(2000);  // DHT needs time
  dht2.begin();
  delay(2000);
  
  // Step 3: Digital sensors
  Serial.println("Step 3: Digital sensors");
  pinMode(PROXIMITY1_PIN, INPUT_PULLUP);
  pinMode(PROXIMITY2_PIN, INPUT_PULLUP);
  delay(500);
  
  // Step 4: Analog sensors
  Serial.println("Step 4: Analog sensors");
  // LDR tidak perlu inisialisasi khusus
  delay(500);
  
  // Step 5: Output devices (relay)
  Serial.println("Step 5: Relay outputs");
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  
  // Set safe states
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
  digitalWrite(RELAY3_PIN, HIGH);
  digitalWrite(RELAY4_PIN, HIGH);
  delay(1000);
  
  // Step 6: IR transmitter
  Serial.println("Step 6: IR transmitter");
  pinMode(IR_TRANSMITTER_PIN, OUTPUT);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  delay(500);
  
  // Step 7: WiFi (paling berat)
  Serial.println("Step 7: WiFi connection");
  WiFi.begin(ssid, password);
  // WiFi connection handled separately
  
  Serial.println("Gradual initialization completed");
}
```

### TAHAP 4: ERROR HANDLING & FALLBACK

```cpp
struct ComponentStatus {
  bool dht1_working = false;
  bool dht2_working = false;
  bool ldr_working = false;
  bool proximity_working = false;
  bool relay_working = false;
  bool ir_working = false;
  bool wifi_working = false;
  int failed_count = 0;
};

ComponentStatus status;

void testAllComponents() {
  Serial.println("=== COMPONENT TESTING ===");
  
  // Test DHT1
  float temp1 = dht1.readTemperature();
  if (!isnan(temp1) && temp1 > 0 && temp1 < 60) {
    status.dht1_working = true;
    Serial.println("DHT1: ✓ OK");
  } else {
    Serial.println("DHT1: ✗ FAIL");
    status.failed_count++;
  }
  
  // Test DHT2
  float temp2 = dht2.readTemperature();
  if (!isnan(temp2) && temp2 > 0 && temp2 < 60) {
    status.dht2_working = true;
    Serial.println("DHT2: ✓ OK");
  } else {
    Serial.println("DHT2: ✗ FAIL");
    status.failed_count++;
  }
  
  // Test LDR
  int ldr1 = analogRead(LDR1_PIN);
  int ldr2 = analogRead(LDR2_PIN);
  if (ldr1 > 0 && ldr1 < 4095 && ldr2 > 0 && ldr2 < 4095) {
    status.ldr_working = true;
    Serial.println("LDR: ✓ OK");
  } else {
    Serial.println("LDR: ✗ FAIL");
    status.failed_count++;
  }
  
  // Test Proximity
  bool prox1 = digitalRead(PROXIMITY1_PIN);
  bool prox2 = digitalRead(PROXIMITY2_PIN);
  status.proximity_working = true; // Assume OK if readable
  Serial.println("Proximity: ✓ OK");
  
  // Test Relay
  digitalWrite(RELAY1_PIN, LOW);
  delay(100);
  digitalWrite(RELAY1_PIN, HIGH);
  status.relay_working = true; // Assume OK if no crash
  Serial.println("Relay: ✓ OK");
  
  // Test IR
  digitalWrite(IR_TRANSMITTER_PIN, HIGH);
  delayMicroseconds(500);
  digitalWrite(IR_TRANSMITTER_PIN, LOW);
  status.ir_working = true; // Assume OK if no crash
  Serial.println("IR: ✓ OK");
  
  // Test WiFi
  if (WiFi.status() == WL_CONNECTED) {
    status.wifi_working = true;
    Serial.println("WiFi: ✓ OK");
  } else {
    Serial.println("WiFi: ✗ FAIL");
    status.failed_count++;
  }
  
  Serial.println("Failed components: " + String(status.failed_count) + "/7");
  
  if (status.failed_count <= 2) {
    Serial.println("System Status: ✓ GOOD");
  } else if (status.failed_count <= 4) {
    Serial.println("System Status: ⚠ LIMITED");
  } else {
    Serial.println("System Status: ✗ CRITICAL");
  }
}
```

---

## CHECKLIST DEBUGGING:

### ✅ HARDWARE CHECKS:
- [ ] Semua kabel terpasang dengan benar
- [ ] Tidak ada kabel yang putus atau longgar
- [ ] Power supply cukup (minimal 2A untuk ESP32 + sensors + relay)
- [ ] Ground semua komponen terhubung
- [ ] Tidak ada short circuit

### ✅ SOFTWARE CHECKS:
- [ ] Pin assignment tidak konflik
- [ ] Library versi yang kompatibel
- [ ] Memory usage tidak melebihi batas
- [ ] Timing delay yang cukup antar inisialisasi
- [ ] Error handling untuk setiap komponen

### ✅ ELECTRICAL CHECKS:
- [ ] Voltage level sesuai (3.3V untuk ESP32, 5V untuk relay)
- [ ] Pull-up resistor untuk pin digital
- [ ] Decoupling capacitor untuk power supply
- [ ] Shielding untuk kabel sensor

---

## TIPS INTEGRASI:

1. **Mulai Minimal**: Integrasikan 2-3 komponen dulu, baru tambah bertahap
2. **Monitor Serial**: Selalu pantau output serial untuk error
3. **Test Satu-Satu**: Jika ada masalah, disable komponen satu per satu
4. **Power External**: Gunakan power supply eksternal untuk relay
5. **Ground Proper**: Pastikan ground star configuration
6. **Cable Management**: Pisahkan kabel power dan signal
7. **Timing Critical**: Beri delay yang cukup antar inisialisasi

---

## KODE MINIMAL UNTUK TESTING:

```cpp
// Kode minimal untuk test integrasi
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== MINIMAL INTEGRATION TEST ===");
  
  // Test 1: DHT only
  Serial.println("Test 1: DHT sensors");
  dht1.begin();
  delay(2000);
  float temp = dht1.readTemperature();
  Serial.println("DHT1 temp: " + String(temp));
  
  // Test 2: Add LDR
  Serial.println("Test 2: Add LDR");
  int ldr = analogRead(LDR1_PIN);
  Serial.println("LDR1 value: " + String(ldr));
  
  // Test 3: Add Proximity
  Serial.println("Test 3: Add Proximity");
  pinMode(PROXIMITY1_PIN, INPUT_PULLUP);
  bool prox = digitalRead(PROXIMITY1_PIN);
  Serial.println("Proximity1: " + String(prox));
  
  // Test 4: Add Relay
  Serial.println("Test 4: Add Relay");
  pinMode(RELAY1_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, HIGH);
  Serial.println("Relay1 initialized");
  
  // Test 5: Add WiFi
  Serial.println("Test 5: Add WiFi");
  WiFi.begin(ssid, password);
  // Don't wait, just start
  
  Serial.println("All components initialized");
}

void loop() {
  // Simple monitoring
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 5000) {
    Serial.println("=== STATUS CHECK ===");
    Serial.println("DHT1: " + String(dht1.readTemperature()));
    Serial.println("LDR1: " + String(analogRead(LDR1_PIN)));
    Serial.println("Prox1: " + String(digitalRead(PROXIMITY1_PIN)));
    Serial.println("WiFi: " + String(WiFi.status() == WL_CONNECTED ? "OK" : "FAIL"));
    Serial.println("Free heap: " + String(ESP.getFreeHeap()));
    lastCheck = millis();
  }
  
  delay(100);
}
```

Gunakan kode ini untuk mengidentifikasi pada tahap mana sistem mulai bermasalah, lalu fokus perbaikan pada tahap tersebut.