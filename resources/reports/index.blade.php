@extends('layouts.app')

@section('title', 'Energy Reports')
@section('page-title', 'Energy Reports')

@section('content')
<div class="p-6 space-y-6">
    <!-- Header -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h2 class="text-2xl font-bold text-gray-900 dark:text-white mb-2">
            <i class="fas fa-chart-line mr-3 text-blue-500"></i>Energy Reports
        </h2>
        <p class="text-gray-600 dark:text-gray-400">Generate and download comprehensive energy consumption reports</p>
    </div>

    <!-- Quick Reports -->
    <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <!-- Daily Report -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="text-center">
                <div class="w-16 h-16 bg-blue-100 dark:bg-blue-900 rounded-full flex items-center justify-center mx-auto mb-4">
                    <i class="fas fa-calendar-day text-blue-600 dark:text-blue-400 text-2xl"></i>
                </div>
                <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-2">Daily Report</h3>
                <p class="text-sm text-gray-600 dark:text-gray-400 mb-4">Today's energy consumption</p>
                <a href="{{ route('reports.daily') }}" class="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-lg text-sm">
                    View Report
                </a>
            </div>
        </div>

        <!-- Weekly Report -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="text-center">
                <div class="w-16 h-16 bg-green-100 dark:bg-green-900 rounded-full flex items-center justify-center mx-auto mb-4">
                    <i class="fas fa-calendar-week text-green-600 dark:text-green-400 text-2xl"></i>
                </div>
                <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-2">Weekly Report</h3>
                <p class="text-sm text-gray-600 dark:text-gray-400 mb-4">This week's summary</p>
                <a href="{{ route('reports.weekly') }}" class="bg-green-600 hover:bg-green-700 text-white px-4 py-2 rounded-lg text-sm">
                    View Report
                </a>
            </div>
        </div>

        <!-- Monthly Report -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="text-center">
                <div class="w-16 h-16 bg-purple-100 dark:bg-purple-900 rounded-full flex items-center justify-center mx-auto mb-4">
                    <i class="fas fa-calendar-alt text-purple-600 dark:text-purple-400 text-2xl"></i>
                </div>
                <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-2">Monthly Report</h3>
                <p class="text-sm text-gray-600 dark:text-gray-400 mb-4">This month's analysis</p>
                <a href="{{ route('reports.monthly') }}" class="bg-purple-600 hover:bg-purple-700 text-white px-4 py-2 rounded-lg text-sm">
                    View Report
                </a>
            </div>
        </div>

        <!-- Efficiency Report -->
        <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6 card-hover">
            <div class="text-center">
                <div class="w-16 h-16 bg-yellow-100 dark:bg-yellow-900 rounded-full flex items-center justify-center mx-auto mb-4">
                    <i class="fas fa-leaf text-yellow-600 dark:text-yellow-400 text-2xl"></i>
                </div>
                <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-2">Efficiency Report</h3>
                <p class="text-sm text-gray-600 dark:text-gray-400 mb-4">Energy efficiency analysis</p>
                <button onclick="showEfficiencyModal()" class="bg-yellow-600 hover:bg-yellow-700 text-white px-4 py-2 rounded-lg text-sm">
                    Generate Report
                </button>
            </div>
        </div>
    </div>

    <!-- Custom Report Generator -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-xl font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-cogs mr-2 text-indigo-500"></i>Custom Report Generator
        </h3>
        
        <form id="customReportForm" action="{{ route('reports.energy') }}" method="POST" class="space-y-6">
            @csrf
            <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
                <!-- Date From -->
                <div>
                    <label class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">From Date</label>
                    <input type="date" name="date_from" value="{{ date('Y-m-d', strtotime('-30 days')) }}" 
                           class="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 dark:bg-gray-700 dark:text-white">
                </div>

                <!-- Date To -->
                <div>
                    <label class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">To Date</label>
                    <input type="date" name="date_to" value="{{ date('Y-m-d') }}" 
                           class="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 dark:bg-gray-700 dark:text-white">
                </div>

                <!-- Device Type -->
                <div>
                    <label class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">Device Type</label>
                    <select name="device_type" class="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 dark:bg-gray-700 dark:text-white">
                        <option value="">All Devices</option>
                        <option value="ac1">AC Unit 1</option>
                        <option value="ac2">AC Unit 2</option>
                        <option value="lamp1">Lamp Circuit 1</option>
                        <option value="lamp2">Lamp Circuit 2</option>
                        <option value="computer">Computer</option>
                        <option value="tv">Smart TV</option>
                        <option value="dispenser">Dispenser</option>
                        <option value="kettle">Electric Kettle</option>
                        <option value="coffee">Coffee Machine</option>
                        <option value="fridge">Refrigerator</option>
                        <option value="router">Wi-Fi Router</option>
                    </select>
                </div>

                <!-- Format -->
                <div>
                    <label class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">Format</label>
                    <select name="format" class="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 dark:bg-gray-700 dark:text-white">
                        <option value="pdf">PDF Report</option>
                        <option value="excel">Excel Spreadsheet</option>
                        <option value="csv">CSV Data</option>
                    </select>
                </div>
            </div>

            <div class="flex justify-end">
                <button type="submit" class="bg-indigo-600 hover:bg-indigo-700 text-white px-6 py-2 rounded-lg flex items-center space-x-2">
                    <i class="fas fa-download"></i>
                    <span>Generate & Download Report</span>
                </button>
            </div>
        </form>
    </div>

    <!-- Device-Specific Reports -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-xl font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-microchip mr-2 text-green-500"></i>Device-Specific Reports
        </h3>
        
        <form id="deviceReportForm" action="{{ route('reports.device') }}" method="POST" class="space-y-6">
            @csrf
            <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
                <!-- Device Selection -->
                <div>
                    <label class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">Select Device</label>
                    <select name="device_id" required class="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 dark:bg-gray-700 dark:text-white">
                        <option value="">Choose Device</option>
                        @foreach(\App\Models\Device::all() as $device)
                            <option value="{{ $device->id }}">{{ $device->name }}</option>
                        @endforeach
                    </select>
                </div>

                <!-- Date From -->
                <div>
                    <label class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">From Date</label>
                    <input type="date" name="date_from" value="{{ date('Y-m-d', strtotime('-7 days')) }}" required
                           class="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 dark:bg-gray-700 dark:text-white">
                </div>

                <!-- Date To -->
                <div>
                    <label class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">To Date</label>
                    <input type="date" name="date_to" value="{{ date('Y-m-d') }}" required
                           class="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 dark:bg-gray-700 dark:text-white">
                </div>

                <!-- Action Buttons -->
                <div class="flex items-end space-x-2">
                    <button type="submit" name="format" value="view" class="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-lg flex items-center space-x-2">
                        <i class="fas fa-eye"></i>
                        <span>View</span>
                    </button>
                    <button type="submit" name="format" value="download" class="bg-green-600 hover:bg-green-700 text-white px-4 py-2 rounded-lg flex items-center space-x-2">
                        <i class="fas fa-download"></i>
                        <span>Download</span>
                    </button>
                </div>
            </div>
        </form>
    </div>

    <!-- Quick Download Buttons -->
    <div class="bg-white dark:bg-gray-800 rounded-lg shadow-sm p-6">
        <h3 class="text-xl font-semibold text-gray-900 dark:text-white mb-6">
            <i class="fas fa-bolt mr-2 text-yellow-500"></i>Quick Downloads
        </h3>
        
        <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
            <a href="{{ route('reports.daily', ['format' => 'download']) }}" 
               class="bg-blue-50 dark:bg-blue-900/20 border border-blue-200 dark:border-blue-800 rounded-lg p-4 hover:bg-blue-100 dark:hover:bg-blue-900/30 transition-colors">
                <div class="flex items-center space-x-3">
                    <i class="fas fa-file-pdf text-blue-600 dark:text-blue-400 text-xl"></i>
                    <div>
                        <h4 class="font-medium text-gray-900 dark:text-white">Today's Report</h4>
                        <p class="text-sm text-gray-600 dark:text-gray-400">Download today's energy report</p>
                    </div>
                </div>
            </a>

            <a href="{{ route('reports.weekly', ['format' => 'download']) }}" 
               class="bg-green-50 dark:bg-green-900/20 border border-green-200 dark:border-green-800 rounded-lg p-4 hover:bg-green-100 dark:hover:bg-green-900/30 transition-colors">
                <div class="flex items-center space-x-3">
                    <i class="fas fa-file-excel text-green-600 dark:text-green-400 text-xl"></i>
                    <div>
                        <h4 class="font-medium text-gray-900 dark:text-white">Weekly Report</h4>
                        <p class="text-sm text-gray-600 dark:text-gray-400">Download this week's summary</p>
                    </div>
                </div>
            </a>

            <a href="{{ route('reports.monthly', ['format' => 'download']) }}" 
               class="bg-purple-50 dark:bg-purple-900/20 border border-purple-200 dark:border-purple-800 rounded-lg p-4 hover:bg-purple-100 dark:hover:bg-purple-900/30 transition-colors">
                <div class="flex items-center space-x-3">
                    <i class="fas fa-file-csv text-purple-600 dark:text-purple-400 text-xl"></i>
                    <div>
                        <h4 class="font-medium text-gray-900 dark:text-white">Monthly Report</h4>
                        <p class="text-sm text-gray-600 dark:text-gray-400">Download monthly analysis</p>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Efficiency Report Modal -->
<div id="efficiencyModal" class="fixed inset-0 bg-black bg-opacity-50 hidden items-center justify-center z-50">
    <div class="bg-white dark:bg-gray-800 rounded-lg p-6 max-w-md w-full mx-4">
        <h3 class="text-lg font-semibold text-gray-900 dark:text-white mb-4">Generate Efficiency Report</h3>
        
        <form action="{{ route('reports.efficiency') }}" method="POST" class="space-y-4">
            @csrf
            <div>
                <label class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">From Date</label>
                <input type="date" name="date_from" value="{{ date('Y-m-d', strtotime('-30 days')) }}" required
                       class="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 dark:bg-gray-700 dark:text-white">
            </div>
            
            <div>
                <label class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2">To Date</label>
                <input type="date" name="date_to" value="{{ date('Y-m-d') }}" required
                       class="w-full px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 dark:bg-gray-700 dark:text-white">
            </div>
            
            <div class="flex justify-end space-x-3 pt-4">
                <button type="button" onclick="hideEfficiencyModal()" class="px-4 py-2 text-gray-600 dark:text-gray-400 hover:text-gray-800 dark:hover:text-gray-200">
                    Cancel
                </button>
                <button type="submit" name="format" value="view" class="bg-blue-600 hover:bg-blue-700 text-white px-4 py-2 rounded-lg">
                    View Report
                </button>
                <button type="submit" name="format" value="download" class="bg-green-600 hover:bg-green-700 text-white px-4 py-2 rounded-lg">
                    Download PDF
                </button>
            </div>
        </form>
    </div>
</div>

@push('scripts')
<script>
function showEfficiencyModal() {
    document.getElementById('efficiencyModal').classList.remove('hidden');
    document.getElementById('efficiencyModal').classList.add('flex');
}

function hideEfficiencyModal() {
    document.getElementById('efficiencyModal').classList.add('hidden');
    document.getElementById('efficiencyModal').classList.remove('flex');
}

// Form validation
document.getElementById('customReportForm').addEventListener('submit', function(e) {
    const dateFrom = new Date(this.date_from.value);
    const dateTo = new Date(this.date_to.value);
    
    if (dateFrom > dateTo) {
        e.preventDefault();
        alert('From date cannot be later than To date');
        return false;
    }
    
    // Show loading indicator
    const submitBtn = this.querySelector('button[type="submit"]');
    const originalText = submitBtn.innerHTML;
    submitBtn.innerHTML = '<i class="fas fa-spinner fa-spin mr-2"></i>Generating Report...';
    submitBtn.disabled = true;
    
    // Re-enable button after 5 seconds (in case of error)
    setTimeout(() => {
        submitBtn.innerHTML = originalText;
        submitBtn.disabled = false;
    }, 5000);
});

document.getElementById('deviceReportForm').addEventListener('submit', function(e) {
    const dateFrom = new Date(this.date_from.value);
    const dateTo = new Date(this.date_to.value);
    
    if (dateFrom > dateTo) {
        e.preventDefault();
        alert('From date cannot be later than To date');
        return false;
    }
});
</script>
@endpush
@endsection