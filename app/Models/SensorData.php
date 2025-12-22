<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Carbon\Carbon;

class SensorData extends Model
{
    use HasFactory;

    protected $fillable = [
        'device_id',
        'location',
        'people_count',
        'ac_status',
        'set_temperature',
        'room_temperature',
        'humidity',
        'light_level',
        'proximity_in',
        'proximity_out',
        'wifi_rssi',
        'status',
        'device_timestamp'
    ];

    protected $casts = [
        'people_count' => 'integer',
        'set_temperature' => 'integer',
        'room_temperature' => 'decimal:2',
        'humidity' => 'decimal:2',
        'light_level' => 'integer',
        'proximity_in' => 'boolean',
        'proximity_out' => 'boolean',
        'wifi_rssi' => 'integer',
        'device_timestamp' => 'integer',
        'created_at' => 'datetime',
        'updated_at' => 'datetime'
    ];

    // Scope untuk data terbaru
    public function scopeLatest($query)
    {
        return $query->orderBy('created_at', 'desc');
    }

    // Scope untuk data hari ini
    public function scopeToday($query)
    {
        return $query->whereDate('created_at', today());
    }

    // Scope untuk data dalam 1 jam terakhir
    public function scopeLastHour($query)
    {
        return $query->where('created_at', '>=', now()->subHour());
    }

    // Scope untuk device tertentu
    public function scopeByDevice($query, $deviceId)
    {
        return $query->where('device_id', $deviceId);
    }

    // Get data terbaru untuk device
    public static function getLatestByDevice($deviceId = 'ESP32_Smart_Energy')
    {
        return static::byDevice($deviceId)->latest()->first();
    }

    // Get data untuk chart (24 jam terakhir)
    public static function getChartData($hours = 24)
    {
        return static::where('created_at', '>=', now()->subHours($hours))
                    ->orderBy('created_at')
                    ->get()
                    ->groupBy(function($item) {
                        return $item->created_at->format('H:i');
                    });
    }

    // Get statistik harian
    public static function getDailyStats()
    {
        $today = static::today();
        
        return [
            'today_count' => $today->count(),
            'avg_people' => round($today->avg('people_count') ?? 0, 1),
            'max_people' => $today->max('people_count') ?? 0,
            'avg_temperature' => round($today->avg('room_temperature') ?? 0, 1),
            'avg_humidity' => round($today->avg('humidity') ?? 0, 1),
            'avg_light' => round($today->avg('light_level') ?? 0, 0),
            'ac_on_time' => $today->where('ac_status', '!=', 'OFF')->count(),
            'ac_on_percentage' => $today->count() > 0 ? 
                round(($today->where('ac_status', '!=', 'OFF')->count() / $today->count()) * 100, 1) : 0,
            'last_update' => $today->latest()->first()?->created_at,
            'first_data_today' => $today->oldest()->first()?->created_at,
            'data_frequency' => static::calculateTodayFrequency()
        ];
    }

    // Calculate average data frequency for today
    private static function calculateTodayFrequency()
    {
        $todayData = static::today()->orderBy('created_at')->limit(10)->get();
        
        if ($todayData->count() < 2) {
            return null;
        }

        $intervals = [];
        for ($i = 1; $i < $todayData->count(); $i++) {
            $interval = $todayData[$i]->created_at->diffInSeconds($todayData[$i-1]->created_at);
            $intervals[] = $interval;
        }

        return round(array_sum($intervals) / count($intervals), 1);
    }

    // Cleanup data lama (lebih dari 30 hari)
    public static function cleanup($days = 30)
    {
        return static::where('created_at', '<', now()->subDays($days))->delete();
    }

    // Format untuk display
    public function getFormattedTemperatureAttribute()
    {
        return $this->room_temperature ? number_format($this->room_temperature, 1) . '°C' : 'N/A';
    }

    public function getFormattedHumidityAttribute()
    {
        return $this->humidity ? number_format($this->humidity, 1) . '%' : 'N/A';
    }

    public function getFormattedLightAttribute()
    {
        return $this->light_level . ' lux';
    }

    public function getFormattedWifiAttribute()
    {
        if (!$this->wifi_rssi) return 'N/A';
        
        $rssi = $this->wifi_rssi;
        if ($rssi >= -50) return 'Excellent (' . $rssi . ' dBm)';
        if ($rssi >= -60) return 'Good (' . $rssi . ' dBm)';
        if ($rssi >= -70) return 'Fair (' . $rssi . ' dBm)';
        return 'Poor (' . $rssi . ' dBm)';
    }

    public function getTimeAgoAttribute()
    {
        return $this->created_at->diffForHumans();
    }
}