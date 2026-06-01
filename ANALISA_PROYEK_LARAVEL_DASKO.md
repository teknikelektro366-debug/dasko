# Analisa Proyek Laravel Dasko

Dokumen ini menjelaskan cara kerja proyek Laravel Dasko pada direktori ini. Analisa hanya mencakup bagian Laravel dan aset yang dipakai oleh aplikasi web. File Arduino `.ino`, dokumen Markdown lama, dependency vendor, dan file di luar scope Laravel tidak dimasukkan sebagai sumber utama analisa.

## 1. Gambaran Umum Proyek

Proyek ini adalah aplikasi Laravel untuk sistem monitoring dan manajemen energi ruang dosen. Aplikasi berperan sebagai backend API, dashboard web, panel kontrol AC/lampu, halaman laporan, halaman input manual, dan panel admin.

Secara garis besar, sistem Laravel menerima data dari perangkat IoT, menyimpan data sensor ke database, menampilkan data terbaru pada dashboard, menyediakan histori dan grafik, mengirim perintah kontrol manual ke perangkat, menghitung konsumsi energi, serta membuat laporan PDF.

Komponen utama aplikasi:

- Laravel 12 sebagai framework backend.
- Database relasional untuk menyimpan data sensor, kontrol AC, perangkat, log energi, konsumsi energi, rule otomasi, user, cache, session, dan token API.
- API publik untuk menerima data sensor dan memberi perintah kontrol kepada perangkat IoT.
- Dashboard web untuk monitoring realtime, histori, grafik, dan kontrol perangkat.
- Modul admin untuk pengelolaan user, perangkat, rule AC, energy log, database, log sistem, dan koneksi Arduino.
- Modul laporan PDF berbasis DomPDF.
- JavaScript frontend untuk polling API, update UI realtime, kontrol AC/lampu, histori, grafik, dan input manual.

## 2. Teknologi dan Dependency

Dependency utama backend:

- Laravel Framework versi 12.
- Laravel Sanctum untuk token API.
- DomPDF melalui paket `barryvdh/laravel-dompdf` untuk pembuatan laporan PDF.
- Laravel Tinker untuk interaksi development.

Dependency frontend dari `package.json` menunjukkan adanya penggunaan Chart.js dan beberapa tool build/minify. Akan tetapi, dashboard Laravel juga banyak memakai JavaScript inline pada Blade dan file publik langsung.

## 3. Struktur Modul Laravel

Struktur utama yang dianalisa:

- `routes/web.php`: route halaman web, dashboard, laporan, dan admin.
- `routes/api.php`: route API sensor, dashboard realtime, kontrol AC, laporan, dan energi.
- `app/Http/Controllers`: controller dashboard, API, laporan, energi, admin, dan autentikasi.
- `app/Models`: model data utama seperti `SensorData`, `AcControl`, `Device`, `EnergyLog`, `EnergyConsumption`, `SensorReading`, `ACAutomationRule`, dan `User`.
- `app/Services`: service komunikasi Arduino dan perhitungan energi.
- `database/migrations`: definisi tabel database.
- `database/seeders`: data awal user, device, sensor dummy, rule AC, dan log energi.
- `resources/views`: dashboard, laporan, admin, auth, input manual, dan PDF templates.
- `public/js` dan beberapa file JavaScript publik: realtime dashboard dan kontrol AC.

## 4. Alur Besar Aplikasi

Alur utama aplikasi dapat diringkas sebagai berikut:

1. Perangkat IoT mengirim data sensor ke API `/api/sensor/data`.
2. Backend memvalidasi data, menormalisasi status AC/lampu, lalu menyimpan data sebagai record baru di tabel `sensor_data`.
3. Dashboard mengambil data terbaru melalui `/api/sensor/latest` atau `/api/dashboard/realtime`.
4. Dashboard juga mengambil data chart, statistik harian, histori sensor, dan histori kontrol AC.
5. Pengguna dapat mengirim perintah kontrol AC/lampu manual melalui `/api/ac/control`.
6. Perangkat IoT melakukan polling ke `/api/ac/control` untuk mengambil perintah aktif.
7. Jika mode auto aktif, perangkat tetap mengontrol AC/lampu berdasarkan jumlah orang.
8. Jika mode manual aktif, perangkat mengikuti status AC/lampu yang dikirim dari dashboard.
9. Modul energi menghitung konsumsi listrik dari data sensor atau dari tabel perangkat/log energi.
10. Modul laporan mengambil data sensor atau energy log dalam rentang tanggal tertentu lalu membuat ringkasan dan PDF.
11. Panel admin menyediakan pengelolaan user, perangkat, rule otomasi, log energi, database, dan koneksi Arduino.

## 5. Route Web

Route web digunakan untuk halaman yang diakses browser.

### 5.1 Route Publik

- `/` mengarah ke halaman dashboard.
- `/dashboard` menampilkan dashboard utama Laravel.
- `/dashboard.html` dan `/index.html` menampilkan dashboard statis.
- `/manual-input` menampilkan form input data manual satu record.
- `/manual-input-bulk` menampilkan form input manual beberapa jam sekaligus.

Autentikasi web sedang dinonaktifkan pada route aktif. Route login/logout ada di file route, tetapi dikomentari.

### 5.2 Route Dashboard

Dashboard memiliki beberapa halaman:

- Halaman utama dashboard.
- Analytics.
- History.
- Devices.
- Automation.
- Settings.

Controller utamanya adalah `DashboardController`.

### 5.3 Route Laporan

Route laporan web menggunakan `ReportController` untuk:

- Index laporan.
- Laporan energi rentang tanggal.
- Laporan harian.
- Laporan mingguan.
- Laporan bulanan.
- Laporan per perangkat.
- Laporan efisiensi.

Route ini lebih berorientasi ke laporan berbasis `EnergyLog` dan halaman Blade.

### 5.4 Route Admin

Route admin berada pada prefix `/admin`. Modul yang tersedia:

- Dashboard admin.
- Overview sistem.
- User management.
- Device management.
- AC automation rules.
- Energy logs.
- Settings.
- Arduino management.
- Database management.
- System logs.

Middleware admin juga tersedia, tetapi pada route aktif middleware admin sedang tidak dipasang karena route admin tidak dibungkus middleware autentikasi/admin.

## 6. Route API

Route API adalah jalur utama komunikasi data.

### 6.1 API Sensor

Prefix `/api/sensor` menyediakan:

- `POST /data`: menerima data sensor dari perangkat atau input manual.
- `GET /data`: mengambil data sensor dengan pagination.
- `GET /latest`: mengambil data sensor terbaru.
- `GET /chart`: mengambil data chart.
- `GET /stats`: mengambil statistik harian.
- `GET /history`: mengambil histori sensor dengan pagination.
- `POST /cleanup`: menghapus data lama.

Controller utama: `SensorDataController`.

### 6.2 API Dashboard

Prefix `/api/dashboard` menyediakan alias untuk data realtime, chart, dan statistik harian.

### 6.3 API Kontrol AC dan Lampu

Prefix `/api/ac` menyediakan:

- `GET /control`: perangkat IoT mengambil kontrol aktif.
- `POST /control`: dashboard menyimpan kontrol manual/auto.
- `POST /emergency-stop`: mematikan AC dan lampu secara manual darurat.
- `POST /auto-mode`: mengembalikan mode kontrol ke otomatis.
- `GET /history`: mengambil histori kontrol.

Controller utama: `AcControlController`.

### 6.4 API Laporan

Prefix `/api/reports` menyediakan laporan harian, mingguan, bulanan, efisiensi, PDF, custom report, dan analytics chart report.

Laporan PDF utama ditangani oleh `PdfReportController`.

### 6.5 API Energi

Prefix `/api/energy` menyediakan:

- Konsumsi energi harian.
- Konsumsi energi bulanan.
- Efisiensi.
- Data daya perangkat.
- Penghematan harian.
- Penghematan bulanan.

Controller utama: `EnergyController`, dengan perhitungan di `EnergyCalculationService`.

## 7. Database dan Model Utama

### 7.1 Tabel `sensor_data`

Tabel ini adalah pusat data realtime dan histori sensor. Field pentingnya:

- Device ID.
- Lokasi.
- Jumlah orang.
- Status AC umum.
- Status AC 1 dan AC 2.
- Set temperature.
- Suhu ruangan.
- Kelembaban.
- Level cahaya.
- Status lampu.
- Proximity masuk dan keluar.
- RSSI WiFi.
- Status perangkat.
- Timestamp dari perangkat.
- Timestamp server.

Model `SensorData` menyediakan scope data terbaru, data hari ini, data satu jam terakhir, filter device, statistik harian, chart data, cleanup data lama, dan formatter tampilan.

### 7.2 Tabel `ac_controls`

Tabel ini menyimpan histori dan status kontrol AC/lampu dari dashboard.

Field pentingnya:

- Device ID.
- Lokasi.
- Status AC 1.
- Status AC 2.
- Status lampu.
- Temperature AC 1.
- Temperature AC 2.
- Mode kontrol umum.
- Mode kontrol AC.
- Mode kontrol lampu.
- Manual override umum.
- Manual override AC.
- Manual override lampu.
- Pembuat perintah.
- Waktu kedaluwarsa.

Model `AcControl` menyediakan pencarian kontrol aktif, pembuatan kontrol baru, pengecekan expired, format status, dan ringkasan kontrol.

Saat kontrol baru dibuat, kontrol lama untuk device dan lokasi yang sama ditandai expired. Dengan demikian, hanya kontrol terbaru yang masih aktif yang dipakai perangkat.

### 7.3 Tabel `devices`

Tabel ini menyimpan master perangkat seperti AC, lampu, komputer, TV, dispenser, kulkas, router, panel, dan perangkat lain.

Field pentingnya:

- Nama perangkat.
- Tipe perangkat.
- Daya perangkat.
- Jumlah unit.
- Lokasi.
- Status aktif.
- Status selalu aktif.
- GPIO pin.
- Relay pin.
- Deskripsi.

Model `Device` memiliki relasi ke sensor reading dan energy log, menghitung total konsumsi daya perangkat, menghitung energi hari ini, serta scope perangkat aktif dan filter tipe.

### 7.4 Tabel `energy_logs`

Tabel ini menyimpan log pemakaian energi per perangkat.

Field pentingnya:

- Device ID.
- Energi yang dikonsumsi.
- Durasi pemakaian.
- Waktu mulai dan selesai.
- Jumlah orang.
- Suhu luar atau suhu lingkungan.
- Target suhu AC.
- Mode hemat energi.
- Status jam kerja.
- Estimasi biaya.

Model `EnergyLog` menyediakan scope hari ini, minggu ini, bulan ini, filter tipe perangkat, dan rating efisiensi berbasis energi per orang.

### 7.5 Tabel `energy_consumptions`

Tabel ini digunakan untuk rekap konsumsi energi yang lebih agregatif per tanggal, lokasi, dan tipe perangkat.

Model `EnergyConsumption` menyediakan ringkasan harian, mingguan, breakdown perangkat, tren efisiensi, penghematan dibanding baseline, analisa peak hour, dan formatter tampilan.

### 7.6 Tabel `sensor_readings`

Tabel ini menyimpan pembacaan sensor generik seperti suhu, kelembaban, cahaya, proximity, dan jumlah orang. Tabel ini lebih banyak dipakai oleh modul `ArduinoController` dan data dummy lama, sedangkan alur IoT utama saat ini memakai `sensor_data`.

### 7.7 Tabel `ac_automation_rules`

Tabel ini menyimpan rule otomasi AC berdasarkan rentang jumlah orang dan rentang suhu.

Field pentingnya:

- Minimum dan maksimum jumlah orang.
- Minimum dan maksimum suhu.
- Jumlah AC yang dibutuhkan.
- Target suhu.
- Deskripsi.
- Status aktif.

Model `ACAutomationRule` dapat mencari rule aktif yang cocok dengan kondisi jumlah orang dan suhu tertentu.

### 7.8 Tabel `users`

User memiliki role, status aktif, dan waktu login terakhir. Role yang dipakai antara lain admin, operator, dan viewer. Model `User` menyediakan pengecekan admin dan scope user aktif.

## 8. Alur Penerimaan Data Sensor

Alur `POST /api/sensor/data`:

1. Request diterima oleh `SensorDataController`.
2. Data request dicatat ke log untuk debugging.
3. Validator memeriksa field wajib dan rentang nilai.
4. Device ID dan lokasi diambil dari request atau menggunakan default.
5. Jumlah orang dibatasi maksimal 25 pada sisi backend.
6. Jika waktu berada pada rentang reset malam, jumlah orang dianggap 0.
7. Status lampu dinormalisasi menjadi ON atau OFF.
8. Status AC dinormalisasi menjadi OFF, ON, atau AC 1+2.
9. Status AC 1 dan AC 2 ditentukan dari input eksplisit atau diturunkan dari status AC umum.
10. Set temperature diambil dari request atau ditentukan berdasarkan jumlah orang.
11. Data disimpan sebagai record baru di `sensor_data`.
12. Jika masih ada constraint unik lama di database, sistem fallback ke update-or-create.
13. Response JSON dikembalikan dengan ID data dan status penyimpanan.

Validasi penting pada data sensor:

- Jumlah orang wajib integer, minimal 0 dan maksimal 25.
- Status AC wajib string.
- Set temperature boleh kosong, minimal 0 dan maksimal 30.
- Suhu ruangan boleh kosong, minimal -10 dan maksimal 60.
- Kelembaban boleh kosong, minimal 0 dan maksimal 100.
- Level cahaya boleh kosong, minimal 0 dan maksimal 1000.
- RSSI WiFi boleh kosong, minimal -100 dan maksimal 0.
- Timestamp perangkat boleh kosong.

## 9. Alur Data Realtime Dashboard

Dashboard realtime mengambil data terbaru melalui API sensor latest. Backend mencari record terbaru berdasarkan filter device ID dan lokasi jika dikirim.

Response realtime berisi:

- ID record.
- Device ID.
- Lokasi.
- Jumlah orang terkini.
- Status AC normal.
- Status AC 1 dan AC 2.
- Set temperature.
- Suhu ruangan.
- Kelembaban.
- Level cahaya.
- Status lampu.
- Proximity masuk dan keluar.
- RSSI WiFi.
- Status perangkat.
- Timestamp server.
- Format tampilan suhu, kelembaban, cahaya, dan WiFi.
- Informasi realtime seperti usia data, frekuensi update, dan kualitas koneksi.

Data dianggap recent jika umur data tidak lebih dari 2 menit. Jika lebih dari itu, connection quality dianggap poor.

Di frontend, modul realtime melakukan polling berkala, default sekitar 3 detik. Jika halaman tidak aktif, interval diperbesar. Jika request gagal berulang, status koneksi berubah menjadi offline atau connecting.

## 10. Logika Kontrol AC dan Lampu

### 10.1 Mode Otomatis

Dalam alur otomatis, status AC dan target suhu didasarkan pada jumlah orang.

Aturan target suhu yang digunakan di backend dan frontend:

- 0 orang: AC OFF, target suhu kosong.
- 1 sampai 5 orang: target 22 derajat Celsius.
- 6 sampai 15 orang: target 20 derajat Celsius.
- Lebih dari 15 orang: target 18 derajat Celsius.

Status AC untuk API dan dashboard:

- 0 orang: OFF.
- 1 sampai 10 orang: ON.
- Lebih dari 10 orang: AC 1+2.

Catatan: pada sisi perangkat IoT, aturan fisik AC lebih rinci karena 6 sampai 10 orang masih memakai satu AC, sedangkan lebih dari 10 orang memakai dua AC. Backend mengikuti normalisasi status umum agar cocok dengan validasi API dan tampilan dashboard.

Lampu otomatis ditentukan dari jumlah orang:

- Jika jumlah orang lebih dari 0, lampu ON.
- Jika jumlah orang 0, lampu OFF.

### 10.2 Mode Manual dari Dashboard

Dashboard dapat mengirim status manual ke `POST /api/ac/control`.

Data kontrol mencakup:

- Status AC 1 dan AC 2.
- Status lampu.
- Temperature AC 1 dan AC 2.
- Mode kontrol umum.
- Mode khusus AC.
- Mode khusus lampu.
- Flag manual override per perangkat.
- Durasi berlaku opsional.
- Pembuat perintah.

Jika salah satu perangkat berada pada mode manual, kontrol umum dianggap manual. Jika tidak ada manual override, kontrol umum dianggap auto.

### 10.3 Kontrol Kedaluwarsa

Kontrol dapat memiliki `expires_at`. Perangkat hanya menerima kontrol aktif yang belum expired. Jika tidak ada kontrol aktif atau kontrol sudah expired, API mengembalikan mode auto.

### 10.4 Emergency Stop

Endpoint emergency stop membuat kontrol manual dengan semua AC dan lampu OFF. Mode AC dan lampu diset manual agar perangkat mengikuti perintah shutdown.

### 10.5 Auto Mode

Endpoint auto mode meng-expire semua kontrol aktif untuk device dan lokasi tersebut. Setelah itu perangkat kembali mengontrol AC/lampu berdasarkan algoritma otomatis.

## 11. Alur Konfirmasi Kontrol di Dashboard

Dashboard utama memiliki logika konfirmasi perintah IoT:

1. Dashboard membaca status terbaru dari sensor latest.
2. Pengguna mengubah mode AC atau lampu ke manual.
3. Pengguna mengirim perintah ON/OFF atau perubahan suhu.
4. Dashboard menyimpan perintah ke API kontrol.
5. Dashboard menunggu perangkat mengirim data sensor baru.
6. Jika record sensor terbaru lebih baru dari baseline dan state sesuai target, kontrol dianggap berhasil.
7. Jika perangkat tidak mengirim status sesuai target dalam timeout, dashboard menampilkan error dan mengembalikan UI ke state sebelumnya.

Timeout konfirmasi pada JavaScript dashboard adalah sekitar 30 detik, dengan polling sekitar 1 detik.

## 12. Input Manual

Aplikasi menyediakan dua halaman input manual:

- Input manual satu record.
- Input manual bulk berdasarkan pola jam tertentu.

Input manual membentuk payload ke API sensor dengan device ID `MANUAL_INPUT`. Nilai suhu, kelembaban, cahaya, AC, set temperature, dan lampu diturunkan dari jumlah orang.

Aturan input manual:

- Jumlah orang dibatasi maksimal 25.
- Lampu ON jika jumlah orang lebih dari 0.
- Lampu OFF jika jumlah orang 0.
- AC OFF jika jumlah orang 0.
- 1 sampai 5 orang: AC ON, suhu 22.
- 6 sampai 10 orang: AC ON, suhu 20.
- 11 sampai 15 orang: AC 1+2, suhu 20.
- 16 orang atau lebih: AC 1+2, suhu 18.
- Cahaya default 500 lux jika ada orang dan 0 lux jika kosong.
- Suhu ruangan default 27 dan kelembaban default 65.

Input bulk memakai pola jam kerja 07:00 sampai 15:00 dengan nilai jumlah orang awal yang sudah disiapkan, lalu mengirim satu record per jam.

## 13. Dashboard dan Frontend

Dashboard utama memakai Blade dengan JavaScript inline yang cukup besar. Fitur yang tersedia:

- Sidebar dan section dashboard.
- Kartu jumlah orang, status AC, set temperature, suhu ruangan, kelembaban, cahaya, dan status lampu.
- Chart harian untuk jumlah orang, suhu AC, kelembaban, cahaya, dan suhu ruangan.
- Kontrol mode AC dan lampu auto/manual.
- Kontrol manual AC 1, AC 2, lampu, dan suhu AC.
- Histori data sensor.
- Histori aktivitas AC.
- Data jumlah orang.
- Data lingkungan.
- Event sistem.
- Download laporan PDF.

File `public/js/realtime-dashboard.js` menyediakan modul polling realtime yang mengambil `/api/sensor/latest`, memperbarui elemen dashboard, menghitung frekuensi update lokal, mengambil total record secara periodik, dan menampilkan kualitas sinyal WiFi.

File `public/ac-control.js` dan `public/ac-control-hosting.js` menyediakan kontrol AC manual versi lama/hosting. Sebagian identitas device dan lokasi pada file ini masih memakai nama lama seperti `ESP32_Smart_Energy_ChangeDetection` dan `Lab Teknik Tegangan Tinggi`, sedangkan alur dashboard utama memakai `UNJA_Prodi_Elektro` dan `Ruang Dosen Prodi Teknik Elektro`.

## 14. Modul Laporan

Ada dua jalur laporan utama:

- `ReportController` untuk laporan berbasis `EnergyLog` dan halaman web laporan.
- `PdfReportController` untuk laporan PDF berbasis `SensorData`.

### 14.1 Laporan PDF Harian

Laporan harian mengambil data `sensor_data` dalam satu hari. Ringkasan yang dihitung:

- Total record.
- Rata-rata jumlah orang.
- Jumlah orang maksimum.
- Rata-rata suhu ruangan.
- Rata-rata kelembaban.
- Rata-rata cahaya.
- Jumlah record saat AC ON.
- Jumlah record saat lampu ON.

Untuk mencegah PDF terlalu besar, laporan harian dibagi menjadi beberapa part jika jumlah record melebihi batas detail per file. Batas detailnya 300 record per PDF.

### 14.2 Laporan Mingguan

Laporan mingguan mengambil data dalam rentang satu minggu. Ringkasan dibuat per hari dengan jumlah record, rata-rata orang, dan rata-rata suhu.

### 14.3 Laporan Bulanan

Laporan bulanan mengambil data dalam rentang satu bulan. Ringkasan dibuat per minggu dengan jumlah record, rata-rata orang, dan rata-rata suhu.

### 14.4 Laporan Efisiensi

Laporan efisiensi menghitung persentase penggunaan AC dan lampu berdasarkan jumlah record status ON dibanding jumlah status valid. Laporan ini juga menghitung rata-rata orang saat AC ON, rata-rata orang saat lampu ON, rata-rata suhu, dan rata-rata kelembaban.

### 14.5 Laporan Custom

Laporan custom menerima rentang tanggal, filter device, dan format PDF atau JSON. Jika format JSON dipilih, data dan ringkasan dikembalikan sebagai JSON. Jika PDF dipilih, laporan dirender memakai view PDF.

### 14.6 Analytics Chart Report

`PdfReportController` juga memiliki laporan chart analytics. Controller membangun data per jam dan dapat membuat gambar chart secara server-side untuk disisipkan ke PDF.

## 15. Modul Energi

Perhitungan energi utama berada pada `EnergyCalculationService`.

### 15.1 Data Perangkat Energi

Service menyimpan daftar perangkat dan daya acuan, antara lain:

- AC Panasonic sebanyak 2 unit, masing-masing 1050 watt.
- Lampu TL sebanyak 12 unit, masing-masing 22 watt.
- Dispenser 420 watt.
- Smart TV 170 watt.
- Kulkas 90 watt.
- Komputer 2 unit, masing-masing 90 watt.
- Router WiFi 20 watt.
- Panci listrik 300 watt.
- Setrika listrik 350 watt.
- Mesin kopi 1350 watt.

Tarif listrik pada service adalah Rp 1.445 per kWh. Pada konfigurasi `energy.php` terdapat nilai tarif lain yaitu Rp 1.467 per kWh, dan pada model `EnergyConsumption` ada default Rp 1.500 per kWh. Ini berarti ada beberapa sumber tarif yang berbeda di proyek.

### 15.2 Perhitungan AC

Jika status AC OFF, konsumsi AC dianggap 0.

Jika AC aktif:

- Satu AC memakai daya 1050 watt.
- Dua AC memakai daya 2100 watt.
- Lama pemakaian dihitung dari selisih waktu mulai dan waktu selesai dalam jam.
- Energi Wh dihitung dari daya dikalikan jam.
- Energi kWh dihitung dari Wh dibagi 1000.
- Biaya dihitung dari kWh dikalikan tarif.

### 15.3 Perhitungan Lampu

Jika status lampu OFF, konsumsi lampu dianggap 0.

Jika lampu aktif:

- Total daya lampu adalah 12 lampu dikali 22 watt, yaitu 264 watt.
- Energi Wh dihitung dari 264 watt dikalikan jam pemakaian.
- Energi kWh dihitung dari Wh dibagi 1000.
- Biaya dihitung dari kWh dikalikan tarif.

### 15.4 Perhitungan Energi Harian

Perhitungan harian mengambil semua record `sensor_data` dalam satu tanggal, urut dari awal ke akhir. Sistem menghitung konsumsi dari pasangan record berurutan:

- Status pada record saat ini dianggap berlaku sampai record berikutnya.
- Selisih waktu antara dua record menjadi durasi pemakaian.
- Konsumsi AC dihitung dari status AC pada record saat ini.
- Konsumsi lampu dihitung dari status lampu pada record saat ini.
- Total AC dan lampu dijumlahkan sepanjang hari.

Selain AC dan lampu, ada konsumsi dasar perangkat selalu aktif:

- Dispenser 420 watt.
- Kulkas 90 watt.
- Router 20 watt.
- Total dasar 530 watt.
- Konsumsi dasar harian adalah 530 watt dikalikan 24 jam, yaitu 12.720 Wh atau 12,72 kWh.

Total energi harian adalah energi AC ditambah energi lampu ditambah energi dasar.

### 15.5 Perhitungan Energi Bulanan

Perhitungan bulanan menjumlahkan perhitungan harian dari awal bulan sampai akhir bulan. Hasil yang dihitung:

- Total kWh AC.
- Total biaya AC.
- Total kWh lampu.
- Total biaya lampu.
- Total kWh dasar.
- Total biaya dasar.
- Total kWh keseluruhan.
- Total biaya keseluruhan.
- Detail harian.

### 15.6 Perhitungan Penghematan Energi

Baseline maksimum penghematan dihitung dengan asumsi AC dan lampu aktif penuh 8 jam:

- Dua AC 1050 watt selama 8 jam menghasilkan 16,8 kWh.
- Dua belas lampu 22 watt selama 8 jam menghasilkan 2,112 kWh.
- Total baseline AC dan lampu adalah 18,912 kWh.

Penghematan dihitung sebagai baseline maksimum dikurangi konsumsi aktual AC dan lampu dari data sensor.

Persentase penghematan dihitung dari energi hemat dibagi baseline maksimum, lalu dikalikan 100 persen.

### 15.7 Data Daya Perangkat

Service juga menghitung konsumsi harian teoritis tiap perangkat:

- Wh = watt per unit dikali jumlah unit dikali jam operasi.
- kWh = Wh dibagi 1000.
- biaya = kWh dikali tarif.

## 16. Rumus yang Digunakan

Rumus-rumus utama proyek:

### 16.1 Energi Wh

Energi dalam Wh dihitung dari daya dalam watt dikalikan lama pemakaian dalam jam.

### 16.2 Energi kWh

Energi dalam kWh dihitung dari Wh dibagi 1000.

### 16.3 Biaya Listrik

Biaya listrik dihitung dari kWh dikalikan tarif per kWh.

### 16.4 Konsumsi Perangkat Multi Unit

Energi perangkat multi unit dihitung dari watt per unit dikalikan jumlah unit dikalikan jam pemakaian.

### 16.5 Konsumsi Berdasarkan Log Menit

Pada log energi berbasis durasi menit, energi Wh dihitung dari daya perangkat dikalikan durasi menit, lalu dibagi 60.

### 16.6 Rata-rata Data Sensor

Rata-rata suhu, kelembaban, cahaya, dan jumlah orang dihitung dari total nilai dibagi jumlah record valid.

### 16.7 Persentase AC ON

Persentase AC ON dihitung dari jumlah record AC aktif dibagi total record, lalu dikalikan 100 persen.

### 16.8 Persentase Lampu ON

Persentase lampu ON dihitung dari jumlah record lampu ON dibagi total record atau total status valid, lalu dikalikan 100 persen.

### 16.9 Frekuensi Update

Frekuensi update dihitung dari rata-rata selisih waktu antar record terbaru.

### 16.10 Usia Data

Usia data dihitung dari waktu sekarang dikurangi timestamp record terbaru.

### 16.11 Kualitas Koneksi Realtime

Data dianggap baik jika usia data terbaru tidak lebih dari 2 menit. Jika lebih dari 2 menit, kualitas koneksi dianggap buruk.

### 16.12 Rating WiFi

RSSI WiFi dikategorikan berdasarkan ambang dBm:

- Sangat kuat atau excellent pada nilai sekitar -30 sampai -50 dBm.
- Good pada sekitar -50 dBm.
- Fair pada sekitar -60 dBm.
- Weak pada sekitar -70 dBm.
- Very weak atau poor jika lebih lemah dari itu.

Ada sedikit perbedaan label antara model dan dashboard, tetapi konsepnya sama: semakin mendekati 0, sinyal semakin baik.

### 16.13 Efisiensi Energi per Orang

Efisiensi pada `EnergyLog` dihitung dari energi dikonsumsi dibagi jumlah orang. Nilai yang lebih kecil dianggap lebih baik.

Kategori rating pada energy log:

- Kurang dari 50: Excellent.
- Kurang dari 100: Good.
- Kurang dari 150: Average.
- Selain itu: Poor.

### 16.14 Efisiensi pada `EnergyConsumption`

Efisiensi agregat dinilai dari persentase efisiensi:

- Minimal 80 persen: Sangat Efisien.
- Minimal 60 persen: Efisien.
- Minimal 40 persen: Cukup Efisien.
- Di bawah 40 persen: Kurang Efisien.

### 16.15 Progress Gradual Mode

Progress gradual mode dihitung dari menit berjalan dibagi total durasi 15 menit, lalu dikalikan 100 persen. Nilainya dibatasi maksimal 100 persen.

### 16.16 Storage Usage

Status kesehatan storage dihitung dari ruang terpakai dibagi total ruang, lalu dikalikan 100 persen.

Ambang health storage:

- Lebih dari 90 persen: critical.
- Lebih dari 75 persen: warning.
- Selain itu: healthy.

## 17. Pengkondisian Utama

### 17.1 Pengkondisian Data Sensor

- Jika validasi request gagal, API mengembalikan status 422.
- Jika tidak ada data terbaru, API realtime mengembalikan 404.
- Jika waktu saat ini berada antara 18:00 sampai sebelum 05:00 WIB, jumlah orang dianggap 0 oleh backend.
- Jika status AC tidak cocok dengan daftar nilai yang dikenal, status dinormalisasi menjadi OFF.
- Jika status lampu bukan ON, status dinormalisasi menjadi OFF.
- Jika status AC unit tidak dikirim, backend menurunkannya dari status AC umum.
- Jika `created_at` dikirim dari input manual, timestamp server record mengikuti nilai tersebut.
- Jika terjadi unique constraint lama pada `sensor_data`, backend fallback ke update record existing.

### 17.2 Pengkondisian Jumlah Orang

- Backend sensor membatasi jumlah orang maksimal 25.
- Model dan dashboard juga membatasi jumlah orang tampilan maksimal 25.
- Rule AC otomatis dapat memakai rentang jumlah orang sampai 100 pada modul admin rule.
- Seeder rule lama memakai rentang sampai 50.

### 17.3 Pengkondisian AC Otomatis

- Jika jumlah orang 0, target suhu kosong dan status AC OFF.
- Jika jumlah orang 1 sampai 5, target suhu 22.
- Jika jumlah orang 6 sampai 15, target suhu 20.
- Jika jumlah orang di atas 15, target suhu 18.
- Jika jumlah orang 1 sampai 10, status AC dashboard adalah ON.
- Jika jumlah orang lebih dari 10, status AC dashboard adalah AC 1+2.

### 17.4 Pengkondisian Lampu

- Jika jumlah orang lebih dari 0, lampu dianggap ON.
- Jika jumlah orang 0, lampu dianggap OFF.
- Pada histori dashboard, status lampu sering diturunkan ulang dari jumlah orang, bukan hanya dari field `lamp_status`.

### 17.5 Pengkondisian Kontrol Manual

- Kontrol AC manual hanya bisa dikirim dari dashboard jika mode AC manual aktif.
- Kontrol lampu manual hanya bisa dikirim dari dashboard jika mode lampu manual aktif.
- Jika mode belum manual, UI menolak perintah dan meminta pengguna memindahkan mode terlebih dahulu.
- Jika perintah tersimpan tetapi perangkat tidak mengirim status baru sesuai target, dashboard menganggap kontrol gagal.

### 17.6 Pengkondisian Kontrol Aktif

- Jika tidak ada kontrol aktif, API mengembalikan mode auto.
- Jika kontrol sudah expired, API juga mengembalikan mode auto.
- Saat kontrol baru dibuat, kontrol lama untuk device dan lokasi yang sama diberi `expires_at` saat ini.

### 17.7 Pengkondisian Request dan Pagination

- Per page data sensor index dibatasi 5 sampai 100.
- Per page history sensor dibatasi 10 sampai 100.
- Data chart membatasi rentang jam antara 1 sampai 168 jam.
- Cleanup sensor membatasi umur data antara 7 sampai 365 hari.
- Laporan PDF harian membatasi detail 300 record per file.

### 17.8 Pengkondisian Laporan

- Jika format tanggal tidak valid, laporan PDF mengembalikan error validasi.
- Jika `date_to` lebih kecil dari `date_from`, custom report mengembalikan error.
- Jika format bukan PDF atau JSON pada custom report, request ditolak.
- Jika HTML PDF terlalu besar, proses render PDF dicegah agar tidak membebani hosting.

### 17.9 Pengkondisian Admin

- User terakhir dengan role admin tidak boleh dihapus.
- User tidak boleh menghapus atau menonaktifkan akun sendiri.
- Device yang selalu aktif tidak boleh dihapus.
- Device yang aktif akan dimatikan dulu sebelum dihapus.
- Bulk action tidak mematikan atau menghapus device always-on.
- Cleanup energy log dapat menghapus data lama, data test tidak realistis, atau seluruh data.

### 17.10 Pengkondisian Jam Kerja dan Hemat Energi

- Jam kerja default 07:00 sampai 17:00.
- Jika working days enabled, hanya hari kerja yang dianggap hari kerja.
- Jika tidak ada orang selama delay hemat energi, status menjadi Energy Saving Mode.
- Jika tidak ada orang selama delay shutdown panel, status menjadi Panel Shutdown.
- Delay default hemat energi adalah 15 menit.
- Delay default shutdown panel adalah 30 menit.
- Suhu hemat energi default adalah 28 derajat Celsius.

### 17.11 Pengkondisian Kesehatan Sistem

- Database sehat jika koneksi PDO berhasil.
- Storage critical jika pemakaian lebih dari 90 persen.
- Storage warning jika pemakaian lebih dari 75 persen.
- Cache sehat jika test put/get cache berhasil.
- Status Arduino diambil dari cache hasil test koneksi.

## 18. Service dan Fungsi Utama

### 18.1 `SensorDataController`

Fungsi utama:

- `index`: mengambil data sensor dengan pagination.
- `store`: menerima dan menyimpan data sensor.
- `latest`: alias data realtime.
- `realtime`: mengambil record sensor terbaru untuk dashboard.
- `chartData`: membuat data chart per jam.
- `dailyStats`: mengambil statistik harian.
- `history`: mengambil histori sensor dengan pagination dan filter device.
- `cleanup`: menghapus data sensor lama.
- Fungsi privat normalisasi AC, lampu, status unit AC, jumlah orang, reset malam, target suhu, dan frekuensi update.

### 18.2 `AcControlController`

Fungsi utama:

- `getControl`: memberi kontrol aktif kepada perangkat IoT.
- `setControl`: menyimpan kontrol dari dashboard.
- `emergencyStop`: membuat perintah shutdown manual.
- `autoMode`: mengembalikan perangkat ke mode auto.
- `history`: mengambil histori kontrol.

### 18.3 `DashboardController`

Fungsi utama:

- `index`: menyusun data dashboard utama.
- `analytics`: menyusun data grafik energi mingguan, bulanan, dan efisiensi.
- `history`: menampilkan histori energy log.
- `devices`: menampilkan perangkat dan status Arduino.
- `automation`: menampilkan rule AC dan rule aktif.
- `settings`: menampilkan konfigurasi aplikasi.
- Fungsi privat mengambil data sensor terbaru, target suhu, reset malam, status jam kerja, status hemat energi, kondisi terkini, kualitas WiFi, dan frekuensi update.

### 18.4 `EnergyCalculationService`

Fungsi utama:

- `calculateACEnergy`: menghitung energi AC berdasarkan status dan durasi.
- `calculateLampEnergy`: menghitung energi lampu berdasarkan status dan durasi.
- `calculateDailyEnergy`: menghitung energi harian dari data sensor.
- `calculateMonthlyEnergy`: menghitung energi bulanan dari total harian.
- `getDevicePowerData`: menghitung konsumsi teoritis tiap perangkat.
- `getTotalDailyConsumption`: menghitung total konsumsi harian teoritis semua perangkat.
- `getTotalDailyCost`: menghitung total biaya harian teoritis.
- `calculateEnergySavings`: menghitung penghematan harian dibanding baseline.
- `calculateMonthlySavings`: menghitung penghematan bulanan.

### 18.5 `EnergyController`

Fungsi utama:

- `getDailyEnergy`: API energi harian.
- `getMonthlyEnergy`: API energi bulanan.
- `getEfficiency`: API efisiensi.
- `getDevicePowerData`: API data daya perangkat.
- `getDailySavings`: API penghematan harian.
- `getMonthlySavings`: API penghematan bulanan.

Catatan: `getEfficiency` memanggil fungsi `calculateEfficiency` pada service, tetapi fungsi tersebut tidak ditemukan pada `EnergyCalculationService` yang dianalisa. Endpoint ini berpotensi error jika dipanggil.

### 18.6 `PdfReportController`

Fungsi utama:

- `dailyReport`: membuat laporan PDF harian.
- `weeklyReport`: membuat laporan PDF mingguan.
- `monthlyReport`: membuat laporan PDF bulanan.
- `efficiencyReport`: membuat laporan PDF efisiensi.
- `customReport`: membuat laporan custom PDF atau JSON.
- `analyticsChartReport`: membuat laporan chart analytics.
- Fungsi privat ringkasan harian, mingguan, bulanan, efisiensi, rata-rata data, status ON, render PDF, fallback PDF, dan pembuatan chart.

### 18.7 `ReportController`

Fungsi utama:

- `index`: halaman index laporan.
- `energyReport`: laporan energi rentang tanggal.
- `dailyReport`: laporan harian energy log.
- `weeklyReport`: laporan mingguan energy log.
- `monthlyReport`: laporan bulanan energy log.
- `deviceReport`: laporan per device.
- `efficiencyReport`: laporan efisiensi berbasis energy log.
- Fungsi privat summary energi, daily, weekly, monthly, device, efficiency, export PDF/Excel/CSV.

### 18.8 `ArduinoService`

Fungsi utama:

- `testConnection`: mengetes koneksi ke perangkat Arduino/ESP32.
- `getConnectionStatus`: mengambil status koneksi dari cache.
- `sendCommand`: mengirim perintah kontrol perangkat.
- `sendIRCommand`: mengirim perintah IR.
- `sendTemperatureCommand`: mengirim perintah suhu AC.
- `getSensorData`: mengambil data sensor langsung dari perangkat.
- `getACAutomationStatus`: mengambil status otomasi AC dari perangkat.
- `updateWorkingHours`: mengirim konfigurasi jam kerja ke perangkat.
- `updateEnergySaving`: mengirim konfigurasi hemat energi ke perangkat.
- `emergencyShutdown`: mengirim perintah shutdown darurat.

### 18.9 `ArduinoController`

Controller ini menyediakan alur lama/generik untuk menerima sensor reading, kontrol device, status sistem, dan otomasi AC. Namun route aktif di `routes/api.php` tidak mendaftarkan endpoint untuk `ArduinoController`. Beberapa view lama masih mencoba memanggil endpoint seperti `/api/arduino/sensor-data` atau `/api/devices/.../control`, tetapi route tersebut tidak terlihat aktif pada file route yang dianalisa.

### 18.10 Controller Admin

- `AdminController`: dashboard admin, overview, settings, Arduino, database, backup, cleanup, reset, log, dan health check.
- `DeviceController`: CRUD perangkat, toggle, bulk action, dan rating efisiensi perangkat.
- `ACRuleController`: CRUD rule otomasi AC, validasi overlap rule, import, export, toggle, dan statistik rule.
- `EnergyLogController`: list, detail, hapus, cleanup, export CSV, dan statistik log energi.
- `UserController`: CRUD user, toggle status, proteksi admin terakhir, dan proteksi akun sendiri.

### 18.11 Auth dan Middleware

- `LoginController`: login berbasis email, active user check, rate limiting 5 percobaan, redirect admin ke `/admin`, logout, dan update `last_login_at`.
- `AdminMiddleware`: memastikan user login dan role admin.

Catatan: route autentikasi dan middleware admin sedang tidak aktif pada `web.php`, sehingga fungsi ini tersedia di kode tetapi tidak menjadi pengaman route aktif saat ini.

## 19. Seeder dan Data Awal

Seeder membuat:

- Admin user.
- Operator user.
- Data perangkat awal.
- Rule otomasi AC awal.
- Sensor reading dummy 24 jam.
- Energy log dummy untuk perangkat aktif.

Data perangkat awal mencakup AC, lampu, komputer, TV, dispenser, teko listrik, mesin kopi, kulkas, router, dan panel listrik.

Rule AC dummy menggunakan rentang jumlah orang dan suhu untuk menentukan jumlah AC dan target suhu. Ini merupakan rule lama/generik dan tidak selalu sama dengan algoritma status AC terbaru berbasis `SensorDataController` dan dashboard utama.

## 20. Catatan Teknis dan Potensi Perhatian

Beberapa hal penting dari hasil analisa:

- Autentikasi web dan middleware admin sedang dinonaktifkan pada route aktif. Ini membuat dashboard dan admin dapat diakses tanpa login jika server berjalan dengan file route saat ini.
- `EnergyController::getEfficiency` memanggil fungsi service yang tidak ditemukan, sehingga endpoint efisiensi energi berpotensi error.
- `Api\ReportController` masih memakai nama field lama seperti `temperature` dan `light_intensity`, sedangkan tabel `sensor_data` memakai `room_temperature` dan `light_level`. Laporan pada controller ini berpotensi menghasilkan nilai kosong atau tidak akurat. `PdfReportController` sudah memakai field yang sesuai.
- `ArduinoController` tidak terlihat terdaftar di `routes/api.php`, tetapi beberapa view lama masih mereferensikan endpoint Arduino/devices yang tidak terlihat aktif.
- Ada beberapa identitas device/lokasi yang berbeda antara modul lama dan modul terbaru. Modul terbaru memakai `UNJA_Prodi_Elektro` dan `Ruang Dosen Prodi Teknik Elektro`, sedangkan beberapa file lama memakai `ESP32_Smart_Energy_ChangeDetection` dan `Lab Teknik Tegangan Tinggi`.
- Tarif listrik muncul dalam beberapa nilai: 1445 pada service energi, 1467 pada konfigurasi energi, dan 1500 pada model/migration konsumsi energi. Ini perlu diseragamkan jika laporan biaya harus konsisten.
- `UserController` mencoba load relasi `energyLogs` pada user, tetapi model `User` yang dianalisa tidak memiliki relasi tersebut.
- `AdminController` pada emergency shutdown menulis nama user dari `auth()->user()`. Jika route admin tetap tanpa autentikasi, bagian ini dapat bermasalah saat tidak ada user login.
- `DeviceController::controlDevice` pada `ArduinoController` memiliki urutan update state yang membuat log energy usage saat mematikan perangkat berpotensi tidak terpanggil sesuai niat, karena status perangkat sudah diupdate sebelum pengecekan kondisi lama.
- Terdapat dua jalur data: `sensor_data` sebagai jalur IoT utama terbaru dan `sensor_readings` sebagai jalur sensor generik lama. Dashboard utama lebih banyak memakai `sensor_data`.
- Beberapa laporan berbasis `EnergyLog`, sementara dashboard realtime dan PDF sensor berbasis `SensorData`. Ini berarti angka laporan energi dan laporan sensor dapat berbeda sumber data.

## 21. Kesimpulan

Proyek Laravel Dasko berfungsi sebagai pusat aplikasi smart energy untuk ruang dosen. Bagian backend menerima data sensor IoT, menyimpan histori sensor, menyediakan data realtime untuk dashboard, menyimpan perintah kontrol AC/lampu dari dashboard, menghitung konsumsi energi, dan membuat laporan.

Inti sistem berada pada tiga alur utama:

- Data sensor masuk ke `sensor_data` melalui API sensor.
- Dashboard membaca `sensor_data` untuk monitoring realtime, chart, histori, dan laporan.
- Kontrol manual disimpan ke `ac_controls`, lalu perangkat mengambilnya melalui polling API.

Rumus utama proyek berpusat pada konsumsi energi: Wh dari watt dikali jam, kWh dari Wh dibagi 1000, biaya dari kWh dikali tarif, dan penghematan dari baseline dikurangi konsumsi aktual. Pengkondisian utama berpusat pada jumlah orang, status AC/lampu, mode auto/manual, validasi data sensor, jam kerja, energi saving, dan batas pagination/laporan.

