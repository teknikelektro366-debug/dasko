# ENHANCEMENT: Lamp Status Display & Single Relay

## Perubahan yang Dilakukan

### 1. ✅ Arduino - Ubah dari 2 Relay ke 1 Relay
**Perubahan**:
- Hapus `lamp2Status` dan `RELAY_LAMP2`
- Gunakan hanya `lampStatus` dan `RELAY_LAMP1` (GPIO 25)
- Update semua fungsi untuk menggunakan 1 relay saja
- 12 lampu TL dikontrol dengan 1 relay

**File**: `InsyaallahIniBisa/InsyaallahIniBisa.ino`

### 2. ✅ Arduino - Tambah Field lamp_status ke API
**Perubahan**:
- Tambah `doc["lamp_status"] = lampStatus ? "ON" : "OFF";` di `sendDataToAPI()`
- Arduino sekarang mengirim status lampu ke Laravel

### 3. ✅ Database - Tambah Kolom lamp_status
**Perubahan**:
- Buat migration untuk menambah kolom `lamp_status` di tabel `sensor_data`
- Kolom: `VARCHAR(10) DEFAULT 'OFF'`

**File**: `database/migrations/2026_02_05_103600_add_lamp_status_to_sensor_data_table.php`

### 4. ✅ Laravel Model - Update SensorData
**Perubahan**:
- Tambah `lamp_status` ke `$fillable` array
- Model sekarang bisa menerima dan menyimpan status lampu

**File**: `app/Models/SensorData.php`

### 5. ✅ Laravel Controller - Update API
**Perubahan**:
- Tambah validasi untuk `lamp_status`
- Simpan `lamp_status` ke database
- Return `lamp_status` dalam response API

**File**: `app/Http/Controllers/Api/SensorDataController.php`

### 6. ✅ Dashboard - Tambah Card Status Lampu
**Perubahan**:
- Tambah card "Status Lampu" di dashboard monitoring
- Menampilkan ON (kuning) atau OFF (abu-abu)
- Update real-time dari data sensor

**File**: `resources/views/dashboard.blade.php`

### 7. ✅ Dashboard - Tambah Kolom Lampu di History
**Perubahan**:
- Tambah kolom "Lampu" di tabel history
- Menampilkan status ON/OFF dengan warna
- Update colspan dari 11 ke 12 untuk semua pesan loading/error

## Hasil Akhir

### Dashboard Monitoring
```
┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐
│ Suhu Ruangan    │ │ Kelembaban      │ │ Jumlah Orang    │ │ Intensitas      │
│ 26.0°C          │ │ 60.0%           │ │ 1               │ │ Cahaya 9 lux    │
└─────────────────┘ └─────────────────┘ └─────────────────┘ └─────────────────┘

┌─────────────────┐
│ Status Lampu    │  ← BARU!
│ ON              │
└─────────────────┘
```

### History Table
```
| ID | Timestamp | Device | Orang | Suhu | Kelembaban | Cahaya | Lampu | AC Status | ... |
|----|-----------|--------|-------|------|------------|--------|-------|-----------|-----|
| 1  | 10:30:15  | ESP32  | 1     | 26°C | 60%        | 9 lux  | ON    | 1 AC ON   | ... |
| 2  | 10:29:45  | ESP32  | 0     | 26°C | 60%        | 9 lux  | OFF   | AC OFF    | ... |
```

## Konfigurasi Hardware

### Pin Configuration (1 Relay)
```cpp
#define RELAY_LAMP1       25    // Relay untuk lampu (12 lampu TL)
// RELAY_LAMP2 dihapus - tidak digunakan lagi
```

### Wiring
```
ESP32 GPIO 25 → Relay Module IN
Relay Module:
- COM → Power Supply +
- NO  → 12 Lampu TL +
- NC  → (tidak digunakan)
```

## Smart Lamp Control Logic

### Kondisi Lampu ON
1. **Ada orang** (people_count > 0) DAN
2. **Cahaya rendah** (light_level < 200 lux) ATAU
3. **Banyak orang** (people_count >= 3) dengan cahaya sedang

### Kondisi Lampu OFF
1. **Tidak ada orang** selama 5 menit (auto shutdown)
2. **Cahaya cukup terang** (light_level >= 400 lux)

### Auto Shutdown
- Lampu mati otomatis setelah 5 menit jika tidak ada orang
- Timer direset jika ada orang masuk

## Testing

### 1. Test Arduino
```bash
# Upload kode Arduino baru
# Cek Serial Monitor:
# - Status lampu ON/OFF
# - Pengiriman data ke API dengan lamp_status
```

### 2. Test Database
```sql
-- Cek kolom baru
DESCRIBE sensor_data;

-- Cek data lamp_status
SELECT id, people_count, lamp_status, created_at 
FROM sensor_data 
ORDER BY created_at DESC 
LIMIT 5;
```

### 3. Test Dashboard
```bash
# Buka dashboard
# Cek card "Status Lampu" muncul
# Buka History tab
# Cek kolom "Lampu" ada dan menampilkan ON/OFF
```

## API Response Baru

### Sebelum
```json
{
  "device_id": "ESP32_UNJA",
  "people_count": 1,
  "light_level": 9,
  "ac_status": "1 AC ON"
}
```

### Sesudah
```json
{
  "device_id": "ESP32_UNJA", 
  "people_count": 1,
  "light_level": 9,
  "lamp_status": "ON",        ← BARU!
  "ac_status": "1 AC ON"
}
```

## Troubleshooting

### Jika Status Lampu Tidak Muncul
1. **Cek Arduino**: Pastikan `lampStatus` variable ter-update
2. **Cek API**: Pastikan `lamp_status` dikirim dalam JSON
3. **Cek Database**: Pastikan kolom `lamp_status` ada dan terisi
4. **Cek Dashboard**: Hard refresh browser (Ctrl+F5)

### Jika Relay Tidak Bekerja
1. **Cek Wiring**: GPIO 25 ke Relay IN
2. **Cek Power**: Relay module dapat power 5V/3.3V
3. **Cek Code**: `digitalWrite(RELAY_LAMP1, HIGH/LOW)`

---

**Status**: ✅ SELESAI - Sistem lampu 1 relay dengan status display
**Hardware**: 1 Relay mengontrol 12 lampu TL
**Dashboard**: Card status lampu + kolom history lampu
**API**: Mengirim dan menyimpan lamp_status