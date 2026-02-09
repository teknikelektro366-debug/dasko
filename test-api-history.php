<?php

$url = 'http://127.0.0.1:8000/api/sensor/history?per_page=5';

$ch = curl_init($url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, [
    'Accept: application/json'
]);

$response = curl_exec($ch);
curl_close($ch);

$result = json_decode($response, true);

if ($result && isset($result['data'])) {
    echo "📊 Data dari API History (5 data terakhir):\n";
    echo str_repeat("=", 80) . "\n\n";
    
    foreach ($result['data'] as $item) {
        echo "ID: {$item['id']} | ";
        echo "Tanggal: {$item['created_at']} | ";
        echo "Orang: {$item['people_count']} | ";
        echo "Suhu: {$item['room_temperature']}°C\n";
    }
} else {
    echo "❌ Gagal mengambil data dari API\n";
    echo "Response: $response\n";
}
