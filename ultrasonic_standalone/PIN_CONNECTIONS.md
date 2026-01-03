# PIN CONNECTIONS - ULTRASONIC STANDALONE

## SENSOR ULTRASONIC HC-SR04

### Sensor MASUK (Sisi Luar Pintu)
- VCC → 5V
- GND → GND
- TRIG → GPIO 2
- ECHO → GPIO 15

### Sensor KELUAR (Sisi Dalam Pintu)
- VCC → 5V
- GND → GND
- TRIG → GPIO 13
- ECHO → GPIO 12

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
- GPIO 2: TRIG_IN (Ultrasonic Masuk)
- GPIO 12: ECHO_OUT (Ultrasonic Keluar)
- GPIO 13: TRIG_OUT (Ultrasonic Keluar)
- GPIO 14: IR_PIN (LED IR langsung + resistor 220Ω)
- GPIO 15: ECHO_IN (Ultrasonic Masuk)
- GPIO 27: DHT22 Data
- GPIO 34: LDR Analog

## CATATAN PENTING
- **Mode Standalone**: Tidak memerlukan WiFi/Internet
- **Pin configuration**: Sesuai ESP32 Smart Energy Production
- Sensor ultrasonic menggunakan 5V, sensor lain 3.3V
- Pin analog 34 khusus untuk LDR
- **LED IR menggunakan resistor 220Ω untuk membatasi arus**
- **LED IR: Anoda ke GPIO 14 (+ resistor), Katoda ke GND**

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

## TIPS PEMASANGAN LED IR
1. **Polaritas**: Pastikan anoda (+) ke GPIO 14, katoda (-) ke GND
2. **Jarak**: Letakkan LED IR menghadap ke AC unit (jarak 1-3 meter)
3. **Sudut**: LED IR memiliki beam angle ~30°, arahkan tepat ke receiver AC
4. **Testing**: Gunakan kamera HP untuk melihat cahaya IR (tampak ungu di kamera)
5. **Resistor**: Wajib pakai resistor 220Ω untuk melindungi LED dan ESP32

## FITUR STANDALONE
- ✅ **Tidak perlu WiFi/Internet**
- ✅ **Ultra fast detection** (300ms cooldown)
- ✅ **Auto AC control** berdasarkan jumlah orang
- ✅ **TFT display** untuk monitoring
- ✅ **Serial commands** untuk kontrol manual
- ✅ **Range detection** 1-15cm optimal
- ✅ **Dual AC support** (AC1 & AC2)

## SERIAL COMMANDS
Ketik di Serial Monitor:
- `STATUS` - Tampilkan status sistem
- `RESET` - Reset jumlah orang ke 0
- `SET <angka>` - Set jumlah orang (0-20)
- `HELP` - Tampilkan bantuan

## AC CONTROL LOGIC
- **0 orang**: AC OFF
- **1-5 orang**: 1 AC ON @ 25°C
- **6-10 orang**: 1 AC ON @ 22°C
- **11-15 orang**: 2 AC ON @ 22°C
- **16-20 orang**: 2 AC MAX @ 20°C
</content>