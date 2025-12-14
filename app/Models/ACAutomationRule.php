<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class ACAutomationRule extends Model
{
    use HasFactory;

    protected $table = 'ac_automation_rules';

    protected $fillable = [
        'min_people',
        'max_people',
        'min_outdoor_temp',
        'max_outdoor_temp',
        'ac_units_needed',
        'target_temperature',
        'description',
        'is_active'
    ];

    protected $casts = [
        'min_people' => 'integer',
        'max_people' => 'integer',
        'min_outdoor_temp' => 'decimal:1',
        'max_outdoor_temp' => 'decimal:1',
        'ac_units_needed' => 'integer',
        'target_temperature' => 'integer',
        'is_active' => 'boolean'
    ];

    public function scopeActive($query)
    {
        return $query->where('is_active', true);
    }

    public function scopeForCondition($query, $peopleCount, $outdoorTemp)
    {
        return $query->where('min_people', '<=', $peopleCount)
                    ->where('max_people', '>=', $peopleCount)
                    ->where('min_outdoor_temp', '<=', $outdoorTemp)
                    ->where('max_outdoor_temp', '>=', $outdoorTemp)
                    ->where('is_active', true);
    }

    public function matches($peopleCount, $outdoorTemp)
    {
        return $peopleCount >= $this->min_people 
            && $peopleCount <= $this->max_people
            && $outdoorTemp >= $this->min_outdoor_temp
            && $outdoorTemp <= $this->max_outdoor_temp
            && $this->is_active;
    }

    public function getConditionTextAttribute()
    {
        $peopleText = $this->min_people == $this->max_people 
            ? $this->min_people 
            : $this->min_people . '-' . $this->max_people;
            
        $tempText = $this->min_outdoor_temp == $this->max_outdoor_temp
            ? $this->min_outdoor_temp . '°C'
            : $this->min_outdoor_temp . '-' . $this->max_outdoor_temp . '°C';

        return "{$peopleText} orang, {$tempText}";
    }

    public function getActionTextAttribute()
    {
        if ($this->ac_units_needed == 0) {
            return $this->target_temperature == 28 ? 'Mode Gradual' : 'AC Mati';
        }

        return "{$this->ac_units_needed} AC, target {$this->target_temperature}°C";
    }
}