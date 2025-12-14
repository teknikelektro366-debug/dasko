<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    public function up()
    {
        Schema::create('sensor_readings', function (Blueprint $table) {
            $table->id();
            $table->string('device_id')->nullable();
            $table->string('sensor_type'); // temperature, humidity, light, proximity, people_count
            $table->string('sensor_name');
            $table->decimal('value', 10, 2);
            $table->string('unit', 10);
            $table->string('location')->default('Lab Teknik Tegangan Tinggi');
            $table->integer('gpio_pin')->nullable();
            $table->timestamp('reading_time')->nullable();
            $table->timestamps();

            $table->index(['sensor_type', 'reading_time']);
            $table->index(['reading_time']);
        });
    }

    public function down()
    {
        Schema::dropIfExists('sensor_readings');
    }
};