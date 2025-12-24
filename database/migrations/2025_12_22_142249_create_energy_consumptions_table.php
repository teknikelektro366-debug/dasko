<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    /**
     * Run the migrations.
     */
    public function up(): void
    {
        Schema::create('energy_consumptions', function (Blueprint $table) {
            $table->id();
            $table->date('date');
            $table->string('location')->default('Ruang Dosen Prodi Teknik Elektro - UNJA');
            $table->string('device_type'); // AC, Lampu, Komputer, dll
            $table->string('device_name');
            $table->integer('power_rating_watts');
            $table->decimal('operating_hours', 5, 2);
            $table->decimal('energy_consumed_kwh', 8, 3);
            $table->decimal('cost_per_kwh', 6, 2)->default(1500.00); // Rp 1500 per kWh
            $table->decimal('total_cost', 10, 2);
            $table->decimal('people_count_avg', 4, 1)->nullable();
            $table->decimal('temperature_avg', 4, 1)->nullable();
            $table->decimal('humidity_avg', 4, 1)->nullable();
            $table->decimal('ac_usage_hours', 5, 2)->default(0);
            $table->decimal('lamp_usage_hours', 5, 2)->default(0);
            $table->decimal('other_devices_hours', 5, 2)->default(0);
            $table->decimal('peak_hours_usage', 5, 2)->default(0); // 08:00-17:00
            $table->decimal('off_peak_hours_usage', 5, 2)->default(0); // 17:00-08:00
            $table->decimal('energy_saved_kwh', 8, 3)->default(0);
            $table->decimal('efficiency_percentage', 5, 1)->default(0);
            $table->text('notes')->nullable();
            $table->timestamps();
            
            // Indexes
            $table->index(['date', 'location']);
            $table->index('device_type');
            $table->index('date');
        });
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void
    {
        Schema::dropIfExists('energy_consumptions');
    }
};
