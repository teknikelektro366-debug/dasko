<?php

// Insert data untuk tanggal 4 Februari 2026, jam 07:00 - 15:00
$url = 'http://127.0.0.1:8000/api/sensor/data';
$date = '2026-02-04';

// Pola data natural untuk Ruang Dosen
$hourlyData = [
    ['hour' => '07', 'people' => 3, 'note' => 'Dosen mulai datang'],
    ['hour' => '08', 'people' => 8, 'note' => 'Jam kerja dimulai'],
    ['hour' => '09', 'people' => 12, 'note' => 'Peak - Banyak dosen & mahasiswa'],
    ['hour' => '10', 'people' => 10, 'note' => 'Dosen mengajar, bimbingan'],
    ['hour' => '11', 'people' => 7, 'note' => 'Beberapa dosen istirahat'],
    ['hour' => '12', 'people' => 4, 'note' => 'Jam istirahat siang'],
    ['hour' => '13', 'people' => 9, 'note' => 'Kembali dari istirahat'],
    ['hour' => '14', 'people' => 11, 'note' => 'Peak siang - Bimbingan & rapat'],
    ['hour' => '15', 'people' => 5, 'note' => 'Jam pulang, mulai sepi']
];

echo "🚀 Mulai insert data untuk tanggal $date (Jam 07:00-15:00)\n";
echo str_repeat("=", 60) . "\n\n";

$successCount = 0;
$errorCount = 0;

foreach ($hourlyData as $item) {
    $peopleCount = $item['people'];
    $timestamp = "$date {$item['hour']}:00:00";
    
    // Logika Lampu
    $lampStatus = $peopleCount > 0 ? 'ON' : 'OFF';
    
    // Logika AC & Suhu
    $acStatus = 'OFF';
    $setTemperature = null;
    
    if ($peopleCount >= 1 && $peopleCount <= 5) {
        $acStatus = '1 AC ON';
        $setTemperature = 25;
    } elseif ($peopleCount >= 6 && $peopleCount <= 9) {
        $acStatus = '2 AC ON';
        $setTemperature = 25;
    } elseif ($peopleCount >= 10 && $peopleCount <= 15) {
        $acStatus = '2 AC ON';
        $setTemperature = 22;
    } elseif ($peopleCount >= 16) {
        $acStatus = '2 AC ON';
        $setTemperature = 20;
    }
    
    $data = [
        'device_id' => 'MANUAL_INPUT',
        'location' => 'Ruang Dosen Prodi Teknik Elektro',
        'people_count' => $peopleCount,
        'room_temperature' => 27,
        'humidity' => 65,
        'light_level' => $peopleCount > 0 ? 500 : 0,
        'ac_status' => $acStatus,
        'set_temperature' => $setTemperature,
        'lamp_status' => $lampStatus,
        'proximity_in' => false,
        'proximity_out' => false,
        'wifi_rssi' => 0,
        'status' => 'active',
        'created_at' => $timestamp
    ];
    
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
    curl_setopt($ch, CURLOPT_HTTPHEADER, [
        'Content-Type: application/json',
        'Accept: application/json'
    ]);
    
    $response = curl_exec($ch);
    $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    curl_close($ch);
    
    $result = json_decode($response, true);
    
    if ($httpCode == 201 && $result && isset($result['success']) && $result['success']) {
        $successCount++;
        echo "✅ {$item['hour']}:00 - {$peopleCount} orang - $lampStatus - $acStatus";
        if ($setTemperature) echo " ({$setTemperature}°C)";
        echo " - ID: {$result['data']['id']}\n";
    } else {
        $errorCount++;
        echo "❌ {$item['hour']}:00 - GAGAL: " . ($result['message'] ?? 'Unknown error') . "\n";
    }
    
    // Delay 100ms
    usleep(100000);
}

echo "\n" . str_repeat("=", 60) . "\n";
echo "📊 HASIL:\n";
echo "   ✅ Berhasil: $successCount data\n";
echo "   ❌ Gagal: $errorCount data\n";
echo "\n🔍 Cek di History: http://127.0.0.1:8000/dashboard (tab History)\n";
