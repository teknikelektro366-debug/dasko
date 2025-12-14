<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class EnergyLog extends Model
{
    use HasFactory;

    protected $fillable = [
        'device_id',
        'energy_consumed',
        'duration_minutes',
        'start_time',
        'end_time',
        'people_count',
        'outdoor_temperature',
        'ac_target_temperature',
        'energy_saving_mode',
        'working_hours',
        'cost_estimate'
    ];

    protected $casts = [
        'energy_consumed' => 'decimal:3',
        'duration_minutes' => 'integer',
        'start_time' => 'datetime',
        'end_time' => 'datetime',
        'people_count' => 'integer',
        'outdoor_temperature' => 'decimal:1',
        'ac_target_temperature' => 'integer',
        'energy_saving_mode' => 'boolean',
        'working_hours' => 'boolean',
        'cost_estimate' => 'decimal:2'
    ];

    public function device()
    {
        return $this->belongsTo(Device::class);
    }

    public function scopeToday($query)
    {
        return $query->whereDate('start_time', today());
    }

    public function scopeThisWeek($query)
    {
        return $query->whereBetween('start_time', [
            now()->startOfWeek(),
            now()->endOfWeek()
        ]);
    }

    public function scopeThisMonth($query)
    {
        return $query->whereMonth('start_time', now()->month)
                    ->whereYear('start_time', now()->year);
    }

    public function scopeByDevice($query, $deviceType)
    {
        return $query->whereHas('device', function ($q) use ($deviceType) {
            $q->where('type', $deviceType);
        });
    }

    public function getEfficiencyRatingAttribute()
    {
        if (!$this->people_count || $this->people_count == 0) {
            return 'N/A';
        }

        $energyPerPerson = $this->energy_consumed / $this->people_count;

        if ($energyPerPerson < 50) return 'Excellent';
        if ($energyPerPerson < 100) return 'Good';
        if ($energyPerPerson < 150) return 'Average';
        return 'Poor';
    }
}