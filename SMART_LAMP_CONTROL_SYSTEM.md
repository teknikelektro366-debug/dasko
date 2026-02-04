# SMART LAMP CONTROL SYSTEM
## Kontrol Lampu Cerdas Berdasarkan Jumlah Orang & Intensitas Cahaya

### 🎯 OVERVIEW
Sistem kontrol lampu pintar yang menggunakan dual relay untuk mengontrol 12 lampu TL (6 lampu per relay) berdasarkan:
- **Jumlah orang** di ruangan
- **Intensitas cahaya** (LDR sensor)
- **Auto-shutdown timer** 5 menit jika tidak ada orang

### ⚡ PIN CONFIGURATION (USER REAL CONFIG - AMAN UPLOAD)

| Component | GPIO Pin | Function | Notes |
|-----------|----------|----------|-------|
| **IR Transmitter** | **4** | **AC Control** | **Pindah dari 15 ke 4 (aman upload)** |
| Proximity IN | 32 | Input (MASUK) | Tidak berubah |
| Proximity OUT | 33 | Input (KELUAR) | Tidak berubah |
| **DHT22 #1** | **16** | **Temperature/Humidity** | **Pindah dari 12 ke 16 (aman upload)** |
| **DHT22 #2** | **17** | **Temperature/Humidity** | **Pindah dari 13 ke 17 (aman upload)** |
| LDR #1 | 35 | Light Sensor | Tidak berubah |
| LDR #2 | 34 | Light Sensor | Tidak berubah |
| **RELAY_LAMP1** | **25** | **Lampu 1 (6 TL)** | **USER CONFIG** |
| **RELAY_LAMP2** | **26** | **Lampu 2 (6 TL)** | **USER CONFIG** |

### 🧠 SMART LAMP CONTROL LOGIC

#### 1. **Berdasarkan Jumlah Orang & Intensitas Cahaya**

```cpp
// 0 Orang
if (peopleCount == 0) {
    // Mulai timer auto-shutdown 5 menit
    shouldLampBeOn = false;
}

// Ada Orang (1+)
else if (peopleCount >= 1) {
    if (lightLevel < 200) {
        // Cahaya kurang - nyalakan lampu
        shouldLampBeOn = true;
    }
    else if (lightLevel >= 200 && lightLevel < 400) {
        // Cahaya sedang - nyalakan jika banyak orang (3+)
        shouldLampBeOn = (peopleCount >= 3);
    }
    else {
        // Cahaya cukup (>= 400 lux) - matikan lampu
        shouldLampBeOn = false;
    }
}
```

#### 2. **Threshold Intensitas Cahaya**
- **< 200 lux**: Cahaya kurang → Lampu ON (jika ada orang)
- **200-400 lux**: Cahaya sedang → Lampu ON jika ≥3 orang
- **≥ 400 lux**: Cahaya cukup → Lampu OFF

#### 3. **Auto-Shutdown Timer**
- **Trigger**: Ketika jumlah orang = 0
- **Duration**: 5 menit
- **Action**: Matikan kedua relay lampu
- **Cancel**: Jika ada orang masuk sebelum timer habis

### 🔧 HARDWARE SETUP

#### Relay Connection
```
ESP32 GPIO 25 → Relay 1 IN → 6 Lampu TL (Jalur 1)
ESP32 GPIO 26 → Relay 2 IN → 6 Lampu TL (Jalur 2)
```

#### Power Requirements
- **ESP32**: 3.3V/5V
- **Relay Module**: 5V (biasanya)
- **Lampu TL**: 220V AC (melalui relay)

### 📊 OPERATIONAL SCENARIOS

#### Scenario 1: Ruangan Kosong
```
People: 0 | Light: Any
Action: Start 5-minute timer → Lamps OFF after 5 minutes
```

#### Scenario 2: 1-2 Orang, Cahaya Kurang
```
People: 1-2 | Light: < 200 lux
Action: Lamps ON (12 TL) | Cancel auto-shutdown
```

#### Scenario 3: 1-2 Orang, Cahaya Sedang
```
People: 1-2 | Light: 200-400 lux
Action: Lamps OFF (cahaya cukup untuk sedikit orang)
```

#### Scenario 4: 3+ Orang, Cahaya Sedang
```
People: 3+ | Light: 200-400 lux
Action: Lamps ON (12 TL) | Cancel auto-shutdown
```

#### Scenario 5: Any People, Cahaya Terang
```
People: Any | Light: ≥ 400 lux
Action: Lamps OFF (cahaya sudah cukup terang)
```

### 🔍 DEBUG OUTPUT

```
💡 SMART LAMP DECISION: ON - 3 people, medium light (250 lux)
💡 ULTRA FAST: Dual Lamps ON - 12 lampu TL (Auto shutdown in 5 minutes)
💡 SMART: Starting 5-minute auto-shutdown timer (no people)
💡 ULTRA FAST: Dual Lamps auto-shutdown after 5 minutes
💡 SMART: Lamps OFF immediately - too bright
```

### 📱 TFT DISPLAY INFO

```
Status Bar: "● Mode Otomatis Aktif | Lamps: ON (12 TL)"
Bottom Info: "Smart Control: People count + Light level | Auto shutdown: 5min"
```

### ⚙️ CONFIGURATION PARAMETERS

```cpp
// Light level thresholds
#define LIGHT_LOW_THRESHOLD    200   // < 200 lux = low light
#define LIGHT_MEDIUM_THRESHOLD 400   // 200-400 lux = medium light
#define LIGHT_HIGH_THRESHOLD   400   // >= 400 lux = bright light

// People count thresholds
#define MIN_PEOPLE_FOR_MEDIUM_LIGHT 3  // Minimum people for medium light

// Auto-shutdown timer
#define LAMP_AUTO_SHUTDOWN_DELAY 300000  // 5 minutes in milliseconds
```

### 🚀 PERFORMANCE FEATURES

- **Response Time**: < 1ms untuk kontrol relay
- **Smart Decision**: Berdasarkan 2 parameter (people + light)
- **Energy Efficient**: Auto-shutdown untuk hemat listrik
- **Dual Control**: 2 relay untuk redundancy dan kontrol terpisah
- **Real-time Monitoring**: Status ditampilkan di TFT dan serial
- **Hosting Integration**: Data dikirim ke dasko.fst.unja.ac.id

### 🔧 TROUBLESHOOTING

#### Lampu Tidak Menyala
1. Cek koneksi GPIO 25 & 26
2. Cek power supply relay module
3. Cek wiring relay ke lampu TL
4. Monitor serial output untuk debug

#### Auto-Shutdown Tidak Bekerja
1. Cek proximity sensor detection
2. Monitor timer di serial output
3. Cek logika people count

#### Smart Control Tidak Akurat
1. Kalibrasi LDR sensor
2. Adjust threshold values
3. Cek pembacaan light level di serial

### 📋 TESTING PROCEDURE

1. **Test Relay**: Manual ON/OFF via serial command
2. **Test Light Sensor**: Monitor LDR readings
3. **Test People Detection**: Verify proximity sensors
4. **Test Smart Logic**: Various people + light combinations
5. **Test Auto-Shutdown**: Leave room empty for 5+ minutes

### ✅ SUCCESS CRITERIA

- ✅ Lampu menyala/mati berdasarkan people count + light level
- ✅ Auto-shutdown bekerja setelah 5 menit tanpa orang
- ✅ Dual relay mengontrol 12 lampu TL dengan benar
- ✅ Smart decision logic berfungsi sesuai threshold
- ✅ Data terintegrasi dengan hosting web
- ✅ TFT menampilkan status lampu dengan akurat

---

**Status**: ✅ **IMPLEMENTED & READY FOR TESTING**  
**Version**: v3.1 Smart Lamp Control + Hosting Web  
**Date**: January 19, 2026  
**Hardware**: ESP32 + Dual Relay + 12 Lampu TL