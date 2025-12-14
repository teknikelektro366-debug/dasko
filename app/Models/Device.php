<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class Device extends Model
{
    use HasFactory;

    protected $fillable = [
        'name',
        'type',
        'power_consumption',
        'units',
        'location',
        'is_active',
        'is_always_on',
        'gpio_pin',
        'relay_pin',
        'description'
    ];

    protected $casts = [
        'is_active' => 'boolean',
        'is_always_on' => 'boolean',
        'power_consumption' => 'decimal:2'
    ];

    // Device types
    const TYPE_AC = 'ac';
    const TYPE_LAMP = 'lamp';
    const TYPE_COMPUTER = 'computer';
    const TYPE_TV = 'tv';
    const TYPE_DISPENSER = 'dispenser';
    const TYPE_KETTLE = 'kettle';
    const TYPE_COFFEE = 'coffee';
    const TYPE_FRIDGE = 'fridge';
    const TYPE_ROUTER = 'router';
    const TYPE_PANEL = 'panel';

    public function sensorReadings()
    {
        return $this->hasMany(SensorReading::class);
    }

    public function energyLogs()
    {
        return $this->hasMany(EnergyLog::class);
    }

    public function getEnergyConsumptionAttribute()
    {
        return $this->power_consumption * $this->units;
    }

    public function getTotalEnergyTodayAttribute()
    {
        return $this->energyLogs()
            ->whereDate('created_at', today())
            ->sum('energy_consumed');
    }

    public function scopeActive($query)
    {
        return $query->where('is_active', true);
    }

    public function scopeByType($query, $type)
    {
        return $query->where('type', $type);
    }
}