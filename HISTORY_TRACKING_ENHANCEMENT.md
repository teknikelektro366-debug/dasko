# HISTORY TRACKING ENHANCEMENT
## Pencatatan Setiap Pergerakan/Perubahan sebagai Record Terpisah

### 🎯 **OVERVIEW**
Mengubah sistem dari "update record yang sama" menjadi "create record baru untuk setiap perubahan signifikan" agar setiap pergerakan people count tercatat sebagai history terpisah.

### 🔄 **PERUBAHAN LOGIKA:**

#### **SEBELUM (Update/Create):**
```php
// Hanya 1 record per device_id + location
$sensorData = SensorData::updateOrCreate([
    'device_id' => $deviceId,
    'location' => $location
], $data);

// Result: Selalu update record yang sama
// History: Hanya melihat state terakhir
```

#### **SESUDAH (Smart Create/Update):**
```php
// Cek perubahan signifikan
if ($latestRecord->people_count != $currentPeopleCount) {
    // CREATE record baru untuk people count change
    $sensorData = SensorData::create($data);
} else {
    // UPDATE record existing untuk minor changes
    $latestRecord->update($minorData);
}

// Result: Record baru untuk setiap perubahan penting
// History: Setiap pergerakan tercatat
```

### 📊 **KRITERIA CREATE RECORD BARU:**

#### **1. People Count Change (Priority 1)**
```php
if ($latestRecord->people_count != $currentPeopleCount) {
    $shouldCreateNewRecord = true;
    $changeReason = "People count changed: {$old} → {$new}";
}
```

#### **2. AC Status Change (Priority 2)**
```php
if ($latestRecord->ac_status != $currentAcStatus) {
    $shouldCreateNewRecord = true;
    $changeReason = "AC status changed: {$old} → {$new}";
}
```

#### **3. Significant Temperature Change (Priority 3)**
```php
if (abs($latestRecord->room_temperature - $currentTemperature) >= 1.0) {
    $shouldCreateNewRecord = true;
    $changeReason = "Temperature changed significantly: {$old}°C → {$new}°C";
}
```

#### **4. Significant Humidity Change (Priority 4)**
```php
if (abs($latestRecord->humidity - $currentHumidity) >= 5.0) {
    $shouldCreateNewRecord = true;
    $changeReason = "Humidity changed significantly: {$old}% → {$new}%";
}
```

#### **5. Significant Light Level Change (Priority 5)**
```php
if (abs($latestRecord->light_level - $currentLightLevel) >= 100) {
    $shouldCreateNewRecord = true;
    $changeReason = "Light level changed significantly: {$old} → {$new} lux";
}
```

#### **6. Periodic Update (Priority 6)**
```php
if ($latestRecord->updated_at->diffInMinutes(now()) >= 5) {
    $shouldCreateNewRecord = true;
    $changeReason = "Periodic update (5+ minutes since last record)";
}
```

### 🎯 **CONTOH SKENARIO:**

#### **Skenario 1: People Count Changes**
```
Time 10:00 - People: 0 → CREATE record #1 (First record)
Time 10:01 - People: 1 → CREATE record #2 (People count: 0 → 1)
Time 10:02 - People: 2 → CREATE record #3 (People count: 1 → 2)
Time 10:03 - People: 1 → CREATE record #4 (People count: 2 → 1)
Time 10:04 - People: 0 → CREATE record #5 (People count: 1 → 0)
```

#### **Skenario 2: Minor Changes Only**
```
Time 10:00 - People: 2, Temp: 25.0°C → CREATE record #1
Time 10:01 - People: 2, Temp: 25.2°C → UPDATE record #1 (minor temp change)
Time 10:02 - People: 2, Temp: 25.4°C → UPDATE record #1 (minor temp change)
Time 10:03 - People: 2, Temp: 26.1°C → CREATE record #2 (temp change ≥1°C)
```

#### **Skenario 3: Mixed Changes**
```
Time 10:00 - People: 1, AC: OFF → CREATE record #1
Time 10:01 - People: 1, AC: OFF → UPDATE record #1 (no significant change)
Time 10:02 - People: 1, AC: ON  → CREATE record #2 (AC status changed)
Time 10:03 - People: 2, AC: ON  → CREATE record #3 (People count changed)
```

### 📈 **EXPECTED HISTORY RESULTS:**

#### **Database Records:**
```sql
SELECT id, people_count, ac_status, room_temperature, created_at 
FROM sensor_data 
ORDER BY created_at DESC;

-- Results:
| ID | People | AC Status | Temp | Created At          |
|----|--------|-----------|------|---------------------|
| 15 | 2      | 1 AC ON   | 25.1 | 2026-01-19 10:05:00 |
| 14 | 1      | 1 AC ON   | 25.0 | 2026-01-19 10:04:00 |
| 13 | 0      | AC OFF    | 24.8 | 2026-01-19 10:03:00 |
| 12 | 1      | 1 AC ON   | 24.9 | 2026-01-19 10:02:00 |
| 11 | 2      | 1 AC ON   | 25.2 | 2026-01-19 10:01:00 |
```

#### **History Page Display:**
- ✅ **Setiap perubahan people count** = record terpisah
- ✅ **Setiap perubahan AC status** = record terpisah  
- ✅ **Perubahan suhu signifikan** = record terpisah
- ✅ **Minor changes** = update record existing
- ✅ **Periodic updates** = record baru setiap 5 menit

### 🔍 **LOGGING & DEBUGGING:**

#### **Arduino Serial Output:**
```
📤 Sending proximity data to hosting web...
   People Count: 2
   AC Status: 1 AC ON
   
✅ SUCCESS: Proximity data sent to hosting web!
📥 Server Response: {
  "success": true,
  "action": "created",
  "change_reason": "People count changed: 1 → 2",
  "data": {"id": 15, "record_type": "new_record"}
}
```

#### **Laravel Log Output:**
```
[2026-01-19 10:05:00] INFO: New sensor record created:
{
  "id": 15,
  "device_id": "ESP32_Proximity_Production_UNJA_ULTRA_FAST",
  "people_count": 2,
  "reason": "People count changed: 1 → 2"
}
```

### 🚀 **BENEFITS:**

#### **1. Complete Movement Tracking**
- ✅ Setiap orang masuk/keluar tercatat
- ✅ Timeline pergerakan lengkap
- ✅ Pattern analysis possible

#### **2. Detailed History**
- ✅ Tidak kehilangan data perubahan
- ✅ Audit trail lengkap
- ✅ Troubleshooting lebih mudah

#### **3. Smart Storage**
- ✅ Record baru hanya untuk perubahan penting
- ✅ Minor updates tidak buat record baru
- ✅ Database tidak bloat dengan data redundant

#### **4. Better Analytics**
- ✅ Occupancy patterns
- ✅ AC usage correlation
- ✅ Environmental change tracking

### 📋 **TESTING SCENARIOS:**

#### **Test 1: People Movement**
1. Start with 0 people
2. 1 person enters → Should create new record
3. 1 person exits → Should create new record
4. Check history → Should show 3 records (0→1→0)

#### **Test 2: Minor Changes**
1. People count stays same
2. Temperature changes slightly (< 1°C)
3. Should update existing record, not create new

#### **Test 3: Mixed Changes**
1. People count changes + temperature changes
2. Should create new record with both changes
3. Check change_reason in response

### ✅ **SUCCESS CRITERIA:**

- ✅ **Every people count change** creates new record
- ✅ **AC status changes** create new records  
- ✅ **Minor sensor updates** update existing record
- ✅ **History shows complete movement timeline**
- ✅ **Database grows appropriately** (not too much, not too little)
- ✅ **Change reasons logged** for debugging

---

**Status**: ✅ **IMPLEMENTED & READY FOR TESTING**  
**Version**: v3.5 - History Movement Tracking  
**Date**: January 19, 2026  
**Impact**: Every movement now creates separate history record