@extends('layouts.app')

@section('title', 'Dashboard')
@section('page-title', 'Smart Energy Dashboard')

@section('content')
<div class="p-6 space-y-6">
    <!-- Real-time Sensor Data -->
    <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <!-- Temperature -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-red-100 dark:bg-red-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-thermometer-half text-red-600 dark:text-red-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Temperature</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white" data-sensor="temperature">27.0°C</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Outdoor</p>
                </div>
            </div>
        </div>

        <!-- Humidity -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-blue-100 dark:bg-blue-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-tint text-blue-600 dark:text-blue-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Humidity</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white" data-sensor="humidity">65.0%</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Relative</p>
                </div>
            </div>
        </div>

        <!-- People Count -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-green-100 dark:bg-green-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-users text-green-600 dark:text-green-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">People Count</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white" data-sensor="people_count">0</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">In Lab</p>
                </div>
            </div>
        </div>

        <!-- Energy Today -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="flex items-center">
                <div class="flex-shrink-0">
                    <div class="w-12 h-12 bg-yellow-100 dark:bg-yellow-900 rounded-lg flex items-center justify-center">
                        <i class="fas fa-bolt text-yellow-600 dark:text-yellow-400 text-xl"></i>
                    </div>
                </div>
                <div class="ml-4">
                    <p class="text-sm font-medium text-gray-500 dark:text-gray-400">Energy Today</p>
                    <p class="text-2xl font-semibold text-gray-900 dark:text-white">0.00</p>
                    <p class="text-sm text-gray-600 dark:text-gray-400">kWh</p>
                </div>
            </div>
        </div>
    </div>

    <!-- Welcome Message -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <div class="text-center">
            <h2 class="text-2xl font-bold text-gray-900 dark:text-white mb-4">
                Welcome to Smart Energy Management System
            </h2>
            <p class="text-gray-600 dark:text-gray-400 mb-6">
                Lab Teknik Tegangan Tinggi - Real-time Energy Monitoring & Control
            </p>
            <div class="grid grid-cols-1 md:grid-cols-3 gap-4 max-w-2xl mx-auto">
                <div class="text-center">
                    <div class="w-16 h-16 bg-blue-100 dark:bg-blue-900 rounded-full flex items-center justify-center mx-auto mb-3">
                        <i class="fas fa-chart-line text-blue-600 dark:text-blue-400 text-2xl"></i>
                    </div>
                    <h3 class="font-semibold text-gray-900 dark:text-white">Real-time Monitoring</h3>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Track energy consumption in real-time</p>
                </div>
                <div class="text-center">
                    <div class="w-16 h-16 bg-green-100 dark:bg-green-900 rounded-full flex items-center justify-center mx-auto mb-3">
                        <i class="fas fa-cogs text-green-600 dark:text-green-400 text-2xl"></i>
                    </div>
                    <h3 class="font-semibold text-gray-900 dark:text-white">Smart Automation</h3>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Automated device control based on conditions</p>
                </div>
                <div class="text-center">
                    <div class="w-16 h-16 bg-purple-100 dark:bg-purple-900 rounded-full flex items-center justify-center mx-auto mb-3">
                        <i class="fas fa-leaf text-purple-600 dark:text-purple-400 text-2xl"></i>
                    </div>
                    <h3 class="font-semibold text-gray-900 dark:text-white">Energy Saving</h3>
                    <p class="text-sm text-gray-600 dark:text-gray-400">Optimize energy usage automatically</p>
                </div>
            </div>
        </div>
    </div>
</div>
@endsection