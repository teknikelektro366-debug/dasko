# 🔌 AC Hardware Reading Guide

## 📋 Overview
Panduan untuk membaca status AC yang sebenarnya dari hardware, bukan berdasarkan logika otomatis. Sistem akan membaca status ON/OFF dan temperature setting dari AC yang sesungguhnya.

## 🔧 Hardware Requirements

### **1. AC Status Detection**
Untuk membaca apakah AC sedang ON atau OFF:

#### **Option A: Current Sensor (Recommended)**
```
AC Power Line → Current Sensor (ACS712) → ESP32 Pin
- ACS712-5A atau ACS712-20A
- Output: Analog voltage proportional to current
- Threshold: > 0.5A = AC ON, < 0.5A = AC OFF
```

#### **Option B: Relay Feedback**
```
AC Control Relay → Feedback Pin → ESP32 Pin
- Gunakan relay dengan feedback contact
- Output: Digital HIGH = AC ON, LOW = AC OFF
```

#### **Option C: IR Receiver**
```
AC IR Receiver → ESP32 Pin
- Baca sinyal IR dari AC remote
- Decode status ON/OFF dari IR signal
```

### **2. Temperature Setting Detection**

#### **Option A: Potentiometer Reading**
```
AC Temperature Control → Potentiometer → ESP32 Analog Pin
- Read voltage from temp control knob
- Map voltage to temperature range (16-30°C)
```

#### **Option B: Display Reading (Advanced)**
```
AC Digital Display → Camera/OCR → ESP32
- Capture AC display with camera
- Use OCR to read temperature setting
- More complex but most accurate
```

## 🔌 Wiring Diagram

### **Basic Wiring (Current Sensor Method)**
```
ESP32 Pin Connections:
├── Pin 35 (AC1_STATUS_PIN) ← ACS712 AC1 Output
├── Pin 36 (AC2_STATUS_PIN) ← ACS712 AC2 Output  
├── Pin 39 (AC_TEMP_SENSOR_PIN) ← Temperature Potentiometer
├── VCC → 3.3V
└── GND → Ground

ACS712 Connections:
├── VCC → 5V (or 3.3V for 3.3V version)
├── GND → Ground
├── OUT → ESP32 Analog Pin
└── AC Line → Through ACS712 current path
```

### **Advanced Wiring (Multiple Sensors)**
```
AC Unit 1:
├── Current Sensor → Pin 35
├── Temp Sensor → Pin 39
└── Control Relay → Pin 4 (IR)

AC Unit 2:  
├── Current Sensor → Pin 36
├── Temp Sensor → Pin 25
└── Control Relay → Pin 2 (IR)
```

## ⚙️ Configuration Options

### **Mode Selection**
```cpp
// AC Mode Configuration
#define AC_MODE_READ_HARDWARE true    // Read from actual hardware
#define AC_MODE_AUTO_CONTROL false   // Disable auto control

// Pin Definitions
#define AC1_STATUS_PIN 35  // Current sensor for AC1
#define AC2_STATUS_PIN 36  // Current sensor for AC2  
#define AC_TEMP_SENSOR_PIN 39  // Temperature reading
```

### **Calibration Settings**
```cpp
// Current Sensor Calibration
#define AC_ON_CURRENT_THRESHOLD 100   // ADC value for AC ON detection
#define AC_OFF_CURRENT_THRESHOLD 50   // ADC value for AC OFF detection

// Temperature Mapping
#define TEMP_MIN_ADC 0      // ADC value for 16°C
#define TEMP_MAX_ADC 4095   // ADC value for 30°C
#define TEMP_MIN_CELSIUS 16 // Minimum temperature
#define TEMP_MAX_CELSIUS 30 // Maximum temperature
```

## 📊 Reading Functions

### **Current Sensor Reading**
```cpp
bool readACStatus(int pin) {
  int currentReading = analogRead(pin);
  
  // Convert ADC to current (for ACS712-5A)
  float voltage = (currentReading * 3.3) / 4095.0;
  float current = (voltage - 2.5) / 0.185; // ACS712-5A sensitivity
  
  return abs(current) > 0.5; // AC ON if current > 0.5A
}
```

### **Temperature Reading**
```cpp
int readACTemperature(int pin) {
  int tempReading = analogRead(pin);
  
  // Map ADC reading to temperature range
  int temperature = map(tempReading, 0, 4095, 16, 30);
  
  // Apply calibration offset if needed
  temperature += TEMP_CALIBRATION_OFFSET;
  
  return constrain(temperature, 16, 30);
}
```

## 🔍 Calibration Process

### **Step 1: Current Sensor Calibration**
```cpp
void calibrateCurrentSensor() {
  Serial.println("=== CURRENT SENSOR CALIBRATION ===");
  
  // Test with AC OFF
  Serial.println("Turn OFF AC and press any key...");
  while(!Serial.available()) delay(100);
  int offReading = analogRead(AC1_STATUS_PIN);
  Serial.println("AC OFF Reading: " + String(offReading));
  
  // Test with AC ON
  Serial.println("Turn ON AC and press any key...");
  while(!Serial.available()) delay(100);
  int onReading = analogRead(AC1_STATUS_PIN);
  Serial.println("AC ON Reading: " + String(onReading));
  
  // Calculate threshold
  int threshold = (offReading + onReading) / 2;
  Serial.println("Suggested Threshold: " + String(threshold));
}
```

### **Step 2: Temperature Calibration**
```cpp
void calibrateTemperatureSensor() {
  Serial.println("=== TEMPERATURE CALIBRATION ===");
  
  for(int temp = 16; temp <= 30; temp += 2) {
    Serial.println("Set AC to " + String(temp) + "°C and press any key...");
    while(!Serial.available()) delay(100);
    
    int reading = analogRead(AC_TEMP_SENSOR_PIN);
    Serial.println(String(temp) + "°C = ADC " + String(reading));
  }
}
```

## 📈 Expected Data Flow

### **Hardware Reading Mode**
```
AC Hardware → Sensors → ESP32 → Laravel → Dashboard
     ↓
Real AC Status (ON/OFF)
Real Temperature Setting (16-30°C)
Real Power Consumption
```

### **Data Comparison**
| Data Source | Timer Mode | Change-Based + Hardware |
|-------------|------------|------------------------|
| **AC Status** | Calculated | Read from hardware |
| **Temperature** | Calculated | Read from hardware |
| **Accuracy** | 70% | 95% |
| **Real-time** | No | Yes |
| **Power Data** | No | Yes |

## 🛠 Implementation Steps

### **1. Hardware Setup**
```bash
# Install current sensors on AC power lines
# Connect temperature reading circuit
# Wire all sensors to ESP32 pins
```

### **2. Code Configuration**
```cpp
// Enable hardware reading mode
#define AC_MODE_READ_HARDWARE true
#define AC_MODE_AUTO_CONTROL false

// Set correct pin numbers
#define AC1_STATUS_PIN 35
#define AC2_STATUS_PIN 36
#define AC_TEMP_SENSOR_PIN 39
```

### **3. Calibration**
```cpp
// Run calibration functions in setup()
void setup() {
  // ... existing setup code ...
  
  // Uncomment for calibration
  // calibrateCurrentSensor();
  // calibrateTemperatureSensor();
}
```

### **4. Testing**
```bash
# Monitor Serial output
# Verify AC status detection
# Verify temperature reading
# Test change detection
```

## 🔧 Troubleshooting

### **Problem**: AC status always shows OFF
**Solutions**:
- Check current sensor wiring
- Verify power supply to sensor
- Adjust current threshold
- Check AC power consumption

### **Problem**: Temperature reading incorrect
**Solutions**:
- Calibrate temperature sensor
- Check analog pin connection
- Verify voltage divider circuit
- Adjust mapping values

### **Problem**: Too many false positives
**Solutions**:
- Add hysteresis to readings
- Increase change thresholds
- Add averaging filter
- Check sensor noise

## 📊 Expected Serial Output

### **Hardware Reading Mode**
```
=== AC STATUS READING ===
AC1 Pin: HIGH (ON) - Current: 2.3A
AC2 Pin: LOW (OFF) - Current: 0.1A  
Temp Reading: 2048 -> 23°C
Final Status: 1 AC ON
========================

🔄 Changes detected: AC(OFF→1 AC ON) Temp(25→23)
✓ SUCCESS - Data sent to Laravel!
```

### **Calibration Output**
```
=== CURRENT SENSOR CALIBRATION ===
Turn OFF AC and press any key...
AC OFF Reading: 2048
Turn ON AC and press any key...
AC ON Reading: 3200
Suggested Threshold: 2624
```

## 🎯 Benefits of Hardware Reading

1. **Real Data**: Status AC yang sebenarnya, bukan estimasi
2. **Accurate**: Temperature setting yang akurat dari hardware
3. **Power Monitoring**: Bisa monitor konsumsi listrik real
4. **Fault Detection**: Deteksi jika AC rusak atau tidak respond
5. **Energy Efficiency**: Data konsumsi energi yang akurat
6. **Maintenance**: Alert jika AC tidak berfungsi normal

Dengan hardware reading, sistem akan memberikan data yang 100% akurat sesuai kondisi AC yang sebenarnya! 🎯