<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class SensorReading extends Model
{
    use HasFactory;

    protected $fillable = [
        'device_id',
        'sensor_type',
        'sensor_name',
        'value',
        'unit',
        'location',
        'gpio_pin',
        'reading_time'
    ];

    protected $casts = [
        'value' => 'decimal:2',
        'reading_time' => 'datetime'
    ];

    // Sensor types
    const TYPE_TEMPERATURE = 'temperature';
    const TYPE_HUMIDITY = 'humidity';
    const TYPE_LIGHT = 'light';
    const TYPE_PROXIMITY = 'proximity';
    const TYPE_PEOPLE_COUNT = 'people_count';

    public function device()
    {
        return $this->belongsTo(Device::class);
    }

    public function scopeByType($query, $type)
    {
        return $query->where('sensor_type', $type);
    }

    public function scopeToday($query)
    {
        return $query->whereDate('reading_time', today());
    }

    public function scopeLatest($query)
    {
        return $query->orderBy('reading_time', 'desc');
    }

    public function getFormattedValueAttribute()
    {
        return $this->value . ' ' . $this->unit;
    }
}