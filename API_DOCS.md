# 📘 API Documentation — Smart Energy Management System (DASKO)

> **Base URL**: `http://{host}/api`  
> **Framework**: Laravel 11  
> **Content-Type**: `application/json`  
> **Last Updated**: 12 April 2026

---

## 📑 Daftar Isi

1. [Sensor Data API](#1-sensor-data-api)
2. [Dashboard API](#2-dashboard-api)
3. [AC Control API](#3-ac-control-api)
4. [Reports API (JSON/HTML)](#4-reports-api-jsonhtml)
5. [PDF Reports API](#5-pdf-reports-api)
6. [Energy Management API](#6-energy-management-api)
7. [Autentikasi](#7-autentikasi)
8. [Model & Struktur Database](#8-model--struktur-database)
9. [Error Handling](#9-error-handling)

---

## 1. Sensor Data API

Prefix: `/api/sensor`

### 1.1 `GET /api/sensor/data` — Ambil Semua Data Sensor (Paginated)

Mengambil data sensor dengan paginasi.

**Query Parameters:**

| Parameter  | Tipe    | Wajib | Default | Deskripsi                              |
|------------|---------|-------|---------|----------------------------------------|
| `per_page` | integer | Tidak | `20`    | Jumlah item per halaman (min: 5, max: 100) |
| `page`     | integer | Tidak | `1`     | Nomor halaman                          |

**Response Sukses** `200 OK`:

```json
{
  "success": true,
  "message": "Sensor data retrieved successfully",
  "data": [
    {
      "id": 1,
      "device_id": "ESP32_Smart_Energy",
      "location": "Lab Teknik Tegangan Tinggi",
      "people_count": 5,
      "ac_status": "ON",
      "set_temperature": 24,
      "room_temperature": "26.50",
      "humidity": "65.30",
      "light_level": 350,
      "lamp_status": "ON",
      "proximity_in": false,
      "proximity_out": false,
      "wifi_rssi": -45,
      "status": "active",
      "device_timestamp": 123456789,
      "created_at": "2026-04-12T10:00:00.000000Z",
      "updated_at": "2026-04-12T10:00:00.000000Z"
    }
  ],
  "pagination": {
    "current_page": 1,
    "last_page": 10,
    "per_page": 20,
    "total": 200,
    "from": 1,
    "to": 20
  },
  "meta": {
    "endpoint": "/api/sensor/data",
    "method": "GET",
    "description": "Get paginated sensor data",
    "parameters": {
      "per_page": "Number of items per page (5-100, default: 20)"
    }
  }
}
```

---

### 1.2 `POST /api/sensor/data` — Kirim Data Sensor (Arduino ESP32)

Endpoint untuk Arduino ESP32 mengirim data sensor ke server. Setiap request akan membuat **record baru**.

**Request Body:**

| Field              | Tipe    | Wajib | Default                         | Deskripsi                              |
|--------------------|---------|-------|---------------------------------|----------------------------------------|
| `device_id`        | string  | Tidak | `"ESP32_Smart_Energy"`          | ID perangkat                           |
| `location`         | string  | Tidak | `"Lab Teknik Tegangan Tinggi"`  | Lokasi sensor                          |
| `people_count`     | integer | **Ya**| —                               | Jumlah orang terdeteksi (0-100)        |
| `ac_status`        | string  | **Ya**| —                               | Status AC (`"ON"`, `"OFF"`, `"AC 1+2"`) |
| `set_temperature`  | integer | Tidak | `null`                          | Suhu set AC (max: 30)                  |
| `room_temperature` | numeric | Tidak | `null`                          | Suhu ruangan (-10 s.d 60)             |
| `humidity`         | numeric | Tidak | `null`                          | Kelembaban (0-100%)                    |
| `light_level`      | integer | Tidak | `0`                             | Intensitas cahaya (0-1000 lux)         |
| `lamp_status`      | string  | Tidak | `"OFF"`                         | Status lampu (`"ON"` / `"OFF"`)        |
| `proximity_in`     | boolean | Tidak | `false`                         | Sensor proximity masuk                 |
| `proximity_out`    | boolean | Tidak | `false`                         | Sensor proximity keluar                |
| `wifi_rssi`        | integer | Tidak | `null`                          | Kekuatan sinyal WiFi (-100 s.d 0 dBm) |
| `status`           | string  | Tidak | `"active"`                      | Status sistem                          |
| `timestamp`        | integer | Tidak | `null`                          | Timestamp dari Arduino (`millis()`)    |
| `created_at`       | string  | Tidak | `null`                          | Timestamp khusus (untuk data historis) |

**Contoh Request:**

```json
{
  "device_id": "ESP32_Smart_Energy",
  "people_count": 3,
  "ac_status": "ON",
  "set_temperature": 24,
  "room_temperature": 27.5,
  "humidity": 62.3,
  "light_level": 400,
  "lamp_status": "ON",
  "wifi_rssi": -52
}
```

**Response Sukses** `201 Created`:

```json
{
  "success": true,
  "message": "Data berhasil created",
  "action": "created",
  "change_reason": "New data from ESP32",
  "data": {
    "id": 150,
    "device_id": "ESP32_Smart_Energy",
    "location": "Lab Teknik Tegangan Tinggi",
    "people_count": 3,
    "ac_status": "ON",
    "set_temperature": 24,
    "room_temperature": 27.5,
    "humidity": 62.3,
    "light_level": 400,
    "lamp_status": "ON",
    "timestamp": "2026-04-12T10:05:00.000000Z",
    "was_recently_created": true,
    "record_type": "new_record"
  }
}
```

**Response Validasi Error** `422 Unprocessable Entity`:

```json
{
  "success": false,
  "message": "Validation failed",
  "errors": {
    "people_count": ["The people count field is required."],
    "ac_status": ["The ac status field is required."]
  }
}
```

---

### 1.3 `GET /api/sensor/latest` — Data Sensor Terbaru (Realtime)

Mengambil data sensor paling baru untuk tampilan dashboard realtime.

**Query Parameters:** Tidak ada

**Response Sukses** `200 OK`:

```json
{
  "success": true,
  "data": {
    "id": 150,
    "device_id": "ESP32_Smart_Energy",
    "location": "Lab Teknik Tegangan Tinggi",
    "people_count": 3,
    "ac_status": "ON",
    "set_temperature": 24,
    "room_temperature": "27.50",
    "humidity": "62.30",
    "light_level": 400,
    "proximity_in": false,
    "proximity_out": false,
    "wifi_rssi": -52,
    "status": "active",
    "created_at": "2026-04-12T10:05:00.000000Z",
    "time_ago": "2 minutes ago",
    "is_recent": true,
    "formatted": {
      "temperature": "27.5°C",
      "humidity": "62.3%",
      "light": "400 lux",
      "wifi": "Good (-52 dBm)"
    },
    "realtime_info": {
      "last_update": "10:05:00",
      "update_frequency": 30.5,
      "data_age_seconds": 120,
      "connection_quality": "good"
    }
  }
}
```

**Response Tidak Ada Data** `404 Not Found`:

```json
{
  "success": false,
  "message": "Tidak ada data sensor",
  "data": null
}
```

---

### 1.4 `GET /api/sensor/chart` — Data untuk Chart

Mengambil data sensor yang dikelompokkan per jam untuk keperluan grafik/chart.

**Query Parameters:**

| Parameter | Tipe    | Wajib | Default | Deskripsi                                |
|-----------|---------|-------|---------|------------------------------------------|
| `hours`   | integer | Tidak | `24`    | Rentang waktu dalam jam (min: 1, max: 168) |

**Response Sukses** `200 OK`:

```json
{
  "success": true,
  "data": [
    {
      "time": "08:00",
      "people_count": 2.5,
      "temperature": 26.3,
      "humidity": 64.2,
      "light_level": 380,
      "ac_on": true
    },
    {
      "time": "09:00",
      "people_count": 5.0,
      "temperature": 25.1,
      "humidity": 60.5,
      "light_level": 420,
      "ac_on": true
    }
  ],
  "period": "24 hours",
  "total_records": 120
}
```

---

### 1.5 `GET /api/sensor/stats` — Statistik Harian

Mengambil statistik ringkasan untuk hari ini.

**Query Parameters:** Tidak ada

**Response Sukses** `200 OK`:

```json
{
  "success": true,
  "data": {
    "today_count": 150,
    "avg_people": 3.5,
    "max_people": 12,
    "avg_temperature": 25.8,
    "avg_humidity": 63.4,
    "avg_light": 350,
    "ac_on_time": 80,
    "ac_on_percentage": 53.3,
    "last_update": "2026-04-12T10:05:00.000000Z",
    "first_data_today": "2026-04-12T00:00:30.000000Z",
    "data_frequency": 30.5
  }
}
```

---

### 1.6 `GET /api/sensor/history` — Riwayat Data Sensor

Mengambil data riwayat sensor dengan paginasi dan filter opsional.

**Query Parameters:**

| Parameter   | Tipe    | Wajib | Default | Deskripsi                                 |
|-------------|---------|-------|---------|-------------------------------------------|
| `per_page`  | integer | Tidak | `50`    | Jumlah item per halaman (min: 10, max: 100) |
| `page`      | integer | Tidak | `1`     | Nomor halaman                             |
| `device_id` | string  | Tidak | —       | Filter berdasarkan device ID              |

**Response Sukses** `200 OK`:

```json
{
  "success": true,
  "data": [ /* ... array SensorData ... */ ],
  "pagination": {
    "current_page": 1,
    "last_page": 5,
    "per_page": 50,
    "total": 250,
    "from": 1,
    "to": 50
  },
  "debug_info": {
    "query_executed": true,
    "total_in_db": 1500,
    "latest_record": "2026-04-12T10:05:00.000000Z"
  }
}
```

---

### 1.7 `POST /api/sensor/cleanup` — Hapus Data Lama

Membersihkan data sensor yang sudah lewat dari batas hari tertentu.

**Request Body:**

| Field  | Tipe    | Wajib | Default | Deskripsi                                |
|--------|---------|-------|---------|------------------------------------------|
| `days` | integer | Tidak | `30`    | Hapus data lebih tua dari N hari (7-365) |

**Contoh Request:**

```json
{
  "days": 60
}
```

**Response Sukses** `200 OK`:

```json
{
  "success": true,
  "message": "Berhasil menghapus 350 data lama (lebih dari 60 hari)",
  "deleted_count": 350
}
```

---

## 2. Dashboard API

Prefix: `/api/dashboard`

> **Catatan:** Endpoint ini merupakan alias/shortcut ke endpoint Sensor Data yang telah ada.

### 2.1 `GET /api/dashboard/realtime` — Data Realtime Dashboard

Alias untuk `GET /api/sensor/latest`. Lihat [1.3](#13-get-apisensorlatest--data-sensor-terbaru-realtime).

---

### 2.2 `GET /api/dashboard/chart/{hours?}` — Data Chart Dashboard

Alias untuk `GET /api/sensor/chart`. Parameter `hours` langsung di URL path.

| Path Parameter | Tipe    | Wajib | Default | Deskripsi               |
|----------------|---------|-------|---------|-------------------------|
| `hours`        | integer | Tidak | `24`    | Rentang waktu (1-168 jam) |

**Contoh:** `GET /api/dashboard/chart/48`

---

### 2.3 `GET /api/dashboard/stats/daily` — Statistik Harian Dashboard

Alias untuk `GET /api/sensor/stats`. Lihat [1.5](#15-get-apisensorstats--statistik-harian).

---

## 3. AC Control API

Prefix: `/api/ac`

### 3.1 `GET /api/ac/control` — Status Kontrol AC Saat Ini

Mengambil pengaturan kontrol AC aktif untuk digunakan oleh Arduino.

**Query Parameters:**

| Parameter   | Tipe   | Wajib | Default                        | Deskripsi      |
|-------------|--------|-------|--------------------------------|----------------|
| `device_id` | string | Tidak | `"ESP32_Smart_Energy"`         | ID perangkat   |
| `location`  | string | Tidak | `"Lab Teknik Tegangan Tinggi"` | Lokasi ruangan |

**Response Sukses (Kontrol Aktif)** `200 OK`:

```json
{
  "success": true,
  "message": "Active control found",
  "data": {
    "id": 25,
    "control_mode": "manual",
    "manual_override": true,
    "ac1_status": true,
    "ac2_status": false,
    "ac1_temperature": 24,
    "ac2_temperature": 25,
    "created_by": "dashboard",
    "expires_at": "2026-04-12T18:00:00.000000Z",
    "formatted_status": "1 AC ON",
    "summary": "AC1: ON (24°C), AC2: OFF"
  }
}
```

**Response (Tanpa Kontrol Aktif / Mode Auto)** `200 OK`:

```json
{
  "success": true,
  "message": "No active control found, using auto mode",
  "data": {
    "control_mode": "auto",
    "manual_override": false,
    "ac1_status": false,
    "ac2_status": false,
    "ac1_temperature": 25,
    "ac2_temperature": 25,
    "expires_at": null
  }
}
```

---

### 3.2 `POST /api/ac/control` — Atur Kontrol AC

Mengatur kontrol AC dari dashboard. Semua kontrol sebelumnya akan di-expire.

**Request Body:**

| Field              | Tipe    | Wajib  | Default              | Deskripsi                                    |
|--------------------|---------|--------|----------------------|----------------------------------------------|
| `device_id`        | string  | Tidak  | `"ESP32_Smart_Energy"` | ID perangkat                               |
| `location`         | string  | Tidak  | `"Lab Teknik Tegangan Tinggi"` | Lokasi ruangan                   |
| `ac1_status`       | boolean | **Ya** | —                    | Status AC unit 1 (`true`/`false`)            |
| `ac2_status`       | boolean | **Ya** | —                    | Status AC unit 2 (`true`/`false`)            |
| `ac1_temperature`  | integer | **Ya** | —                    | Suhu AC 1 (16-30°C)                         |
| `ac2_temperature`  | integer | **Ya** | —                    | Suhu AC 2 (16-30°C)                         |
| `control_mode`     | string  | **Ya** | —                    | Mode kontrol: `"auto"`, `"manual"`, `"schedule"` |
| `duration_minutes` | integer | Tidak  | `null`               | Durasi kontrol dalam menit (1-1440)          |
| `created_by`       | string  | Tidak  | `"dashboard"`        | Sumber pengaturan                            |

**Contoh Request:**

```json
{
  "ac1_status": true,
  "ac2_status": true,
  "ac1_temperature": 22,
  "ac2_temperature": 24,
  "control_mode": "manual",
  "duration_minutes": 120
}
```

**Response Sukses** `201 Created`:

```json
{
  "success": true,
  "message": "AC control berhasil diatur",
  "data": {
    "id": 26,
    "control_mode": "manual",
    "manual_override": true,
    "ac1_status": true,
    "ac2_status": true,
    "ac1_temperature": 22,
    "ac2_temperature": 24,
    "expires_at": "2026-04-12T12:05:00.000000Z",
    "formatted_status": "2 AC ON",
    "summary": "AC1: ON (22°C), AC2: ON (24°C)"
  }
}
```

**Response Validasi Error** `422 Unprocessable Entity`:

```json
{
  "success": false,
  "message": "Validation failed",
  "errors": {
    "ac1_status": ["The ac1 status field is required."],
    "control_mode": ["The selected control mode is invalid."]
  }
}
```

---

### 3.3 `POST /api/ac/emergency-stop` — Emergency Stop (Matikan Semua AC)

Mematikan semua unit AC secara darurat.

**Request Body (Opsional):**

| Field       | Tipe   | Wajib | Default                        | Deskripsi      |
|-------------|--------|-------|--------------------------------|----------------|
| `device_id` | string | Tidak | `"ESP32_Smart_Energy"`         | ID perangkat   |
| `location`  | string | Tidak | `"Lab Teknik Tegangan Tinggi"` | Lokasi ruangan |

**Response Sukses** `200 OK`:

```json
{
  "success": true,
  "message": "Emergency stop activated - All AC units turned OFF",
  "data": {
    "id": 27,
    "control_mode": "manual",
    "ac1_status": false,
    "ac2_status": false,
    "formatted_status": "OFF"
  }
}
```

---

### 3.4 `POST /api/ac/auto-mode` — Kembali ke Mode Otomatis

Mengembalikan kontrol AC ke mode otomatis. Semua kontrol manual yang ada akan di-expire.

**Request Body (Opsional):**

| Field       | Tipe   | Wajib | Default                        | Deskripsi      |
|-------------|--------|-------|--------------------------------|----------------|
| `device_id` | string | Tidak | `"ESP32_Smart_Energy"`         | ID perangkat   |
| `location`  | string | Tidak | `"Lab Teknik Tegangan Tinggi"` | Lokasi ruangan |

**Response Sukses** `200 OK`:

```json
{
  "success": true,
  "message": "AC control returned to auto mode",
  "data": {
    "control_mode": "auto",
    "manual_override": false,
    "message": "Arduino will control AC based on people count"
  }
}
```

---

### 3.5 `GET /api/ac/history` — Riwayat Kontrol AC

Mengambil riwayat perubahan kontrol AC.

**Query Parameters:**

| Parameter   | Tipe    | Wajib | Default                        | Deskripsi                                 |
|-------------|---------|-------|--------------------------------|-------------------------------------------|
| `device_id` | string  | Tidak | `"ESP32_Smart_Energy"`         | ID perangkat                              |
| `location`  | string  | Tidak | `"Lab Teknik Tegangan Tinggi"` | Lokasi ruangan                            |
| `per_page`  | integer | Tidak | `20`                           | Jumlah item per halaman (min: 10, max: 100) |

**Response Sukses** `200 OK`:

```json
{
  "success": true,
  "data": [
    {
      "id": 27,
      "device_id": "ESP32_Smart_Energy",
      "location": "Lab Teknik Tegangan Tinggi",
      "ac1_status": false,
      "ac2_status": false,
      "ac1_temperature": 25,
      "ac2_temperature": 25,
      "control_mode": "manual",
      "manual_override": true,
      "created_by": "emergency_stop",
      "expires_at": null,
      "created_at": "2026-04-12T10:10:00.000000Z",
      "updated_at": "2026-04-12T10:10:00.000000Z"
    }
  ],
  "pagination": {
    "current_page": 1,
    "last_page": 3,
    "per_page": 20,
    "total": 50
  }
}
```

---

## 4. Reports API (JSON/HTML)

Prefix: `/api/reports`

### 4.1 `GET /api/reports/daily` — Laporan Harian

Mengambil data laporan harian dalam format JSON atau HTML (untuk print/PDF manual melalui browser).

**Query Parameters:**

| Parameter | Tipe   | Wajib | Default    | Deskripsi                                  |
|-----------|--------|-------|------------|--------------------------------------------|
| `date`    | string | Tidak | Hari ini   | Tanggal laporan (format: `YYYY-MM-DD`)     |
| `format`  | string | Tidak | `"json"`   | Format output: `"json"` atau `"pdf"` (HTML untuk print) |

**Response Sukses (JSON)** `200 OK`:

```json
{
  "success": true,
  "data": [ /* ... array SensorData ... */ ],
  "summary": {
    "date": "12 April 2026",
    "total_records": 120,
    "avg_temperature": 25.8,
    "avg_humidity": 63.4,
    "avg_light_intensity": 350,
    "total_people_detected": 450,
    "ac_on_percentage": 53.3
  }
}
```

**Response (format=pdf)**: HTML page yang dapat di-print/save as PDF melalui browser.

---

### 4.2 `GET /api/reports/weekly` — Laporan Mingguan

**Query Parameters:**

| Parameter    | Tipe   | Wajib | Default            | Deskripsi                                  |
|--------------|--------|-------|--------------------|--------------------------------------------|
| `week_start` | string | Tidak | Awal minggu ini    | Tanggal awal minggu (`YYYY-MM-DD`)         |
| `format`     | string | Tidak | `"json"`           | Format output: `"json"` atau `"pdf"`       |

**Response Sukses (JSON)** `200 OK`:

```json
{
  "success": true,
  "data": [ /* ... array SensorData ... */ ],
  "summary": {
    "period": "07 Apr 2026 - 13 Apr 2026",
    "total_records": 840,
    "avg_temperature": 25.5,
    "avg_humidity": 62.8,
    "total_people_detected": 1250
  }
}
```

---

### 4.3 `GET /api/reports/monthly` — Laporan Bulanan

**Query Parameters:**

| Parameter | Tipe   | Wajib | Default         | Deskripsi                               |
|-----------|--------|-------|-----------------|-----------------------------------------|
| `month`   | string | Tidak | Bulan ini       | Bulan laporan (format: `YYYY-MM`)       |
| `format`  | string | Tidak | `"json"`        | Format output: `"json"` atau `"pdf"`    |

**Response Sukses (JSON)** `200 OK`:

```json
{
  "success": true,
  "data": [ /* ... array SensorData ... */ ],
  "summary": {
    "month": "April 2026",
    "total_records": 3600,
    "avg_temperature": 25.6,
    "avg_humidity": 63.1,
    "total_people_detected": 5400
  }
}
```

---

### 4.4 `GET /api/reports/efficiency` — Laporan Efisiensi

Saat ini merupakan redirect ke endpoint **Monthly Report**.

---

### 4.5 `GET /api/reports/custom` — Laporan Kustom (PDF)

Membuat laporan kustom dengan rentang tanggal dan filter perangkat. Endpoint ini diarahkan ke `PdfReportController::customReport`.

**Query Parameters:**

| Parameter     | Tipe   | Wajib | Default    | Deskripsi                                    |
|---------------|--------|-------|------------|----------------------------------------------|
| `date_from`   | string | Tidak | 7 hari lalu| Tanggal awal (`YYYY-MM-DD`)                  |
| `date_to`     | string | Tidak | Hari ini   | Tanggal akhir (`YYYY-MM-DD`)                 |
| `device_type` | string | Tidak | `"all"`    | Filter device (`"all"` atau device ID spesifik) |
| `format`      | string | Tidak | `"pdf"`    | Format output: `"pdf"` atau `"json"`         |

**Response (format=pdf)**: PDF file yang ditampilkan di browser (stream).

**Response (format=json)** `200 OK`:

```json
{
  "data": [ /* ... array SensorData ... */ ],
  "summary": {
    "period": "05 Apr 2026 - 12 Apr 2026",
    "total_records": 500,
    "avg_people": 3.2,
    "max_people": 15,
    "avg_temperature": 25.7,
    "avg_humidity": 63.0,
    "avg_light": 360,
    "ac_on_count": 280,
    "lamp_on_count": 300,
    "device_type": "Semua Perangkat"
  }
}
```

---

## 5. PDF Reports API

Prefix: `/api/reports/pdf`

> Endpoint ini menggunakan DomPDF untuk generate file PDF langsung (stream ke browser).

### 5.1 `GET /api/reports/pdf/daily` — PDF Laporan Harian

**Query Parameters:**

| Parameter | Tipe   | Wajib | Default  | Deskripsi                              |
|-----------|--------|-------|----------|----------------------------------------|
| `date`    | string | Tidak | Hari ini | Tanggal laporan (`YYYY-MM-DD`)         |

**Response**: `application/pdf` — File PDF ditampilkan langsung di browser.

**Response Error** `500`:

```json
{
  "error": "Failed to generate PDF: {error_message}"
}
```

---

### 5.2 `GET /api/reports/pdf/weekly` — PDF Laporan Mingguan

**Query Parameters:**

| Parameter    | Tipe   | Wajib | Default         | Deskripsi                              |
|--------------|--------|-------|-----------------|----------------------------------------|
| `week_start` | string | Tidak | Awal minggu ini | Tanggal awal minggu (`YYYY-MM-DD`)     |

**Response**: `application/pdf`

---

### 5.3 `GET /api/reports/pdf/monthly` — PDF Laporan Bulanan

**Query Parameters:**

| Parameter | Tipe   | Wajib | Default   | Deskripsi                         |
|-----------|--------|-------|-----------|-----------------------------------|
| `month`   | string | Tidak | Bulan ini | Bulan laporan (`YYYY-MM`)         |

**Response**: `application/pdf`

---

### 5.4 `GET /api/reports/pdf/efficiency` — PDF Laporan Efisiensi

**Query Parameters:**

| Parameter   | Tipe   | Wajib | Default     | Deskripsi                          |
|-------------|--------|-------|-------------|------------------------------------|
| `date_from` | string | Tidak | 7 hari lalu | Tanggal awal (`YYYY-MM-DD`)        |
| `date_to`   | string | Tidak | Hari ini    | Tanggal akhir (`YYYY-MM-DD`)       |

**Response**: `application/pdf`

**Konten laporan meliputi:**
- Persentase penggunaan AC
- Persentase penggunaan lampu
- Rata-rata orang saat AC/lampu menyala
- Rata-rata suhu dan kelembaban

---

## 6. Energy Management API

Prefix: `/api/energy`

### 6.1 `GET /api/energy/daily` — Konsumsi Energi Harian

Menghitung konsumsi energi harian berdasarkan data sensor.

**Query Parameters:**

| Parameter | Tipe   | Wajib | Default  | Deskripsi                     |
|-----------|--------|-------|----------|-------------------------------|
| `date`    | string | Tidak | Hari ini | Tanggal (`YYYY-MM-DD`)        |

**Response Sukses** `200 OK`:

```json
{
  "date": "2026-04-12",
  "ac_wh": 8400,
  "ac_kwh": 8.40,
  "ac_cost": 12138,
  "ac_hours": 8.00,
  "lamp_wh": 2112,
  "lamp_kwh": 2.11,
  "lamp_cost": 3049,
  "lamp_hours": 8.00,
  "base_wh": 12720,
  "base_kwh": 12.72,
  "base_cost": 18380,
  "total_wh": 23232,
  "total_kwh": 23.23,
  "total_cost": 33567
}
```

> **Rumus**: `Wh = Daya (Watt) × Lama Pemakaian (Jam)`, `kWh = Wh / 1000`, `Biaya = kWh × Rp 1.445`

---

### 6.2 `GET /api/energy/monthly` — Konsumsi Energi Bulanan

**Query Parameters:**

| Parameter | Tipe   | Wajib | Default   | Deskripsi               |
|-----------|--------|-------|-----------|-------------------------|
| `month`   | string | Tidak | Bulan ini | Bulan (`YYYY-MM`)       |

**Response Sukses** `200 OK`:

```json
{
  "month": "April 2026",
  "ac_kwh": 252.00,
  "ac_cost": 364140,
  "lamp_kwh": 63.36,
  "lamp_cost": 91555,
  "base_kwh": 381.60,
  "base_cost": 551412,
  "total_kwh": 696.96,
  "total_cost": 1007107,
  "daily_data": [ /* ... array per-hari ... */ ]
}
```

---

### 6.3 `GET /api/energy/efficiency` — Efisiensi Energi

Menghitung efisiensi penggunaan energi dalam rentang waktu tertentu.

**Query Parameters:**

| Parameter   | Tipe   | Wajib | Default     | Deskripsi                    |
|-------------|--------|-------|-------------|------------------------------|
| `date_from` | string | Tidak | 7 hari lalu | Tanggal awal (`YYYY-MM-DD`)  |
| `date_to`   | string | Tidak | Hari ini    | Tanggal akhir (`YYYY-MM-DD`) |

**Response Sukses** `200 OK`: Data efisiensi dari `EnergyCalculationService`.

---

### 6.4 `GET /api/energy/devices` — Data Daya Perangkat

Mengambil daftar semua perangkat beserta konsumsi energinya.

**Query Parameters:** Tidak ada

**Response Sukses** `200 OK`:

```json
{
  "devices": {
    "AC Panasonic CS-YN12WKJ": {
      "quantity": 2,
      "watt_per_unit": 1050,
      "hours_per_day": 8,
      "consumption_wh": 16800,
      "consumption_kwh": 16.80,
      "cost": 24276
    },
    "Lampu TL": {
      "quantity": 12,
      "watt_per_unit": 22,
      "hours_per_day": 8,
      "consumption_wh": 2112,
      "consumption_kwh": 2.11,
      "cost": 3049
    },
    "Dispenser Sanken": { "..." : "..." },
    "Smart TV Horion": { "..." : "..." },
    "Kulkas Sharp": { "..." : "..." },
    "Komputer": { "..." : "..." },
    "Router Wi-Fi": { "..." : "..." },
    "Panci Listrik": { "..." : "..." },
    "Setrika Listrik": { "..." : "..." },
    "Mesin Kopi": { "..." : "..." }
  },
  "total_daily_consumption": 29.79,
  "total_daily_cost": 43047
}
```

---

### 6.5 `GET /api/energy/savings/daily` — Penghematan Energi Harian

Membandingkan konsumsi aktual vs konsumsi maksimal (jika AC & Lampu menyala 8 jam penuh).

**Query Parameters:**

| Parameter | Tipe   | Wajib | Default  | Deskripsi              |
|-----------|--------|-------|----------|------------------------|
| `date`    | string | Tidak | Hari ini | Tanggal (`YYYY-MM-DD`) |

**Response Sukses** `200 OK`:

```json
{
  "date": "2026-04-12",
  "max_consumption_kwh": 18.91,
  "actual_consumption_kwh": 10.51,
  "saved_kwh": 8.40,
  "saved_cost": 12138,
  "savings_percentage": 44.4,
  "max_ac_kwh": 16.80,
  "actual_ac_kwh": 8.40,
  "max_lamp_kwh": 2.11,
  "actual_lamp_kwh": 2.11
}
```

---

### 6.6 `GET /api/energy/savings/monthly` — Penghematan Energi Bulanan

**Query Parameters:**

| Parameter | Tipe   | Wajib | Default   | Deskripsi          |
|-----------|--------|-------|-----------|--------------------|
| `month`   | string | Tidak | Bulan ini | Bulan (`YYYY-MM`)  |

**Response Sukses** `200 OK`:

```json
{
  "month": "April 2026",
  "max_consumption_kwh": 567.36,
  "actual_consumption_kwh": 315.36,
  "saved_kwh": 252.00,
  "saved_cost": 364140,
  "savings_percentage": 44.4
}
```

---

## 7. Autentikasi

### `GET /api/user` — Info User (Sanctum)

> **Middleware**: `auth:sanctum`  
> Endpoint ini memerlukan token autentikasi Laravel Sanctum.

**Header:**

```
Authorization: Bearer {token}
```

**Response Sukses** `200 OK`:

```json
{
  "id": 1,
  "name": "Admin",
  "email": "admin@example.com",
  "email_verified_at": "2026-01-01T00:00:00.000000Z",
  "created_at": "2026-01-01T00:00:00.000000Z",
  "updated_at": "2026-01-01T00:00:00.000000Z"
}
```

> **Catatan**: Saat ini, seluruh endpoint API lainnya **tidak memerlukan autentikasi** (public access).

---

## 8. Model & Struktur Database

### 8.1 Tabel `sensor_data`

| Kolom              | Tipe            | Default                        | Deskripsi                      |
|--------------------|-----------------|--------------------------------|--------------------------------|
| `id`               | bigint (PK)     | auto-increment                 | Primary key                    |
| `device_id`        | varchar(255)    | `"ESP32_Smart_Energy"`         | ID perangkat ESP32             |
| `location`         | varchar(255)    | `"Lab Teknik Tegangan Tinggi"` | Lokasi sensor dipasang         |
| `people_count`     | integer         | `0`                            | Jumlah orang terdeteksi        |
| `ac_status`        | varchar(50)     | `"OFF"`                        | Status AC                      |
| `set_temperature`  | integer         | `null`                         | Suhu pengaturan AC             |
| `room_temperature` | decimal(5,2)    | `null`                         | Suhu ruangan aktual            |
| `humidity`         | decimal(5,2)    | `null`                         | Kelembaban udara               |
| `light_level`      | integer         | `0`                            | Intensitas cahaya (lux)        |
| `lamp_status`      | varchar(10)     | `"OFF"`                        | Status lampu                   |
| `proximity_in`     | boolean         | `false`                        | Sensor proximity masuk         |
| `proximity_out`    | boolean         | `false`                        | Sensor proximity keluar        |
| `wifi_rssi`        | integer         | `null`                         | Kekuatan sinyal WiFi (dBm)    |
| `status`           | varchar(50)     | `"active"`                     | Status sistem                  |
| `device_timestamp` | bigint          | `null`                         | Timestamp `millis()` Arduino   |
| `created_at`       | timestamp       | auto                           | Waktu record dibuat            |
| `updated_at`       | timestamp       | auto                           | Waktu record diupdate          |

**Indexes:** `(device_id, created_at)`, `(created_at)`

---

### 8.2 Tabel `ac_controls`

| Kolom              | Tipe                                  | Default              | Deskripsi                        |
|--------------------|---------------------------------------|----------------------|----------------------------------|
| `id`               | bigint (PK)                           | auto-increment       | Primary key                      |
| `device_id`        | varchar(255)                          | `"ESP32_Smart_Energy"` | ID perangkat                   |
| `location`         | varchar(255)                          | `"Lab Teknik Tegangan Tinggi"` | Lokasi ruangan           |
| `ac1_status`       | boolean                               | `false`              | Status AC unit 1                 |
| `ac2_status`       | boolean                               | `false`              | Status AC unit 2                 |
| `ac1_temperature`  | integer                               | `25`                 | Suhu set AC 1 (°C)              |
| `ac2_temperature`  | integer                               | `25`                 | Suhu set AC 2 (°C)              |
| `control_mode`     | enum(`auto`, `manual`, `schedule`)    | `"manual"`           | Mode kontrol                     |
| `manual_override`  | boolean                               | `false`              | Override manual aktif            |
| `created_by`       | varchar(255)                          | `"system"`           | Sumber pengaturan                |
| `expires_at`       | timestamp                             | `null`               | Waktu kadaluarsa kontrol         |
| `created_at`       | timestamp                             | auto                 | Waktu record dibuat              |
| `updated_at`       | timestamp                             | auto                 | Waktu record diupdate            |

**Indexes:** `(device_id, location)`, `(expires_at)`

---

## 9. Error Handling

Semua endpoint menggunakan format error yang konsisten:

### Error Validasi `422 Unprocessable Entity`

```json
{
  "success": false,
  "message": "Validation failed",
  "errors": {
    "field_name": ["Error message"]
  }
}
```

### Server Error `500 Internal Server Error`

```json
{
  "success": false,
  "message": "Deskripsi error",
  "error": "Detail error (hanya muncul jika APP_DEBUG=true)"
}
```

### Not Found `404 Not Found`

```json
{
  "success": false,
  "message": "Tidak ada data sensor",
  "data": null
}
```

### PDF Generation Error `500`

```json
{
  "error": "Failed to generate PDF: {error_message}"
}
```

---

## 📊 Ringkasan Seluruh Endpoint

| # | Method   | Path                            | Deskripsi                          | Auth |
|---|----------|---------------------------------|------------------------------------|------|
| 1 | `GET`    | `/api/sensor/data`              | Ambil data sensor (paginated)      | ❌   |
| 2 | `POST`   | `/api/sensor/data`              | Kirim data sensor dari ESP32       | ❌   |
| 3 | `GET`    | `/api/sensor/latest`            | Data sensor terbaru (realtime)     | ❌   |
| 4 | `GET`    | `/api/sensor/chart`             | Data chart (per jam)               | ❌   |
| 5 | `GET`    | `/api/sensor/stats`             | Statistik harian                   | ❌   |
| 6 | `GET`    | `/api/sensor/history`           | Riwayat data sensor                | ❌   |
| 7 | `POST`   | `/api/sensor/cleanup`           | Hapus data lama                    | ❌   |
| 8 | `GET`    | `/api/dashboard/realtime`       | Alias → sensor/latest              | ❌   |
| 9 | `GET`    | `/api/dashboard/chart/{hours?}` | Alias → sensor/chart               | ❌   |
| 10| `GET`    | `/api/dashboard/stats/daily`    | Alias → sensor/stats               | ❌   |
| 11| `GET`    | `/api/ac/control`               | Status kontrol AC saat ini         | ❌   |
| 12| `POST`   | `/api/ac/control`               | Atur kontrol AC                    | ❌   |
| 13| `POST`   | `/api/ac/emergency-stop`        | Emergency stop (matikan semua AC)  | ❌   |
| 14| `POST`   | `/api/ac/auto-mode`             | Kembali ke mode otomatis           | ❌   |
| 15| `GET`    | `/api/ac/history`               | Riwayat kontrol AC                 | ❌   |
| 16| `GET`    | `/api/reports/daily`            | Laporan harian (JSON/HTML)         | ❌   |
| 17| `GET`    | `/api/reports/weekly`           | Laporan mingguan (JSON/HTML)       | ❌   |
| 18| `GET`    | `/api/reports/monthly`          | Laporan bulanan (JSON/HTML)        | ❌   |
| 19| `GET`    | `/api/reports/efficiency`       | Laporan efisiensi (→ monthly)      | ❌   |
| 20| `GET`    | `/api/reports/custom`           | Laporan kustom (PDF/JSON)          | ❌   |
| 21| `GET`    | `/api/reports/pdf/daily`        | PDF laporan harian                 | ❌   |
| 22| `GET`    | `/api/reports/pdf/weekly`       | PDF laporan mingguan               | ❌   |
| 23| `GET`    | `/api/reports/pdf/monthly`      | PDF laporan bulanan                | ❌   |
| 24| `GET`    | `/api/reports/pdf/efficiency`   | PDF laporan efisiensi              | ❌   |
| 25| `GET`    | `/api/energy/daily`             | Konsumsi energi harian             | ❌   |
| 26| `GET`    | `/api/energy/monthly`           | Konsumsi energi bulanan            | ❌   |
| 27| `GET`    | `/api/energy/efficiency`        | Efisiensi energi                   | ❌   |
| 28| `GET`    | `/api/energy/devices`           | Data daya per perangkat            | ❌   |
| 29| `GET`    | `/api/energy/savings/daily`     | Penghematan energi harian          | ❌   |
| 30| `GET`    | `/api/energy/savings/monthly`   | Penghematan energi bulanan         | ❌   |
| 31| `GET`    | `/api/user`                     | Info user (Sanctum auth)           | ✅   |

> **Keterangan Auth**: ❌ = Tidak perlu autentikasi (public), ✅ = Perlu token Sanctum

---

> 📝 **Catatan Penting:**
> - Semua waktu dalam response menggunakan format **ISO 8601** (`YYYY-MM-DDTHH:mm:ss.000000Z`)
> - Tarif listrik yang digunakan: **Rp 1.445/kWh**
> - Perangkat utama yang dimonitor: 2 unit AC Panasonic (1050W), 12 Lampu TL (22W)
> - Base load 24 jam: Dispenser (420W) + Kulkas (90W) + Router WiFi (20W) = 530W
