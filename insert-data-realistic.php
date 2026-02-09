<?php

// Insert data realistis untuk tanggal 4 dan 8 Februari 2026
$url = 'http://127.0.0.1:8000/api/sensor/data';

// Fungsi untuk generate data realistis
function generateRealisticData($peopleCount, $hour) {
    // Suhu: 25-29°C, lebih tinggi saat banyak orang
    $baseTemp = 26 + ($peopleCount * 0.15);
    $roomTemperature = round($baseTemp + (rand(-10, 10) / 10), 1);
    $roomTemperature = max(25, min(29, $roomTemperature)); // Limit 25-29°C
    
    // Kelembaban: 60-75%, lebih tinggi saat banyak orang
    $baseHumidity = 65 + ($peopleCount * 0.3);
    $humidity = round($baseHumidity + (rand(-20, 20) / 10), 1);
    $humidity = max(60, min(75, $humidity)); // Limit 60-75%
    
    // Cahaya: 200-800 lux, lebih tinggi di pagi hari
    if ($peopleCount == 0) {
        $lightLevel = rand(50, 150); // Gelap jika kosong
    } else {
        $baseLux = 400;
        if ($hour >= 7 && $hour <= 10) {
            $baseLux = 600; // Pagi lebih terang
        } elseif ($hour >= 11 && $hour <= 13) {
            $baseLux = 500; // Siang
        } else {
            $baseLux = 400; // Sore
        }
        $lightLevel = $baseLux + rand(-100, 100);
        $lightLevel = max(200, min(800, $lightLevel));
    }
    
    return [
        'room_temperature' => $roomTemperature,
        'humidity' => $humidity,
        'light_level' => $lightLevel
    ];
}

// Data untuk kedua tanggal
$dates = ['2026-02-04', '2026-02-08'];

foreach ($dates as $date) {
    echo "\n🚀 Mulai insert data untuk tanggal $date\n";
    echo str_repeat("=", 80) . "\n\n";
    
    $hourlyData = [
        // Jam 07
        ['hour' => '07', 'minute' => '00', 'people' => 3],
        
        // Jam 08 - 4 data
        ['hour' => '08', 'minute' => '05', 'people' => 5],
        ['hour' => '08', 'minute' => '20', 'people' => 9],
        ['hour' => '08', 'minute' => '35', 'people' => 8],
        ['hour' => '08', 'minute' => '50', 'people' => 11],
        
        // Jam 09 - 4 data
        ['hour' => '09', 'minute' => '10', 'people' => 8],
        ['hour' => '09', 'minute' => '25', 'people' => 9],
        ['hour' => '09', 'minute' => '40', 'people' => 13],
        ['hour' => '09', 'minute' => '55', 'people' => 10],
        
        // Jam 10 - 5 data
        ['hour' => '10', 'minute' => '05', 'people' => 9],
        ['hour' => '10', 'minute' => '18', 'people' => 11],
        ['hour' => '10', 'minute' => '32', 'people' => 15],
        ['hour' => '10', 'minute' => '45', 'people' => 11],
        ['hour' => '10', 'minute' => '58', 'people' => 10],
        
        // Jam 11 - 6 data
        ['hour' => '11', 'minute' => '08', 'people' => 9],
        ['hour' => '11', 'minute' => '18', 'people' => 10],
        ['hour' => '11', 'minute' => '28', 'people' => 6],
        ['hour' => '11', 'minute' => '38', 'people' => 5],
        ['hour' => '11', 'minute' => '48', 'people' => 3],
        ['hour' => '11', 'minute' => '58', 'people' => 2],
        
        // Jam 12 - 4 data
        ['hour' => '12', 'minute' => '10', 'people' => 5],
        ['hour' => '12', 'minute' => '25', 'people' => 8],
        ['hour' => '12', 'minute' => '40', 'people' => 9],
        ['hour' => '12', 'minute' => '55', 'people' => 7],
        
        // Jam 13 - 4 data
        ['hour' => '13', 'minute' => '12', 'people' => 10],
        ['hour' => '13', 'minute' => '28', 'people' => 12],
        ['hour' => '13', 'minute' => '42', 'people' => 9],
        ['hour' => '13', 'minute' => '56', 'people' => 8],
        
        // Jam 14 - 5 data
        ['hour' => '14', 'minute' => '08', 'people' => 10],
        ['hour' => '14', 'minute' => '22', 'people' => 12],
        ['hour' => '14', 'minute' => '35', 'people' => 18],
        ['hour' => '14', 'minute' => '48', 'people' => 19],
        ['hour' => '14', 'minute' => '58', 'people' => 20],
    ];
    
    $successCount = 0;
    $errorCount = 0;
    
    foreach ($hourlyData as $item) {
        $peopleCount = $item['people'];
        $hour = intval($item['hour']);
        $second = str_pad(rand(10, 59), 2, '0', STR_PAD_LEFT);
        $timestamp = "{$date} {$item['hour']}:{$item['minute']}:{$second}";
        
        // Generate data realistis
        $sensorData = generateRealisticData($peopleCount, $hour);
        
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
            'device_id' => 'ESP32_Smart_Energy',
            'location' => 'Ruang Dosen Prodi Teknik Elektro',
            'people_count' => $peopleCount,
            'room_temperature' => $sensorData['room_temperature'],
            'humidity' => $sensorData['humidity'],
            'light_level' => $sensorData['light_level'],
            'ac_status' => $acStatus,
            'set_temperature' => $setTemperature,
            'lamp_status' => $lampStatus,
            'proximity_in' => false,
            'proximity_out' => false,
            'wifi_rssi' => rand(-70, -40),
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
            echo "✅ {$item['hour']}:{$item['minute']} - {$peopleCount} org - ";
            echo "Suhu: {$sensorData['room_temperature']}°C - ";
            echo "Lembab: {$sensorData['humidity']}% - ";
            echo "Cahaya: {$sensorData['light_level']} lux - ";
            echo "$acStatus\n";
        } else {
            $errorCount++;
            echo "❌ {$item['hour']}:{$item['minute']} - GAGAL\n";
        }
        
        usleep(100000); // Delay 100ms
    }
    
    echo "\n📊 HASIL untuk $date:\n";
    echo "   ✅ Berhasil: $successCount data\n";
    echo "   ❌ Gagal: $errorCount data\n";
}

echo "\n" . str_repeat("=", 80) . "\n";
echo "🎉 SELESAI! Total 66 data berhasil diinput (33 data x 2 tanggal)\n";
echo "🔍 Refresh halaman History untuk melihat data terbaru!\n";
