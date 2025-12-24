<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Carbon\Carbon;

class EnergyConsumption extends Model
{
    use HasFactory;

    protected $table = 'energy_consumptions';

    protected $fillable = [
        'date',
        'location',
        'device_type',
        'device_name',
        'power_rating_watts',
        'operating_hours',
        'energy_consumed_kwh',
        'cost_per_kwh',
        'total_cost',
        'people_count_avg',
        'temperature_avg',
        'humidity_avg',
        'ac_usage_hours',
        'lamp_usage_hours',
        'other_devices_hours',
        'peak_hours_usage',
        'off_peak_hours_usage',
        'energy_saved_kwh',
        'efficiency_percentage',
        'notes'
    ];

    protected $casts = [
        'date' => 'date',
        'power_rating_watts' => 'integer',
        'operating_hours' => 'decimal:2',
        'energy_consumed_kwh' => 'decimal:3',
        'cost_per_kwh' => 'decimal:2',
        'total_cost' => 'decimal:2',
        'people_count_avg' => 'decimal:1',
        'temperature_avg' => 'decimal:1',
        'humidity_avg' => 'decimal:1',
        'ac_usage_hours' => 'decimal:2',
        'lamp_usage_hours' => 'decimal:2',
        'other_devices_hours' => 'decimal:2',
        'peak_hours_usage' => 'decimal:2',
        'off_peak_hours_usage' => 'decimal:2',
        'energy_saved_kwh' => 'decimal:3',
        'efficiency_percentage' => 'decimal:1'
    ];

    /**
     * Get daily consumption summary
     */
    public static function getDailySummary($date)
    {
        return self::where('date', $date)
                   ->selectRaw('
                       date,
                       SUM(energy_consumed_kwh) as total_energy_kwh,
                       SUM(total_cost) as total_cost,
                       AVG(people_count_avg) as avg_people,
                       AVG(temperature_avg) as avg_temperature,
                       AVG(humidity_avg) as avg_humidity,
                       SUM(energy_saved_kwh) as total_saved_kwh,
                       AVG(efficiency_percentage) as avg_efficiency
                   ')
                   ->groupBy('date')
                   ->first();
    }

    /**
     * Get weekly consumption summary
     */
    public static function getWeeklySummary($startDate, $endDate)
    {
        return self::whereBetween('date', [$startDate, $endDate])
                   ->selectRaw('
                       WEEK(date) as week_number,
                       MIN(date) as week_start,
                       MAX(date) as week_end,
                       SUM(energy_consumed_kwh) as total_energy_kwh,
                       SUM(total_cost) as total_cost,
                       AVG(people_count_avg) as avg_people,
                       AVG(temperature_avg) as avg_temperature,
                       SUM(energy_saved_kwh) as total_saved_kwh,
                       AVG(efficiency_percentage) as avg_efficiency
                   ')
                   ->groupBy('week_number')
                   ->orderBy('week_start')
                   ->get();
    }

    /**
     * Get device type breakdown
     */
    public static function getDeviceBreakdown($startDate, $endDate)
    {
        return self::whereBetween('date', [$startDate, $endDate])
                   ->selectRaw('
                       device_type,
                       COUNT(*) as days_count,
                       SUM(energy_consumed_kwh) as total_energy_kwh,
                       SUM(total_cost) as total_cost,
                       AVG(operating_hours) as avg_operating_hours,
                       SUM(energy_saved_kwh) as total_saved_kwh
                   ')
                   ->groupBy('device_type')
                   ->orderBy('total_energy_kwh', 'desc')
                   ->get();
    }

    /**
     * Get efficiency trends
     */
    public static function getEfficiencyTrends($days = 14)
    {
        return self::where('date', '>=', now()->subDays($days))
                   ->selectRaw('
                       date,
                       SUM(energy_consumed_kwh) as daily_energy_kwh,
                       SUM(energy_saved_kwh) as daily_saved_kwh,
                       AVG(efficiency_percentage) as daily_efficiency,
                       AVG(people_count_avg) as daily_people_avg
                   ')
                   ->groupBy('date')
                   ->orderBy('date')
                   ->get();
    }

    /**
     * Calculate energy savings compared to baseline
     */
    public static function calculateSavings($startDate, $endDate)
    {
        $actualConsumption = self::whereBetween('date', [$startDate, $endDate])
                                ->sum('energy_consumed_kwh');
        
        $energySaved = self::whereBetween('date', [$startDate, $endDate])
                          ->sum('energy_saved_kwh');
        
        $baselineConsumption = $actualConsumption + $energySaved;
        
        $savingsPercentage = $baselineConsumption > 0 
            ? ($energySaved / $baselineConsumption) * 100 
            : 0;
        
        return [
            'actual_consumption_kwh' => $actualConsumption,
            'energy_saved_kwh' => $energySaved,
            'baseline_consumption_kwh' => $baselineConsumption,
            'savings_percentage' => round($savingsPercentage, 1),
            'cost_savings' => $energySaved * 1500 // Rp 1500 per kWh
        ];
    }

    /**
     * Get peak usage hours analysis
     */
    public static function getPeakUsageAnalysis($startDate, $endDate)
    {
        return self::whereBetween('date', [$startDate, $endDate])
                   ->selectRaw('
                       SUM(peak_hours_usage) as total_peak_hours,
                       SUM(off_peak_hours_usage) as total_off_peak_hours,
                       AVG(peak_hours_usage) as avg_peak_hours,
                       AVG(off_peak_hours_usage) as avg_off_peak_hours
                   ')
                   ->first();
    }

    /**
     * Get formatted energy consumption
     */
    public function getFormattedEnergyAttribute()
    {
        return number_format($this->energy_consumed_kwh, 3) . ' kWh';
    }

    /**
     * Get formatted cost
     */
    public function getFormattedCostAttribute()
    {
        return 'Rp ' . number_format($this->total_cost, 0, ',', '.');
    }

    /**
     * Get efficiency status
     */
    public function getEfficiencyStatusAttribute()
    {
        if ($this->efficiency_percentage >= 80) {
            return 'Sangat Efisien';
        } elseif ($this->efficiency_percentage >= 60) {
            return 'Efisien';
        } elseif ($this->efficiency_percentage >= 40) {
            return 'Cukup Efisien';
        } else {
            return 'Kurang Efisien';
        }
    }
}