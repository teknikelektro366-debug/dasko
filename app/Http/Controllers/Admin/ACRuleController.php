<?php

namespace App\Http\Controllers\Admin;

use App\Http\Controllers\Controller;
use App\Models\ACAutomationRule;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Cache;

class ACRuleController extends Controller
{
    public function index()
    {
        $rules = ACAutomationRule::orderBy('min_people')->paginate(15);
        
        // Get current conditions for testing
        $sensorData = Cache::get('latest_sensor_data', [
            'people_count' => 0,
            'temperature' => 27.0
        ]);

        $activeRule = ACAutomationRule::forCondition(
            $sensorData['people_count'],
            $sensorData['temperature']
        )->first();

        return view('admin.ac-rules.index', compact('rules', 'sensorData', 'activeRule'));
    }

    public function create()
    {
        return view('admin.ac-rules.create');
    }

    public function store(Request $request)
    {
        $request->validate([
            'min_people' => 'required|integer|min:0|max:100',
            'max_people' => 'required|integer|min:0|max:100|gte:min_people',
            'min_outdoor_temp' => 'required|numeric|between:0,50',
            'max_outdoor_temp' => 'required|numeric|between:0,50|gte:min_outdoor_temp',
            'ac_units_needed' => 'required|integer|min:0|max:10',
            'target_temperature' => 'required|integer|between:16,35',
            'description' => 'required|string|max:255',
            'is_active' => 'boolean'
        ]);

        // Check for overlapping rules
        $overlapping = ACAutomationRule::where('is_active', true)
            ->where(function($query) use ($request) {
                $query->whereBetween('min_people', [$request->min_people, $request->max_people])
                      ->orWhereBetween('max_people', [$request->min_people, $request->max_people])
                      ->orWhere(function($q) use ($request) {
                          $q->where('min_people', '<=', $request->min_people)
                            ->where('max_people', '>=', $request->max_people);
                      });
            })
            ->where(function($query) use ($request) {
                $query->whereBetween('min_outdoor_temp', [$request->min_outdoor_temp, $request->max_outdoor_temp])
                      ->orWhereBetween('max_outdoor_temp', [$request->min_outdoor_temp, $request->max_outdoor_temp])
                      ->orWhere(function($q) use ($request) {
                          $q->where('min_outdoor_temp', '<=', $request->min_outdoor_temp)
                            ->where('max_outdoor_temp', '>=', $request->max_outdoor_temp);
                      });
            })
            ->exists();

        if ($overlapping) {
            return redirect()->back()
                ->withInput()
                ->with('error', 'Rule yang overlapping sudah ada. Silakan periksa kondisi yang sudah ada.');
        }

        ACAutomationRule::create([
            'min_people' => $request->min_people,
            'max_people' => $request->max_people,
            'min_outdoor_temp' => $request->min_outdoor_temp,
            'max_outdoor_temp' => $request->max_outdoor_temp,
            'ac_units_needed' => $request->ac_units_needed,
            'target_temperature' => $request->target_temperature,
            'description' => $request->description,
            'is_active' => $request->boolean('is_active', true)
        ]);

        return redirect()->route('admin.ac-rules.index')
            ->with('success', 'AC automation rule berhasil ditambahkan.');
    }

    public function show(ACAutomationRule $acRule)
    {
        // Get usage statistics for this rule
        $stats = $this->getRuleUsageStats($acRule);
        
        return view('admin.ac-rules.show', compact('acRule', 'stats'));
    }

    public function edit(ACAutomationRule $acRule)
    {
        return view('admin.ac-rules.edit', compact('acRule'));
    }

    public function update(Request $request, ACAutomationRule $acRule)
    {
        $request->validate([
            'min_people' => 'required|integer|min:0|max:100',
            'max_people' => 'required|integer|min:0|max:100|gte:min_people',
            'min_outdoor_temp' => 'required|numeric|between:0,50',
            'max_outdoor_temp' => 'required|numeric|between:0,50|gte:min_outdoor_temp',
            'ac_units_needed' => 'required|integer|min:0|max:10',
            'target_temperature' => 'required|integer|between:16,35',
            'description' => 'required|string|max:255',
            'is_active' => 'boolean'
        ]);

        // Check for overlapping rules (excluding current rule)
        $overlapping = ACAutomationRule::where('is_active', true)
            ->where('id', '!=', $acRule->id)
            ->where(function($query) use ($request) {
                $query->whereBetween('min_people', [$request->min_people, $request->max_people])
                      ->orWhereBetween('max_people', [$request->min_people, $request->max_people])
                      ->orWhere(function($q) use ($request) {
                          $q->where('min_people', '<=', $request->min_people)
                            ->where('max_people', '>=', $request->max_people);
                      });
            })
            ->where(function($query) use ($request) {
                $query->whereBetween('min_outdoor_temp', [$request->min_outdoor_temp, $request->max_outdoor_temp])
                      ->orWhereBetween('max_outdoor_temp', [$request->min_outdoor_temp, $request->max_outdoor_temp])
                      ->orWhere(function($q) use ($request) {
                          $q->where('min_outdoor_temp', '<=', $request->min_outdoor_temp)
                            ->where('max_outdoor_temp', '>=', $request->max_outdoor_temp);
                      });
            })
            ->exists();

        if ($overlapping) {
            return redirect()->back()
                ->withInput()
                ->with('error', 'Rule yang overlapping sudah ada. Silakan periksa kondisi yang sudah ada.');
        }

        $acRule->update([
            'min_people' => $request->min_people,
            'max_people' => $request->max_people,
            'min_outdoor_temp' => $request->min_outdoor_temp,
            'max_outdoor_temp' => $request->max_outdoor_temp,
            'ac_units_needed' => $request->ac_units_needed,
            'target_temperature' => $request->target_temperature,
            'description' => $request->description,
            'is_active' => $request->boolean('is_active')
        ]);

        return redirect()->route('admin.ac-rules.index')
            ->with('success', 'AC automation rule berhasil diperbarui.');
    }

    public function destroy(ACAutomationRule $acRule)
    {
        $acRule->delete();

        return redirect()->route('admin.ac-rules.index')
            ->with('success', 'AC automation rule berhasil dihapus.');
    }

    public function toggle(ACAutomationRule $acRule)
    {
        $acRule->update(['is_active' => !$acRule->is_active]);

        return response()->json([
            'status' => 'success',
            'message' => 'Status rule berhasil diubah.',
            'is_active' => $acRule->is_active
        ]);
    }

    public function import(Request $request)
    {
        $request->validate([
            'rules_file' => 'required|file|mimes:json,csv'
        ]);

        try {
            $file = $request->file('rules_file');
            $content = file_get_contents($file->getPathname());
            
            if ($file->getClientOriginalExtension() === 'json') {
                $rules = json_decode($content, true);
            } else {
                // Handle CSV import
                $rules = $this->parseCsvRules($content);
            }

            $imported = 0;
            foreach ($rules as $ruleData) {
                // Validate rule data
                if ($this->validateRuleData($ruleData)) {
                    ACAutomationRule::create($ruleData);
                    $imported++;
                }
            }

            return redirect()->back()
                ->with('success', "Berhasil mengimpor {$imported} rules.");

        } catch (\Exception $e) {
            return redirect()->back()
                ->with('error', 'Gagal mengimpor rules: ' . $e->getMessage());
        }
    }

    public function export()
    {
        $rules = ACAutomationRule::all();
        
        $filename = 'ac_automation_rules_' . now()->format('Y_m_d_H_i_s') . '.json';
        
        return response()->json($rules->toArray())
            ->header('Content-Disposition', 'attachment; filename="' . $filename . '"');
    }

    private function getRuleUsageStats(ACAutomationRule $rule)
    {
        // This would require additional tracking in the energy logs
        // For now, return basic stats
        return [
            'times_activated' => 0, // Would need to track this
            'total_energy_saved' => 0, // Would need to calculate this
            'avg_temperature_achieved' => $rule->target_temperature,
            'efficiency_rating' => 'Good' // Would need to calculate this
        ];
    }

    private function parseCsvRules($content)
    {
        $lines = explode("\n", $content);
        $header = str_getcsv(array_shift($lines));
        $rules = [];

        foreach ($lines as $line) {
            if (trim($line)) {
                $data = str_getcsv($line);
                $rules[] = array_combine($header, $data);
            }
        }

        return $rules;
    }

    private function validateRuleData($data)
    {
        $required = ['min_people', 'max_people', 'min_outdoor_temp', 'max_outdoor_temp', 
                    'ac_units_needed', 'target_temperature', 'description'];
        
        foreach ($required as $field) {
            if (!isset($data[$field])) {
                return false;
            }
        }

        return true;
    }
}