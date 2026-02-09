# Perbaikan Kontrol Lampu Otomatis

## Masalah yang Ditemukan

Dari screenshot yang diberikan, terlihat bahwa:
- Ada **1 orang** di ruangan
- Status lampu masih **OFF** (seharusnya ON)

## Penyebab Masalah

1. **Relay tidak diupdate saat pertama kali dipanggil** - Fungsi `kontrolLampu()` hanya mengupdate relay jika ada perubahan status, tapi saat startup, variabel `lastLamp1` sudah diset ke `false` sehingga tidak ada perubahan yang terdeteksi.

2. **Tidak ada inisialisasi lampu di setup()** - Setelah sistem startup, fungsi `kontrolLampu()` tidak dipanggil untuk menginisialisasi status lampu berdasarkan jumlah orang saat itu.

## Perbaikan yang Dilakukan

### 1. Menambahkan Flag `firstCall` di `kontrolLampu()`

```cpp
void kontrolLampu(String &lampStatus) {
  bool lampON = false;
  
  if (jumlahOrang > 0) {
    lampON = true;
    lampStatus = "ON";
  } else {
    lampON = false;
    lampStatus = "OFF";
  }
  
  // Update relay jika ada perubahan ATAU jika ini adalah panggilan pertama
  static bool firstCall = true;
  if (lampON != lastLamp1 || firstCall) {
    // Set relay - LOW = ON, HIGH = OFF
    digitalWrite(RELAY_LAMP1, lampON ? LOW : HIGH);
    
    lastLamp1 = lampON;
    lastLampStatus = lampStatus;
    firstCall = false;  // Setelah panggilan pertama, flag diset false
  }
  
  currentData.lamp1 = lampON;
  currentData.lampStatus = lampStatus;
}
```

**Penjelasan:**
- Variabel `firstCall` diinisialisasi sebagai `true`
- Saat pertama kali dipanggil, relay akan diupdate meskipun tidak ada perubahan
- Setelah panggilan pertama, `firstCall` diset ke `false` sehingga relay hanya diupdate saat ada perubahan

### 2. Menambahkan Inisialisasi Lampu di `setup()`

```cpp
void setup() {
  // ... kode lainnya ...
  
  // Reset people counter to ensure clean start
  jumlahOrang = 0;
  lastJumlahOrang = -1;
  Serial.println("✓ People counter reset to 0");
  
  // Initialize lamp control based on current people count
  String lampStatus = "OFF";
  kontrolLampu(lampStatus);
  currentData.lampStatus = lampStatus;
  Serial.println("✓ Lamp control initialized: " + lampStatus);
}
```

**Penjelasan:**
- Setelah sistem startup, `kontrolLampu()` dipanggil untuk menginisialisasi status lampu
- Jika ada orang di ruangan saat startup, lampu akan langsung ON
- Jika tidak ada orang, lampu akan OFF

## Logika Kontrol Lampu

### Kondisi ON
```
jumlahOrang > 0  →  Lampu ON  →  Relay GPIO 25 = LOW
```

### Kondisi OFF
```
jumlahOrang = 0  →  Lampu OFF  →  Relay GPIO 25 = HIGH
```

## Cara Kerja Relay

- **GPIO 25 = LOW** → Relay ON → Lampu NYALA
- **GPIO 25 = HIGH** → Relay OFF → Lampu MATI

## Frekuensi Update

Fungsi `kontrolLampu()` dipanggil di 3 tempat:

1. **Saat interrupt triggered** (deteksi orang masuk/keluar)
   - Langsung update saat ada perubahan jumlah orang
   - Paling responsif

2. **Saat regular update** (setiap loop)
   - Backup jika interrupt tidak trigger
   - Memastikan status selalu sinkron

3. **Setiap 3 detik** (light control check)
   - Periodic check untuk memastikan relay dalam kondisi benar
   - Mencegah relay stuck

## Testing

### 1. Upload Kode ke ESP32
```bash
# Di Arduino IDE:
1. Buka file HOSTING.ino
2. Pilih board: ESP32 Dev Module
3. Pilih port COM yang sesuai
4. Klik Upload
```

### 2. Monitor Serial Output
```bash
# Buka Serial Monitor (Ctrl+Shift+M)
# Baud rate: 115200

# Output yang diharapkan saat startup:
✓ People counter reset to 0
✓ Lamp control initialized: OFF

# Output saat ada orang masuk:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
💡 LAMP CONTROL CHANGE DETECTED!
   Jumlah orang: 1
   Status baru: ON
   Relay GPIO 25: LOW (ON)
✅ Relay updated successfully!
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

# Output saat orang keluar:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
💡 LAMP CONTROL CHANGE DETECTED!
   Jumlah orang: 0
   Status baru: OFF
   Relay GPIO 25: HIGH (OFF)
✅ Relay updated successfully!
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

### 3. Cek Dashboard
```bash
# Buka dashboard di browser:
http://localhost:8000

# Atau di production:
https://dasko.fst.unja.ac.id

# Cek bagian "Status Lampu":
- Jika ada orang → Status: ON (warna hijau/kuning)
- Jika tidak ada orang → Status: OFF (warna abu-abu)
```

## Troubleshooting

### Lampu Tidak Menyala Meskipun Ada Orang

**Kemungkinan penyebab:**
1. Relay tidak terhubung dengan benar
2. Kabel relay tertukar (NO/NC)
3. Power supply relay tidak cukup

**Solusi:**
```cpp
// Test manual relay di Serial Monitor:
// Ketik: test relay

// Atau tambahkan di loop() untuk test:
digitalWrite(RELAY_LAMP1, LOW);  // Force ON
delay(2000);
digitalWrite(RELAY_LAMP1, HIGH); // Force OFF
delay(2000);
```

### Status Lampu di Dashboard Tidak Update

**Kemungkinan penyebab:**
1. Data tidak terkirim ke database
2. Kolom `lamp_status` tidak ada di database

**Solusi:**
```bash
# Cek database:
php artisan tinker
>>> \App\Models\SensorData::latest()->first()->lamp_status

# Jika kolom tidak ada, jalankan migration:
php artisan migrate

# Cek log Laravel:
tail -f storage/logs/laravel.log
```

### Relay Berkedip-kedip (Flickering)

**Kemungkinan penyebab:**
1. Sensor proximity terlalu sensitif
2. Noise pada deteksi

**Solusi:**
```cpp
// Sudah ada di kode:
#define NOISE_FILTER_TIME 30      // 30ms filter untuk noise
#define PERSON_COOLDOWN 200       // 200ms cooldown
#define DEBOUNCE_DELAY 50         // 50ms debounce

// Jika masih berkedip, tingkatkan nilai:
#define PERSON_COOLDOWN 500       // 500ms cooldown
```

## Verifikasi Perbaikan

Setelah upload kode baru:

✅ **Saat startup dengan 0 orang:**
- Lampu OFF
- Dashboard menampilkan "Status Lampu: OFF"

✅ **Saat 1 orang masuk:**
- Lampu ON dalam < 1 detik
- Dashboard update ke "Status Lampu: ON"
- Serial monitor menampilkan log perubahan

✅ **Saat orang keluar:**
- Lampu OFF dalam < 1 detik
- Dashboard update ke "Status Lampu: OFF"
- Serial monitor menampilkan log perubahan

## File yang Dimodifikasi

- `HOSTING (1)/HOSTING/HOSTING.ino`
  - Fungsi `kontrolLampu()` - Menambahkan flag `firstCall`
  - Fungsi `setup()` - Menambahkan inisialisasi lampu

## Status

✅ Perbaikan selesai
✅ Kode siap di-upload ke ESP32
⏳ Menunggu testing di hardware

## Catatan Penting

1. **Jangan lupa upload kode ke ESP32** - Perbaikan hanya berlaku setelah kode di-upload
2. **Monitor Serial Output** - Untuk memastikan relay bekerja dengan benar
3. **Cek wiring relay** - Pastikan relay terhubung ke GPIO 25 dan power supply cukup
4. **Test dengan orang masuk/keluar** - Pastikan deteksi proximity bekerja dengan baik
