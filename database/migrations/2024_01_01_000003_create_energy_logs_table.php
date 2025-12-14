<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    public function up()
    {
        Schema::create('energy_logs', function (Blueprint $table) {
            $table->id();
            $table->string('device_id');
            $table->decimal('energy_consumed', 10, 3); // kWh
            $table->integer('duration_minutes');
            $table->timestamp('start_time')->nullable();
            $table->timestamp('end_time')->nullable();
            $table->integer('people_count')->nullable();
            $table->decimal('outdoor_temperature', 4, 1)->nullable();
            $table->integer('ac_target_temperature')->nullable();
            $table->boolean('energy_saving_mode')->default(false);
            $table->boolean('working_hours')->default(true);
            $table->decimal('cost_estimate', 10, 2)->nullable(); // IDR
            $table->timestamps();

            $table->index(['device_id', 'start_time']);
            $table->index(['start_time']);
            $table->index(['working_hours']);
        });
    }

    public function down()
    {
        Schema::dropIfExists('energy_logs');
    }
};