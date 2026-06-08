<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="csrf-token" content="{{ csrf_token() }}">
    <title>Akses Proyek - Smart Energy Dashboard</title>
    <link rel="stylesheet" href="{{ asset('css/style.css') }}?v={{ time() }}">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <style>
        .unlock-page .sidebar {
            overflow: hidden;
        }

        .unlock-page .main-content {
            min-height: 100vh;
        }

        .unlock-wrapper {
            min-height: calc(100vh - 96px);
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 30px;
        }

        .unlock-card {
            width: 100%;
            max-width: 460px;
            background: white;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
            box-shadow: 0 10px 30px rgba(14, 50, 97, 0.12);
            overflow: hidden;
        }

        .unlock-card-header {
            background: #0e3261;
            color: white;
            padding: 24px;
            text-align: center;
        }

        .unlock-icon {
            width: 58px;
            height: 58px;
            margin: 0 auto 14px;
            border-radius: 50%;
            background: rgba(255, 255, 255, 0.15);
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 1.4rem;
        }

        .unlock-card-header h1 {
            font-size: 1.35rem;
            font-weight: 700;
            margin-bottom: 6px;
        }

        .unlock-card-header p {
            color: rgba(255, 255, 255, 0.82);
            font-size: 0.9rem;
        }

        .unlock-card-body {
            padding: 24px;
        }

        .unlock-label {
            display: flex;
            align-items: center;
            gap: 8px;
            font-weight: 600;
            color: #374151;
            margin-bottom: 8px;
        }

        .unlock-input {
            width: 100%;
            padding: 12px 14px;
            border: 1px solid #d1d5db;
            border-radius: 8px;
            font-size: 0.95rem;
            outline: none;
            transition: all 0.2s ease;
        }

        .unlock-input:focus {
            border-color: #0e3261;
            box-shadow: 0 0 0 3px rgba(14, 50, 97, 0.12);
        }

        .password-field {
            position: relative;
        }

        .password-field .unlock-input {
            padding-right: 46px;
        }

        .password-toggle {
            position: absolute;
            right: 4px;
            top: 0;
            bottom: 0;
            width: 42px;
            border: none;
            background: transparent;
            color: #6b7280;
            cursor: pointer;
            font-size: 1rem;
            padding: 0;
            display: flex;
            align-items: center;
            justify-content: center;
            user-select: none;
            -webkit-user-select: none;
            touch-action: manipulation;
        }

        .password-toggle:hover {
            color: #0e3261;
        }

        .password-toggle:focus {
            outline: none;
        }

        .password-toggle:focus-visible {
            color: #0e3261;
            border-radius: 6px;
            box-shadow: inset 0 0 0 2px rgba(14, 50, 97, 0.18);
        }

        .password-toggle:hover {
            color: #0e3261;
        }

        .unlock-button {
            width: 100%;
            margin-top: 18px;
            border: none;
            border-radius: 8px;
            padding: 12px 16px;
            background: #0e3261;
            color: white;
            font-weight: 700;
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .unlock-button:hover {
            background: #0b2850;
        }

        .unlock-error {
            margin-top: 10px;
            padding: 10px 12px;
            border-radius: 8px;
            background: #fef2f2;
            border: 1px solid #fecaca;
            color: #b91c1c;
            font-size: 0.88rem;
        }

        .unlock-note {
            margin-top: 16px;
            color: #6b7280;
            font-size: 0.85rem;
            text-align: center;
        }

        @media (max-width: 768px) {
            .unlock-page .sidebar {
                display: none;
            }

            .unlock-page .main-content {
                margin-left: 0;
                width: 100%;
            }

            .unlock-wrapper {
                padding: 20px;
            }
        }
    </style>
</head>

<body class="unlock-page">
    <div class="sidebar" id="sidebar">
        <div class="sidebar-toggle">
            <i class="fas fa-lock" id="toggleIcon"></i>
            <h3><img src="{{ asset('img/smartenergy-logo.png') }}" class="sidebar-text" alt="Smart Energy Logo"
                    style="height: 30px; vertical-align: middle;"></h3>
        </div>

        <div class="sidebar-content">
            <ul>
                <li><a href="#" class="active" title="Akses"><i class="fas fa-shield-alt"></i> <span
                            class="sidebar-text">Akses Proyek</span></a></li>
                <li><a href="#" title="Dashboard"><i class="fas fa-desktop"></i> <span
                            class="sidebar-text">Monitoring</span></a></li>
                <li><a href="#" title="Analisa"><i class="fas fa-chart-line"></i> <span
                            class="sidebar-text">Analisa</span></a></li>
                <li><a href="#" title="Laporan"><i class="fas fa-file-alt"></i> <span
                            class="sidebar-text">Laporan</span></a></li>
            </ul>

            <div class="sidebar-footer">
                <div class="user-info">
                    <i class="fas fa-user-lock"></i>
                    <span class="sidebar-text">Guest</span>
                </div>
                <div class="system-status">
                    <i class="fas fa-circle" style="color: #f59e0b;"></i>
                    <span class="sidebar-text">Access Locked</span>
                </div>
            </div>
        </div>
    </div>

    <div class="main-content">
        <div class="navbar">
            <i class="fas fa-building"></i>
            Ruang Dosen Prodi Teknik Elektro - UNJA
        </div>
        <div class="subnav">
            <i class="fas fa-key"></i>
            Masukkan Password  untuk membuka dashboard
        </div>

        <div class="unlock-wrapper">
            <section class="unlock-card">
                <div class="unlock-card-header">
                    <div class="unlock-icon"><i class="fas fa-lock"></i></div>
                    <h1>Smart Energy Dashboard</h1>
                    <p>Akses web terkunci untuk melindungi halaman dashboard.</p>
                </div>

                <div class="unlock-card-body">
                    <form method="POST" action="{{ route('site-password.unlock') }}">
                        @csrf

                        <label for="password" class="unlock-label">
                            <i class="fas fa-key"></i>
                            Password 
                        </label>
                        <div class="password-field">
                            <input id="password" class="unlock-input" type="password" name="password" required autofocus
                                autocomplete="current-password" placeholder="Masukkan password">
                            <button type="button" class="password-toggle" aria-label="Tampilkan password"
                                aria-pressed="false">
                                <i class="fas fa-eye" id="passwordToggleIcon"></i>
                            </button>
                        </div>

                        @error('password')
                            <div class="unlock-error"><i class="fas fa-exclamation-triangle"></i> {{ $message }}</div>
                        @enderror

                        <button type="submit" class="unlock-button">
                            <i class="fas fa-unlock-alt"></i> Buka Dashboard
                        </button>
                    </form>

                    <p class="unlock-note">Akses tersimpan pada session browser dan otomatis kedaluwarsa dalam 24 jam.
                    </p>
                </div>
            </section>
        </div>
    </div>
    <script>
        const passwordInput = document.getElementById('password');
        const passwordToggle = document.querySelector('.password-toggle');
        const passwordToggleIcon = document.getElementById('passwordToggleIcon');

        passwordToggle.addEventListener('pointerdown', function (event) {
            event.preventDefault();
        });

        passwordToggle.addEventListener('click', function () {
            const shouldShow = passwordInput.type === 'password';

            passwordInput.type = shouldShow ? 'text' : 'password';
            passwordToggleIcon.className = shouldShow ? 'fas fa-eye-slash' : 'fas fa-eye';
            passwordToggle.setAttribute('aria-label', shouldShow ? 'Sembunyikan password' : 'Tampilkan password');
            passwordToggle.setAttribute('aria-pressed', shouldShow ? 'true' : 'false');
            passwordInput.focus({ preventScroll: true });
        });
    </script>
</body>

</html>
