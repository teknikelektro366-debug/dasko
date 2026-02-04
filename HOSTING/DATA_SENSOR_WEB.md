# DATA SENSOR YANG TERBACA DI WEB HOSTING

## ✅ **SEMUA DATA SENSOR YANG DIKIRIM KE WEB:**

### 🏢 **Informasi Device & Lokasi**
```json
{
  "device_id": "ESP32_Proximity_SSL_Proxy_UNJA_Prodi_Elektro",
  "location": "Ruang Prodi Dosen Teknik Elektro - UNJA - SSL + Proxy Detection",
  "sensor_type": "Enhanced_SSL_Proxy_Sensor"
}
```

### 👥 **Data Proximity Sensor (Deteksi Orang)**
```json
{
  "people_count": 5,           // Jumlah orang di ruangan (0-20)
  "proximity_in": true,        // Status sensor MASUK (true/false)
  "proximity_out": false       // Status sensor KELUAR (true/false)
}
```

### 🌡️ **Data Sensor Suhu & Kelembaban (DHT22)**
```json
{
  "room_temperature": 25.5,    // Suhu ruangan dalam °C
  "humidity": 65.2             // Kelembaban dalam %
}
```

### 💡 **Data Sensor Cahaya (LDR)**
```json
{
  "light_level": 450           // Intensitas cahaya dalam lux (0-1000)
}
```

### 🌡️ **Data Kontrol AC**
```json
{
  "ac_status": "2 AC ON (5 orang)",  // Status AC saat ini
  "set_temperature": 22               // Suhu yang diset untuk AC
}
```

### 🌐 **Data Koneksi WiFi**
```json
{
  "wifi_rssi": -45,            // Kekuatan sinyal WiFi dalam dBm
  "hosting_domain": "dasko.fst.unja.ac.id"
}
```

### 🔒 **Data SSL & Security**
```json
{
  "ssl_enabled": true,
  "ssl_configured": true,
  "ssl_last_success": true,
  "ssl_consecutive_failures": 0,
  "ssl_method": "root_ca",
  "ssl_last_error": ""
}
```

### 🌐 **Data Proxy Detection**
```json
{
  "proxy_detected": false,
  "proxy_type": "none",
  "connection_type": "direct",
  "hosting_reachable": true,
  "consecutive_failures": 0,
  "real_ip": "192.168.1.100",
  "proxy_ip": ""
}
```

### ⚙️ **Data Sistem ESP32**
```json
{
  "uptime": 3600,              // Waktu hidup ESP32 dalam detik
  "free_heap": 245760,         // Memory yang tersisa dalam bytes
  "timestamp": 1640995200000,  // Timestamp saat data dikirim
  "status": "active"           // Status device
}
```

### 📊 **Data Update Information**
```json
{
  "update_reason": "Enhanced Proximity entry - Count: 5",
  "update_type": "enhanced_ssl_proxy_detection"
}
```

## 📡 **CONTOH DATA LENGKAP YANG DIKIRIM KE WEB:**

```json
{
  "device_id": "ESP32_Proximity_SSL_Proxy_UNJA_Prodi_Elektro",
  "location": "Ruang Prodi Dosen Teknik Elektro - UNJA - SSL + Proxy Detection",
  "people_count": 5,
  "ac_status": "1 AC ON (5 orang)",
  "set_temperature": 22,
  "room_temperature": 25.5,
  "humidity": 65.2,
  "light_level": 450,
  "proximity_in": true,
  "proximity_out": false,
  "wifi_rssi": -45,
  "uptime": 3600,
  "free_heap": 245760,
  "timestamp": 1640995200000,
  "status": "active",
  "update_reason": "Enhanced Proximity entry - Count: 5",
  "update_type": "enhanced_ssl_proxy_detection",
  "hosting_domain": "dasko.fst.unja.ac.id",
  "ssl_enabled": true,
  "sensor_type": "Enhanced_SSL_Proxy_Sensor",
  "ssl_configured": true,
  "ssl_last_success": true,
  "ssl_consecutive_failures": 0,
  "ssl_last_error": "",
  "ssl_method": "root_ca",
  "proxy_detected": false,
  "proxy_type": "none",
  "connection_type": "direct",
  "hosting_reachable": true,
  "consecutive_failures": 0,
  "real_ip": "192.168.1.100",
  "proxy_ip": ""
}
```

## 🔄 **KAPAN DATA DIKIRIM KE WEB:**

### 1. **Automatic Triggers:**
- ✅ Ketika ada orang masuk ruangan (proximity sensor IN)
- ✅ Ketika ada orang keluar ruangan (proximity sensor OUT)
- ✅ Ketika suhu berubah ≥ 0.3°C
- ✅ Ketika kelembaban berubah ≥ 1.0%
- ✅ Ketika intensitas cahaya berubah ≥ 50 lux
- ✅ Ketika status AC berubah
- ✅ Force update setiap 2 menit (jika tidak ada perubahan)

### 2. **Manual Triggers:**
- ✅ Ketika menggunakan serial command `add` atau `sub`
- ✅ Ketika menggunakan serial command `reset`

## 🌐 **ENDPOINT WEB YANG MENERIMA DATA:**

```
POST https://dasko.fst.unja.ac.id/api/sensor/data
Content-Type: application/json
```

## 📊 **MONITORING REAL-TIME:**

Data akan dikirim secara real-time ke web hosting, sehingga Anda bisa:

1. **Monitor jumlah orang** di Ruang Prodi Dosen Teknik Elektro
2. **Track suhu dan kelembaban** ruangan
3. **Monitor intensitas cahaya** untuk kontrol lampu otomatis
4. **Monitor status AC** dan pengaturan suhu
5. **Monitor koneksi WiFi** dan kualitas sinyal
6. **Monitor status SSL** dan keamanan koneksi
7. **Detect proxy** jika ada di jaringan kampus
8. **Monitor kesehatan sistem** ESP32

## ✅ **KESIMPULAN:**

**YA, SEMUA DATA SENSOR AKAN TERBACA DI WEB!** 

ESP32 akan mengirim data lengkap dari semua sensor (proximity, DHT22, LDR) plus informasi sistem, SSL, dan proxy detection ke hosting web dasko.fst.unja.ac.id secara real-time dan aman melalui HTTPS.