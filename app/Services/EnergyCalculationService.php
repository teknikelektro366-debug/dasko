<?php

namespace App\Services;

use App\Models\SensorData;
use Carbon\Carbon;

class EnergyCalculationService
{
    /**
     * Tabel Konsumsi Energi - Ruang Dosen Prodi Teknik Elektro
     * 
     * RUMUS MENGHITUNG PENGGUNAAN ENERGI LISTRIK:
     * Wh = (Daya (Watt) × Lama Pemakaian (Jam)) / kWh
     * 
     * Atau lebih tepatnya:
     * Wh = Daya (Watt) × Lama Pemakaian (Jam)
     * kWh = Wh / 1000
     * 
     * Biaya = kWh × Tarif (Rp 1.445/kWh)
     */
    private $devices = [
        'AC Panasonic CS-YN12WKJ' => ['qty' => 2, 'watt' => 1050, 'hours' => 8],
        'Lampu TL' => ['qty' => 12, 'watt' => 22, 'hours' => 8],
        'Dispenser Sanken' => ['qty' => 1, 'watt' => 420, 'hours' => 24],
        'Smart TV Horion' => ['qty' => 1, 'watt' => 170, 'hours' => 8],
        'Kulkas Sharp' => ['qty' => 1, 'watt' => 90, 'hours' => 24],
        'Komputer' => ['qty' => 2, 'watt' => 90, 'hours' => 8],
        'Router Wi-Fi' => ['qty' => 1, 'watt' => 20, 'hours' => 24],
        'Panci Listrik' => ['qty' => 1, 'watt' => 300, 'hours' => 1],
        'Setrika Listrik' => ['qty' => 1, 'watt' => 350, 'hours' => 1],
        'Mesin Kopi' => ['qty' => 1, 'watt' => 1350, 'hours' => 1],
    ];

    private $tarif = 1445; // Rp per kWh

    /**
     * Calculate AC energy consumption
     * Rumus: Wh = Watt × Jam
     *        kWh = Wh / 1000
     */
    public function calculateACEnergy($startTime, $endTime, $acStatus)
    {
        if ($acStatus === 'OFF' || $acStatus === 'AC OFF') {
            return ['consumption_kwh' => 0, 'consumption_wh' => 0, 'cost' => 0, 'hours' => 0];
        }

        $hours = Carbon::parse($endTime)->diffInHours(Carbon::parse($startTime));
        
        // 1 AC = 1050W, 2 AC = 2100W
        $watt = (strpos($acStatus, 'AC 1+2') !== false) ? 2100 : 1050;
        
        // Wh = Watt × Jam
        $wh = $watt * $hours;
        
        // kWh = Wh / 1000
        $kwh = $wh / 1000;
        
        $cost = $kwh * $this->tarif;

        return [
            'consumption_wh' => round($wh, 0),
            'consumption_kwh' => round($kwh, 2),
            'cost' => round($cost, 0),
            'hours' => $hours,
        ];
    }

    /**
     * Calculate Lamp energy consumption
     * Rumus: Wh = Watt × Jam × Jumlah
     *        kWh = Wh / 1000
     */
    public function calculateLampEnergy($startTime, $endTime, $lampStatus)
    {
        if ($lampStatus === 'OFF') {
            return ['consumption_kwh' => 0, 'consumption_wh' => 0, 'cost' => 0, 'hours' => 0];
        }

        $hours = Carbon::parse($endTime)->diffInHours(Carbon::parse($startTime));
        
        // 12 lampu × 22W = 264W
        $watt = 264;
        
        // Wh = Watt × Jam
        $wh = $watt * $hours;
        
        // kWh = Wh / 1000
        $kwh = $wh / 1000;
        
        $cost = $kwh * $this->tarif;

        return [
            'consumption_wh' => round($wh, 0),
            'consumption_kwh' => round($kwh, 2),
            'cost' => round($cost, 0),
            'hours' => $hours,
        ];
    }

    /**
     * Calculate daily energy from sensor data
     * Menggunakan rumus: Wh = Watt × Jam, kemudian kWh = Wh / 1000
     */
    public function calculateDailyEnergy($date)
    {
        $start = Carbon::parse($date)->startOfDay();
        $end = Carbon::parse($date)->endOfDay();

        $data = SensorData::whereBetween('created_at', [$start, $end])
                         ->orderBy('created_at', 'asc')
                         ->get();

        $totalACWh = 0;
        $totalLampWh = 0;
        $acHours = 0;
        $lampHours = 0;

        // Calculate AC & Lamp consumption
        for ($i = 0; $i < $data->count() - 1; $i++) {
            $ac = $this->calculateACEnergy(
                $data[$i]->created_at,
                $data[$i + 1]->created_at,
                $data[$i]->ac_status
            );
            $totalACWh += $ac['consumption_wh'];
            $acHours += $ac['hours'];

            $lamp = $this->calculateLampEnergy(
                $data[$i]->created_at,
                $data[$i + 1]->created_at,
                $data[$i]->lamp_status
            );
            $totalLampWh += $lamp['consumption_wh'];
            $lampHours += $lamp['hours'];
        }

        // Convert Wh to kWh
        $totalACKWh = $totalACWh / 1000;
        $totalLampKWh = $totalLampWh / 1000;

        // Base consumption (devices running 24h)
        // Dispenser (420W) + Kulkas (90W) + Router (20W) = 530W
        // Wh = 530W × 24h = 12,720 Wh
        // kWh = 12,720 / 1000 = 12.72 kWh
        $baseWh = 530 * 24;
        $baseKWh = $baseWh / 1000;
        $baseCost = $baseKWh * $this->tarif;

        return [
            'date' => $start->format('Y-m-d'),
            'ac_wh' => round($totalACWh, 0),
            'ac_kwh' => round($totalACKWh, 2),
            'ac_cost' => round($totalACKWh * $this->tarif, 0),
            'ac_hours' => round($acHours, 2),
            'lamp_wh' => round($totalLampWh, 0),
            'lamp_kwh' => round($totalLampKWh, 2),
            'lamp_cost' => round($totalLampKWh * $this->tarif, 0),
            'lamp_hours' => round($lampHours, 2),
            'base_wh' => $baseWh,
            'base_kwh' => $baseKWh,
            'base_cost' => round($baseCost, 0),
            'total_wh' => round($totalACWh + $totalLampWh + $baseWh, 0),
            'total_kwh' => round($totalACKWh + $totalLampKWh + $baseKWh, 2),
            'total_cost' => round(($totalACKWh + $totalLampKWh + $baseKWh) * $this->tarif, 0),
        ];
    }

    /**
     * Calculate monthly energy
     */
    public function calculateMonthlyEnergy($month)
    {
        $start = Carbon::parse($month)->startOfMonth();
        $end = Carbon::parse($month)->endOfMonth();

        $totalAC = 0;
        $totalLamp = 0;
        $totalBase = 0;
        $dailyData = [];

        for ($date = $start->copy(); $date->lte($end); $date->addDay()) {
            $daily = $this->calculateDailyEnergy($date->format('Y-m-d'));
            $totalAC += $daily['ac_kwh'];
            $totalLamp += $daily['lamp_kwh'];
            $totalBase += $daily['base_kwh'];
            $dailyData[] = $daily;
        }

        return [
            'month' => $start->format('F Y'),
            'ac_kwh' => round($totalAC, 2),
            'ac_cost' => round($totalAC * $this->tarif, 0),
            'lamp_kwh' => round($totalLamp, 2),
            'lamp_cost' => round($totalLamp * $this->tarif, 0),
            'base_kwh' => round($totalBase, 2),
            'base_cost' => round($totalBase * $this->tarif, 0),
            'total_kwh' => round($totalAC + $totalLamp + $totalBase, 2),
            'total_cost' => round(($totalAC + $totalLamp + $totalBase) * $this->tarif, 0),
            'daily_data' => $dailyData,
        ];
    }

    /**
     * Get device list with energy calculation
     * Rumus: Wh = Watt × Jumlah × Jam
     *        kWh = Wh / 1000
     */
    public function getDevicePowerData()
    {
        $result = [];
        foreach ($this->devices as $name => $device) {
            // Wh = Watt × Qty × Hours
            $wh = $device['watt'] * $device['qty'] * $device['hours'];
            
            // kWh = Wh / 1000
            $kwh = $wh / 1000;
            
            $result[$name] = [
                'quantity' => $device['qty'],
                'watt_per_unit' => $device['watt'],
                'hours_per_day' => $device['hours'],
                'consumption_wh' => round($wh, 0),
                'consumption_kwh' => round($kwh, 2),
                'cost' => round($kwh * $this->tarif, 0),
            ];
        }
        return $result;
    }

    /**
     * Get total daily consumption
     * Total Wh dari semua perangkat, kemudian convert ke kWh
     */
    public function getTotalDailyConsumption()
    {
        $totalWh = 0;
        foreach ($this->devices as $device) {
            // Wh = Watt × Qty × Hours
            $totalWh += $device['watt'] * $device['qty'] * $device['hours'];
        }
        // kWh = Wh / 1000
        return round($totalWh / 1000, 2);
    }

    /**
     * Get total daily cost
     */
    public function getTotalDailyCost()
    {
        return round($this->getTotalDailyConsumption() * $this->tarif, 0);
    }

    /**
     * Calculate energy savings
     * Membandingkan konsumsi aktual vs konsumsi jika AC & Lampu jalan terus 8 jam
     */
    public function calculateEnergySavings($date)
    {
        $daily = $this->calculateDailyEnergy($date);
        
        // Konsumsi maksimal jika AC & Lampu jalan 8 jam penuh
        // AC: 2 unit × 1050W × 8h = 16.8 kWh
        // Lampu: 12 unit × 22W × 8h = 2.112 kWh
        $maxACKWh = 16.8;
        $maxLampKWh = 2.112;
        $maxTotalKWh = $maxACKWh + $maxLampKWh; // 18.912 kWh
        
        // Konsumsi aktual dari sensor
        $actualACKWh = $daily['ac_kwh'];
        $actualLampKWh = $daily['lamp_kwh'];
        $actualTotalKWh = $actualACKWh + $actualLampKWh;
        
        // Energi yang dihemat
        $savedKWh = $maxTotalKWh - $actualTotalKWh;
        $savedCost = $savedKWh * $this->tarif;
        
        // Persentase penghematan
        $savingsPercentage = ($maxTotalKWh > 0) ? ($savedKWh / $maxTotalKWh) * 100 : 0;
        
        return [
            'date' => $date,
            'max_consumption_kwh' => round($maxTotalKWh, 2),
            'actual_consumption_kwh' => round($actualTotalKWh, 2),
            'saved_kwh' => round($savedKWh, 2),
            'saved_cost' => round($savedCost, 0),
            'savings_percentage' => round($savingsPercentage, 1),
            'max_ac_kwh' => $maxACKWh,
            'actual_ac_kwh' => round($actualACKWh, 2),
            'max_lamp_kwh' => $maxLampKWh,
            'actual_lamp_kwh' => round($actualLampKWh, 2),
        ];
    }

    /**
     * Calculate monthly energy savings
     */
    public function calculateMonthlySavings($month)
    {
        $start = Carbon::parse($month)->startOfMonth();
        $end = Carbon::parse($month)->endOfMonth();

        $totalSavedKWh = 0;
        $totalSavedCost = 0;
        $totalMaxKWh = 0;
        $totalActualKWh = 0;

        for ($date = $start->copy(); $date->lte($end); $date->addDay()) {
            $savings = $this->calculateEnergySavings($date->format('Y-m-d'));
            $totalSavedKWh += $savings['saved_kwh'];
            $totalSavedCost += $savings['saved_cost'];
            $totalMaxKWh += $savings['max_consumption_kwh'];
            $totalActualKWh += $savings['actual_consumption_kwh'];
        }

        $savingsPercentage = ($totalMaxKWh > 0) ? ($totalSavedKWh / $totalMaxKWh) * 100 : 0;

        return [
            'month' => $start->format('F Y'),
            'max_consumption_kwh' => round($totalMaxKWh, 2),
            'actual_consumption_kwh' => round($totalActualKWh, 2),
            'saved_kwh' => round($totalSavedKWh, 2),
            'saved_cost' => round($totalSavedCost, 0),
            'savings_percentage' => round($savingsPercentage, 1),
        ];
    }
}
