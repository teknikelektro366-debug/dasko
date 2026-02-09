# Troubleshooting: Lampu Tidak Menyala

## Situasi Saat Ini
- Dashboard menampilkan: **1 orang di ruangan**
- Status lampu: **OFF** (seharusnya ON)
- Device: ESP32_Proximity_Production_UNJA

## Langkah Troubleshooting

### 1. Cek Apakah Kode Sudah Di-Upload
**PENTING:** Perbaikan kode hanya berlaku setelah di-upload ke ESP32!

```bash
# Di Arduino IDE:
1. Buka file: HOSTING (1)/HOSTING/HOSTING.ino
2. Pilih Board: ESP32 Dev Module
3. Pilih Port: COM yang sesuai (cek di Device Manager)
4. Klik Upload (Ctrl+U)
5. Tunggu sampai "Done uploading"
```

### 2. Buka Serial Monitor
```bash
# Di Arduino IDE:
1. Klik Tools → Serial Monitor (Ctrl+Shift+M)
2. Set baud rate: 115200
3. Lihat output saat startup
```

**Output yang diharapkan saat startup:**
```
✅ SYSTEM READY!
✓ People counter reset to 0
✓ Lamp control initialized: OFF

🔍 kontrolLampu() called:
   jumlahOrang = 0
   lastLamp1 = false
   → Decision: LAMP OFF (tidak ada orang)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
💡 LAMP CONTROL CHANGE DETECTED!
   Jumlah orang: 0
   Status baru: OFF
   Relay GPIO 25: HIGH (OFF)
   First call: YES
   Relay state verified: HIGH (OFF)
✅ Relay updated successfully!
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

### 3. Test Manual Relay di Serial Monitor

Ketik command berikut di Serial Monitor:

#### Test 1: Force Lamp ON
```
lampon
```
**Expected output:**
```
💡 FORCE LAMP ON
   Relay GPIO 25 set to LOW (ON)
   Verified state: LOW (ON)
```
**Cek:** Apakah lampu fisik menyala?
- ✅ Ya → Relay bekerja, masalah di logika kontrol
- ❌ Tidak → Masalah di wiring atau relay rusak

#### Test 2: Force Lamp OFF
```
lampoff
```
**Expected output:**
```
💡 FORCE LAMP OFF
   Relay GPIO 25 set to HIGH (OFF)
   Verified state: HIGH (OFF)
```
**Cek:** Apakah lampu fisik mati?

#### Test 3: Test Relay Cycle
```
lamptest
```
**Expected output:**
```
💡 LAMP RELAY TEST
   Testing relay ON/OFF cycle...
   → Setting relay ON (LOW)
   → State: LOW (ON) ✅
   → Setting relay OFF (HIGH)
   → State: HIGH (OFF) ✅
   Test complete!
```
**Cek:** Apakah lampu menyala 2 detik, lalu mati 2 detik?

### 4. Test Deteksi Orang

#### Cek Jumlah Orang Saat Ini
```
status
```
**Cari baris:**
```
People Count: X
```

#### Manual Add Person
```
add
```
**Expected output:**
```
➕ Manual add person
🔍 kontrolLampu() called:
   jumlahOrang = 1
   → Decision: LAMP ON (ada 1 orang)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
💡 LAMP CONTROL CHANGE DETECTED!
   Jumlah orang: 1
   Status baru: ON
   Relay GPIO 25: LOW (ON)
✅ Relay updated successfully!
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```
**Cek:** Apakah lampu menyala?

#### Manual Subtract Person
```
sub
```
**Expected output:**
```
➖ Manual subtract person
🔍 kontrolLampu() called:
   jumlahOrang = 0
   → Decision: LAMP OFF (tidak ada orang)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
💡 LAMP CONTROL CHANGE DETECTED!
   Jumlah orang: 0
   Status baru: OFF
   Relay GPIO 25: HIGH (OFF)
✅ Relay updated successfully!
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```
**Cek:** Apakah lampu mati?

### 5. Cek Wiring Relay

**Pin Configuration:**
```
ESP32 GPIO 25 → Relay IN
ESP32 GND     → Relay GND
ESP32 VCC     → Relay VCC (5V atau 3.3V sesuai relay)

Relay COM     → Lampu Phase/Live
Relay NO      → Tidak digunakan
Relay NC      → Tidak digunakan (atau ke saklar manual)
```

**Catatan Penting:**
- Relay LOW (0V) = ON → Lampu menyala
- Relay HIGH (3.3V) = OFF → Lampu mati

**Jika relay terbalik (NO/NC tertukar):**
```cpp
// Ubah di kode Arduino:
digitalWrite(RELAY_LAMP1, lampON ? HIGH : LOW);  // Terbalik
```

### 6. Cek Power Supply Relay

**Relay membutuhkan power yang cukup:**
- Relay 5V → Gunakan VIN (5V) dari ESP32
- Relay 3.3V → Gunakan 3.3V dari ESP32

**Jika relay tidak klik:**
- Power supply tidak cukup
- Gunakan external power supply 5V
- Atau gunakan transistor/MOSFET untuk drive relay

### 7. Cek Database

**Apakah data terkirim ke database?**
```bash
# Di terminal Laravel:
php artisan tinker

# Cek data terakhir:
>>> $data = \App\Models\SensorData::latest()->first();
>>> $data->people_count
>>> $data->lamp_status
>>> $data->created_at
```

**Expected:**
- `people_count` = 1
- `lamp_status` = "ON"
- `created_at` = waktu terbaru

**Jika `lamp_status` = "OFF" meskipun `people_count` = 1:**
- Masalah di Arduino, bukan di dashboard
- Fungsi `kontrolLampu()` tidak dipanggil atau tidak bekerja

## Kemungkinan Masalah

### A. Kode Belum Di-Upload
**Solusi:** Upload kode Arduino yang sudah diperbaiki

### B. Relay Tidak Terhubung
**Solusi:** Cek wiring, pastikan GPIO 25 terhubung ke Relay IN

### C. Relay Rusak
**Solusi:** Test dengan multimeter atau ganti relay

### D. Power Supply Tidak Cukup
**Solusi:** Gunakan external power supply 5V untuk relay

### E. Logika Terbalik (NO/NC)
**Solusi:** Tukar kabel relay atau ubah kode (HIGH/LOW)

### F. Fungsi kontrolLampu() Tidak Dipanggil
**Solusi:** Cek Serial Monitor, pastikan ada log "kontrolLampu() called"

### G. Variable jumlahOrang Tidak Update
**Solusi:** Cek proximity sensor, test dengan command "add"

## Diagnostic Commands

```bash
# Cek semua sensor
sensors

# Cek status sistem
status

# Test relay
lamptest

# Force lamp ON
lampon

# Force lamp OFF
lampoff

# Add person manually
add

# Subtract person manually
sub

# Reset counter
reset

# Show help
help
```

## Expected Behavior

### Saat Ada Orang Masuk:
1. Proximity IN sensor detect
2. `jumlahOrang++` (increment)
3. `kontrolLampu()` dipanggil
4. Relay GPIO 25 = LOW
5. Lampu menyala
6. Dashboard update: "Status Lampu: ON"

### Saat Orang Keluar:
1. Proximity OUT sensor detect
2. `jumlahOrang--` (decrement)
3. `kontrolLampu()` dipanggil
4. Relay GPIO 25 = HIGH
5. Lampu mati
6. Dashboard update: "Status Lampu: OFF"

## Next Steps

1. **Upload kode Arduino** yang sudah diperbaiki
2. **Buka Serial Monitor** dan lihat output
3. **Test dengan command** `lampon` dan `lampoff`
4. **Jika relay bekerja**, test dengan `add` dan `sub`
5. **Jika masih tidak bekerja**, kirim screenshot Serial Monitor output

## Screenshot yang Dibutuhkan

Untuk troubleshooting lebih lanjut, kirimkan screenshot:
1. Serial Monitor saat startup
2. Serial Monitor saat ketik `status`
3. Serial Monitor saat ketik `lamptest`
4. Serial Monitor saat ketik `add`
5. Foto wiring relay (jika memungkinkan)
