<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Models\AcControl;
use Illuminate\Http\Request;
use Illuminate\Http\JsonResponse;
use Illuminate\Support\Facades\Log;
use Illuminate\Support\Facades\Validator;

class AcControlController extends Controller
{
    /**
     * Get current AC control settings for Arduino
     */
    public function getControl(Request $request): JsonResponse
    {
        try {
            $deviceId = $request->input('device_id', 'ESP32_Smart_Energy');
            $location = $request->input('location', 'Lab Teknik Tegangan Tinggi');

            $control = AcControl::getActiveControl($deviceId, $location);

            if (!$control) {
                return response()->json([
                    'success' => true,
                    'message' => 'No active control found, using auto mode',
                    'data' => [
                        'control_mode' => 'auto',
                        'manual_override' => false,
                        'ac1_status' => false,
                        'ac2_status' => false,
                        'ac1_temperature' => 25,
                        'ac2_temperature' => 25,
                        'expires_at' => null
                    ]
                ]);
            }

            // Check if expired
            if ($control->isExpired()) {
                return response()->json([
                    'success' => true,
                    'message' => 'Control expired, using auto mode',
                    'data' => [
                        'control_mode' => 'auto',
                        'manual_override' => false,
                        'ac1_status' => false,
                        'ac2_status' => false,
                        'ac1_temperature' => 25,
                        'ac2_temperature' => 25,
                        'expires_at' => null
                    ]
                ]);
            }

            return response()->json([
                'success' => true,
                'message' => 'Active control found',
                'data' => [
                    'id' => $control->id,
                    'control_mode' => $control->control_mode,
                    'manual_override' => $control->manual_override,
                    'ac1_status' => $control->ac1_status,
                    'ac2_status' => $control->ac2_status,
                    'ac1_temperature' => $control->ac1_temperature,
                    'ac2_temperature' => $control->ac2_temperature,
                    'created_by' => $control->created_by,
                    'expires_at' => $control->expires_at?->toISOString(),
                    'formatted_status' => $control->formatted_status,
                    'summary' => $control->summary
                ]
            ]);

        } catch (\Exception $e) {
            Log::error('Error getting AC control:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Error getting AC control',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }

    /**
     * Set AC control from dashboard
     */
    public function setControl(Request $request): JsonResponse
    {
        try {
            $validator = Validator::make($request->all(), [
                'device_id' => 'string|max:255',
                'location' => 'string|max:255',
                'ac1_status' => 'required|boolean',
                'ac2_status' => 'required|boolean',
                'ac1_temperature' => 'required|integer|min:16|max:30',
                'ac2_temperature' => 'required|integer|min:16|max:30',
                'control_mode' => 'required|in:auto,manual,schedule',
                'duration_minutes' => 'nullable|integer|min:1|max:1440', // Max 24 hours
                'created_by' => 'string|max:255'
            ]);

            if ($validator->fails()) {
                return response()->json([
                    'success' => false,
                    'message' => 'Validation failed',
                    'errors' => $validator->errors()
                ], 422);
            }

            $deviceId = $request->input('device_id', 'ESP32_Smart_Energy');
            $location = $request->input('location', 'Lab Teknik Tegangan Tinggi');
            $durationMinutes = $request->input('duration_minutes');

            $controlData = [
                'ac1_status' => $request->input('ac1_status'),
                'ac2_status' => $request->input('ac2_status'),
                'ac1_temperature' => $request->input('ac1_temperature'),
                'ac2_temperature' => $request->input('ac2_temperature'),
                'control_mode' => $request->input('control_mode'),
                'manual_override' => $request->input('control_mode') === 'manual',
                'created_by' => $request->input('created_by', 'dashboard'),
                'expires_at' => $durationMinutes ? now()->addMinutes($durationMinutes) : null
            ];

            $control = AcControl::setControl($deviceId, $location, $controlData);

            Log::info('AC control set:', [
                'control_id' => $control->id,
                'device_id' => $deviceId,
                'location' => $location,
                'summary' => $control->summary,
                'created_by' => $control->created_by
            ]);

            return response()->json([
                'success' => true,
                'message' => 'AC control berhasil diatur',
                'data' => [
                    'id' => $control->id,
                    'control_mode' => $control->control_mode,
                    'manual_override' => $control->manual_override,
                    'ac1_status' => $control->ac1_status,
                    'ac2_status' => $control->ac2_status,
                    'ac1_temperature' => $control->ac1_temperature,
                    'ac2_temperature' => $control->ac2_temperature,
                    'expires_at' => $control->expires_at?->toISOString(),
                    'formatted_status' => $control->formatted_status,
                    'summary' => $control->summary
                ]
            ], 201);

        } catch (\Exception $e) {
            Log::error('Error setting AC control:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString(),
                'request_data' => $request->all()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Error setting AC control',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }

    /**
     * Turn off all AC units (emergency stop)
     */
    public function emergencyStop(Request $request): JsonResponse
    {
        try {
            $deviceId = $request->input('device_id', 'ESP32_Smart_Energy');
            $location = $request->input('location', 'Lab Teknik Tegangan Tinggi');

            $controlData = [
                'ac1_status' => false,
                'ac2_status' => false,
                'ac1_temperature' => 25,
                'ac2_temperature' => 25,
                'control_mode' => 'manual',
                'manual_override' => true,
                'created_by' => 'emergency_stop',
                'expires_at' => null
            ];

            $control = AcControl::setControl($deviceId, $location, $controlData);

            Log::warning('Emergency AC stop activated:', [
                'control_id' => $control->id,
                'device_id' => $deviceId,
                'location' => $location
            ]);

            return response()->json([
                'success' => true,
                'message' => 'Emergency stop activated - All AC units turned OFF',
                'data' => [
                    'id' => $control->id,
                    'control_mode' => $control->control_mode,
                    'ac1_status' => false,
                    'ac2_status' => false,
                    'formatted_status' => 'OFF'
                ]
            ]);

        } catch (\Exception $e) {
            Log::error('Error during emergency stop:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Error during emergency stop',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }

    /**
     * Return to auto mode
     */
    public function autoMode(Request $request): JsonResponse
    {
        try {
            $deviceId = $request->input('device_id', 'ESP32_Smart_Energy');
            $location = $request->input('location', 'Lab Teknik Tegangan Tinggi');

            // Expire all existing controls
            AcControl::where('device_id', $deviceId)
                     ->where('location', $location)
                     ->update(['expires_at' => now()]);

            Log::info('AC control returned to auto mode:', [
                'device_id' => $deviceId,
                'location' => $location
            ]);

            return response()->json([
                'success' => true,
                'message' => 'AC control returned to auto mode',
                'data' => [
                    'control_mode' => 'auto',
                    'manual_override' => false,
                    'message' => 'Arduino will control AC based on people count'
                ]
            ]);

        } catch (\Exception $e) {
            Log::error('Error returning to auto mode:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Error returning to auto mode',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }

    /**
     * Get AC control history
     */
    public function history(Request $request): JsonResponse
    {
        try {
            $deviceId = $request->input('device_id', 'ESP32_Smart_Energy');
            $location = $request->input('location', 'Lab Teknik Tegangan Tinggi');
            $perPage = min(max($request->input('per_page', 20), 10), 100);

            $history = AcControl::where('device_id', $deviceId)
                               ->where('location', $location)
                               ->latest()
                               ->paginate($perPage);

            return response()->json([
                'success' => true,
                'data' => $history->items(),
                'pagination' => [
                    'current_page' => $history->currentPage(),
                    'last_page' => $history->lastPage(),
                    'per_page' => $history->perPage(),
                    'total' => $history->total()
                ]
            ]);

        } catch (\Exception $e) {
            Log::error('Error getting AC control history:', [
                'message' => $e->getMessage(),
                'trace' => $e->getTraceAsString()
            ]);

            return response()->json([
                'success' => false,
                'message' => 'Error getting AC control history',
                'error' => config('app.debug') ? $e->getMessage() : 'Internal server error'
            ], 500);
        }
    }
}