<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="csrf-token" content="{{ csrf_token() }}">
    <title>Input Data Harian - Smart Energy</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            padding: 40px;
            max-width: 1200px;
            margin: 0 auto;
        }
        h1 {
            color: #667eea;
            margin-bottom: 10px;
            text-align: center;
        }
        .subtitle {
            text-align: center;
            color: #666;
            margin-bottom: 30px;
        }
        .date-selector {
            margin-bottom: 30px;
            text-align: center;
        }
        .date-selector input {
            padding: 12px 20px;
            border: 2px solid #667eea;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 600;
        }
        .info-box {
            background: #e3f2fd;
            border-left: 4px solid #2196f3;
            padding: 15px;
            margin-bottom: 20px;
            border-radius: 4px;
        }
        .info-box h3 {
            color: #1976d2;
            margin-bottom: 8px;
        }
        .info-box ul {
            margin-left: 20px;
            color: #555;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-bottom: 20px;
        }
        th {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 15px;
            text-align: left;
            font-weight: 600;
        }
        td {
            padding: 12px 15px;
            border-bottom: 1px solid #e0e0e0;
        }
        tr:hover {
            background: #f5f5f5;
        }
        input[type="number"] {
            width: 100%;
            padding: 8px;
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            font-size: 14px;
            transition: border-color 0.3s;
        }
        input[type="number"]:focus {
            outline: none;
            border-color: #667eea;
        }
        .time-label {
            font-weight: 600;
            color: #333;
        }
        .note {
            font-size: 12px;
            color: #999;
            font-style: italic;
        }
        .btn {
            width: 100%;
            padding: 15px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 18px;
            font-weight: 600;
            cursor: pointer;
            transition: transform 0.2s;
        }
        .btn:hover {
            transform: translateY(-2px);
        }
        .btn:disabled {
            opacity: 0.6;
            cursor: not-allowed;
        }
        .alert {
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 20px;
            display: none;
        }
        .alert-success {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .alert-error {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        .back-link {
            text-align: center;
            margin-top: 20px;
        }
        .back-link a {
            color: #667eea;
            text-decoration: none;
            font-weight: 600;
        }
        .back-link a:hover {
            text-decoration: underline;
        }
        .progress {
            margin-top: 10px;
            display: none;
        }
        .progress-bar {
            width: 100%;
            height: 30px;
            background: #e0e0e0;
            border-radius: 15px;
            overflow: hidden;
        }
        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
            width: 0%;
            transition: width 0.3s;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-weight: 600;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>📊 Input Data Harian Ruang Dosen</h1>
        <p class="subtitle">Ruang Dosen Prodi Teknik Elektro - UNJA</p>

        <div class="date-selector">
            <label><strong>Pilih Tanggal:</strong></label><br>
            <input type="date" id="selectedDate" required>
        </div>

        <div class="info-box">
            <h3>ℹ️ Informasi Ruang Dosen</h3>
            <ul>
                <li><strong>Total Dosen:</strong> 15 orang</li>
                <li><strong>Jam Kerja:</strong> 07:00 - 15:00 (Senin-Jumat)</li>
                <li><strong>Aktivitas:</strong> Mengajar, Bimbingan Mahasiswa, Rapat, Penelitian</li>
                <li><strong>Catatan:</strong> Data sudah diisi dengan estimasi natural, silakan edit sesuai kondisi aktual</li>
            </ul>
        </div>

        <div id="alertSuccess" class="alert alert-success"></div>
        <div id="alertError" class="alert alert-error"></div>

        <form id="bulkInputForm">
            <table>
                <thead>
                    <tr>
                        <th>Jam</th>
                        <th>Jumlah Orang</th>
                        <th>Keterangan</th>
                    </tr>
                </thead>
                <tbody id="hourlyData">
                    <!-- Data will be populated by JavaScript -->
                </tbody>
            </table>

            <div class="progress" id="progressBar">
                <div class="progress-bar">
                    <div class="progress-fill" id="progressFill">0%</div>
                </div>
            </div>

            <button type="submit" class="btn" id="submitBtn">💾 Simpan Semua Data (Jam 07:00 - 15:00)</button>
        </form>

        <div class="back-link">
            <a href="/dashboard">← Kembali ke Dashboard</a>
        </div>
    </div>

    <script>
        // Set default date to 4th of current month
        const today = new Date();
        const defaultDate = new Date(today.getFullYear(), today.getMonth(), 4);
        document.getElementById('selectedDate').valueAsDate = defaultDate;

        // Natural data pattern for Ruang Dosen Prodi Teknik Elektro
        // Jam kerja: 07:00 - 15:00
        const hourlyPattern = [
            { hour: '07', people: 3, note: 'Dosen mulai datang' },
            { hour: '08', people: 8, note: 'Jam kerja dimulai, beberapa dosen mengajar' },
            { hour: '09', people: 12, note: 'Peak - Banyak dosen & mahasiswa bimbingan' },
            { hour: '10', people: 10, note: 'Dosen mengajar, mahasiswa bimbingan' },
            { hour: '11', people: 7, note: 'Beberapa dosen istirahat/mengajar' },
            { hour: '12', people: 4, note: 'Jam istirahat siang' },
            { hour: '13', people: 9, note: 'Kembali dari istirahat, bimbingan siang' },
            { hour: '14', people: 11, note: 'Peak siang - Bimbingan & rapat' },
            { hour: '15', people: 5, note: 'Jam pulang, mulai sepi' }
        ];

        // Populate table
        const tbody = document.getElementById('hourlyData');
        hourlyPattern.forEach(item => {
            const row = `
                <tr>
                    <td class="time-label">${item.hour}:00 - ${String(parseInt(item.hour) + 1).padStart(2, '0')}:00</td>
                    <td>
                        <input type="number" 
                               name="people_${item.hour}" 
                               min="0" 
                               max="50" 
                               value="${item.people}" 
                               required>
                    </td>
                    <td class="note">${item.note}</td>
                </tr>
            `;
            tbody.innerHTML += row;
        });

        // Handle form submission
        document.getElementById('bulkInputForm').addEventListener('submit', async function(e) {
            e.preventDefault();

            const selectedDate = document.getElementById('selectedDate').value;
            if (!selectedDate) {
                alert('Pilih tanggal terlebih dahulu!');
                return;
            }

            const submitBtn = document.getElementById('submitBtn');
            const progressBar = document.getElementById('progressBar');
            const progressFill = document.getElementById('progressFill');
            
            submitBtn.disabled = true;
            submitBtn.textContent = '⏳ Menyimpan data...';
            progressBar.style.display = 'block';

            let successCount = 0;
            let errorCount = 0;
            const totalHours = hourlyPattern.length; // 9 jam (07:00-15:00)

            for (let i = 0; i < hourlyPattern.length; i++) {
                const item = hourlyPattern[i];
                const peopleInput = document.querySelector(`input[name="people_${item.hour}"]`);
                const peopleCount = parseInt(peopleInput.value);

                const timestamp = `${selectedDate} ${item.hour}:00:00`;

                // Logika Lampu: Ada orang = ON, Tidak ada = OFF
                const lampStatus = peopleCount > 0 ? 'ON' : 'OFF';

                // Logika AC & Suhu berdasarkan jumlah orang
                let acStatus = 'OFF';
                let setTemperature = null;

                if (peopleCount >= 1 && peopleCount <= 5) {
                    acStatus = '1 AC ON';
                    setTemperature = 25;
                } else if (peopleCount >= 6 && peopleCount <= 9) {
                    acStatus = '2 AC ON';
                    setTemperature = 25;
                } else if (peopleCount >= 10 && peopleCount <= 15) {
                    acStatus = '2 AC ON';
                    setTemperature = 22;
                } else if (peopleCount >= 16) {
                    acStatus = '2 AC ON';
                    setTemperature = 20;
                }

                const data = {
                    device_id: 'MANUAL_INPUT',
                    location: 'Ruang Dosen Prodi Teknik Elektro',
                    people_count: peopleCount,
                    room_temperature: 27,
                    humidity: 65,
                    light_level: peopleCount > 0 ? 500 : 0,
                    ac_status: acStatus,
                    set_temperature: setTemperature,
                    lamp_status: lampStatus,
                    proximity_in: false,
                    proximity_out: false,
                    wifi_rssi: 0,
                    status: 'active',
                    created_at: timestamp
                };

                try {
                    const response = await fetch('/api/sensor/data', {
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/json',
                            'X-CSRF-TOKEN': document.querySelector('meta[name="csrf-token"]').content
                        },
                        body: JSON.stringify(data)
                    });

                    if (response.ok) {
                        successCount++;
                    } else {
                        errorCount++;
                    }
                } catch (error) {
                    errorCount++;
                }

                // Update progress
                const progress = Math.round(((i + 1) / totalHours) * 100);
                progressFill.style.width = progress + '%';
                progressFill.textContent = progress + '%';

                // Small delay to avoid overwhelming the server
                await new Promise(resolve => setTimeout(resolve, 100));
            }

            // Show results
            submitBtn.disabled = false;
            submitBtn.textContent = '💾 Simpan Semua Data (Jam 07:00 - 15:00)';
            progressBar.style.display = 'none';

            if (errorCount === 0) {
                document.getElementById('alertSuccess').textContent = 
                    `✅ Berhasil menyimpan ${successCount} data! Lihat di History.`;
                document.getElementById('alertSuccess').style.display = 'block';
                document.getElementById('alertError').style.display = 'none';
            } else {
                document.getElementById('alertError').textContent = 
                    `⚠️ Berhasil: ${successCount}, Gagal: ${errorCount}. Coba lagi untuk data yang gagal.`;
                document.getElementById('alertError').style.display = 'block';
                document.getElementById('alertSuccess').style.display = 'none';
            }

            // Hide message after 5 seconds
            setTimeout(() => {
                document.getElementById('alertSuccess').style.display = 'none';
                document.getElementById('alertError').style.display = 'none';
            }, 5000);
        });
    </script>
</body>
</html>
