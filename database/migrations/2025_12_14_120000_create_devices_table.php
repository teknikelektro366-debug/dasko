<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    public function up()
    {
        Schema::create('devices', function (Blueprint $table) {
            $table->id();
            $table->string('name');
            $table->string('type');
            $table->decimal('power_consumption', 8, 2); // Watts
            $table->integer('units')->default(1);
            $table->string('location')->default('Lab Teknik Tegangan Tinggi');
            $table->boolean('is_active')->default(false);
            $table->boolean('is_always_on')->default(false);
            $table->integer('gpio_pin')->nullable();
            $table->integer('relay_pin')->nullable();
            $table->text('description')->nullable();
            $table->timestamps();

            $table->index(['type', 'is_active']);
        });
    }

    public function down()
    {
        Schema::dropIfExists('devices');
    }
};