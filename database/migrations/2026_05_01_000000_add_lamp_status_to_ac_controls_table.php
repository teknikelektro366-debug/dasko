<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    public function up(): void
    {
        Schema::table('ac_controls', function (Blueprint $table) {
            $table->boolean('lamp_status')->default(false)->after('ac2_status');
        });
    }

    public function down(): void
    {
        Schema::table('ac_controls', function (Blueprint $table) {
            $table->dropColumn('lamp_status');
        });
    }
};
