# 🔧 PCB Final Configuration - ESP32 Smart Energy

## ✅ Pin Configuration PCB Final:

### 📍 Pin Mapping (Sudah Dicetak di PCB):

| **Component** | **Pin GPIO** | **Function** | **Notes** |
|---------------|--------------|--------------|-----------|
| **IR Transmitter** | GPIO 15 | Output | IR LED + 220Ω resistor |
| **Proximity IN** | GPIO 32 | Input_Pullup | Sensor MASUK (Entry) |
| **Proximity OUT** | GPIO 33 | Input_Pullup | Sensor KELUAR (Exit) |
| **DHT22 #1** | GPIO 12 | Digital | Temperature & Humidity |
| **DHT22 #2** | GPIO 13 | Digital | Temperature & Humidity |
| **LDR #1** | GPIO 34 | Analog Input | Light Sensor 1 |
| **LDR #2** | GPIO 35 | Analog Input | Light Sensor 2 |

### 🔌 Hardware Connections:

#### IR Transmitter (GPIO 15):
```
ESP32 GPIO 15 → 220Ω Resistor → IR LED (+)
IR LED (-) → GND
```

#### Proximity Sensors:
```
Proximity IN (GPIO 32):
- VCC → 3.3V
- GND → GND  
- OUT → GPIO 32 (INPUT_PULLUP)

Proximity OUT (GPIO 33):
- VCC → 3.3V
- GND → GND
- OUT → GPIO 33 (INPUT_PULLUP)
```

#### DHT22 Sensors:
```
DHT22 #1 (GPIO 12):
- VCC → 3.3V
- GND → GND
- DATA → GPIO 12

DHT22 #2 (GPIO 13):
- VCC → 3.3V  
- GND → GND
- DATA → GPIO 13
```

#### LDR Sensors:
```
LDR #1 (GPIO 34):
- LDR → GPIO 34 & 10kΩ to GND
- VCC → 3.3V

LDR #2 (GPIO 35):
- LDR → GPIO 35 & 10kΩ to GND
- VCC → 3.3V
```

## 🚀 Code Configuration:

### 📝 Pin Definitions:
```cpp
// ================= PINS - PCB FINAL CONFIGURATION =================
#define IR_PIN 15               // IR Transmitter pin (PCB FINAL)
#define PROXIMITY_PIN_IN  32    // Proximity sensor MASUK (PCB FINAL)
#define PROXIMITY_PIN_OUT 33    // Proximity sensor KELUAR (PCB FINAL)
#define DHTPIN    12            // DHT22 pin 1 (PCB FINAL)
#define DHTPIN2   13            // DHT22 pin 2 (PCB FINAL)
#define LDR_PIN   34            // LDR pin 1 (PCB FINAL)
#define LDR_PIN2  35            // LDR pin 2 (PCB FINAL)
```

### ⚡ Ultra Fast Settings:
```cpp
#define PERSON_COOLDOWN 100     // 100ms cooldown untuk deteksi cepat
#define DEBOUNCE_DELAY 50       // 50ms debounce untuk responsif maksimal
```

### 🔧 Hardware Initialization:
```cpp
void setup() {
  // Initialize pins - PCB FINAL CONFIGURATION
  pinMode(PROXIMITY_PIN_IN, INPUT_PULLUP);   // GPIO 32
  pinMode(PROXIMITY_PIN_OUT, INPUT_PULLUP);  // GPIO 33
  pinMode(LDR_PIN, INPUT);                   // GPIO 34
  pinMode(LDR_PIN2, INPUT);                  // GPIO 35
  pinMode(IR_PIN, OUTPUT);                   // GPIO 15
  
  // Setup interrupts
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN_IN), proximityInInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN_OUT), proximityOutInterrupt, CHANGE);
  
  // Initialize sensors
  dht.begin();   // DHT22 on GPIO 12
  dht2.begin();  // DHT22 on GPIO 13
  irsend.begin(); // IR on GPIO 15
}
```

## 📊 Sensor Reading Functions:

### 🌡️ Dual DHT22 (Average Reading):
```cpp
float temp1 = dht.readTemperature();   // GPIO 12
float temp2 = dht2.readTemperature();  // GPIO 13
float avgTemp = (temp1 + temp2) / 2.0; // Average
```

### 💡 Dual LDR (Average Reading):
```cpp
int light1 = analogRead(LDR_PIN);      // GPIO 34
int light2 = analogRead(LDR_PIN2);     // GPIO 35
int avgLight = (light1 + light2) / 2;  // Average
```

### 🚶 Proximity Detection:
```cpp
bool proximityIn = !digitalRead(PROXIMITY_PIN_IN);   // GPIO 32 (LOW = detected)
bool proximityOut = !digitalRead(PROXIMITY_PIN_OUT); // GPIO 33 (LOW = detected)
```

## ⚡ Ultra Fast Performance:

### 🎯 Response Times:
- **Proximity Detection**: <100ms
- **IR Transmission**: <5ms  
- **AC Control**: <100ms total
- **Data Update**: Real-time

### 🔄 Detection Logic:
```cpp
// Entry Detection (GPIO 32)
if (proximityIn && !lastProximityIn) {
    jumlahOrang++;
    kontrolAC(); // Immediate AC response
}

// Exit Detection (GPIO 33)  
if (proximityOut && !lastProximityOut) {
    if (jumlahOrang > 0) {
        jumlahOrang--;
        kontrolAC(); // Immediate AC response
    }
}
```

## 🔍 Testing & Debugging:

### 📊 Serial Monitor Output:
```
Pin Configuration (PCB FINAL ESP32 Smart Energy):
- PROXIMITY_IN: GPIO 32 (Sensor MASUK) - PCB FINAL
- PROXIMITY_OUT: GPIO 33 (Sensor KELUAR) - PCB FINAL  
- DHT22_1: GPIO 12 - PCB FINAL
- DHT22_2: GPIO 13 - PCB FINAL
- LDR_1: GPIO 34 - PCB FINAL
- LDR_2: GPIO 35 - PCB FINAL
- IR LED: GPIO 15 - PCB FINAL (+ resistor 220Ω)
```

### 🧪 Proximity Test:
```
🔍 PROXIMITY DEBUG (ULTRA FAST MODE - PCB FINAL):
   GPIO 32 (MASUK): LOW | Detected: YES
   GPIO 33 (KELUAR): HIGH | Detected: NO
```

### 🌡️ Sensor Test:
```
🌡️ DHT22 SENSOR READINGS:
   DHT22-1 (GPIO 12): Suhu: 25.2°C | Kelembaban: 65.1%
   DHT22-2 (GPIO 13): Suhu: 25.0°C | Kelembaban: 64.8%
   RATA-RATA Suhu: 25.1°C
```

## 🎛️ AC Control Logic:

### 👥 People Count → AC Response:
- **0 orang**: AC OFF
- **1-5 orang**: 1 AC ON 25°C
- **6-10 orang**: 1 AC ON 22°C  
- **11-15 orang**: 2 AC ON 22°C
- **16+ orang**: 2 AC MAX 20°C

### 📡 IR Transmission (GPIO 15):
```cpp
// Ultra Fast IR Control
ac1.on();
ac1.setTemp(25);
ac1.send(); // <5ms response time
```

## 🔧 Troubleshooting PCB:

### ✅ Pin Verification:
1. **GPIO 15 (IR)**: Check IR LED + 220Ω resistor
2. **GPIO 32/33 (Proximity)**: Check INPUT_PULLUP
3. **GPIO 12/13 (DHT22)**: Check 3.3V power
4. **GPIO 34/35 (LDR)**: Check analog reading 0-4095

### 🔍 Common Issues:
- **IR tidak bekerja**: Periksa polaritas LED dan resistor
- **Proximity selalu HIGH/LOW**: Periksa koneksi VCC/GND
- **DHT22 NaN**: Periksa koneksi data dan power
- **LDR tidak berubah**: Periksa pull-down resistor 10kΩ

### 📊 Expected Values:
- **Proximity**: HIGH (no object), LOW (object detected)
- **DHT22**: Temperature 20-35°C, Humidity 30-80%
- **LDR**: 0-4095 (mapped to 0-1000 lux)
- **IR**: Visual check dengan kamera HP (IR LED menyala)

## 🎉 PCB Final Features:

### ✅ Advantages:
- **Fixed Pin Layout**: Tidak perlu ubah konfigurasi lagi
- **Dual Sensors**: DHT22 dan LDR untuk akurasi tinggi
- **Ultra Fast Response**: <100ms total response time
- **Reliable Connections**: PCB traces lebih stabil
- **Easy Debugging**: Pin numbers jelas di PCB

### 🚀 Performance:
- **Detection Speed**: 100ms cooldown
- **IR Response**: <5ms transmission
- **Sensor Accuracy**: Dual sensor averaging
- **Network**: HTTPS ke dasko.fst.unja.ac.id
- **Display**: Real-time TFT updates

---

## 📞 Support:

Jika ada masalah dengan PCB:
1. **Check pin connections** sesuai tabel di atas
2. **Verify power supply** 3.3V untuk semua sensor
3. **Test individual sensors** dengan serial monitor
4. **Check IR LED** dengan kamera HP (infrared visible)

**Status**: ✅ PCB Final configuration completed and optimized!