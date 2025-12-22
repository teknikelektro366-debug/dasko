# Arduino ESP32 to Laravel API Integration Guide

## 📋 Overview
Panduan lengkap untuk mengintegrasikan Arduino ESP32 dengan Laravel API untuk sistem Smart Energy. ESP32 akan mengirim data sensor secara realtime ke Laravel tanpa HTML dashboard.

## 🔧 Konfigurasi Arduino

### 1. Update Konfigurasi WiFi dan API
Buka file `INIFULL_clean_api_only/INIFULL_clean_api_only.ino` dan ubah konfigurasi berikut:

```cpp
// ================= CONFIGURATION =================
// WiFi Configuration
const char* ssid = "NAMA_WIFI_ANDA";           // ← GANTI DENGAN NAMA WIFI ANDA
const char* password = "PASSWORD_WIFI_ANDA";    // ← GANTI DENGAN PASSWORD WIFI ANDA

// Laravel API Configuration
const char* apiURL = "http://192.168.1.100:8000/api/sensor/data";  // ← GANTI DENGAN IP LARAVEL SERVER ANDA
```

### 2. Cara Mendapatkan IP Laravel Server
Jalankan perintah berikut di terminal Laravel:

```bash
# Windows
ipconfig

# Linux/Mac
ifconfig
```

Cari IP address yang dimulai dengan `192.168.x.x` atau `10.x.x.x`

### 3. Library yang Diperlukan
Pastikan library berikut sudah terinstall di Arduino IDE:

- **TFT_eSPI** (by Bodmer)
- **DHT sensor library** (by Adafruit)
- **ArduinoJson** (by Benoit Blanchon)
- **IRremoteESP8266** (by David Conran)
- **WiFi** (built-in ESP32)
- **HTTPClient** (built-in ESP32)

## 🚀 Setup Laravel API

### 1. Pastikan API Routes Aktif
File `routes/api.php` sudah dikonfigurasi dengan endpoint:
```php
Route::prefix('sensor')->group(function () {
    Route::post('/data', [SensorDataController::class, 'store']);
    Route::get('/latest', [SensorDataController::class, 'latest']);
});
```

### 2. Jalankan Laravel Server
```bash
# Jalankan migration jika belum
php artisan migrate

# Start Laravel server
php artisan serve --host=0.0.0.0 --port=8000
```

**PENTING**: Gunakan `--host=0.0.0.0` agar server bisa diakses dari ESP32 di jaringan yang sama.

### 3. Test API Endpoint
Buka browser dan akses:
```
http://IP_LARAVEL_SERVER:8000/api/sensor/latest
```

Jika berhasil, akan muncul response JSON.

## 📡 Testing Koneksi

### 1. Upload Code ke ESP32
1. Buka Arduino IDE
2. Load file `INIFULL_clean_api_only/INIFULL_clean_api_only.ino`
3. Pilih board ESP32 Dev Module
4. Upload ke ESP32

### 2. Monitor Serial Output
Buka Serial Monitor (115200 baud) untuk melihat:
```
=== SMART ENERGY SYSTEM - API ONLY VERSION ===
Focus: Send data to Laravel API only
Connecting to WiFi: LAB TEKNIK TEGANGAN TINGGI
✓ WiFi connected successfully!
✓ IP Address: 192.168.1.105
✓ Signal Strength: -45 dBm
✓ API Endpoint: http://192.168.1.100:8000/api/sensor/data
SYSTEM READY - Sending data to Laravel API
```

### 3. Verifikasi Data Diterima Laravel
Check Laravel logs:
```bash
tail -f storage/logs/laravel.log
```

Atau check database:
```bash
php artisan tinker
>>> App\Models\SensorData::latest()->first()
```

## 🔍 Troubleshooting

### Problem: WiFi Tidak Terkoneksi
**Solusi:**
1. Pastikan SSID dan password benar
2. Pastikan ESP32 dalam jangkauan WiFi
3. Restart ESP32 dan router

### Problem: API Error 404
**Solusi:**
1. Pastikan Laravel server berjalan
2. Check IP address Laravel server
3. Pastikan endpoint `/api/sensor/data` tersedia

### Problem: API Error 422 (Validation Failed)
**Solusi:**
1. Check format data JSON yang dikirim ESP32
2. Pastikan semua field required ada
3. Check Laravel validation rules

### Problem: Data Tidak Masuk Database
**Solusi:**
1. Check Laravel logs: `tail -f storage/logs/laravel.log`
2. Pastikan database connection aktif
3. Run migration: `php artisan migrate`

## 📊 Monitoring Data

### 1. Via Laravel Dashboard
Akses dashboard Laravel untuk melihat data realtime:
```
http://IP_LARAVEL_SERVER:8000/dashboard
```

### 2. Via API Endpoints
```bash
# Data terbaru
curl http://IP_LARAVEL_SERVER:8000/api/sensor/latest

# Data chart (24 jam terakhir)
curl http://IP_LARAVEL_SERVER:8000/api/sensor/chart

# Statistik harian
curl http://IP_LARAVEL_SERVER:8000/api/sensor/stats
```

### 3. Via Database
```sql
SELECT * FROM sensor_data ORDER BY created_at DESC LIMIT 10;
```

## 📈 Data Format

ESP32 mengirim data dalam format JSON:
```json
{
  "device_id": "ESP32_Smart_Energy",
  "location": "Lab Teknik Tegangan Tinggi",
  "people_count": 5,
  "ac_status": "1 AC ON",
  "set_temperature": 25,
  "room_temperature": 28.5,
  "humidity": 65.2,
  "light_level": 450,
  "proximity_in": false,
  "proximity_out": true,
  "wifi_rssi": -45,
  "uptime": 12345,
  "free_heap": 234567,
  "timestamp": 1640995200000,
  "status": "active"
}
```

## 🔄 Automatic Features

### 1. Auto Reconnect WiFi
ESP32 akan otomatis reconnect jika WiFi terputus.

### 2. Data Backup
Jika API utama gagal, data akan dikirim ke backup server.

### 3. Error Handling
Semua error akan ditampilkan di Serial Monitor untuk debugging.

### 4. Data Validation
Laravel akan memvalidasi semua data sebelum disimpan ke database.

## ⚡ Performance Tips

1. **Interval Pengiriman**: Default 5 detik, bisa diubah di `API_UPDATE_INTERVAL`
2. **WiFi Signal**: Pastikan signal strength > -70 dBm
3. **Memory Usage**: Monitor free heap di Serial Monitor
4. **Database Cleanup**: Jalankan cleanup rutin untuk data lama

## 🎯 Next Steps

1. **Test semua sensor** (proximity, DHT22, LDR)
2. **Verifikasi AC control** via IR
3. **Setup dashboard Laravel** untuk monitoring
4. **Configure alerts** untuk kondisi abnormal
5. **Setup backup system** untuk data penting

## 📞 Support

Jika ada masalah:
1. Check Serial Monitor ESP32
2. Check Laravel logs
3. Verify network connectivity
4. Test API endpoints manually

---
**Status**: ✅ Ready for Production
**Last Updated**: December 22, 2025