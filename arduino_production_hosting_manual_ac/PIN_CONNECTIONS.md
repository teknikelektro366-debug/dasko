# PIN CONNECTIONS - ESP32 SMART ENERGY PRODUCTION

## SENSOR PROXIMITY

### Sensor MASUK (Sisi Luar Pintu)
- VCC → 3.3V atau 5V (tergantung tipe sensor)
- GND → GND
- OUT → GPIO 2 (Digital Output)

### Sensor KELUAR (Sisi Dalam Pintu)
- VCC → 3.3V atau 5V (tergantung tipe sensor)
- GND → GND
- OUT → GPIO 15 (Digital Output)

## SENSOR LAINNYA

### DHT22 (Suhu & Kelembaban)
- VCC → 3.3V
- GND → GND
- DATA → GPIO 27

### LDR (Light Sensor)
- VCC → 3.3V
- GND → GND
- SIGNAL → GPIO 34 (Analog)

### IR LED (AC Control) - LED IR Langsung
- Anoda (+) → GPIO 14 (melalui resistor 220Ω)
- Katoda (-) → GND

## TFT DISPLAY
- Menggunakan pin SPI default ESP32
- Tidak ada konflik dengan sensor lain

## RINGKASAN PIN YANG DIGUNAKAN
- GPIO 2: PROXIMITY_IN (Sensor Masuk)
- GPIO 14: IR_PIN (LED IR langsung + resistor 220Ω)
- GPIO 15: PROXIMITY_OUT (Sensor Keluar)
- GPIO 27: DHT22 Data
- GPIO 34: LDR Analog

## CATATAN PENTING
- Semua pin sudah dipastikan tidak ada yang sama
- Sensor proximity menggunakan digital output (HIGH/LOW)
- Kebanyakan sensor proximity: LOW = Object detected, HIGH = No object
- Pin analog 34 khusus untuk LDR
- **LED IR menggunakan resistor 220Ω untuk membatasi arus**
- **LED IR: Anoda ke GPIO 14 (+ resistor), Katoda ke GND**

## TIPE SENSOR PROXIMITY YANG DIDUKUNG

### 1. IR Proximity Sensor (Infrared)
- **Voltage**: 3.3V - 5V
- **Output**: Digital (LOW = detected, HIGH = clear)
- **Range**: 2-30cm (adjustable dengan potensiometer)
- **Contoh**: FC-51, TCRT5000

### 2. Ultrasonic Proximity (HC-SR04 mode proximity)
- **Voltage**: 5V
- **Output**: Digital (dapat diprogram untuk proximity mode)
- **Range**: 2-400cm

### 3. Capacitive Proximity Sensor
- **Voltage**: 3.3V - 5V
- **Output**: Digital (LOW = detected, HIGH = clear)
- **Range**: 1-10cm
- **Deteksi**: Metal dan non-metal objects

## WIRING DIAGRAM SENSOR PROXIMITY

```
Sensor MASUK (GPIO 2):
VCC ---- 3.3V/5V ESP32
GND ---- GND ESP32
OUT ---- GPIO 2 ESP32

Sensor KELUAR (GPIO 15):
VCC ---- 3.3V/5V ESP32
GND ---- GND ESP32
OUT ---- GPIO 15 ESP32
```

## WIRING DIAGRAM LED IR

```
ESP32 GPIO 14 ----[220Ω Resistor]----[LED IR Anoda(+)]
                                           |
                                      [LED IR Katoda(-)]
                                           |
ESP32 GND --------------------------------+
```

## SPESIFIKASI LED IR
- **Tipe**: LED IR 5mm atau 3mm
- **Wavelength**: 940nm (standar untuk AC remote)
- **Forward Voltage**: ~1.2V - 1.4V
- **Forward Current**: 20mA (dengan resistor 220Ω)
- **Resistor**: 220Ω (wajib untuk membatasi arus)

## PERHITUNGAN RESISTOR
```
Voltage ESP32 = 3.3V
LED Forward Voltage = 1.3V
LED Forward Current = 20mA

Resistor = (3.3V - 1.3V) / 0.02A = 100Ω minimum
Gunakan 220Ω untuk safety margin
```

## TIPS PEMASANGAN SENSOR PROXIMITY
1. **Posisi**: Pasang sensor MASUK di sisi luar pintu, sensor KELUAR di sisi dalam
2. **Ketinggian**: Pasang setinggi dada orang dewasa (~120-150cm)
3. **Jarak**: Atur sensitivitas agar mendeteksi orang pada jarak 10-30cm
4. **Kalibrasi**: Gunakan potensiometer pada sensor untuk mengatur jarak deteksi
5. **Testing**: Test dengan berjalan masuk/keluar untuk memastikan deteksi akurat

## TIPS PEMASANGAN LED IR
1. **Polaritas**: Pastikan anoda (+) ke GPIO 14, katoda (-) ke GND
2. **Jarak**: Letakkan LED IR menghadap ke AC unit (jarak 1-3 meter)
3. **Sudut**: LED IR memiliki beam angle ~30°, arahkan tepat ke receiver AC
4. **Testing**: Gunakan kamera HP untuk melihat cahaya IR (tampak ungu di kamera)
5. **Resistor**: Wajib pakai resistor 220Ω untuk melindungi LED dan ESP32