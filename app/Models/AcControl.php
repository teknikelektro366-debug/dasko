<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Carbon\Carbon;

class AcControl extends Model
{
    use HasFactory;

    protected $table = 'ac_controls';

    protected $fillable = [
        'device_id',
        'location',
        'ac1_status',
        'ac2_status',
        'ac1_temperature',
        'ac2_temperature',
        'control_mode',
        'manual_override',
        'created_by',
        'expires_at'
    ];

    protected $casts = [
        'ac1_status' => 'boolean',
        'ac2_status' => 'boolean',
        'manual_override' => 'boolean',
        'expires_at' => 'datetime'
    ];

    /**
     * Get active AC control for device
     */
    public static function getActiveControl($deviceId, $location = null)
    {
        $query = self::where('device_id', $deviceId);
        
        if ($location) {
            $query->where('location', $location);
        }
        
        return $query->where(function($q) {
                $q->whereNull('expires_at')
                  ->orWhere('expires_at', '>', now());
            })
            ->latest()
            ->first();
    }

    /**
     * Create or update AC control
     */
    public static function setControl($deviceId, $location, $data)
    {
        // Expire existing controls
        self::where('device_id', $deviceId)
            ->where('location', $location)
            ->update(['expires_at' => now()]);

        // Create new control
        return self::create([
            'device_id' => $deviceId,
            'location' => $location,
            'ac1_status' => $data['ac1_status'] ?? false,
            'ac2_status' => $data['ac2_status'] ?? false,
            'ac1_temperature' => $data['ac1_temperature'] ?? 25,
            'ac2_temperature' => $data['ac2_temperature'] ?? 25,
            'control_mode' => $data['control_mode'] ?? 'manual',
            'manual_override' => $data['manual_override'] ?? true,
            'created_by' => $data['created_by'] ?? 'system',
            'expires_at' => $data['expires_at'] ?? null
        ]);
    }

    /**
     * Check if control is expired
     */
    public function isExpired()
    {
        return $this->expires_at && $this->expires_at->isPast();
    }

    /**
     * Get formatted status
     */
    public function getFormattedStatusAttribute()
    {
        if (!$this->ac1_status && !$this->ac2_status) {
            return 'OFF';
        }
        
        if ($this->ac1_status && $this->ac2_status) {
            return '2 AC ON';
        }
        
        return '1 AC ON';
    }

    /**
     * Get control summary
     */
    public function getSummaryAttribute()
    {
        $summary = [];
        
        if ($this->ac1_status) {
            $summary[] = "AC1: ON ({$this->ac1_temperature}°C)";
        } else {
            $summary[] = "AC1: OFF";
        }
        
        if ($this->ac2_status) {
            $summary[] = "AC2: ON ({$this->ac2_temperature}°C)";
        } else {
            $summary[] = "AC2: OFF";
        }
        
        return implode(', ', $summary);
    }
}