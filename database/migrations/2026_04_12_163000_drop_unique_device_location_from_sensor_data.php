<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;
use Illuminate\Support\Facades\DB;

return new class extends Migration
{
    /**
     * Run the migrations.
     * 
     * Menghapus UNIQUE constraint pada (device_id, location) yang menyebabkan error:
     * "Duplicate entry 'UNJA_Prodi_Elektro-Ruang Dosen Prodi Teknik Elektro' 
     *  for key 'sensor_data_device_location_unique'"
     * 
     * Constraint ini salah karena tabel sensor_data menyimpan data time-series
     * (banyak record per device per lokasi). Constraint yang benar hanya INDEX 
     * biasa (non-unique) untuk mempercepat query.
     */
    public function up(): void
    {
        Schema::table('sensor_data', function (Blueprint $table) {
            // Coba drop unique index — nama bisa bervariasi tergantung bagaimana dibuat
            // Gunakan raw SQL agar lebih aman
        });

        // Drop unique constraint menggunakan raw SQL (lebih reliable)
        // Cek dan drop semua kemungkinan nama unique index
        $possibleIndexNames = [
            'sensor_data_device_location_unique',
            'sensor_data_device_id_location_unique',
        ];

        foreach ($possibleIndexNames as $indexName) {
            try {
                DB::statement("ALTER TABLE `sensor_data` DROP INDEX `{$indexName}`");
                \Log::info("Dropped unique index: {$indexName}");
            } catch (\Exception $e) {
                // Index mungkin tidak ada, lanjutkan
                \Log::info("Index {$indexName} not found, skipping: " . $e->getMessage());
            }
        }
    }

    /**
     * Reverse the migrations.
     * Tidak perlu restore — unique constraint ini memang seharusnya tidak ada
     */
    public function down(): void
    {
        // Intentionally left empty — unique constraint tidak boleh ada
        // karena sensor_data adalah tabel time-series
    }
};
