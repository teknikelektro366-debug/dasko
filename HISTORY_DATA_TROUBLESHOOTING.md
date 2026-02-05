# TROUBLESHOOTING: Data History Tidak Tercatat

## Masalah
Data history tidak tercatat di dashboard, padahal sistem terlihat berjalan normal.

## Langkah Troubleshooting

### 1. Cek Database Langsung
Jalankan query SQL ini di database untuk melihat apakah data benar-benar masuk:

```sql
-- Cek total records
SELECT COUNT(*) as total_records FROM sensor_data;

-- Cek 10 record terbaru
SELECT id, device_id, people_count, ac_status, created_at 
FROM sensor_data 
ORDER BY created_at DESC 
LIMIT 10;

-- Cek record hari ini
SELECT COUNT(*) as today_records 
FROM sensor_data 
WHERE DATE(created_at) = CURDATE();

-- Cek record 1 jam terakhir
SELECT COUNT(*) as last_hour_records 
FROM sensor_data 
WHERE created_at >= DATE_SUB(NOW(), INTERVAL 1 HOUR);
```

### 2. Test API Endpoints
Buka file `test_api_endpoints.html` di browser dan jalankan semua test:

1. **Test History API** - `/api/sensor/history`
2. **Test Data API** - `/api/sensor/data` 
3. **Test Latest API** - `/api/sensor/latest`
4. **Test Send Data** - Simulasi pengiriman dari ESP32
5. **Test Multiple Send** - Kirim 5 data berturut-turut

### 3. Cek Log Laravel
Periksa log Laravel untuk melihat apakah ada error:

```bash
# Di terminal/command prompt
tail -f storage/logs/laravel.log
```

Atau buka file `storage/logs/laravel.log` dan cari error terbaru.

### 4. Cek Serial Monitor Arduino
Buka Serial Monitor di Arduino IDE dan lihat apakah:

- ✅ WiFi terhubung dengan baik
- ✅ Data sensor terbaca dengan benar  
- ✅ API request berhasil dikirim
- ✅ Response dari server menunjukkan sukses

Contoh output yang benar:
```
📤 Sending proximity data to hosting web (dasko.fst.unja.ac.id)...
   Reason: ULTRA FAST Proximity entry - Count: 1
   People Count: 1
   AC Status: 1 AC ON
✅ SUCCESS: Proximity data sent to hosting web!
✅ Laravel saved with ID: 123
```

### 5. Test Manual API Call
Gunakan Postman atau curl untuk test manual:

```bash
curl -X POST http://localhost/api/sensor/data \
  -H "Content-Type: application/json" \
  -H "Accept: application/json" \
  -d '{
    "device_id": "ESP32_Manual_Test",
    "location": "Manual Test Location", 
    "people_count": 3,
    "ac_status": "1 AC ON",
    "set_temperature": 25,
    "room_temperature": 28.5,
    "humidity": 65.0,
    "light_level": 300,
    "proximity_in": true,
    "proximity_out": false,
    "wifi_rssi": -45,
    "status": "active"
  }'
```

## Kemungkinan Penyebab & Solusi

### 1. Database Connection Issue
**Gejala**: API error, data tidak masuk database
**Solusi**: 
- Cek file `.env` untuk konfigurasi database
- Pastikan MySQL/database server berjalan
- Test koneksi database

### 2. Arduino Tidak Mengirim Data
**Gejala**: Tidak ada log pengiriman di Serial Monitor
**Solusi**:
- Cek koneksi WiFi Arduino
- Pastikan URL API benar: `https://dasko.fst.unja.ac.id/api/sensor/data`
- Cek sertifikat SSL/HTTPS

### 3. API Validation Error
**Gejala**: Arduino mengirim data tapi gagal validasi
**Solusi**:
- Cek format data JSON yang dikirim Arduino
- Pastikan semua field required ada
- Cek tipe data sesuai validasi

### 4. Dashboard Tidak Load Data
**Gejala**: Data ada di database tapi tidak muncul di dashboard
**Solusi**:
- Cek JavaScript console untuk error
- Test API endpoint `/api/sensor/history` langsung
- Pastikan tidak ada error di controller

### 5. Timing Issue
**Gejala**: Data kadang masuk kadang tidak
**Solusi**:
- Cek apakah Arduino mengirim data hanya saat ada perubahan
- Pastikan tidak ada race condition
- Cek network stability

## Script Debug

### A. Debug Database (PHP)
```bash
php debug_history_data.php
```

### B. Debug API (HTML)
Buka `test_api_endpoints.html` di browser

### C. Debug Arduino
Upload kode Arduino dan monitor Serial Monitor untuk melihat:
- Koneksi WiFi
- Pembacaan sensor
- Pengiriman data API
- Response dari server

## Checklist Troubleshooting

- [ ] Database berjalan dan dapat diakses
- [ ] Tabel `sensor_data` ada dan struktur benar
- [ ] API endpoints dapat diakses (test dengan browser/Postman)
- [ ] Arduino terhubung WiFi dengan baik
- [ ] Arduino mengirim data ke URL yang benar
- [ ] Data JSON dari Arduino valid
- [ ] Laravel log tidak menunjukkan error
- [ ] Dashboard JavaScript tidak ada error
- [ ] Browser dapat mengakses API endpoints

## Langkah Selanjutnya

1. **Jalankan semua test di atas**
2. **Catat hasil setiap test**
3. **Identifikasi di mana masalahnya**
4. **Terapkan solusi yang sesuai**
5. **Test ulang untuk memastikan fix berhasil**

---

**Catatan**: Jika semua test menunjukkan hasil normal tapi history masih tidak muncul, kemungkinan ada masalah di frontend JavaScript atau caching browser. Coba hard refresh (Ctrl+F5) atau buka di incognito mode.