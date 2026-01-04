# WiFi Setup Guide for ESP32 Smart Energy System

## Quick Fix for WiFi Connection Issues

### 1. **Update WiFi Credentials in Code**

Open `INIFULL_copy_20251222133147.ino` and find these lines:

```cpp
// WiFi Configuration - Multiple Networks Support
const char* ssid = "YOUR_WIFI_SSID";        // Ganti dengan nama WiFi Anda
const char* password = "YOUR_WIFI_PASSWORD"; // Ganti dengan password WiFi Anda

// Alternative WiFi networks (fallback)
const char* ssid2 = "AndroidAP";
const char* password2 = "12345678";

const char* ssid3 = "LAPTOP_HOTSPOT";
const char* password3 = "password123";
```

**Replace with your actual WiFi credentials:**

```cpp
const char* ssid = "NamaWiFiAnda";
const char* password = "PasswordWiFiAnda";
```

### 2. **Common WiFi Network Names to Try**

If you don't know your WiFi name, try these common ones:

```cpp
// For home networks
const char* ssid = "HOME";
const char* password = "12345678";

// For mobile hotspot
const char* ssid = "AndroidAP";
const char* password = "12345678";

// For iPhone hotspot
const char* ssid = "iPhone";
const char* password = "12345678";
```

### 3. **Mobile Hotspot Setup (Recommended)**

**Android:**
1. Go to Settings → Mobile Hotspot
2. Set Name: `AndroidAP`
3. Set Password: `12345678`
4. Enable hotspot

**iPhone:**
1. Go to Settings → Personal Hotspot
2. Set Name: `iPhone`
3. Set Password: `12345678`
4. Enable hotspot

### 4. **WiFi Network Scanner**

The code now includes a WiFi scanner that will show available networks:

```
=== SCANNING WIFI NETWORKS ===
Found 5 networks:
No. | SSID                     | Signal | Security
----|--------------------------|--------|----------
 1  | HOME_WIFI               | -45    | Secured
 2  | AndroidAP               | -52    | Secured
 3  | OFFICE_NET              | -67    | Secured
 4  | FREE_WIFI               | -78    | Open
 5  | NEIGHBOR_WIFI           | -85    | Secured
```

### 5. **Offline Mode Support**

If WiFi fails, the system automatically enables **Offline Mode**:

✅ **Still Works:**
- Proximity sensor detection
- People counting
- AC control (auto mode only)
- TFT display
- Local operation

❌ **Disabled:**
- API updates to website
- Manual AC control from web
- Remote monitoring

### 6. **Troubleshooting Steps**

**Step 1: Check Network Availability**
- Ensure WiFi network is turned on
- Check if other devices can connect
- Verify 2.4GHz network (ESP32 doesn't support 5GHz)

**Step 2: Check Credentials**
- Verify SSID name (case-sensitive)
- Verify password (case-sensitive)
- No special characters in SSID if possible

**Step 3: Signal Strength**
- Move ESP32 closer to router
- Check for interference
- Use WiFi analyzer app to check signal

**Step 4: ESP32 Hardware**
- Check antenna connection
- Verify power supply (stable 3.3V or 5V)
- Try different ESP32 board if available

### 7. **Quick Test Networks**

**Option 1: Mobile Hotspot**
```cpp
const char* ssid = "AndroidAP";
const char* password = "12345678";
```

**Option 2: Simple Home Network**
```cpp
const char* ssid = "HOME";
const char* password = "password";
```

**Option 3: Open Network (if available)**
```cpp
const char* ssid = "FREE_WIFI";
const char* password = "";  // Empty for open networks
```

### 8. **Serial Monitor Output**

When WiFi scanning runs, you'll see:

```
=== SCANNING WIFI NETWORKS ===
Scan completed!
✅ Found 3 networks:
No. | SSID                     | Signal | Security
----|--------------------------|--------|----------
 1  | YOUR_WIFI_NAME          | -45    | Secured
 2  | NEIGHBOR_WIFI           | -67    | Secured
 3  | AndroidAP               | -52    | Secured

💡 TIP: Update WiFi credentials in code with available network
💡 TIP: Use 2.4GHz networks (ESP32 doesn't support 5GHz)
```

### 9. **Success Indicators**

When WiFi connects successfully:

```
✅ WiFi CONNECTED!
✅ Network: YOUR_WIFI_NAME
✅ IP: 192.168.1.100
✅ Signal: -45 dBm
✅ Production API: https://dasko.fst.unja.ac.id/api/sensor/data
✅ AC Control API: https://dasko.fst.unja.ac.id/api/ac/control
✅ Domain: dasko.fst.unja.ac.id
```

### 10. **Emergency Backup**

If all else fails, the system will continue working in **Offline Mode** with full local functionality. You can add WiFi later without losing any core features.

## Need Help?

1. Check Serial Monitor for detailed error messages
2. Try mobile hotspot first (easiest option)
3. Verify your WiFi supports 2.4GHz
4. System works offline if needed