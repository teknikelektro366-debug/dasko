<?php

namespace Database\Seeders;

use App\Models\User;
use Illuminate\Database\Console\Seeds\WithoutModelEvents;
use Illuminate\Database\Seeder;

class DatabaseSeeder extends Seeder
{
    use WithoutModelEvents;

    /**
     * Seed the application's database.
     */
    public function run(): void
    {
        // Create admin user
        User::factory()->create([
            'name' => 'Admin User',
            'email' => 'admin@smartenergy.lab',
            'role' => 'admin',
            'is_active' => true,
        ]);

        // Create operator user
        User::factory()->create([
            'name' => 'Operator User',
            'email' => 'operator@smartenergy.lab',
            'role' => 'operator',
            'is_active' => true,
        ]);

        // Seed dummy data for devices, sensors, etc.
        $this->call([
            DummyDataSeeder::class,
        ]);
    }
}
