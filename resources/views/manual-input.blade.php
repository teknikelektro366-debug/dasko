<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="csrf-token" content="{{ csrf_token() }}">
    <title>Input Data Manual - Smart Energy</title>
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
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            padding: 40px;
            max-width: 600px;
            width: 100%;
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
        .form-group {
            margin-bottom: 20px;
        }
        label {
            display: block;
            margin-bottom: 8px;
            color: #333;
            font-weight: 600;
        }
        input, select {
            width: 100%;
            padding: 12px;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            font-size: 16px;
            transition: border-color 0.3s;
        }
        input:focus, select:focus {
            outline: none;
            border-color: #667eea;
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
        .btn:active {
            transform: translateY(0);
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
        .row {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>📝 Input Data Manual</h1>
        <p class="subtitle">Masukkan jumlah orang per jam</p>

        <div id="alertSuccess" class="alert alert-success"></div>
        <div id="alertError" class="alert alert-error"></div>

        <form id="manualInputForm">
            <div class="form-group">
                <label>Tanggal</label>
                <input type="date" name="date" required>
            </div>

            <div class="form-group">
                <label>Jam</label>
                <select name="hour" required>
                    <option value="">-- Pilih Jam --</option>
                    <option value="07">07:00 - 08:00</option>
                    <option value="08">08:00 - 09:00</option>
                    <option value="09">09:00 - 10:00</option>
                    <option value="10">10:00 - 11:00</option>
                    <option value="11">11:00 - 12:00</option>
                    <option value="12">12:00 - 13:00</option>
                    <option value="13">13:00 - 14:00</option>
                    <option value="14">14:00 - 15:00</option>
                    <option value="15">15:00 - 16:00</option>
                </select>
            </div>

            <div class="form-group">
                <label>Jumlah Orang</label>
                <input type="number" name="people_count" min="0" max="100" value="0" required>
            </div>

            <button type="submit" class="btn">💾 Simpan Data</button>
        </form>

        <div class="back-link">
            <a href="/dashboard">← Kembali ke Dashboard</a>
        </div>
    </div>

    <script>
        // Set default date to 4th of current month
        const today = new Date();
        const defaultDate = new Date(today.getFullYear(), today.getMonth(), 4);
        document.querySelector('input[name="date"]').valueAsDate = defaultDate;

        document.getElementById('manualInputForm').addEventListener('submit', async function(e) {
            e.preventDefault();

            const formData = new FormData(this);
            const date = formData.get('date');
            const hour = formData.get('hour');
            const timestamp = `${date} ${hour}:00:00`;
            const peopleCount = parseInt(formData.get('people_count'));

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

                const result = await response.json();

                if (response.ok) {
                    document.getElementById('alertSuccess').textContent = '✅ Data berhasil disimpan! Lihat di History.';
                    document.getElementById('alertSuccess').style.display = 'block';
                    document.getElementById('alertError').style.display = 'none';
                    
                    // Reset form
                    document.querySelector('input[name="people_count"]').value = 0;
                    
                    // Hide success message after 3 seconds
                    setTimeout(() => {
                        document.getElementById('alertSuccess').style.display = 'none';
                    }, 3000);
                } else {
                    throw new Error(result.message || 'Gagal menyimpan data');
                }
            } catch (error) {
                document.getElementById('alertError').textContent = '❌ Error: ' + error.message;
                document.getElementById('alertError').style.display = 'block';
                document.getElementById('alertSuccess').style.display = 'none';
            }
        });
    </script>
</body>
</html>
