<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    public function up()
    {
        Schema::create('ac_automation_rules', function (Blueprint $table) {
            $table->id();
            $table->integer('min_people');
            $table->integer('max_people');
            $table->decimal('min_outdoor_temp', 4, 1);
            $table->decimal('max_outdoor_temp', 4, 1);
            $table->integer('ac_units_needed');
            $table->integer('target_temperature');
            $table->text('description');
            $table->boolean('is_active')->default(true);
            $table->timestamps();

            $table->index(['min_people', 'max_people']);
            $table->index(['min_outdoor_temp', 'max_outdoor_temp']);
            $table->index(['is_active']);
        });
    }

    public function down()
    {
        Schema::dropIfExists('ac_automation_rules');
    }
};