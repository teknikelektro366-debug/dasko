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
        Schema::create('ac_controls', function (Blueprint $table) {
            $table->id();
            $table->string('device_id')->default('ESP32_Smart_Energy');
            $table->string('location')->default('Lab Teknik Tegangan Tinggi');
            $table->boolean('ac1_status')->default(false);
            $table->boolean('ac2_status')->default(false);
            $table->integer('ac1_temperature')->default(25);
            $table->integer('ac2_temperature')->default(25);
            $table->enum('control_mode', ['auto', 'manual', 'schedule'])->default('manual');
            $table->boolean('manual_override')->default(false);
            $table->string('created_by')->default('system');
            $table->timestamp('expires_at')->nullable();
            $table->timestamps();
            
            // Indexes
            $table->index(['device_id', 'location']);
            $table->index('expires_at');
        });
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void
    {
        Schema::dropIfExists('ac_controls');
    }
};
