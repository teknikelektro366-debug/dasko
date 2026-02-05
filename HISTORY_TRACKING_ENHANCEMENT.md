# ENHANCEMENT: History Data Tracking

## Masalah yang Dilaporkan
Data history tidak tercatat di dashboard, padahal sistem terlihat berjalan normal.

## Langkah-Langkah Debug & Perbaikan

### 1. Debug Cepat - Buka Browser Console
1. **Buka dashboard** di browser
2. **Tekan F12** untuk membuka Developer Tools
3. **Pilih tab Console**
4. **Copy-paste script ini** dan tekan Enter:

```javascript
// Quick debug script
fetch('/api/sensor/history?per_page=5')
  .then(response => response.json())
  .then(data => {
    console.log('History API Response:', data);
    if (data.success && data.data && data.data.length > 0) {
      console.log('✅ Data ada di database:', data.data.length, 'records');
      console.log('📊 Sample data:', data.data[0]);
    } else {
      console.log('❌ Tidak ada data di database');
    }
  })
  .catch(error => console.error('❌ API Error:', error));
```

### 2. Test API Endpoints Langsung
Buka URL ini di browser untuk test manual:

- **History API**: `http://localhost/api/sensor/history?per_page=10`
- **Data API**: `http://localhost/api/sensor/data?per_page=10`  
- **Latest API**: `http://localhost/api/sensor/latest`

**Expected Response**:
```json
{
  "success": true,
  "data": [
    {
      "id": 123,
      "device_id": "ESP32_Proximity_Production_UNJA_ULTRA_FAST",
      "people_count": 2,
      "ac_status": "1 AC ON",
      "created_at": "2026-02-04T10:30:00.000000Z"
    }
  ],
  "pagination": {
    "total": 50,
    "current_page": 1
  }
}
```

### 3. Cek Database Langsung
Jalankan query SQL ini di phpMyAdmin/database tool:

```sql
-- Cek apakah ada data
SELECT COUNT(*) as total FROM sensor_data;

-- Cek 5 record terbaru
SELECT id, device_id, people_count, ac_status, created_at 
FROM sensor_data 
ORDER BY created_at DESC 
LIMIT 5;

-- Cek data hari ini
SELECT COUNT(*) as today_count 
FROM sensor_data 
WHERE DATE(created_at) = CURDATE();
```

### 4. Test Pengiriman Data Manual
Buka file `test_api_endpoints.html` di browser dan:

1. **Klik "Kirim Test Data"** - untuk test single data
2. **Klik "Kirim 5 Data Berturut-turut"** - untuk test multiple data
3. **Lihat hasilnya** - apakah berhasil tersimpan

### 5. Cek Arduino Serial Monitor
Buka Serial Monitor Arduino IDE dan pastikan melihat:

```
📤 Sending proximity data to hosting web (dasko.fst.unja.ac.id)...
   Reason: ULTRA FAST Proximity entry - Count: 1
   People Count: 1
   AC Status: 1 AC ON
✅ SUCCESS: Proximity data sent to hosting web!
✅ Laravel saved with ID: 123
```

## Kemungkinan Masalah & Solusi

### Skenario 1: API Mengembalikan Data Kosong
**Gejala**: API response `{"success": true, "data": []}`
**Penyebab**: Tidak ada data di database
**Solusi**:
1. Cek apakah Arduino benar-benar mengirim data
2. Cek koneksi WiFi Arduino
3. Test manual data send dengan `test_api_endpoints.html`

### Skenario 2: API Error 500/404
**Gejala**: API response error atau tidak dapat diakses
**Penyebab**: Masalah server/routing
**Solusi**:
1. Cek file `routes/api.php` - pastikan route ada
2. Cek Laravel log di `storage/logs/laravel.log`
3. Pastikan web server berjalan

### Skenario 3: Data Ada di Database, Tapi Dashboard Kosong
**Gejala**: Database ada data, API return data, tapi dashboard history kosong
**Penyebab**: Masalah JavaScript frontend
**Solusi**:
1. Cek browser console untuk JavaScript errors
2. Hard refresh browser (Ctrl+F5)
3. Test di incognito mode
4. Cek apakah fungsi `loadSensorData()` dipanggil

### Skenario 4: Arduino Tidak Mengirim Data
**Gejala**: Serial Monitor tidak menunjukkan pengiriman data
**Penyebab**: Masalah koneksi atau konfigurasi Arduino
**Solusi**:
1. Cek koneksi WiFi: `WiFi.status() == WL_CONNECTED`
2. Cek URL API: `https://dasko.fst.unja.ac.id/api/sensor/data`
3. Cek apakah fungsi `sendDataToAPI()` dipanggil saat ada perubahan

## Script Debug Otomatis

### A. Debug Database (PHP)
```bash
php debug_history_data.php
```

### B. Debug Dashboard (JavaScript)
Copy script dari `debug_dashboard_history.js` ke browser console

### C. Debug API (HTML)
Buka `test_api_endpoints.html` di browser

## Perbaikan yang Sudah Diterapkan

### 1. ✅ API Controller - Always Create New Records
```php
// Setiap data dari ESP32 membuat record baru
$sensorData = SensorData::create([
    'device_id' => $deviceId,
    'location' => $location,
    'people_count' => $request->input('people_count'),
    // ... fields lainnya
]);
```

### 2. ✅ Arduino - Change-Based Updates
```cpp
// Kirim data hanya saat ada perubahan
if (jumlahOrang != lastJumlahOrang) {
    sendDataToAPI("Proximity entry - Count: " + String(jumlahOrang));
}
```

### 3. ✅ Dashboard - Stable Loading
```javascript
// Fungsi loadSensorData() sudah benar
function loadSensorData() {
    fetch('/api/sensor/history?per_page=50')
        .then(response => response.json())
        .then(data => {
            // Render data ke tabel
        });
}
```

## Checklist Troubleshooting

**Database & Backend**:
- [ ] Database server berjalan
- [ ] Tabel `sensor_data` ada dan dapat diakses
- [ ] API endpoints dapat diakses langsung
- [ ] Laravel log tidak ada error

**Arduino & Hardware**:
- [ ] Arduino terhubung WiFi
- [ ] Serial Monitor menunjukkan pengiriman data
- [ ] Response dari server sukses (status 200/201)
- [ ] Data JSON valid dan lengkap

**Frontend & Dashboard**:
- [ ] Browser console tidak ada error JavaScript
- [ ] Network tab menunjukkan API request berhasil
- [ ] Fungsi `loadSensorData()` dapat dipanggil
- [ ] DOM elements (tabel history) ada

## Langkah Selanjutnya

1. **Jalankan debug script** di browser console
2. **Test API endpoints** langsung di browser
3. **Cek database** untuk memastikan data tersimpan
4. **Monitor Arduino** Serial Monitor saat ada pergerakan
5. **Identifikasi** di mana masalahnya (database/API/frontend)
6. **Terapkan solusi** yang sesuai
7. **Test ulang** untuk memastikan fix berhasil

---

**Catatan Penting**: 
- Sistem dirancang untuk membuat record baru setiap kali ada perubahan people count
- Setiap gerakan (0→1→2→1→0) harus tercatat sebagai record terpisah
- Dashboard history harus menampilkan semua perubahan dengan timestamp

**Kontak Support**: Jika masalah masih berlanjut setelah mengikuti semua langkah di atas, berikan hasil dari semua test debug untuk analisis lebih lanjut.