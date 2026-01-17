# SETUP ESP32 TO COMPUTER CONNECTION - STEP BY STEP

## 🎯 KONFIGURASI BERHASIL DIUPDATE!

**Computer IP Address**: `192.168.0.102`
**ESP32 akan connect ke**: `http://192.168.0.102:8000/api/sensor/data`

## 📋 LANGKAH-LANGKAH SETUP:

### **STEP 1: START LARAVEL SERVER**
```bash
# Jalankan file batch ini:
start-localhost-server.bat

# Atau manual:
php artisan serve --host=0.0.0.0 --port=8000
```

### **STEP 2: TEST CONNECTION**
```bash
# Jalankan file batch ini untuk test:
test-esp32-connection.bat
```

**Expected Output:**
```
HTTP Status: 200
{"success":true,"message":"Data berhasil created",...}
```

### **STEP 3: UPLOAD ARDUINO CODE**
1. **Code sudah diupdate** dengan IP `192.168.0.102`
2. **Upload ke ESP32**
3. **Buka Serial Monitor** (115200 baud)

### **STEP 4: MONITOR HASIL**
**Serial Monitor ESP32 akan menampilkan:**
```
✓ WiFi CONNECTED!
✓ IP Address: 192.168.0.101
✓ Target API: http://192.168.0.102:8000/api/sensor/data
🔍 Testing connection to computer IP...
📡 Computer IP test response: 200  ← HARUS 200!

📤 Sending proximity data to computer (192.168.0.102)...
📋 JSON Data to send: {"device_id":"ESP32_Proximity_Production_UNJA",...}
✅ HTTP POST successful (Code: 200)
✅ SUCCESS: Proximity data sent to computer!
📥 Server Response: {"success":true,"message":"Data berhasil updated",...}
✅ Laravel saved with ID: 1
```

## 🌐 ACCESS POINTS:

### **Dari Komputer:**
- Dashboard: `http://localhost:8000`
- API: `http://localhost:8000/api/sensor/data`

### **Dari ESP32:**
- API: `http://192.168.0.102:8000/api/sensor/data`
- AC Control: `http://192.168.0.102:8000/api/ac/control`

### **Dari Device Lain di Network:**
- Dashboard: `http://192.168.0.102:8000`
- API: `http://192.168.0.102:8000/api/sensor/data`

## 🔧 TROUBLESHOOTING:

### **Jika ESP32 tidak bisa connect:**

#### 1. Cek Network:
- ESP32 dan komputer harus di WiFi yang sama
- WiFi: `LAB TEKNIK TEGANGAN TINGGI`

#### 2. Cek Firewall:
```cmd
# Disable Windows Firewall sementara untuk testing
```

#### 3. Test Manual:
```cmd
# Dari komputer, test apakah server accessible:
curl http://192.168.0.102:8000/api/sensor/data
```

#### 4. Cek IP Address:
```cmd
# Jika IP berubah, update Arduino code:
ipconfig
```

## 📊 EXPECTED DASHBOARD RESULT:

Setelah ESP32 connect, dashboard akan menampilkan:
- **Status Koneksi ESP32**: `Online` (bukan Offline)
- **Jumlah Orang di Ruangan**: Update real-time
- **Suhu Ruangan**: Data dari DHT22
- **Kelembaban Ruangan**: Data dari DHT22
- **Status AC**: Berdasarkan jumlah orang
- **Sensor Deteksi Orang**: IN/OUT status

## 🚀 TESTING PROCEDURE:

### **1. Start Server:**
```bash
start-localhost-server.bat
```

### **2. Test Connection:**
```bash
test-esp32-connection.bat
```

### **3. Upload ESP32:**
- Upload Arduino code yang sudah diupdate
- Monitor Serial Monitor

### **4. Test Proximity:**
- Gerakkan tangan di depan sensor MASUK (GPIO 2)
- Lihat Serial Monitor: `🚶 → PERSON ENTERED! (ULTRA FAST)`
- Cek dashboard: Jumlah orang bertambah

### **5. Test Temperature:**
- Lihat Serial Monitor untuk DHT22 readings
- Cek dashboard: Suhu dan kelembaban update

## ✅ SUCCESS INDICATORS:

### **Serial Monitor ESP32:**
```
✅ HTTP POST successful (Code: 200)
✅ SUCCESS: Proximity data sent to computer!
✅ Laravel saved with ID: X
```

### **Laravel Dashboard:**
- Status ESP32: `Online`
- Data update real-time
- Proximity count berubah saat test sensor

### **Laravel Logs:**
```bash
# Monitor logs:
tail -f storage/logs/laravel.log

# Should show:
[INFO] Arduino data received: {"device_id":"ESP32_Proximity_Production_UNJA",...}
[INFO] Sensor data updated successfully: {"id":1,...}
```

## 🎯 FINAL CHECKLIST:

```
□ Laravel server running: php artisan serve --host=0.0.0.0 --port=8000
□ ESP32 code updated dengan IP 192.168.0.102
□ ESP32 dan komputer di WiFi yang sama
□ Firewall disabled untuk testing
□ Serial Monitor menampilkan HTTP 200 response
□ Dashboard menampilkan data real-time
□ Proximity sensor test berhasil
□ Temperature readings muncul
```

**Sekarang ESP32 akan terhubung langsung ke komputer Anda!** 🚀

## 📞 NEXT STEPS:

1. **Jalankan `start-localhost-server.bat`**
2. **Upload Arduino code ke ESP32**
3. **Test proximity sensor dengan tangan**
4. **Monitor dashboard di `http://localhost:8000`**
5. **Enjoy real-time IoT monitoring!** 🎉