# 🔧 Troubleshooting Connection Refused Error

## ❌ Problem
ESP32 gagal mengirim data ke Laravel dengan error "connection refused"

## 🔍 Root Cause Analysis
1. **ngrok URL expired** - URL `https://equilibrious-piddly-felicidad.ngrok-free.dev` sudah tidak aktif
2. **Laravel server not running** - Server Laravel mungkin tidak berjalan
3. **Network mismatch** - ESP32 dan laptop tidak di jaringan yang sama
4. **HTTPS vs HTTP** - ESP32 mungkin kesulitan dengan HTTPS

## ✅ Solutions

### Solution 1: Gunakan IP Lokal (RECOMMENDED)
Jika ESP32 dan laptop di WiFi yang sama:

1. **Cek IP laptop Anda:**
   ```cmd
   ipconfig
   ```
   Cari IP address (contoh: 192.168.1.100)

2. **Update arduino2_fixed.ino:**
   ```cpp
   const char* apiURL = "http://192.168.1.100:8000/api/sensor/data";
   ```
   Ganti `192.168.1.100` dengan IP laptop Anda

3. **Pastikan Laravel server running:**
   ```cmd
   php artisan serve --host=0.0.0.0 --port=8000
   ```

### Solution 2: Setup ngrok Baru
Jika perlu akses dari internet:

1. **Install ngrok:**
   - Download dari https://ngrok.com/
   - Extract dan jalankan

2. **Start Laravel server:**
   ```cmd
   php artisan serve
   ```

3. **Start ngrok tunnel:**
   ```cmd
   ngrok http 8000
   ```

4. **Copy URL baru ke Arduino:**
   ```cpp
   const char* apiURL = "https://your-new-url.ngrok-free.app/api/sensor/data";
   ```

### Solution 3: Bluetooth Bridge (ALTERNATIVE)
Jika WiFi bermasalah, gunakan Bluetooth:

1. **Upload bluetooth code ke ESP32**
2. **Jalankan Python bridge:**
   ```cmd
   start_bluetooth_bridge.bat
   ```

## 🧪 Testing Steps

### 1. Test Laravel API
```cmd
php test_api.php
```
Harus menunjukkan semua test PASSED

### 2. Test dari ESP32
Upload `arduino2_fixed.ino` dan monitor Serial:
- Harus terlihat "✓ WiFi CONNECTED!"
- Harus terlihat "✓ SUCCESS - Data sent to Laravel!"

### 3. Manual Test
Test API dengan curl:
```cmd
curl -X POST http://localhost:8000/api/sensor/data ^
  -H "Content-Type: application/json" ^
  -d "{\"device_id\":\"test\",\"people_count\":1,\"ac_status\":\"OFF\"}"
```

## 🔧 Debugging Commands

### Check Laravel Server
```cmd
# Start server
php artisan serve

# Check routes
php artisan route:list | findstr sensor

# Check logs
tail -f storage/logs/laravel.log
```

### Check Network
```cmd
# Check IP
ipconfig

# Test connectivity
ping 192.168.1.100

# Check port
netstat -an | findstr 8000
```

### ESP32 Serial Monitor
Monitor untuk pesan:
- `✓ WiFi CONNECTED!` - WiFi OK
- `✓ SUCCESS - Data sent to Laravel!` - API OK
- `✗ Connection refused` - Server issue
- `✗ Connection failed` - Network issue

## 📋 Checklist

- [ ] Laravel server running (`php artisan serve`)
- [ ] ESP32 dan laptop di WiFi yang sama
- [ ] IP address benar di Arduino code
- [ ] API endpoint accessible (`test_api.php` PASSED)
- [ ] No firewall blocking port 8000
- [ ] WiFi credentials benar di Arduino

## 🚀 Quick Fix Commands

```cmd
# 1. Start Laravel server
php artisan serve --host=0.0.0.0

# 2. Test API
php test_api.php

# 3. Get your IP
ipconfig | findstr IPv4

# 4. Update Arduino code dengan IP yang benar
# 5. Upload dan test
```

## 📞 Common Error Messages

| Error | Cause | Solution |
|-------|-------|----------|
| Connection refused | Server not running | Start `php artisan serve` |
| Connection failed | Wrong IP/URL | Check IP address |
| Timeout | Network issue | Check WiFi connection |
| 404 Not Found | Wrong endpoint | Check route exists |
| 500 Server Error | Laravel error | Check `storage/logs/laravel.log` |

## 🎯 Expected Output

**ESP32 Serial Monitor:**
```
=== CONNECTING TO WIFI ===
✓ WiFi CONNECTED!
✓ IP Address: 192.168.1.101
✓ API URL: http://192.168.1.100:8000/api/sensor/data

=== SENDING TO LARAVEL API ===
JSON Data: {"device_id":"ESP32_Smart_Energy",...}
Response Code: 201
✓ SUCCESS - Data sent to Laravel!
✓ Laravel confirmed data saved
✓ Saved with ID: 123
```

**Laravel Log:**
```
[2025-12-22 11:38:43] local.INFO: Arduino data received: {"device_id":"ESP32_Smart_Energy",...}
[2025-12-22 11:38:43] local.INFO: Sensor data saved successfully: {"id":123}
```