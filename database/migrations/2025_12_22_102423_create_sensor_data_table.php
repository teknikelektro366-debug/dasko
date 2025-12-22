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
        Schema::create('sensor_data', function (Blueprint $table) {
            $table->id();
            $table->string('device_id')->default('ESP32_Smart_Energy');
            $table->string('location')->default('Lab Teknik Tegangan Tinggi');
            
            // Data sensor utama
            $table->integer('people_count')->default(0);
            $table->string('ac_status')->default('OFF');
            $table->integer('set_temperature')->nullable();
            $table->decimal('room_temperature', 5, 2)->nullable();
            $table->decimal('humidity', 5, 2)->nullable();
            $table->integer('light_level')->default(0);
            
            // Data sensor detail
            $table->boolean('proximity_in')->default(false);
            $table->boolean('proximity_out')->default(false);
            $table->integer('wifi_rssi')->nullable();
            
            // Status sistem
            $table->string('status')->default('active');
            $table->bigInteger('device_timestamp')->nullable(); // millis() dari Arduino
            
            $table->timestamps();
            
            // Index untuk query cepat
            $table->index(['device_id', 'created_at']);
            $table->index('created_at');
        });
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void
    {
        Schema::dropIfExists('sensor_data');
    }
};