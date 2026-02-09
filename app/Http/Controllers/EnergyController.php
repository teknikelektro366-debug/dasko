<?php

namespace App\Http\Controllers;

use App\Services\EnergyCalculationService;
use Illuminate\Http\Request;
use Carbon\Carbon;

class EnergyController extends Controller
{
    protected $energyService;

    public function __construct(EnergyCalculationService $energyService)
    {
        $this->energyService = $energyService;
    }

    /**
     * Get daily energy consumption
     */
    public function getDailyEnergy(Request $request)
    {
        $date = $request->input('date', now()->format('Y-m-d'));
        $data = $this->energyService->calculateDailyEnergy($date);
        
        return response()->json($data);
    }

    /**
     * Get monthly energy consumption
     */
    public function getMonthlyEnergy(Request $request)
    {
        $month = $request->input('month', now()->format('Y-m'));
        $data = $this->energyService->calculateMonthlyEnergy($month);
        
        return response()->json($data);
    }

    /**
     * Get energy efficiency
     */
    public function getEfficiency(Request $request)
    {
        $dateFrom = $request->input('date_from', now()->subDays(7)->format('Y-m-d'));
        $dateTo = $request->input('date_to', now()->format('Y-m-d'));
        
        $startDate = Carbon::parse($dateFrom);
        $endDate = Carbon::parse($dateTo);
        
        $data = $this->energyService->calculateEfficiency($startDate, $endDate);
        
        return response()->json($data);
    }

    /**
     * Get device power data
     */
    public function getDevicePowerData()
    {
        $data = $this->energyService->getDevicePowerData();
        
        return response()->json([
            'devices' => $data,
            'total_daily_consumption' => $this->energyService->getTotalDailyConsumption(),
            'total_daily_cost' => $this->energyService->getTotalDailyCost(),
        ]);
    }

    /**
     * Get energy savings (daily)
     */
    public function getDailySavings(Request $request)
    {
        $date = $request->input('date', now()->format('Y-m-d'));
        $data = $this->energyService->calculateEnergySavings($date);
        
        return response()->json($data);
    }

    /**
     * Get energy savings (monthly)
     */
    public function getMonthlySavings(Request $request)
    {
        $month = $request->input('month', now()->format('Y-m'));
        $data = $this->energyService->calculateMonthlySavings($month);
        
        return response()->json($data);
    }
}
