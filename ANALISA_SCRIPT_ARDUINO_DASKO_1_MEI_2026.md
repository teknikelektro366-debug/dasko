# Analisa Script Arduino Dasko 1 Mei 2026

Dokumen ini menjelaskan cara kerja umum script Arduino `Script Arduino Dasko 1 Mei 2026.ino`. Penjelasan dibuat dalam bentuk konseptual tanpa menyertakan potongan kode, agar mudah dipahami sebagai dokumentasi sistem.

## 1. Gambaran Umum Sistem

Script ini dibuat untuk perangkat ESP32 yang berfungsi sebagai sistem monitoring dan kontrol otomatis ruangan. Sistem membaca jumlah orang yang masuk dan keluar ruangan, membaca kondisi lingkungan, menampilkan status ke layar TFT, mengendalikan AC dan lampu, serta mengirim data ke server web Dasko.

Komponen utama yang digunakan adalah:

- ESP32 sebagai mikrokontroler utama.
- Dua sensor proximity untuk mendeteksi orang masuk dan keluar.
- Sensor DHT22 untuk membaca suhu dan kelembaban ruangan.
- Sensor LDR untuk membaca intensitas cahaya.
- TFT display untuk menampilkan status ruangan secara lokal.
- IR transmitter untuk mengirim perintah ke AC Panasonic.
- Relay lampu pada satu jalur lampu dengan konfigurasi NC atau Normally Closed.
- WiFi untuk koneksi ke server Dasko.
- API server untuk menyimpan data sensor dan mengambil perintah kontrol dari dashboard.

Secara garis besar, sistem bekerja dengan prinsip berikut:

1. ESP32 melakukan inisialisasi sensor, display, relay, IR, watchdog, WiFi, SSL/proxy, dan kalibrasi awal.
2. Loop utama terus membaca sensor proximity, DHT22, dan LDR.
3. Jika sensor masuk mendeteksi objek stabil, jumlah orang bertambah.
4. Jika sensor keluar mendeteksi objek stabil, jumlah orang berkurang.
5. Jumlah orang dibatasi dari 0 sampai kapasitas maksimum 18 orang.
6. Status AC dan lampu ditentukan dari jumlah orang, kecuali dashboard mengaktifkan kontrol manual.
7. Data dikirim ke API saat ada perubahan penting atau saat heartbeat berkala.
8. TFT diperbarui saat nilai penting berubah atau pada interval tertentu.
9. Watchdog dan pemantauan memori dipakai untuk mengurangi risiko perangkat restart atau hang.

## 2. Konfigurasi Utama

### 2.1 Koneksi dan API

Script menggunakan jaringan WiFi dengan SSID yang sudah ditentukan di dalam sketch. Target server adalah domain Dasko pada alamat `dasko.fst.unja.ac.id`.

Endpoint API yang digunakan:

- Endpoint pengiriman data sensor: `/api/sensor/data`.
- Endpoint kontrol AC dan lampu dari dashboard: `/api/ac/control`.
- Endpoint deteksi proxy: `/api/proxy/detection`.
- Endpoint health check: `/api/health`.

Walaupun terdapat konfigurasi sertifikat Root CA dan fungsi HTTPS, konfigurasi aktif pada script memilih mode SSL insecure dan preferensi HTTP. Artinya, pada kondisi aktif script cenderung memakai HTTP langsung untuk pengiriman data dan polling kontrol, sementara fungsi HTTPS tetap tersedia sebagai dukungan atau fallback bila preferensi diubah.

### 2.2 Identitas Perangkat

Perangkat memakai identitas:

- Device ID: `UNJA_Prodi_Elektro`.
- Lokasi: `Ruang Dosen Prodi Teknik Elektro`.

Identitas ini dikirim ke server saat pengiriman data sensor, polling kontrol, dan deteksi proxy.

### 2.3 Pin Hardware

Konfigurasi pin yang digunakan:

- IR transmitter: GPIO 4.
- Proximity masuk: GPIO 32.
- Proximity keluar: GPIO 14.
- DHT22: GPIO 13.
- LDR: GPIO 34.
- Relay lampu: GPIO 25.

Catatan: pada beberapa teks log/comment di script masih ada penyebutan GPIO35 untuk LDR, tetapi nilai konfigurasi aktif menggunakan GPIO 34.

### 2.4 Batasan dan Interval Penting

Nilai batas dan interval penting yang digunakan:

- Kapasitas maksimum ruangan: 18 orang.
- Cooldown antar deteksi orang: 1500 ms.
- Debounce sensor proximity: 80 ms.
- Minimal durasi presence agar dianggap orang: 220 ms.
- Durasi di bawah 180 ms dianggap noise atau gerakan pintu.
- Ambang perubahan suhu untuk kirim data: 0,1 derajat Celsius.
- Ambang perubahan kelembaban untuk kirim data: 0,5 persen.
- Ambang perubahan cahaya untuk kirim data: 20 lux.
- Interval minimal antar pengiriman data penting: 200 ms.
- Interval polling kontrol AC/lampu dari API: 5000 ms.
- Interval heartbeat: 300000 ms atau 5 menit.
- Interval deteksi proxy: 120000 ms atau 2 menit.
- Kapasitas nilai cahaya yang dikirim ke API: 0 sampai 1000 lux.

## 3. Struktur Data Utama

Script memakai beberapa struktur data untuk menyimpan status sistem.

### 3.1 Status Proxy

Struktur status proxy menyimpan informasi apakah koneksi diduga melalui proxy, apakah koneksi langsung, tipe proxy, IP real, IP proxy, waktu pemeriksaan terakhir, jumlah kegagalan berturut-turut, dan apakah hosting dapat dijangkau.

Data ini digunakan untuk menyesuaikan timeout, header request, dan logging jaringan.

### 3.2 Status SSL

Struktur status SSL menyimpan apakah SSL telah dikonfigurasi, apakah koneksi SSL terakhir berhasil, pesan error terakhir, waktu test SSL terakhir, dan jumlah kegagalan berturut-turut.

Walaupun mode aktif memakai HTTP, struktur ini tetap dipakai oleh fungsi test SSL dan request HTTPS.

### 3.3 Status Sensor Proximity

Struktur sensor proximity menyimpan kondisi mentah dan kondisi yang sudah didebounce dari sensor masuk dan keluar. Struktur ini juga menyimpan timestamp perubahan terakhir untuk masing-masing sensor.

Pada script ini, sensor proximity dianggap mendeteksi objek saat sinyal terbaca LOW, karena pin memakai mode pull-up.

### 3.4 Data Terkini Sistem

Struktur data terkini menyimpan nilai utama yang dipakai oleh UI dan API, yaitu:

- Jumlah orang.
- Status AC.
- Set temperature AC.
- Suhu ruangan.
- Kelembaban.
- Level cahaya.
- Status proximity masuk dan keluar.
- Timestamp perangkat.
- Status WiFi dan RSSI.
- Status lampu.
- Status lampu jalur 1 dan jalur 2.

Lampu jalur 2 tersedia di struktur, tetapi pada implementasi saat ini tidak digunakan.

## 4. Alur Kerja Saat Setup

Fungsi `setup()` menjalankan tahap awal sistem. Urutannya secara garis besar adalah:

1. Membuka komunikasi Serial pada baud rate 115200.
2. Mengaktifkan watchdog timer dengan timeout 60 detik.
3. Mengatur CPU ESP32 ke 240 MHz.
4. Mengatur mode pin proximity, LDR, IR, dan relay.
5. Memastikan relay lampu mulai dalam kondisi OFF secara paksa.
6. Memasang interrupt pada sensor proximity masuk dan keluar.
7. Menginisialisasi DHT22.
8. Melakukan test pembacaan DHT22.
9. Menginisialisasi IR transmitter dan objek AC Panasonic.
10. Mengirim sinyal test IR awal ke AC.
11. Melakukan kalibrasi baseline sensor proximity sebanyak 50 pembacaan.
12. Melakukan test proximity selama 5 detik.
13. Menginisialisasi TFT, melakukan test warna, lalu menggambar UI awal.
14. Menghubungkan ESP32 ke WiFi.
15. Jika WiFi berhasil, melakukan test SSL dan deteksi proxy.
16. Menginisialisasi status internal SSL, proxy, dan counter orang.
17. Mereset watchdog setelah setup selesai.

## 5. Alur Kerja Loop Utama

Fungsi `loop()` adalah pusat kerja sistem. Loop berjalan terus menerus selama perangkat aktif.

Urutan kerja utama dalam loop:

1. Watchdog direset agar perangkat tidak dianggap hang.
2. Memori heap diperiksa setiap 10 detik.
3. Sensor dibaca melalui fungsi pembacaan sensor.
4. Algoritma pendeteksian orang dijalankan.
5. Jika interrupt proximity terjadi, sistem segera memperbarui data, AC, lampu, dan TFT.
6. Jika jumlah orang berubah tanpa interrupt, sistem tetap menyinkronkan AC, lampu, dan TFT.
7. Deteksi proxy dilakukan hanya saat sensor pintu tidak sedang aktif, agar tidak mengganggu deteksi orang.
8. API kontrol dashboard dipolling setiap 5 detik.
9. Kontrol lampu otomatis diperiksa setiap 3 detik.
10. Perubahan signifikan pada data sensor diperiksa dan dikirim ke API bila perlu.
11. Heartbeat dikirim bila tidak ada update dalam interval tertentu.
12. Kontrol AC otomatis diperiksa berkala.
13. Kontrol AC dan lampu juga dipastikan ulang setiap 10 detik saat jumlah orang tidak berubah.
14. TFT diperbarui setiap 3 detik atau saat data berubah.
15. Status WiFi diperiksa periodik setiap 120 detik.
16. Health check internal ditulis ke Serial setiap 10 menit.
17. Perintah Serial dari pengguna diproses jika tersedia.
18. Watchdog direset kembali di akhir loop.

## 6. Cara Kerja Sensor dan Perhitungan Jumlah Orang

### 6.1 Pembacaan Proximity

Sensor proximity menggunakan logika aktif LOW:

- HIGH berarti tidak ada objek.
- LOW berarti objek terdeteksi.

Setiap perubahan state sensor dicatat waktunya. Setelah perubahan stabil melewati debounce 80 ms, nilai sensor dianggap valid. Hasil valid ini kemudian diubah menjadi status deteksi objek.

### 6.2 Interrupt Proximity

Kedua sensor proximity dipasang interrupt pada perubahan sinyal. Interrupt tidak langsung menghitung orang, tetapi hanya menandai bahwa ada perubahan yang perlu diproses segera oleh loop utama.

Interrupt dibatasi dengan interval minimal 30 ms agar perubahan terlalu cepat tidak memicu proses berlebihan.

### 6.3 Deteksi Masuk

Orang dianggap masuk jika sensor masuk mendeteksi objek secara stabil. Prosesnya:

1. Saat sensor masuk pertama kali mendeteksi objek, waktu awal dicatat.
2. Sistem menunggu apakah objek tetap terdeteksi.
3. Jika durasi deteksi mencapai minimal 220 ms dan cooldown 1500 ms sudah lewat, jumlah orang ditambah 1.
4. Data sensor diperbarui.
5. Jika WiFi aktif, data dikirim ke API dengan alasan orang masuk terkonfirmasi.
6. Jika objek hilang sebelum durasi minimal, deteksi dibatalkan sebagai noise atau gerakan pintu.

### 6.4 Deteksi Keluar

Orang dianggap keluar jika sensor keluar mendeteksi objek secara stabil. Prosesnya:

1. Saat sensor keluar pertama kali mendeteksi objek, waktu awal dicatat.
2. Sistem menunggu apakah objek tetap terdeteksi.
3. Jika durasi deteksi mencapai minimal 220 ms dan cooldown 1500 ms sudah lewat, jumlah orang dikurangi 1.
4. Pengurangan hanya dilakukan jika jumlah orang lebih dari 0.
5. Jika jumlah orang sudah 0, sistem menolak pengurangan dan menulis warning ke Serial.
6. Jika WiFi aktif, data dikirim ke API dengan alasan orang keluar terkonfirmasi.

### 6.5 Batas Jumlah Orang

Setelah proses deteksi, jumlah orang selalu dibatasi dalam rentang 0 sampai 18. Pembatasan ini mencegah nilai negatif dan mencegah nilai melebihi kapasitas maksimum.

## 7. Pembacaan Sensor Lingkungan

### 7.1 DHT22

DHT22 membaca suhu dan kelembaban. Karena sensor DHT22 tidak boleh dibaca terlalu cepat, script hanya membaca sensor ini minimal setiap 2,5 detik.

Jika pembacaan valid, nilai cache diperbarui. Jika pembacaan gagal, sistem menggunakan nilai cache terakhir dan menulis pesan kemungkinan masalah hardware, wiring, pull-up resistor, atau sensor lockup.

Validasi suhu DHT22:

- Nilai suhu harus lebih besar dari -40 derajat Celsius.
- Nilai suhu harus lebih kecil dari 80 derajat Celsius.

Validasi kelembaban:

- Nilai kelembaban harus minimal 0 persen.
- Nilai kelembaban harus maksimal 100 persen.

Jika cache tidak valid saat data sistem disinkronkan, script memakai fallback suhu 25 derajat Celsius dan kelembaban 60 persen.

### 7.2 LDR

LDR dibaca setiap 1 detik jika fitur LDR diaktifkan. Pembacaan dilakukan beberapa kali lalu dirata-ratakan untuk mengurangi noise.

Jika nilai mentah sangat rendah, sistem menganggap kemungkinan sensor tidak terpasang atau pin floating, lalu nilai cahaya dibuat 0.

Nilai analog ESP32 berada pada rentang 0 sampai 4095. Nilai ini dipetakan ke rentang 0 sampai 1000 lux agar sesuai dengan batas API.

### 7.3 Proximity

Nilai proximity masuk dan keluar disimpan sebagai boolean pada data terkini. Nilai ini dikirim ke API untuk menunjukkan apakah sensor sedang mendeteksi objek.

## 8. Rumus dan Perhitungan yang Digunakan

### 8.1 Mapping Nilai LDR ke Lux

Nilai mentah LDR dari ADC ESP32 berada pada rentang 0 sampai 4095. Script mengubahnya ke rentang 0 sampai 1000 lux dengan perbandingan linear.

Rumus konseptualnya:

- Nilai lux = nilai ADC dibagi 4095, lalu dikalikan 1000.
- Setelah itu nilai dibatasi agar tidak kurang dari 0 dan tidak lebih dari 1000.

### 8.2 Rata-rata Sampel LDR

Untuk mengurangi noise, script mengambil 5 sampel LDR.

Rumus konseptualnya:

- Nilai rata-rata LDR = total semua sampel dibagi jumlah sampel.

### 8.3 Deteksi Perubahan Suhu

Perubahan suhu dianggap signifikan jika selisih absolut suhu sekarang dan suhu sebelumnya minimal 0,1 derajat Celsius.

Rumus konseptualnya:

- Selisih suhu = nilai mutlak dari suhu sekarang dikurangi suhu sebelumnya.
- Jika selisih suhu minimal 0,1, data dikirim ke API.

### 8.4 Deteksi Perubahan Kelembaban

Perubahan kelembaban dianggap signifikan jika selisih absolut kelembaban sekarang dan sebelumnya minimal 0,5 persen.

Rumus konseptualnya:

- Selisih kelembaban = nilai mutlak dari kelembaban sekarang dikurangi kelembaban sebelumnya.
- Jika selisih kelembaban minimal 0,5, data dikirim ke API.

### 8.5 Deteksi Perubahan Cahaya

Perubahan cahaya dianggap signifikan jika selisih absolut level cahaya sekarang dan sebelumnya minimal 20 lux.

Rumus konseptualnya:

- Selisih cahaya = nilai mutlak dari cahaya sekarang dikurangi cahaya sebelumnya.
- Jika selisih cahaya minimal 20 lux, data dikirim ke API.

### 8.6 Perhitungan Durasi Presence

Durasi presence digunakan untuk membedakan orang lewat dengan noise atau gerakan pintu.

Rumus konseptualnya:

- Durasi presence = waktu sekarang dikurangi waktu pertama kali sensor mendeteksi objek.
- Jika durasi minimal 220 ms, deteksi bisa dianggap orang.
- Jika durasi kurang dari 180 ms dan objek sudah hilang, deteksi dianggap noise atau gerakan pintu.

### 8.7 Cooldown Deteksi Orang

Cooldown mencegah satu orang terhitung berkali-kali saat masih berada di area sensor.

Rumus konseptualnya:

- Selisih trigger = waktu sekarang dikurangi waktu trigger sah terakhir.
- Jika selisih minimal 1500 ms, deteksi baru boleh dihitung.

### 8.8 Pembatasan Nilai

Beberapa nilai selalu dibatasi agar tidak keluar dari rentang aman:

- Jumlah orang dibatasi 0 sampai 18.
- Set temperature manual AC dibatasi 16 sampai 30 derajat Celsius.
- Level cahaya dibatasi 0 sampai 1000 lux.
- Set temperature yang dikirim ke API dibatasi 16 sampai 30 derajat Celsius saat AC aktif.
- Suhu yang dikirim ke API divalidasi dalam rentang -10 sampai 60 derajat Celsius.
- Kelembaban yang dikirim ke API divalidasi dalam rentang 0 sampai 100 persen.

### 8.9 Pembulatan Suhu dan Kelembaban untuk API

Suhu dan kelembaban dikirim dengan pembulatan dua angka di belakang koma.

Rumus konseptualnya:

- Nilai dikalikan 100.
- Hasilnya dibulatkan.
- Hasil pembulatan dibagi 100 kembali.

### 8.10 Perhitungan Uptime

Uptime yang ditampilkan pada TFT dihitung dari waktu berjalan perangkat.

Rumus konseptualnya:

- Uptime menit = waktu berjalan dalam milidetik dibagi 60000.

### 8.11 Estimasi Fragmentasi Heap

Script menampilkan estimasi fragmentasi heap berdasarkan ukuran alokasi heap terbesar dan free heap.

Rumus konseptualnya:

- Fragmentasi = 100 dikurangi persentase heap terbesar terhadap free heap.

Nilai ini digunakan untuk logging, bukan untuk kontrol langsung selain warning memori rendah.

## 9. Logika Kontrol AC Otomatis

Kontrol AC otomatis ditentukan dari jumlah orang di ruangan. Sistem mengirim sinyal IR ke AC hanya jika status ON/OFF atau target suhu berubah, sehingga tidak terus-menerus mengirim sinyal yang sama.

Aturan kontrol AC:

- 0 orang: semua AC mati, set temperature 0.
- 1 sampai 5 orang: AC 1 menyala, target 22 derajat Celsius.
- 6 sampai 10 orang: AC 1 menyala, target 20 derajat Celsius.
- 11 sampai 15 orang: AC 1 dan AC 2 menyala, target 20 derajat Celsius.
- 16 sampai 18 orang: AC 1 dan AC 2 menyala, target 18 derajat Celsius.

Saat AC menyala, mode AC diset ke mode dingin dan fan diset otomatis. Script menggunakan library IR Panasonic untuk membentuk dan mengirim sinyal remote.

Status AC yang dikirim ke API dinormalisasi menjadi salah satu dari:

- `OFF` jika tidak ada AC aktif.
- `ON` jika hanya salah satu AC aktif.
- `AC 1+2` jika kedua AC aktif.

## 10. Logika Kontrol Lampu

Kontrol lampu otomatis pada script ini berbasis jumlah orang, bukan berbasis nilai LDR.

Aturan kontrol lampu:

- Jika jumlah orang lebih dari 0, lampu ON.
- Jika jumlah orang sama dengan 0, lampu OFF.

Relay lampu menggunakan konfigurasi NC:

- LOW berarti jalur NC tersambung, sehingga lampu dapat menyala.
- HIGH berarti jalur NC terputus, sehingga lampu dipadamkan paksa.

LDR tetap dibaca dan dikirim sebagai data monitoring, tetapi tidak menjadi syarat utama untuk mematikan lampu saat ruangan berisi orang.

## 11. Mode Otomatis dan Manual

Script memiliki mode otomatis dan manual untuk AC serta lampu.

### 11.1 Mode Otomatis

Pada mode otomatis:

- AC mengikuti aturan jumlah orang.
- Lampu mengikuti aturan jumlah orang.
- Perubahan jumlah orang langsung memicu sinkronisasi kontrol AC dan lampu.

### 11.2 Mode Manual dari Dashboard

Dashboard dapat mengirim perintah manual melalui API kontrol. Data yang dibaca meliputi:

- ID kontrol.
- Mode kontrol umum.
- Mode kontrol AC.
- Mode kontrol lampu.
- Override manual.
- Status AC 1 dan AC 2.
- Status lampu.
- Suhu target AC 1 dan AC 2.

Jika dashboard mengaktifkan manual override, perangkat menerapkan perintah dashboard hanya pada perangkat yang diset manual. Perangkat yang tetap dalam mode auto masih mengikuti algoritma jumlah orang.

Script menyimpan ID kontrol manual terakhir agar perintah yang sama tidak diterapkan dan dikirim ulang terus-menerus.

### 11.3 Mode Manual dari Serial

Melalui Serial, pengguna juga dapat mengubah mode ke manual atau auto. Mode manual dari Serial mematikan auto mode AC dan lampu, lalu mengaktifkan jalur relay NC agar saklar fisik dapat bekerja penuh.

## 12. Pengiriman Data ke API

Pengiriman data dilakukan oleh fungsi `sendDataToAPI()`. Fungsi ini hanya bekerja jika WiFi terhubung.

Sebelum mengirim data, sistem:

1. Membaca ulang DHT22 dan LDR sesuai interval yang diizinkan.
2. Menyinkronkan proximity dan jumlah orang ke data terkini.
3. Jika AC dalam mode otomatis, menyinkronkan status AC dari jumlah orang.
4. Menormalisasi status AC dan lampu agar sesuai validasi API.
5. Membatasi nilai jumlah orang, suhu target, dan cahaya dalam rentang aman.
6. Memvalidasi suhu dan kelembaban.
7. Membentuk payload JSON sesuai struktur API.
8. Mengirim payload ke server.
9. Memproses response server untuk mengetahui apakah data tersimpan.

Data yang dikirim mencakup:

- Device ID.
- Lokasi.
- Jumlah orang.
- Status AC umum.
- Status AC 1 dan AC 2.
- Set temperature jika AC aktif.
- Suhu ruangan.
- Kelembaban.
- Level cahaya.
- Status lampu.
- Status proximity masuk dan keluar.
- RSSI WiFi.
- Status perangkat.
- Timestamp dari perangkat.

Karena konfigurasi aktif memilih preferensi HTTP, pengiriman data akan memakai HTTP langsung. Jika preferensi ini diubah, script memiliki jalur HTTPS dan fallback HTTP.

## 13. Deteksi Perubahan Signifikan

Fungsi deteksi perubahan signifikan memantau perubahan berikut:

- Jumlah orang.
- Suhu ruangan.
- Kelembaban.
- Level cahaya.
- Status proximity masuk.
- Status proximity keluar.
- Status AC.
- Status lampu.

Jika salah satu nilai berubah melewati ambang atau berbeda dari nilai sebelumnya, sistem langsung mencoba mengirim data ke API, selama interval minimal antar pengiriman sudah terpenuhi.

## 14. Heartbeat

Heartbeat adalah pengiriman data berkala untuk monitoring koneksi. Heartbeat dikirim setiap 5 menit atau saat sistem perlu update paksa karena terlalu lama tidak ada pengiriman data.

Heartbeat tidak menggantikan pengiriman data saat perubahan sensor terjadi. Perubahan sensor tetap dikirim sesegera mungkin.

## 15. TFT Display

TFT display menampilkan dashboard lokal dengan header dan empat tile utama:

- Jumlah orang.
- Suhu AC atau status OFF.
- Suhu ruangan.
- Status lampu.

Bagian bawah layar menampilkan status tambahan seperti status AC, status online/offline, status lampu, sisa memori, cahaya, kelembaban, uptime, dan mode perangkat.

Update TFT dibuat selektif untuk mengurangi flicker:

- Tile diperbarui jika jumlah orang berubah.
- Tile diperbarui jika suhu ruangan berubah secara integer.
- Tile diperbarui jika status AC berubah.
- Tile diperbarui jika status lampu berubah.
- Tile juga diperbarui secara berkala agar layar tetap sinkron.
- Header diperbarui lebih jarang dibanding tile.

Warna tile jumlah orang berubah berdasarkan kepadatan:

- Hijau untuk jumlah rendah.
- Oranye untuk jumlah menengah.
- Merah untuk jumlah tinggi.

## 16. Koneksi WiFi, SSL, HTTP, dan Proxy

### 16.1 WiFi

Saat koneksi WiFi dimulai, ESP32 mencoba terhubung sampai 30 kali dengan jeda 500 ms. Jika berhasil, perangkat menyimpan status connected, RSSI, IP lokal, gateway, dan DNS.

Jika gagal, sistem tetap berjalan dalam mode standalone. Sensor, AC, lampu, dan TFT tetap aktif, tetapi pengiriman data ke API tidak bisa dilakukan.

### 16.2 SSL

Script memiliki tiga metode SSL:

- Root CA certificate.
- Certificate fingerprint.
- Insecure atau tanpa verifikasi sertifikat.

Konfigurasi aktif memilih mode insecure. Fungsi test SSL tetap tersedia untuk mencoba koneksi ke port HTTPS server.

Catatan konsistensi: beberapa pesan Serial menyebut metode ROOT_CA atau secure, tetapi konfigurasi aktif pada script memilih SSL insecure dan preferensi HTTP.

### 16.3 HTTP dan HTTPS Request

Script menyediakan dua fungsi request:

- Request HTTPS menggunakan `WiFiClientSecure`.
- Request HTTP fallback menggunakan `WiFiClient`.

Pada mode aktif, preferensi HTTP membuat script langsung memakai HTTP untuk API sensor, polling kontrol, dan deteksi proxy.

### 16.4 Deteksi Proxy

Deteksi proxy dilakukan secara berkala. Sistem mengirim informasi perangkat seperti IP lokal, gateway, DNS, RSSI, device ID, dan lokasi ke endpoint proxy detection.

Jika proxy terdeteksi, status koneksi diperbarui. Informasi proxy juga dapat digunakan untuk menambah header request dan menyesuaikan timeout/retry pada request HTTPS.

## 17. Watchdog dan Monitoring Memori

Watchdog timer digunakan untuk mencegah perangkat hang terlalu lama. Watchdog direset di awal loop, setelah operasi sensor, dan di akhir loop.

Monitoring memori dilakukan setiap 10 detik. Script membaca free heap, minimum free heap, dan estimasi fragmentasi heap.

Pengkondisian memori:

- Jika free heap di bawah 20000 byte, sistem menulis warning, memberi jeda, mereset watchdog, dan melewati sisa loop sementara.
- Jika free heap di bawah 10000 byte, sistem menulis kondisi critical, memberi jeda lebih lama, mereset watchdog, dan melewati sisa loop sementara.

Tujuannya adalah mengurangi risiko restart akibat memori terlalu rendah.

## 18. Perintah Serial yang Tersedia

Script menyediakan beberapa perintah Serial untuk pengujian dan kontrol manual:

- `reset`: mereset jumlah orang ke 0 dan menyinkronkan perangkat.
- `test`: menampilkan pembacaan proximity, DHT22 dari cache, dan LDR.
- `add`: menambah jumlah orang secara manual, maksimal sampai 18.
- `sub`: mengurangi jumlah orang secara manual, tidak boleh kurang dari 0.
- `manual`: mengaktifkan mode manual penuh.
- `auto`: mengaktifkan mode otomatis.
- `help`: menampilkan daftar perintah.
- `ssl`: menampilkan status SSL dan menjalankan test SSL.
- `proxy`: menampilkan status proxy dan memaksa deteksi ulang.
- `hosting`: menampilkan status endpoint hosting dan menjalankan test konektivitas.
- `cert`: menampilkan informasi sertifikat.
- `testmode`: mengubah status test mode.
- `sensors`: menampilkan ringkasan pembacaan sensor dan status sistem.
- `simple`: menjalankan test sederhana selama 60 detik, sensor masuk menambah orang dan sensor keluar mengurangi orang.

## 19. Pengkondisian Utama yang Digunakan

Bagian ini merangkum pengkondisian penting yang membentuk perilaku script.

### 19.1 Pengkondisian Koneksi

- Jika WiFi tidak terhubung, request API tidak dijalankan.
- Jika WiFi berhasil terhubung, sistem menguji SSL dan menjalankan deteksi proxy.
- Jika request HTTP/HTTPS berhasil dengan kode 200 atau 201, request dianggap sukses.
- Jika response server memiliki struktur sukses, data dianggap tersimpan.
- Jika response berisi error validasi, detail error ditulis ke Serial.

### 19.2 Pengkondisian Sensor Proximity

- Jika state proximity berubah, waktu perubahan disimpan.
- Jika perubahan sudah melewati debounce 80 ms, state dianggap stabil.
- Jika sensor masuk aktif cukup lama dan cooldown terpenuhi, jumlah orang bertambah.
- Jika sensor keluar aktif cukup lama dan cooldown terpenuhi, jumlah orang berkurang.
- Jika sensor keluar aktif saat jumlah orang 0, pengurangan dibatalkan.
- Jika durasi deteksi terlalu pendek, event dianggap noise atau gerakan pintu.

### 19.3 Pengkondisian DHT22

- Jika interval pembacaan belum 2,5 detik, pembacaan DHT22 dilewati.
- Jika suhu valid, cache suhu diperbarui.
- Jika suhu tidak valid, cache lama dipakai.
- Jika kelembaban valid, cache kelembaban diperbarui.
- Jika kelembaban tidak valid, cache lama dipakai.
- Jika data cache tidak valid saat sinkronisasi, sistem memakai nilai default.

### 19.4 Pengkondisian LDR

- Jika LDR dinonaktifkan, nilai cahaya selalu 0.
- Jika interval pembacaan belum 1 detik, pembacaan LDR dilewati.
- Jika nilai mentah terlalu rendah, cahaya dianggap 0.
- Jika nilai mentah valid, nilai dipetakan ke 0 sampai 1000 lux.

### 19.5 Pengkondisian AC

- Jika jumlah orang 0, AC mati.
- Jika jumlah orang 1 sampai 5, satu AC menyala pada 22 derajat Celsius.
- Jika jumlah orang 6 sampai 10, satu AC menyala pada 20 derajat Celsius.
- Jika jumlah orang 11 sampai 15, dua AC menyala pada 20 derajat Celsius.
- Jika jumlah orang 16 sampai 18, dua AC menyala pada 18 derajat Celsius.
- Jika status AC dan target suhu tidak berubah, sinyal IR tidak dikirim ulang.
- Jika dashboard mengatur AC manual, perintah dashboard diterapkan.

### 19.6 Pengkondisian Lampu

- Jika jumlah orang lebih dari 0, lampu dinyalakan.
- Jika jumlah orang 0, lampu dimatikan.
- Jika status lampu tidak berubah, relay tidak ditulis ulang.
- Jika dashboard mengatur lampu manual, perintah dashboard diterapkan.

### 19.7 Pengkondisian API dan Update

- Jika jumlah orang berubah, data dikirim.
- Jika suhu berubah minimal 0,1 derajat Celsius, data dikirim.
- Jika kelembaban berubah minimal 0,5 persen, data dikirim.
- Jika cahaya berubah minimal 20 lux, data dikirim.
- Jika proximity berubah, data dikirim.
- Jika status AC berubah, data dikirim.
- Jika status lampu berubah, data dikirim.
- Jika interval minimal antar pengiriman belum terpenuhi, pengiriman ditahan.
- Jika tidak ada update dalam waktu tertentu, heartbeat dikirim.

### 19.8 Pengkondisian TFT

- Jika data utama berubah, tile diperbarui.
- Jika tidak ada perubahan, tile tetap diperbarui berkala.
- Header diperbarui pada interval yang lebih lama untuk mengurangi flicker.

### 19.9 Pengkondisian Memori

- Jika free heap rendah, operasi dikurangi sementara.
- Jika free heap kritis, sistem memberi waktu recovery lebih lama.
- Watchdog selalu direset sebelum dan sesudah operasi penting.

## 20. Fungsi yang Digunakan

### 20.1 Fungsi Interrupt

- `proximityInInterrupt()`: menandai adanya perubahan pada sensor masuk.
- `proximityOutInterrupt()`: menandai adanya perubahan pada sensor keluar.

Kedua fungsi ini dibuat ringan agar aman dijalankan sebagai interrupt handler.

### 20.2 Fungsi SSL dan Proxy

- `setupSSL()`: mengatur metode SSL pada client HTTPS.
- `testSSLConnection()`: menguji koneksi SSL ke hosting.
- `getCertificateFingerprint()`: menampilkan informasi sertifikat, walaupun fingerprint tidak dipakai sebagai metode aktif.
- `checkProxyDetection()`: menjalankan pemeriksaan proxy secara berkala.
- `detectProxy()`: mengirim data deteksi proxy ke server dan membaca hasilnya.
- `handleProxyConnection()`: menangani perubahan status saat proxy terdeteksi.
- `testHostingConnectivity()`: menguji apakah hosting dapat dijangkau.

### 20.3 Fungsi HTTP dan WiFi

- `connectWiFi()`: menghubungkan ESP32 ke jaringan WiFi dan memulai test SSL/proxy.
- `makeHTTPSRequest()`: membuat request HTTPS dengan header tambahan dan dukungan proxy.
- `makeHTTPRequest()`: membuat request HTTP biasa sebagai jalur utama atau fallback.

### 20.4 Fungsi Sensor

- `readSensors()`: membaca DHT22, LDR, dan proximity, lalu melakukan debounce proximity.
- `detectPeople()`: menentukan orang masuk atau keluar berdasarkan sensor proximity, durasi presence, dan cooldown.
- `readDHTRealtime()`: membaca DHT22 dengan interval minimal dan cache.
- `readLDRRealtime()`: membaca LDR dengan sampling rata-rata dan mapping ke lux.
- `updateSensorData()`: menyinkronkan data sensor terbaru ke struktur data utama.
- `resetPeopleCounter()`: mereset jumlah orang dan menyinkronkan AC, lampu, serta TFT.

### 20.5 Fungsi UI TFT

- `drawModernHeader()`: menggambar header layar TFT.
- `drawModernTiles()`: menggambar tile utama dan status bar bawah.
- `drawUI()`: menggambar seluruh tampilan awal.
- `updateTFT()`: memperbarui tampilan secara selektif berdasarkan perubahan data.

### 20.6 Fungsi Kontrol AC dan Lampu

- `kontrolAC()`: mengatur AC otomatis berdasarkan jumlah orang dan mengirim sinyal IR jika perlu.
- `applyManualACControl()`: menerapkan perintah AC manual dari dashboard.
- `kontrolLampu()`: mengatur lampu otomatis berdasarkan jumlah orang.
- `applyManualLampControl()`: menerapkan perintah lampu manual dari dashboard.
- `syncDeviceControlsAfterPeopleChange()`: menyinkronkan sensor, AC, lampu, TFT, dan status setelah jumlah orang berubah.

### 20.7 Fungsi API dan Sinkronisasi Data

- `checkACControlAPI()`: mengambil perintah mode otomatis/manual, AC, lampu, dan suhu dari dashboard.
- `sendDataToAPI()`: mengirim data sensor dan status perangkat ke API.
- `sendHeartbeat()`: mengirim data heartbeat untuk monitoring koneksi.
- `hasSignificantChange()`: mendeteksi perubahan penting dan mengirim data jika perlu.
- `shouldForceUpdate()`: menentukan apakah heartbeat atau update paksa perlu dilakukan.
- `canSendUpdate()`: menentukan apakah interval minimal antar pengiriman sudah terpenuhi.
- `updatePreviousData()`: memperbarui timestamp pengiriman/update terakhir.

### 20.8 Fungsi Normalisasi Nilai API

- `targetSetTemperatureFromPeopleCount()`: menentukan target suhu dari jumlah orang.
- `apiACStatusFromPeopleCount()`: menentukan status AC API dari jumlah orang.
- `normalizeACStatus()`: menormalisasi status AC berdasarkan state hardware terakhir.
- `normalizeLampStatus()`: menormalisasi status lampu menjadi ON atau OFF.

Catatan: `apiACStatusFromPeopleCount()` tersedia, tetapi pengiriman data aktual lebih banyak memakai normalisasi dari state hardware melalui `normalizeACStatus()`.

### 20.9 Fungsi Utama Arduino

- `setup()`: menjalankan semua proses inisialisasi.
- `loop()`: menjalankan proses monitoring, kontrol, komunikasi API, update TFT, dan perintah Serial secara terus menerus.

## 21. Catatan Teknis dan Potensi Perhatian

Beberapa hal yang perlu diperhatikan dari hasil analisa script:

- Konfigurasi aktif memilih `PREFER_HTTP`, sehingga request API utama menggunakan HTTP, meskipun nama endpoint awal memakai HTTPS.
- Konfigurasi SSL aktif menggunakan mode insecure, tetapi beberapa log Serial masih menyebut ROOT_CA atau secure. Ini dapat membingungkan saat debugging.
- Komentar pin LDR di beberapa bagian menyebut GPIO35, tetapi konfigurasi aktif memakai GPIO34.
- Ambang `LUX_THRESHOLD` masih didefinisikan, tetapi logika lampu otomatis tidak lagi menggunakan LDR sebagai syarat utama.
- Fungsi kontrol AC dan lampu dirancang agar tidak mengirim perintah berulang jika status tidak berubah. Ini baik untuk mengurangi beban IR dan relay.
- DHT22 dibaca dengan cache untuk menghindari NaN akibat pembacaan terlalu cepat.
- Sensor proximity memakai kombinasi interrupt, debounce, minimum presence time, dan cooldown untuk mengurangi hitungan palsu.
- Sistem tetap bisa berjalan lokal tanpa WiFi, tetapi data API dan kontrol dashboard tidak aktif saat WiFi terputus.
- Watchdog dan monitoring memori menjadi bagian penting karena script cukup besar dan banyak memakai String, JSON, HTTP, TFT, dan IR.

## 22. Kesimpulan

Script ini adalah sistem smart room berbasis ESP32 untuk Ruang Dosen Prodi Teknik Elektro. Fungsi utamanya adalah menghitung jumlah orang dengan dua sensor proximity, mengontrol AC dan lampu berdasarkan jumlah orang, membaca suhu/kelembaban/cahaya, menampilkan informasi pada TFT, serta menyinkronkan data dengan dashboard web melalui API.

Logika utama sistem dapat diringkas sebagai berikut:

- Jumlah orang menjadi pusat keputusan kontrol.
- AC dikendalikan bertingkat berdasarkan jumlah orang.
- Lampu menyala jika ruangan berisi orang dan mati jika kosong.
- Sensor DHT22 dan LDR dipakai untuk monitoring dan pelaporan.
- API dipakai untuk penyimpanan data dan kontrol manual dari dashboard.
- Watchdog, debounce, cooldown, cache sensor, dan pembatasan nilai dipakai untuk menjaga stabilitas sistem.

