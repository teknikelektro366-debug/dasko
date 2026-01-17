# 📄 Panduan Download Laporan PDF - Smart Energy Dashboard

## ✅ Fitur Download PDF Sudah Diimplementasikan

### 🎯 Fitur yang Tersedia:

1. **Laporan Harian** - Data sensor dan aktivitas hari ini
2. **Laporan Mingguan** - Ringkasan 7 hari terakhir  
3. **Laporan Bulanan** - Analisis komprehensif bulanan
4. **Laporan Efisiensi** - Evaluasi efisiensi energi
5. **Laporan Kustom** - Generator laporan dengan filter tanggal dan perangkat

### 🔧 Cara Menggunakan:

#### 1. Akses Bagian Laporan
- Buka dashboard di `http://192.168.0.102:8000/dashboard`
- Klik menu **"Laporan"** di sidebar kiri
- Pilih jenis laporan yang diinginkan

#### 2. Download Laporan Cepat
- **Laporan Harian**: Klik tombol "PDF" pada card "Laporan Harian"
- **Laporan Mingguan**: Klik tombol "PDF" pada card "Laporan Mingguan"  
- **Laporan Bulanan**: Klik tombol "PDF" pada card "Laporan Bulanan"
- **Laporan Efisiensi**: Klik tombol "PDF" pada card "Laporan Efisiensi"

#### 3. Generator Laporan Kustom
- Pilih **Tanggal Mulai** dan **Tanggal Akhir**
- Pilih **Jenis Perangkat** (opsional)
- Pilih **Format**: PDF Report
- Klik **"Generate & Download"**

#### 4. Download Cepat
- **Laporan Hari Ini**: Klik link "Laporan Hari Ini"
- **Laporan Minggu Ini**: Klik link "Laporan Minggu Ini"
- **Laporan Bulan Ini**: Klik link "Laporan Bulan Ini"

### 📊 Isi Laporan PDF:

#### Laporan Harian:
- **Header**: Judul, tanggal, informasi universitas
- **Ringkasan**: Total data, suhu rata-rata, kelembaban, orang terdeteksi, penggunaan AC
- **Data Per Jam**: Breakdown aktivitas setiap jam
- **Footer**: Informasi sistem dan copyright

#### Laporan Mingguan:
- **Ringkasan Mingguan**: Statistik 7 hari
- **Data Per Hari**: Breakdown harian dalam seminggu
- **Analisis Tren**: Pola penggunaan energi

#### Laporan Bulanan:
- **Ringkasan Bulanan**: Statistik keseluruhan bulan
- **Data Per Minggu**: Breakdown mingguan
- **Efisiensi Bulanan**: Analisis efisiensi energi

#### Laporan Efisiensi:
- **Skor Efisiensi**: Persentase efisiensi energi
- **Analisis AC**: Penggunaan AC vs okupansi ruangan
- **Rekomendasi**: Peluang penghematan energi

### 🌐 API Endpoints:

```
GET /api/reports/daily?format=pdf&date=2025-01-18
GET /api/reports/weekly?format=pdf&week_start=2025-01-13
GET /api/reports/monthly?format=pdf&month=2025-01
GET /api/reports/efficiency?format=pdf&date_from=2024-12-18&date_to=2025-01-18
GET /api/reports/custom?format=pdf&date_from=2025-01-01&date_to=2025-01-18&device_type=ac1
```

### 🎨 Format PDF:

- **Encoding**: UTF-8 (mendukung bahasa Indonesia)
- **Layout**: Professional dengan header dan footer
- **Styling**: Clean design dengan warna biru tema universitas
- **Tabel**: Data terstruktur dengan styling yang rapi
- **Responsif**: Dapat dibuka di semua perangkat

### 📁 Nama File Download:

- Harian: `laporan_harian_2025_01_18.pdf`
- Mingguan: `laporan_mingguan_2025_01_13.pdf`
- Bulanan: `laporan_bulanan_2025_01.pdf`
- Efisiensi: `laporan_efisiensi_2024_12_18.pdf`
- Kustom: `laporan_kustom_2025_01_01.pdf`

### 🔍 Data yang Disertakan:

#### Sensor Data:
- Suhu ruangan (°C)
- Kelembaban udara (%)
- Intensitas cahaya (lux)
- Jumlah orang terdeteksi
- Status AC (ON/OFF)
- Set temperature AC
- Timestamp dengan zona waktu Indonesia (WIB)

#### Statistik:
- Total records data
- Rata-rata suhu dan kelembaban
- Persentase penggunaan AC
- Jam/hari puncak aktivitas
- Skor efisiensi energi
- Peluang penghematan energi

### ⚡ Fitur Tambahan:

1. **Auto-Generate**: Laporan dibuat otomatis berdasarkan data real-time
2. **Timezone Indonesia**: Semua timestamp menggunakan WIB
3. **Professional Layout**: Design sesuai standar laporan universitas
4. **Multi-Format**: Mendukung PDF, Excel, dan CSV (future enhancement)
5. **Filter Data**: Dapat memfilter berdasarkan perangkat dan tanggal

### 🚀 Cara Testing:

1. **Pastikan Laravel Server Berjalan**:
   ```bash
   php artisan serve --host=0.0.0.0 --port=8000
   ```

2. **Akses Dashboard**:
   ```
   http://192.168.0.102:8000/dashboard
   ```

3. **Test Download**:
   - Klik menu "Laporan"
   - Pilih jenis laporan
   - Klik tombol "PDF"
   - File akan otomatis terdownload

### 🔧 Troubleshooting:

#### Jika Download Tidak Berfungsi:
1. **Cek Server Laravel**: Pastikan `php artisan serve` berjalan
2. **Cek Network**: Pastikan ESP32 dan komputer di jaringan yang sama
3. **Cek Browser**: Pastikan browser mengizinkan download
4. **Cek Data**: Pastikan ada data sensor di database

#### Jika PDF Kosong:
1. **Cek Database**: Pastikan tabel `sensor_data` memiliki data
2. **Cek Tanggal**: Pastikan filter tanggal sesuai dengan data yang ada
3. **Cek Controller**: Pastikan `ReportController` dapat mengakses model `SensorData`

### 📞 Support:

Jika mengalami masalah:
1. Cek log Laravel: `storage/logs/laravel.log`
2. Cek browser console untuk error JavaScript
3. Test API endpoint langsung di browser
4. Pastikan semua dependency terinstall

---

## ✅ Status Implementasi:

- ✅ API Routes untuk laporan
- ✅ ReportController dengan fungsi PDF
- ✅ JavaScript functions untuk download
- ✅ HTML template untuk PDF
- ✅ Styling professional untuk laporan
- ✅ Support timezone Indonesia (WIB)
- ✅ Multiple report types (daily, weekly, monthly, efficiency, custom)
- ✅ Data filtering dan summary statistics

**Fitur download PDF laporan sudah siap digunakan!** 🎉