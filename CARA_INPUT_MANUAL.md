# 📝 Cara Input Data Manual

## Akses Halaman Input Manual

Ada 3 cara untuk input data manual:

### 1. **Input Data Harian (Jam 07:00 - 15:00)** ⭐ RECOMMENDED
**URL:** `http://127.0.0.1:8000/manual-input-bulk`

**Fitur:**
- Input data untuk 9 jam sekaligus (07:00 - 15:00)
- Data sudah terisi dengan pola natural ruang dosen
- Tinggal edit angka sesuai kondisi aktual
- Progress bar untuk tracking proses simpan

**Pola Data Natural:**
- **07:00** - 3 orang (Dosen mulai datang)
- **08:00** - 8 orang (Jam kerja dimulai, beberapa dosen mengajar)
- **09:00** - 12 orang (Peak - Banyak dosen & mahasiswa bimbingan)
- **10:00** - 10 orang (Dosen mengajar, mahasiswa bimbingan)
- **11:00** - 7 orang (Beberapa dosen istirahat/mengajar)
- **12:00** - 4 orang (Jam istirahat siang)
- **13:00** - 9 orang (Kembali dari istirahat, bimbingan siang)
- **14:00** - 11 orang (Peak siang - Bimbingan & rapat)
- **15:00** - 5 orang (Jam pulang, mulai sepi)

**Cara Pakai:**
1. Buka `http://127.0.0.1:8000/manual-input-bulk`
2. Pilih tanggal
3. Edit jumlah orang per jam sesuai kondisi aktual
4. Klik "Simpan Semua Data"
5. Tunggu progress bar sampai 100%
6. Data akan muncul di History dengan badge "📝 Manual"

---

### 2. **Input Data Per Jam**
**URL:** `http://127.0.0.1:8000/manual-input`

**Fitur:**
- Input data satu jam saja
- Pilih tanggal dan jam secara manual
- Cocok untuk update data spesifik

**Cara Pakai:**
1. Buka `http://127.0.0.1:8000/manual-input`
2. Pilih tanggal
3. Pilih jam (dropdown 00:00 - 23:00)
4. Masukkan jumlah orang
5. Klik "Simpan Data"

---

### 3. **Menu Pilihan**
**URL:** `http://127.0.0.1:8000/input-manual.html`

Halaman menu untuk memilih metode input (Harian atau Per Jam)

---

## Lihat Data di History

1. Buka Dashboard: `http://127.0.0.1:8000/dashboard`
2. Klik tab **"History"**
3. Klik sub-tab **"Data Sensor"**
4. Data manual akan muncul dengan:
   - **Device:** MANUAL_INPUT
   - **Sumber:** 📝 Manual (badge kuning)
   - **Timestamp:** Sesuai tanggal & jam yang dipilih

Data dari ESP32 akan muncul dengan:
- **Device:** ESP32_Smart_Energy
- **Sumber:** 🔌 ESP32 (badge hijau)

---

## Informasi Ruang Dosen

- **Total Dosen:** 15 orang
- **Jam Kerja:** 07:00 - 15:00 (Senin-Jumat)
- **Aktivitas:** 
  - Mengajar
  - Bimbingan Mahasiswa
  - Rapat
  - Penelitian

---

## Tips

1. **Gunakan Input Harian** untuk mengisi data historis atau data harian sekaligus
2. **Gunakan Input Per Jam** untuk koreksi data spesifik
3. Data manual akan otomatis mengatur:
   - AC ON jika jumlah orang > 5
   - Lampu ON jika ada orang
   - Cahaya 500 lux jika ada orang, 0 jika kosong
4. Data akan langsung muncul di History dan digunakan untuk perhitungan energi

---

## Troubleshooting

**Q: Data tidak muncul di History?**
- Refresh halaman History
- Pastikan tanggal yang dipilih sesuai
- Cek apakah ada error saat simpan

**Q: Error "Duplicate entry"?**
- Data untuk tanggal & jam tersebut sudah ada
- Pilih tanggal/jam yang berbeda
- Atau hapus data lama dari database terlebih dahulu

**Q: Progress bar stuck?**
- Tunggu beberapa saat
- Jika lebih dari 1 menit, refresh halaman
- Cek data yang berhasil tersimpan di History
