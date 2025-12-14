@extends('layouts.app')

@section('title', 'Dashboard')

@section('content')
<!-- Navbar -->
<div class="navbar">
    <i class="fas fa-building"></i> Ruang Dosen Prodi Teknik Elektro
</div>
<div class="subnav" id="waktuRealtime">
    <i class="fas fa-clock"></i> Memuat waktu...
</div>

<!-- ===== Monitoring Section ===== -->
<section id="monitoring" class="content-section active" style="padding: 30px;">
    <!-- Sensor Cards -->
    <div class="sensor-section">
        <h3><i class="fas fa-chart-line"></i> Data Sensor Real-time</h3>
        <div class="sensor-grid">
            <div class="card sensor-card">
                <div class="card-icon" style="background: #fee2e2; color: #dc2626;">
                    <i class="fas fa-snowflake"></i>
                </div>
                <div class="title">Suhu Ruangan</div>
                <div class="value" id="suhuValue">{{ $sensorData['temperature'] ?? 27 }} °C</div>
            </div>

            <div class="card sensor-card">
                <div class="card-icon" style="background: #dbeafe; color: #2563eb;">
                    <i class="fas fa-tint"></i>
                </div>
                <div class="title">Kelembaban Ruangan</div>
                <div class="value" id="humidityValue">{{ $sensorData['humidity'] ?? 65 }} %</div>
            </div>

            <div class="card sensor-card">
                <div class="card-icon" style="background: #dcfce7; color: #16a34a;">
                    <i class="fas fa-users"></i>
                </div>
                <div class="title">Jumlah Orang di Ruangan</div>
                <div class="value" id="jumlahOrang">{{ $sensorData['people_count'] ?? 0 }}</div>
            </div>

            <div class="card sensor-card">
                <div class="card-icon" style="background: #fef3c7; color: #d97706;">
                    <i class="fas fa-sun"></i>
                </div>
                <div class="title">Intensitas Cahaya (LDR)</div>
                <div class="value" id="ldrValue">{{ $sensorData['light_intensity'] ?? '-' }}</div>
            </div>
        </div>
    </div>

    <!-- Status Cards -->
    <div class="status-section" style="margin-top: 40px;">
        <h3><i class="fas fa-info-circle"></i> Status Sistem</h3>
        <div class="status-grid">
            <div class="card status-card">
                <div class="card-icon" style="background: #dbeafe; color: #2563eb;">
                    <i class="fas fa-clock"></i>
                </div>
                <div class="title">Status Jam Kerja</div>
                <div id="workingHoursStatus" class="status-{{ ($workingHoursStatus['is_working_hours'] ?? false) ? 'on' : 'off' }}" style="font-weight: bold; color: {{ ($workingHoursStatus['is_working_hours'] ?? false) ? '#16a34a' : '#dc2626' }};">
                    {{ $workingHoursStatus['status'] ?? 'Di Luar Jam Kerja' }}
                </div>
                <div class="work-hours-info" style="margin-top: 10px; font-size: 0.9rem; color: #6b7280;">
                    <div id="workingHoursRange">{{ $workingHoursStatus['working_range'] ?? '07:00 - 17:00 WIB' }}</div>
                    <div id="workingDayInfo">Senin - Jumat</div>
                    <div id="currentTimeDisplay">{{ $workingHoursStatus['current_time'] ?? now()->format('H:i') }} ({{ $workingHoursStatus['current_day'] ?? now()->format('l') }})</div>
                </div>
            </div>

            <div class="card status-card">
                <div class="card-icon" style="background: #dcfce7; color: #16a34a;">
                    <i class="fas fa-leaf"></i>
                </div>
                <div class="title">Energi yang Di Hemat</div>
                <div class="value" id="energySave">0%</div>
            </div>

            <div class="card status-card">
                <div class="card-icon" style="background: #f3e8ff; color: #9333ea;">
                    <i class="fas fa-chart-pie"></i>
                </div>
                <div class="title">Pemakaian Hari Ini</div>
                <div class="daily-usage" style="margin-top: 10px;">
                    <div class="usage-item" style="display: flex; justify-content: space-between; margin-bottom: 5px;">
                        <span>Total Perangkat:</span>
                        <span id="totalDevicesOn">{{ $devices->where('is_active', true)->count() ?? 0 }}</span>
                    </div>
                    <div class="usage-item" style="display: flex; justify-content: space-between;">
                        <span>Total Energi:</span>
                        <span id="dailyTotalEnergy">{{ number_format(($todayEnergy ?? 0), 0) }} Wh</span>
                    </div>
                </div>
            </div>

            <div class="card status-card">
                <div class="card-icon" style="background: #fef3c7; color: #d97706;">
                    <i class="fas fa-shield-alt"></i>
                </div>
                <div class="title">Auto Shutdown</div>
                <div id="autoShutdownStatus" class="status-on" style="font-weight: bold; color: #16a34a;">AKTIF</div>
                <div class="shutdown-info" style="margin-top: 10px; font-size: 0.9rem;">
                    <div class="usage-item" style="display: flex; justify-content: space-between; margin-bottom: 5px;">
                        <span>Waktu Tanpa Orang:</span>
                        <span id="noPersonTime">{{ $energySavingStatus['minutes_without_people'] ?? 0 }} menit</span>
                    </div>
                    <div class="usage-item" style="display: flex; justify-content: space-between;">
                        <span>Status:</span>
                        <span id="shutdownCountdown">{{ $energySavingStatus['current_state'] ?? 'Normal' }}</span>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <!-- Device Control Section -->
    <div class="device-section" style="margin-top: 40px;">
        <h3><i class="fas fa-microchip"></i> Kontrol Perangkat Listrik</h3>
        <div class="device-grid">
            @php
                $acDevices = $devices->where('type', 'ac');
                $lampDevices = $devices->where('type', 'lamp');
                $panelDevice = $devices->where('type', 'panel')->first();
            @endphp

            <!-- AC Panasonic -->
            <div class="card device-card ac-card">
                <div class="card-icon" style="background: #dbeafe; color: #2563eb;">
                    <i class="fas fa-snowflake"></i>
                </div>
                <div class="title">AC Panasonic CS-PN12WKJ ({{ $acDevices->count() }} Unit)</div>
                <div class="device-info" style="margin: 10px 0; color: #6b7280; font-size: 0.9rem;">
                    <span>Daya: 1050W per unit | Total Konsumsi: {{ number_format($acDevices->sum('power_consumption') / 1000, 1) }} kWh</span>
                </div>
                
                <!-- Status kedua AC -->
                <div class="ac-units-status" style="margin: 15px 0;">
                    @foreach($acDevices as $index => $ac)
                    <div class="ac-unit" style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; padding: 8px; background: #f9fafb; border-radius: 6px;">
                        <span>AC Unit {{ $index + 1 }}:</span>
                        <div style="display: flex; align-items: center; gap: 10px;">
                            <div id="ac{{ $index + 1 }}Status" class="mini-status status-{{ $ac->is_active ? 'on' : 'off' }}" style="padding: 4px 8px; border-radius: 4px; font-size: 0.8rem; font-weight: bold; color: white; background: {{ $ac->is_active ? '#16a34a' : '#dc2626' }};">
                                {{ $ac->is_active ? 'HIDUP' : 'MATI' }}
                            </div>
                            <button onclick="toggleACUnit({{ $index + 1 }})" class="mini-btn" style="padding: 6px 10px; background: #6366f1; color: white; border: none; border-radius: 4px; cursor: pointer;">
                                <i class="fas fa-power-off"></i>
                            </button>
                        </div>
                    </div>
                    @endforeach
                </div>
                
                <!-- Status keseluruhan -->
                <div id="acStatus" class="status-{{ $acDevices->where('is_active', true)->count() > 0 ? 'on' : 'off' }}" style="text-align: center; padding: 10px; font-weight: bold; color: {{ $acDevices->where('is_active', true)->count() > 0 ? '#16a34a' : '#dc2626' }};">
                    @if($acDevices->where('is_active', true)->count() == $acDevices->count())
                        SEMUA HIDUP
                    @elseif($acDevices->where('is_active', true)->count() > 0)
                        SEBAGIAN HIDUP
                    @else
                        SEMUA MATI
                    @endif
                </div>
                
                <!-- Kontrol keseluruhan -->
                <button onclick="toggleAllAC()" style="width: 100%; padding: 12px; background: #6366f1; color: white; border: none; border-radius: 6px; font-weight: bold; cursor: pointer; margin: 10px 0;">
                    <i class="fas fa-power-off"></i> KONTROL SEMUA AC
                </button>
                
                <div class="device-controls">
                    <div class="title" style="margin-top:10px; font-weight: 600;">Suhu Setting</div>
                    <div class="value" id="acTempValue" style="font-size: 1.5rem; font-weight: bold; margin: 10px 0;">24 °C</div>
                    <div class="temp-controls" style="display: flex; gap: 10px; justify-content: center; margin: 10px 0;">
                        <button onclick="tempUp()" class="temp-btn" style="padding: 8px 12px; background: #dc2626; color: white; border: none; border-radius: 4px; cursor: pointer;">
                            <i class="fas fa-plus"></i>
                        </button>
                        <button onclick="tempDown()" class="temp-btn" style="padding: 8px 12px; background: #2563eb; color: white; border: none; border-radius: 4px; cursor: pointer;">
                            <i class="fas fa-minus"></i>
                        </button>
                    </div>
                    <div class="gradual-controls" style="margin-top: 10px; text-align: center;">
                        <button onclick="startACGradualMode()" class="btn-small" style="font-size: 11px; padding: 6px 10px; background: #16a34a; color: white; border: none; border-radius: 4px; cursor: pointer;">
                            <i class="fas fa-thermometer-half"></i> Mode Gradual
                        </button>
                    </div>
                </div>
            </div>

            <!-- Lampu TL -->
            <div class="card device-card lamp-card">
                <div class="card-icon" style="background: #fef3c7; color: #d97706;">
                    <i class="fas fa-lightbulb"></i>
                </div>
                <div class="title">Lampu TL ({{ $lampDevices->sum('units') }} Unit - {{ $lampDevices->count() }} Jalur)</div>
                <div class="device-info" style="margin: 10px 0; color: #6b7280; font-size: 0.9rem;">
                    <span>Daya: 22W per unit | Total Konsumsi: {{ number_format($lampDevices->sum('power_consumption') / 1000, 3) }} kWh</span>
                </div>
                
                <!-- Status kedua jalur lampu -->
                <div class="lamp-circuits-status" style="margin: 15px 0;">
                    @foreach($lampDevices as $index => $lamp)
                    <div class="lamp-circuit" style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; padding: 8px; background: #f9fafb; border-radius: 6px;">
                        <span>Jalur {{ $index + 1 }} ({{ $lamp->units }} Unit):</span>
                        <div style="display: flex; align-items: center; gap: 10px;">
                            <div id="lamp{{ $index + 1 }}Status" class="mini-status status-{{ $lamp->is_active ? 'on' : 'off' }}" style="padding: 4px 8px; border-radius: 4px; font-size: 0.8rem; font-weight: bold; color: white; background: {{ $lamp->is_active ? '#16a34a' : '#dc2626' }};">
                                {{ $lamp->is_active ? 'HIDUP' : 'MATI' }}
                            </div>
                            <button onclick="toggleLampCircuit({{ $index + 1 }})" class="mini-btn" style="padding: 6px 10px; background: #d97706; color: white; border: none; border-radius: 4px; cursor: pointer;">
                                <i class="fas fa-power-off"></i>
                            </button>
                        </div>
                    </div>
                    @endforeach
                </div>
                
                <!-- Status keseluruhan -->
                <div id="lampStatus" class="status-{{ $lampDevices->where('is_active', true)->count() > 0 ? 'on' : 'off' }}" style="text-align: center; padding: 10px; font-weight: bold; color: {{ $lampDevices->where('is_active', true)->count() > 0 ? '#16a34a' : '#dc2626' }};">
                    @if($lampDevices->where('is_active', true)->count() == $lampDevices->count())
                        SEMUA HIDUP
                    @elseif($lampDevices->where('is_active', true)->count() > 0)
                        SEBAGIAN HIDUP
                    @else
                        SEMUA MATI
                    @endif
                </div>
                
                <!-- Kontrol keseluruhan -->
                <button onclick="toggleAllLamps()" style="width: 100%; padding: 12px; background: #d97706; color: white; border: none; border-radius: 6px; font-weight: bold; cursor: pointer; margin: 10px 0;">
                    <i class="fas fa-power-off"></i> KONTROL SEMUA LAMPU
                </button>
            </div>

            <!-- Panel Listrik -->
            @if($panelDevice)
            <div class="card device-card">
                <div class="card-icon" style="background: #fee2e2; color: #dc2626;">
                    <i class="fas fa-bolt"></i>
                </div>
                <div class="title">Panel Listrik (1 Unit)</div>
                <div class="device-info" style="margin: 10px 0; color: #6b7280; font-size: 0.9rem;">
                    <span>Daya: {{ $panelDevice->power_consumption }}W | Konsumsi: {{ number_format($panelDevice->power_consumption / 1000, 1) }} kWh</span>
                </div>
                <div id="panelStatus" class="status-{{ $panelDevice->is_active ? 'on' : 'off' }}" style="text-align: center; padding: 10px; font-weight: bold; color: {{ $panelDevice->is_active ? '#16a34a' : '#dc2626' }};">
                    {{ $panelDevice->is_active ? 'MENYALA' : 'MATI' }}
                </div>
                <button onclick="toggleDevice('panel')" style="width: 100%; padding: 12px; background: #dc2626; color: white; border: none; border-radius: 6px; font-weight: bold; cursor: pointer; margin: 10px 0;">
                    <i class="fas fa-power-off"></i> ON / OFF
                </button>
            </div>
            @endif
        </div>
    </div>
</section>

<!-- ===== Analytics Section ===== -->
<section id="analytics" class="content-section" style="padding: 30px;">
    <header>
        <h1><i class="fas fa-chart-bar"></i> Analisa</h1>
    </header>
    <div class="chart-container" style="background: white; padding: 20px; border-radius: 8px; margin-top: 20px;">
        <canvas id="myChart"></canvas>
    </div>
</section>

<!-- ===== History Section ===== -->
<section id="history" class="content-section" style="padding: 30px;">
    <header>
        <h1><i class="fas fa-table"></i> Riwayat Pemakaian Harian</h1>
    </header>

    <!-- Tab Navigation -->
    <div class="history-tabs" style="display: flex; gap: 10px; margin: 20px 0; border-bottom: 2px solid #e5e7eb;">
        <button class="tab-button active" onclick="showHistoryTab('summary')" style="padding: 10px 20px; border: none; background: #6366f1; color: white; border-radius: 6px 6px 0 0; cursor: pointer;">
            <i class="fas fa-chart-bar"></i> Ringkasan
        </button>
        <button class="tab-button" onclick="showHistoryTab('lamp')" style="padding: 10px 20px; border: none; background: #e5e7eb; color: #374151; border-radius: 6px 6px 0 0; cursor: pointer;">
            <i class="fas fa-lightbulb"></i> Lampu
        </button>
        <button class="tab-button" onclick="showHistoryTab('ac')" style="padding: 10px 20px; border: none; background: #e5e7eb; color: #374151; border-radius: 6px 6px 0 0; cursor: pointer;">
            <i class="fas fa-snowflake"></i> AC
        </button>
        <button class="tab-button" onclick="showHistoryTab('other-devices')" style="padding: 10px 20px; border: none; background: #e5e7eb; color: #374151; border-radius: 6px 6px 0 0; cursor: pointer;">
            <i class="fas fa-plug"></i> Perangkat Lain
        </button>
        <button class="tab-button" onclick="showHistoryTab('environment')" style="padding: 10px 20px; border: none; background: #e5e7eb; color: #374151; border-radius: 6px 6px 0 0; cursor: pointer;">
            <i class="fas fa-thermometer-half"></i> Lingkungan
        </button>
    </div>

    <!-- Tabel Ringkasan -->
    <div id="summary-table" class="history-table-container" style="background: white; padding: 20px; border-radius: 8px;">
        <h3><i class="fas fa-chart-bar"></i> Ringkasan Harian</h3>
        <table class="history-table" style="width: 100%; border-collapse: collapse; margin-top: 15px;">
            <thead>
                <tr style="background: #f9fafb;">
                    <th style="padding: 12px; text-align: left; border-bottom: 2px solid #e5e7eb;"><i class="fas fa-hashtag"></i> No</th>
                    <th style="padding: 12px; text-align: left; border-bottom: 2px solid #e5e7eb;"><i class="fas fa-calendar-alt"></i> Tanggal</th>
                    <th style="padding: 12px; text-align: left; border-bottom: 2px solid #e5e7eb;"><i class="fas fa-bolt"></i> Total Energi (Wh)</th>
                    <th style="padding: 12px; text-align: left; border-bottom: 2px solid #e5e7eb;"><i class="fas fa-clock"></i> Total Waktu Operasi</th>
                    <th style="padding: 12px; text-align: left; border-bottom: 2px solid #e5e7eb;"><i class="fas fa-users"></i> Max Orang</th>
                    <th style="padding: 12px; text-align: left; border-bottom: 2px solid #e5e7eb;"><i class="fas fa-leaf"></i> Efisiensi</th>
                </tr>
            </thead>
            <tbody id="summary-body">
                <tr>
                    <td colspan="6" style="padding: 20px; text-align: center; color: #6b7280;">Data akan dimuat...</td>
                </tr>
            </tbody>
        </table>
    </div>
</section>

<!-- ===== Reports Section ===== -->
<section id="reports" class="content-section" style="padding: 30px;">
    <header>
        <h1><i class="fas fa-file-chart-line"></i> Laporan Energi</h1>
    </header>
    
    <!-- Quick Reports -->
    <div class="reports-container" style="display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; margin-top: 20px;">
        <div class="report-card card">
            <div class="report-icon" style="text-align: center; margin-bottom: 15px;">
                <i class="fas fa-calendar-day" style="font-size: 3rem; color: #6366f1;"></i>
            </div>
            <h3>Laporan Harian</h3>
            <p>Konsumsi energi hari ini</p>
            <div class="report-actions" style="display: flex; gap: 10px; margin-top: 15px;">
                <button onclick="viewDailyReport()" style="flex: 1; padding: 8px; background: #6366f1; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-eye"></i> Lihat
                </button>
                <button onclick="downloadDailyReport()" style="flex: 1; padding: 8px; background: #16a34a; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-download"></i> PDF
                </button>
            </div>
        </div>
        
        <div class="report-card card">
            <div class="report-icon" style="text-align: center; margin-bottom: 15px;">
                <i class="fas fa-calendar-week" style="font-size: 3rem; color: #16a34a;"></i>
            </div>
            <h3>Laporan Mingguan</h3>
            <p>Ringkasan pemakaian energi 7 hari terakhir</p>
            <div class="report-actions" style="display: flex; gap: 10px; margin-top: 15px;">
                <button onclick="viewWeeklyReport()" style="flex: 1; padding: 8px; background: #6366f1; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-eye"></i> Lihat
                </button>
                <button onclick="downloadWeeklyReport()" style="flex: 1; padding: 8px; background: #16a34a; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-download"></i> PDF
                </button>
            </div>
        </div>
        
        <div class="report-card card">
            <div class="report-icon" style="text-align: center; margin-bottom: 15px;">
                <i class="fas fa-calendar-alt" style="font-size: 3rem; color: #d97706;"></i>
            </div>
            <h3>Laporan Bulanan</h3>
            <p>Analisis komprehensif pemakaian bulanan</p>
            <div class="report-actions" style="display: flex; gap: 10px; margin-top: 15px;">
                <button onclick="viewMonthlyReport()" style="flex: 1; padding: 8px; background: #6366f1; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-eye"></i> Lihat
                </button>
                <button onclick="downloadMonthlyReport()" style="flex: 1; padding: 8px; background: #16a34a; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-download"></i> PDF
                </button>
            </div>
        </div>
        
        <div class="report-card card">
            <div class="report-icon" style="text-align: center; margin-bottom: 15px;">
                <i class="fas fa-leaf" style="font-size: 3rem; color: #059669;"></i>
            </div>
            <h3>Laporan Efisiensi</h3>
            <p>Evaluasi tingkat efisiensi energi</p>
            <div class="report-actions" style="display: flex; gap: 10px; margin-top: 15px;">
                <button onclick="viewEfficiencyReport()" style="flex: 1; padding: 8px; background: #6366f1; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-eye"></i> Lihat
                </button>
                <button onclick="downloadEfficiencyReport()" style="flex: 1; padding: 8px; background: #16a34a; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-download"></i> PDF
                </button>
            </div>
        </div>
    </div>
</section>

<!-- ===== Devices Section ===== -->
<section id="devices" class="content-section" style="padding: 30px;">
    <header>
        <h1><i class="fas fa-microchip"></i> Manajemen Perangkat</h1>
    </header>
    <div class="devices-grid" style="display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; margin-top: 20px;">
        @foreach($devices ?? [] as $device)
        <div class="device-card card">
            <div class="device-status {{ $device->is_active ? 'online' : 'offline' }}" style="width: 12px; height: 12px; border-radius: 50%; background: {{ $device->is_active ? '#16a34a' : '#dc2626' }}; margin-bottom: 10px;"></div>
            <i class="fas fa-{{ $device->type == 'ac' ? 'snowflake' : ($device->type == 'lamp' ? 'lightbulb' : ($device->type == 'panel' ? 'bolt' : 'plug')) }} device-icon" style="font-size: 2rem; color: #6366f1; margin-bottom: 15px;"></i>
            <h3>{{ $device->name }}</h3>
            <p>Status: <span class="status-{{ $device->is_active ? 'online' : 'offline' }}" style="color: {{ $device->is_active ? '#16a34a' : '#dc2626' }}; font-weight: bold;">{{ $device->is_active ? 'Online' : 'Offline' }}</span></p>
            <p>Konsumsi: {{ $device->power_consumption }}W</p>
            <div class="device-controls" style="display: flex; gap: 10px; margin-top: 15px;">
                @if(!$device->is_always_on)
                <button class="btn-small" onclick="toggleDevice('{{ $device->type }}')" style="flex: 1; padding: 8px; background: #6366f1; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-power-off"></i>
                </button>
                @else
                <button class="btn-small" disabled style="flex: 1; padding: 8px; background: #9ca3af; color: white; border: none; border-radius: 4px; cursor: not-allowed;">
                    <i class="fas fa-lock"></i>
                </button>
                @endif
                <button class="btn-small" style="flex: 1; padding: 8px; background: #6b7280; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-cog"></i>
                </button>
            </div>
        </div>
        @endforeach
    </div>
</section>

<!-- ===== Automation Section ===== -->
<section id="automation" class="content-section" style="padding: 30px;">
    <header>
        <h1><i class="fas fa-robot"></i> Sistem Otomasi</h1>
    </header>
    <div class="automation-container" style="margin-top: 20px;">
        <div class="automation-rule card" style="margin-bottom: 20px;">
            <div class="rule-header" style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">
                <div style="display: flex; align-items: center; gap: 10px;">
                    <i class="fas fa-clock" style="color: #6366f1;"></i>
                    <h3>Jadwal Otomatis</h3>
                </div>
                <div class="toggle-switch active" onclick="toggleScheduledShutdown()">
                    <span class="toggle-slider"></span>
                </div>
            </div>
            <p>Listrik di ruangan mati otomatis di luar jam 07:00 - 17:00</p>
        </div>

        <div class="automation-rule card" style="margin-bottom: 20px;">
            <div class="rule-header" style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">
                <div style="display: flex; align-items: center; gap: 10px;">
                    <i class="fas fa-calendar-week" style="color: #16a34a;"></i>
                    <h3>Hari Kerja</h3>
                </div>
                <div class="toggle-switch active" onclick="toggleWorkingDays()">
                    <span class="toggle-slider"></span>
                </div>
            </div>
            <p>Sistem aktif hanya pada hari kerja (Senin - Jumat)</p>
        </div>

        <div class="automation-rule card" style="margin-bottom: 20px;">
            <div class="rule-header" style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">
                <div style="display: flex; align-items: center; gap: 10px;">
                    <i class="fas fa-lightbulb" style="color: #d97706;"></i>
                    <h3>Kontrol Lampu Otomatis</h3>
                </div>
                <div class="toggle-switch active" onclick="toggleAutoLampControl()">
                    <span class="toggle-slider"></span>
                </div>
            </div>
            <p>Lampu menyala otomatis saat intensitas cahaya rendah/gelap atau hujan</p>
        </div>

        <div class="automation-rule card" style="margin-bottom: 20px;">
            <div class="rule-header" style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">
                <div style="display: flex; align-items: center; gap: 10px;">
                    <i class="fas fa-users" style="color: #059669;"></i>
                    <h3>Deteksi Kehadiran & AC Otomatis</h3>
                </div>
                <div class="toggle-switch active" onclick="togglePresenceDetection()">
                    <span class="toggle-slider"></span>
                </div>
            </div>
            <p>AC menyala otomatis saat ada orang di ruangan</p>
        </div>

        <div class="automation-rule card" style="margin-bottom: 20px;">
            <div class="rule-header" style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">
                <div style="display: flex; align-items: center; gap: 10px;">
                    <i class="fas fa-power-off" style="color: #dc2626;"></i>
                    <h3>Auto Shutdown</h3>
                </div>
                <div class="toggle-switch active" id="autoShutdownToggle" onclick="toggleAutoShutdown()">
                    <span class="toggle-slider"></span>
                </div>
            </div>
            <p>Panel listrik dimatikan otomatis setelah jam kerja atau tidak ada orang selama 15 menit</p>
            <div class="automation-controls" style="display: flex; gap: 10px; margin-top: 15px;">
                <button onclick="emergencyOverride()" class="emergency-btn" style="padding: 10px 15px; background: #dc2626; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-exclamation-triangle"></i> Emergency Override
                </button>
                <button onclick="manualShutdownAll()" class="shutdown-btn" style="padding: 10px 15px; background: #6b7280; color: white; border: none; border-radius: 4px; cursor: pointer;">
                    <i class="fas fa-power-off"></i> Manual Shutdown
                </button>
            </div>
        </div>

        <!-- AC Rules Table -->
        @if(isset($acRules) && $acRules->count() > 0)
        <div class="automation-rule card">
            <div class="rule-header" style="margin-bottom: 15px;">
                <div style="display: flex; align-items: center; gap: 10px;">
                    <i class="fas fa-snowflake" style="color: #2563eb;"></i>
                    <h3>Estimasi AC Berdasarkan Kondisi</h3>
                </div>
            </div>
            <p>Sistem AC otomatis berdasarkan jumlah orang dan suhu luar ruangan</p>
            <div class="rules-table" style="margin-top: 15px; overflow-x: auto;">
                <table class="ac-rules-table" style="width: 100%; border-collapse: collapse;">
                    <thead>
                        <tr style="background: #f9fafb;">
                            <th style="padding: 12px; text-align: left; border: 1px solid #e5e7eb;">Jumlah Orang</th>
                            <th style="padding: 12px; text-align: left; border: 1px solid #e5e7eb;">Suhu Luar Ruangan (°C)</th>
                            <th style="padding: 12px; text-align: left; border: 1px solid #e5e7eb;">AC Aktif</th>
                            <th style="padding: 12px; text-align: left; border: 1px solid #e5e7eb;">Target Suhu Dalam (°C)</th>
                            <th style="padding: 12px; text-align: left; border: 1px solid #e5e7eb;">Keterangan</th>
                        </tr>
                    </thead>
                    <tbody>
                        @foreach($acRules as $rule)
                        <tr>
                            <td style="padding: 10px; border: 1px solid #e5e7eb;">{{ $rule->min_people }}-{{ $rule->max_people }}</td>
                            <td style="padding: 10px; border: 1px solid #e5e7eb;">{{ $rule->min_outdoor_temp }}-{{ $rule->max_outdoor_temp }}</td>
                            <td style="padding: 10px; border: 1px solid #e5e7eb;">{{ $rule->ac_units_needed == 0 ? 'Mode Gradual' : $rule->ac_units_needed . ' AC' }}</td>
                            <td style="padding: 10px; border: 1px solid #e5e7eb;">{{ $rule->target_temperature }}°C</td>
                            <td style="padding: 10px; border: 1px solid #e5e7eb;">{{ $rule->description }}</td>
                        </tr>
                        @endforeach
                    </tbody>
                </table>
            </div>
        </div>
        @endif
    </div>
</section>

<!-- ===== Alerts Section ===== -->
<section id="alerts" class="content-section" style="padding: 30px;">
    <header>
        <h1><i class="fas fa-bell"></i> Notifikasi & Peringatan</h1>
    </header>
    <div class="alerts-container" style="margin-top: 20px;">
        <div class="alert-item warning card" style="border-left: 4px solid #d97706; margin-bottom: 15px;">
            <div style="display: flex; align-items: flex-start; gap: 15px;">
                <i class="fas fa-exclamation-triangle" style="color: #d97706; font-size: 1.5rem; margin-top: 5px;"></i>
                <div class="alert-content" style="flex: 1;">
                    <h4 style="margin: 0 0 5px 0; color: #d97706;">Konsumsi Energi Tinggi</h4>
                    <p style="margin: 0 0 10px 0; color: #374151;">Pemakaian listrik hari ini 15% lebih tinggi dari rata-rata</p>
                    <span class="alert-time" style="font-size: 0.8rem; color: #6b7280;">2 jam yang lalu</span>
                </div>
            </div>
        </div>

        <div class="alert-item info card" style="border-left: 4px solid #2563eb; margin-bottom: 15px;">
            <div style="display: flex; align-items: flex-start; gap: 15px;">
                <i class="fas fa-info-circle" style="color: #2563eb; font-size: 1.5rem; margin-top: 5px;"></i>
                <div class="alert-content" style="flex: 1;">
                    <h4 style="margin: 0 0 5px 0; color: #2563eb;">Sistem Update</h4>
                    <p style="margin: 0 0 10px 0; color: #374151;">Firmware sensor berhasil diperbarui ke versi 2.1.0</p>
                    <span class="alert-time" style="font-size: 0.8rem; color: #6b7280;">5 jam yang lalu</span>
                </div>
            </div>
        </div>

        <div class="alert-item success card" style="border-left: 4px solid #16a34a; margin-bottom: 15px;">
            <div style="display: flex; align-items: flex-start; gap: 15px;">
                <i class="fas fa-check-circle" style="color: #16a34a; font-size: 1.5rem; margin-top: 5px;"></i>
                <div class="alert-content" style="flex: 1;">
                    <h4 style="margin: 0 0 5px 0; color: #16a34a;">Target Penghematan Tercapai</h4>
                    <p style="margin: 0 0 10px 0; color: #374151;">Berhasil menghemat 12% energi minggu ini</p>
                    <span class="alert-time" style="font-size: 0.8rem; color: #6b7280;">1 hari yang lalu</span>
                </div>
            </div>
        </div>
    </div>
</section>

<!-- ===== Settings Section ===== -->
<section id="settings" class="content-section" style="padding: 30px;">
    <header>
        <h1><i class="fas fa-cog"></i> Pengaturan Sistem</h1>
    </header>
    <div class="settings-container" style="display: grid; grid-template-columns: repeat(auto-fit, minmax(400px, 1fr)); gap: 20px; margin-top: 20px;">
        <div class="settings-group card">
            <h3><i class="fas fa-user"></i> Profil Pengguna</h3>
            <div class="setting-item" style="margin: 15px 0;">
                <label style="display: block; margin-bottom: 5px; font-weight: 600;">Nama Pengguna</label>
                <input type="text" value="Admin User" style="width: 100%; padding: 8px; border: 1px solid #d1d5db; border-radius: 4px;" />
            </div>
            <div class="setting-item" style="margin: 15px 0;">
                <label style="display: block; margin-bottom: 5px; font-weight: 600;">Email</label>
                <input type="email" value="admin@elektro.ac.id" style="width: 100%; padding: 8px; border: 1px solid #d1d5db; border-radius: 4px;" />
            </div>
        </div>

        <div class="settings-group card">
            <h3><i class="fas fa-bell"></i> Notifikasi</h3>
            <div class="setting-item" style="display: flex; justify-content: space-between; align-items: center; margin: 15px 0;">
                <label style="font-weight: 600;">Email Notifikasi</label>
                <div class="toggle-switch active">
                    <span class="toggle-slider"></span>
                </div>
            </div>
            <div class="setting-item" style="display: flex; justify-content: space-between; align-items: center; margin: 15px 0;">
                <label style="font-weight: 600;">Push Notification</label>
                <div class="toggle-switch">
                    <span class="toggle-slider"></span>
                </div>
            </div>
        </div>

        <div class="settings-group card">
            <h3><i class="fas fa-palette"></i> Tampilan</h3>
            <div class="setting-item" style="display: flex; justify-content: space-between; align-items: center; margin: 15px 0;">
                <label style="font-weight: 600;">Mode Gelap</label>
                <div class="toggle-switch active" id="darkModeToggle" onclick="toggleDarkMode()">
                    <span class="toggle-slider"></span>
                </div>
            </div>
        </div>
        
        <div class="settings-group card">
            <h3><i class="fas fa-microchip"></i> Koneksi Arduino</h3>
            <div class="setting-item" style="display: flex; justify-content: space-between; align-items: center; margin: 15px 0;">
                <label style="font-weight: 600;">Gunakan Data Arduino</label>
                <div class="toggle-switch" id="arduinoToggle" onclick="toggleArduinoConnection()">
                    <span class="toggle-slider"></span>
                </div>
            </div>
            <div class="setting-item" style="margin: 15px 0;">
                <label style="display: block; margin-bottom: 5px; font-weight: 600;">IP Address Arduino</label>
                <input type="text" id="arduinoIP" value="192.168.1.100" onchange="updateArduinoIP()" style="width: 100%; padding: 8px; border: 1px solid #d1d5db; border-radius: 4px;" />
            </div>
            <div class="setting-item" style="display: flex; justify-content: space-between; align-items: center; margin: 15px 0;">
                <label style="font-weight: 600;">Status Koneksi</label>
                <span id="connectionStatus" class="connection-status disconnected" style="padding: 4px 8px; border-radius: 4px; font-size: 0.8rem; font-weight: bold; color: white; background: #dc2626;">Terputus</span>
            </div>
        </div>
    </div>
</section>
@endsection

@push('scripts')
<script>
// Device control functions
async function toggleDevice(deviceType) {
    try {
        const response = await fetch('/api/devices/' + deviceType + '/control', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').content
            },
            body: JSON.stringify({
                action: 'toggle'
            })
        });
        
        const result = await response.json();
        
        if (result.status === 'success') {
            location.reload();
        } else {
            alert('Error: ' + result.message);
        }
    } catch (error) {
        alert('Error controlling device');
    }
}

function toggleACUnit(unit) {
    toggleDevice('ac' + unit);
}

function toggleAllAC() {
    // Toggle all AC units
    toggleDevice('ac1');
    setTimeout(() => toggleDevice('ac2'), 500);
}

function toggleLampCircuit(circuit) {
    toggleDevice('lamp' + circuit);
}

function toggleAllLamps() {
    // Toggle all lamp circuits
    toggleDevice('lamp1');
    setTimeout(() => toggleDevice('lamp2'), 500);
}

let currentTemp = 24;

function tempUp() {
    if (currentTemp < 30) {
        currentTemp++;
        document.getElementById('acTempValue').textContent = currentTemp + ' °C';
        // Send temperature command to Arduino
        sendTemperatureCommand(currentTemp);
    }
}

function tempDown() {
    if (currentTemp > 16) {
        currentTemp--;
        document.getElementById('acTempValue').textContent = currentTemp + ' °C';
        // Send temperature command to Arduino
        sendTemperatureCommand(currentTemp);
    }
}

async function sendTemperatureCommand(temp) {
    try {
        await fetch('/api/arduino/ac-temperature', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').content
            },
            body: JSON.stringify({
                temperature: temp
            })
        });
    } catch (error) {
        console.error('Error sending temperature command:', error);
    }
}

function startACGradualMode() {
    alert('Mode Gradual AC diaktifkan - Suhu akan dinaikkan bertahap ke 28°C dalam 15 menit');
}

// Automation functions
function toggleScheduledShutdown() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function toggleWorkingDays() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function toggleAutoLampControl() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function togglePresenceDetection() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function toggleAutoShutdown() {
    const toggle = event.target.closest('.toggle-switch');
    toggle.classList.toggle('active');
}

function emergencyOverride() {
    if (confirm('Apakah Anda yakin ingin melakukan Emergency Override? Ini akan menonaktifkan semua otomasi untuk 1 jam.')) {
        alert('Emergency Override diaktifkan - Semua otomasi dinonaktifkan selama 1 jam');
    }
}

function manualShutdownAll() {
    if (confirm('Apakah Anda yakin ingin mematikan semua perangkat?')) {
        alert('Semua perangkat akan dimatikan dalam 10 detik...');
    }
}

// History tab functions
function showHistoryTab(tabName) {
    // Hide all tables
    const tables = document.querySelectorAll('.history-table-container');
    tables.forEach(table => {
        table.style.display = 'none';
    });
    
    // Show selected table
    const targetTable = document.getElementById(tabName + '-table');
    if (targetTable) {
        targetTable.style.display = 'block';
    } else {
        document.getElementById('summary-table').style.display = 'block';
    }
    
    // Update active tab
    const tabs = document.querySelectorAll('.tab-button');
    tabs.forEach(tab => {
        tab.style.background = '#e5e7eb';
        tab.style.color = '#374151';
    });
    event.target.style.background = '#6366f1';
    event.target.style.color = 'white';
}

// Report functions
function viewDailyReport() {
    alert('Menampilkan laporan harian...');
}

function downloadDailyReport() {
    alert('Mengunduh laporan harian PDF...');
}

function viewWeeklyReport() {
    alert('Menampilkan laporan mingguan...');
}

function downloadWeeklyReport() {
    alert('Mengunduh laporan mingguan PDF...');
}

function viewMonthlyReport() {
    alert('Menampilkan laporan bulanan...');
}

function downloadMonthlyReport() {
    alert('Mengunduh laporan bulanan PDF...');
}

function viewEfficiencyReport() {
    alert('Menampilkan laporan efisiensi...');
}

function downloadEfficiencyReport() {
    alert('Mengunduh laporan efisiensi PDF...');
}

// Settings functions
function toggleDarkMode() {
    const toggle = document.getElementById('darkModeToggle');
    toggle.classList.toggle('active');
    // Implement dark mode toggle
}

function toggleArduinoConnection() {
    const toggle = document.getElementById('arduinoToggle');
    toggle.classList.toggle('active');
    
    const status = document.getElementById('connectionStatus');
    if (toggle.classList.contains('active')) {
        status.textContent = 'Terhubung';
        status.style.background = '#16a34a';
    } else {
        status.textContent = 'Terputus';
        status.style.background = '#dc2626';
    }
}

function updateArduinoIP() {
    const ip = document.getElementById('arduinoIP').value;
    alert('IP Arduino diperbarui ke: ' + ip);
}

// Initialize chart
function initChart() {
    const ctx = document.getElementById('myChart');
    if (ctx) {
        new Chart(ctx, {
            type: 'line',
            data: {
                labels: ['00:00', '04:00', '08:00', '12:00', '16:00', '20:00'],
                datasets: [{
                    label: 'Konsumsi Energi (kWh)',
                    data: [0.5, 0.3, 2.1, 3.5, 2.8, 1.2],
                    borderColor: '#6366f1',
                    backgroundColor: 'rgba(99, 102, 241, 0.1)',
                    tension: 0.4
                }]
            },
            options: {
                responsive: true,
                plugins: {
                    title: {
                        display: true,
                        text: 'Konsumsi Energi Harian'
                    }
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'kWh'
                        }
                    }
                }
            }
        });
    }
}

// Update real-time data every 5 seconds
setInterval(() => {
    fetch('/api/arduino/sensor-data')
        .then(response => response.json())
        .then(data => {
            if (data.temperature !== undefined) {
                document.getElementById('suhuValue').textContent = data.temperature + ' °C';
            }
            if (data.humidity !== undefined) {
                document.getElementById('humidityValue').textContent = data.humidity + ' %';
            }
            if (data.people_count !== undefined) {
                document.getElementById('jumlahOrang').textContent = data.people_count;
            }
            if (data.light_intensity !== undefined) {
                document.getElementById('ldrValue').textContent = data.light_intensity;
            }
        })
        .catch(error => console.error('Error updating sensor data:', error));
}, 5000);

// Initialize when page loads
document.addEventListener('DOMContentLoaded', function() {
    initChart();
});
</script>
@endpush