<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    public function up(): void
    {
        Schema::table('ac_controls', function (Blueprint $table) {
            if (!Schema::hasColumn('ac_controls', 'ac_control_mode')) {
                $table->enum('ac_control_mode', ['auto', 'manual'])->default('auto')->after('control_mode');
            }

            if (!Schema::hasColumn('ac_controls', 'lamp_control_mode')) {
                $table->enum('lamp_control_mode', ['auto', 'manual'])->default('auto')->after('ac_control_mode');
            }

            if (!Schema::hasColumn('ac_controls', 'ac_manual_override')) {
                $table->boolean('ac_manual_override')->default(false)->after('manual_override');
            }

            if (!Schema::hasColumn('ac_controls', 'lamp_manual_override')) {
                $table->boolean('lamp_manual_override')->default(false)->after('ac_manual_override');
            }
        });
    }

    public function down(): void
    {
        Schema::table('ac_controls', function (Blueprint $table) {
            if (Schema::hasColumn('ac_controls', 'lamp_manual_override')) {
                $table->dropColumn('lamp_manual_override');
            }

            if (Schema::hasColumn('ac_controls', 'ac_manual_override')) {
                $table->dropColumn('ac_manual_override');
            }

            if (Schema::hasColumn('ac_controls', 'lamp_control_mode')) {
                $table->dropColumn('lamp_control_mode');
            }

            if (Schema::hasColumn('ac_controls', 'ac_control_mode')) {
                $table->dropColumn('ac_control_mode');
            }
        });
    }
};
