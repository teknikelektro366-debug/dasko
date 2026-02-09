# Cara Kerja Sistem Download PDF

## Alur Kerja

### 1. User Klik Tombol PDF di Dashboard
```
Dashboard (Laporan Section) 
  → Button "PDF" diklik
  → JavaScript function dipanggil
```

### 2. JavaScript Membuat Request
```javascript
// Contoh untuk Laporan Harian
function downloadDailyReport() {
    const today = new Date().toISOString().split('T')[0];
    const url = `/api/reports/pdf/daily?date=${today}`;
    
    // Buat link temporary untuk trigger download
    const link = document.createElement('a');
    link.href = url;
    link.target = '_blank';
    link.download = `laporan_harian_${today}.pdf`;
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
}
```

### 3. Laravel Route Menerima Request
```php
// routes/api.php
Route::prefix('reports')->group(function () {
    Route::get('/pdf/daily', [PdfReportController::class, 'dailyReport']);
    Route::get('/pdf/weekly', [PdfReportController::class, 'weeklyReport']);
    Route::get('/pdf/monthly', [PdfReportController::class, 'monthlyReport']);
    Route::get('/pdf/efficiency', [PdfReportController::class, 'efficiencyReport']);
});
```

### 4. Controller Mengambil Data dari Database
```php
// app/Http/Controllers/PdfReportController.php
public function dailyReport(Request $request)
{
    // Ambil parameter tanggal
    $date = $request->input('date', now()->format('Y-m-d'));
    $startDate = Carbon::parse($date)->startOfDay();
    $endDate = Carbon::parse($date)->endOfDay();
    
    // Query data dari database
    $data = SensorData::whereBetween('created_at', [$startDate, $endDate])
                     ->orderBy('created_at', 'desc')
                     ->get();
    
    // Hitung summary
    $summary = $this->calculateDailySummary($data, $startDate);
    
    // Generate PDF
    $pdf = PDF::loadView('reports.pdf.daily', [
        'data' => $data,
        'summary' => $summary,
        'date' => $startDate
    ]);
    
    // Return PDF untuk download
    return $pdf->download('laporan_harian_' . $startDate->format('Y_m_d') . '.pdf');
}
```

### 5. Blade Template Merender HTML
```blade
<!-- resources/views/reports/pdf/daily.blade.php -->
<table>
    <thead>
        <tr>
            <th>Waktu</th>
            <th>Orang</th>
            <th>Suhu</th>
            <th>Kelembaban</th>
            <th>Cahaya</th>
            <th>AC</th>
            <th>Lampu</th>
        </tr>
    </thead>
    <tbody>
        @forelse($data as $item)
        <tr>
            <td>{{ $item->created_at->format('H:i:s') }}</td>
            <td>{{ $item->people_count }}</td>
            <td>{{ number_format($item->room_temperature, 1) }}°C</td>
            <td>{{ number_format($item->humidity, 1) }}%</td>
            <td>{{ $item->light_level }} lux</td>
            <td>{{ $item->ac_status }}</td>
            <td>{{ $item->lamp_status }}</td>
        </tr>
        @empty
        <tr>
            <td colspan="7">Tidak ada data</td>
        </tr>
        @endforelse
    </tbody>
</table>
```

### 6. DomPDF Mengkonversi HTML ke PDF
```
HTML Template 
  → DomPDF Library
  → PDF File
  → Browser Download
```

## Jenis Laporan

### 1. Laporan Harian
- **Endpoint:** `/api/reports/pdf/daily?date=YYYY-MM-DD`
- **Parameter:** `date` (opsional, default: hari ini)
- **Data:** Semua record sensor untuk 1 hari
- **Summary:** Total record, rata-rata orang, max orang, rata-rata suhu

### 2. Laporan Mingguan
- **Endpoint:** `/api/reports/pdf/weekly?week_start=YYYY-MM-DD`
- **Parameter:** `week_start` (opsional, default: awal minggu ini)
- **Data:** Data per hari dalam 1 minggu
- **Summary:** Total record, rata-rata orang, max orang, data harian

### 3. Laporan Bulanan
- **Endpoint:** `/api/reports/pdf/monthly?month=YYYY-MM`
- **Parameter:** `month` (opsional, default: bulan ini)
- **Data:** Data per minggu dalam 1 bulan
- **Summary:** Total record, rata-rata orang, max orang, data mingguan

### 4. Laporan Efisiensi
- **Endpoint:** `/api/reports/pdf/efficiency?date_from=YYYY-MM-DD&date_to=YYYY-MM-DD`
- **Parameter:** `date_from`, `date_to` (opsional, default: 30 hari terakhir)
- **Data:** Analisis penggunaan AC dan Lampu
- **Summary:** Persentase penggunaan, rata-rata orang saat AC/Lampu ON

## Package yang Digunakan

### barryvdh/laravel-dompdf
- **Fungsi:** Mengkonversi HTML ke PDF
- **Instalasi:** `composer require barryvdh/laravel-dompdf`
- **Config:** `config/dompdf.php`
- **Dokumentasi:** https://github.com/barryvdh/laravel-dompdf

## Struktur File

```
app/
  Http/
    Controllers/
      PdfReportController.php          # Controller untuk generate PDF

resources/
  views/
    reports/
      pdf/
        daily.blade.php                # Template PDF Harian
        weekly.blade.php               # Template PDF Mingguan
        monthly.blade.php              # Template PDF Bulanan
        efficiency.blade.php           # Template PDF Efisiensi

routes/
  api.php                              # Route untuk PDF endpoints

config/
  dompdf.php                           # Konfigurasi DomPDF
```

## Customisasi

### Mengubah Style PDF
Edit file template di `resources/views/reports/pdf/*.blade.php`:
```html
<style>
    body {
        font-family: Arial, sans-serif;
        font-size: 12px;
    }
    .header {
        background: #4F46E5;
        color: white;
        padding: 20px;
    }
    /* Tambahkan style lainnya */
</style>
```

### Menambahkan Data Baru
Edit controller `PdfReportController.php`:
```php
private function calculateDailySummary($data, $date)
{
    return [
        'date' => $date->format('d F Y'),
        'total_records' => $data->count(),
        'avg_people' => round($data->avg('people_count') ?? 0, 1),
        // Tambahkan data summary lainnya
    ];
}
```

### Mengubah Nama File Download
Edit controller method:
```php
$filename = 'laporan_custom_' . $startDate->format('Y_m_d') . '.pdf';
return $pdf->download($filename);
```

## Tips

1. **Untuk data besar:** Gunakan pagination atau limit data
2. **Untuk gambar:** Gunakan base64 encoding atau absolute path
3. **Untuk font custom:** Tambahkan font di config dompdf
4. **Untuk landscape:** Gunakan `$pdf->setPaper('a4', 'landscape')`
5. **Untuk preview:** Gunakan `$pdf->stream()` instead of `download()`
