<!DOCTYPE html>
<html lang="{{ str_replace('_', '-', app()->getLocale()) }}">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="csrf-token" content="{{ csrf_token() }}">

    <title>Login - {{ config('app.name', 'Smart Energy Management') }}</title>

    <!-- Fonts -->
    <link rel="preconnect" href="https://fonts.bunny.net">
    <link href="https://fonts.bunny.net/css?family=figtree:400,500,600&display=swap" rel="stylesheet" />
    
    <!-- Font Awesome -->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    
    <!-- Tailwind CSS -->
    <script src="https://cdn.tailwindcss.com"></script>
    
    <!-- Alpine.js -->
    <script defer src="https://unpkg.com/alpinejs@3.x.x/dist/cdn.min.js"></script>

    <style>
        .gradient-bg {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        }
        
        .glassmorphism {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.2);
        }
        
        .floating-animation {
            animation: floating 3s ease-in-out infinite;
        }
        
        @keyframes floating {
            0%, 100% { transform: translateY(0px); }
            50% { transform: translateY(-10px); }
        }
    </style>
</head>
<body class="gradient-bg min-h-screen flex items-center justify-center p-4">
    <div class="w-full max-w-md">
        <!-- Logo and Title -->
        <div class="text-center mb-8">
            <div class="inline-flex items-center justify-center w-16 h-16 bg-white rounded-full shadow-lg mb-4 floating-animation">
                <i class="fas fa-bolt text-2xl text-indigo-600"></i>
            </div>
            <h1 class="text-3xl font-bold text-white mb-2">Smart Energy Management</h1>
            <p class="text-indigo-100">Lab Teknik Tegangan Tinggi</p>
        </div>

        <!-- Login Form -->
        <div class="glassmorphism rounded-2xl p-8 shadow-2xl">
            <form method="POST" action="{{ route('login') }}" x-data="{ loading: false }" @submit="loading = true">
                @csrf

                <!-- Email -->
                <div class="mb-6">
                    <label for="email" class="block text-sm font-medium text-white mb-2">
                        <i class="fas fa-envelope mr-2"></i>Email Address
                    </label>
                    <input id="email" 
                           type="email" 
                           name="email" 
                           value="{{ old('email') }}" 
                           required 
                           autofocus
                           class="w-full px-4 py-3 bg-white/20 border border-white/30 rounded-lg text-white placeholder-white/70 focus:outline-none focus:ring-2 focus:ring-white/50 focus:border-transparent transition-all duration-200"
                           placeholder="Enter your email">
                    @error('email')
                        <p class="mt-2 text-sm text-red-200">{{ $message }}</p>
                    @enderror
                </div>

                <!-- Password -->
                <div class="mb-6">
                    <label for="password" class="block text-sm font-medium text-white mb-2">
                        <i class="fas fa-lock mr-2"></i>Password
                    </label>
                    <div class="relative" x-data="{ showPassword: false }">
                        <input id="password" 
                               :type="showPassword ? 'text' : 'password'"
                               name="password" 
                               required
                               class="w-full px-4 py-3 pr-12 bg-white/20 border border-white/30 rounded-lg text-white placeholder-white/70 focus:outline-none focus:ring-2 focus:ring-white/50 focus:border-transparent transition-all duration-200"
                               placeholder="Enter your password">
                        <button type="button" 
                                @click="showPassword = !showPassword"
                                class="absolute inset-y-0 right-0 pr-3 flex items-center text-white/70 hover:text-white">
                            <i :class="showPassword ? 'fas fa-eye-slash' : 'fas fa-eye'"></i>
                        </button>
                    </div>
                    @error('password')
                        <p class="mt-2 text-sm text-red-200">{{ $message }}</p>
                    @enderror
                </div>

                <!-- Remember Me -->
                <div class="flex items-center justify-between mb-6">
                    <label class="flex items-center">
                        <input type="checkbox" 
                               name="remember" 
                               class="rounded border-white/30 bg-white/20 text-indigo-600 focus:ring-white/50">
                        <span class="ml-2 text-sm text-white">Remember me</span>
                    </label>
                </div>

                <!-- Submit Button -->
                <button type="submit" 
                        :disabled="loading"
                        class="w-full bg-white text-indigo-600 font-semibold py-3 px-4 rounded-lg hover:bg-indigo-50 focus:outline-none focus:ring-2 focus:ring-white/50 transition-all duration-200 disabled:opacity-50 disabled:cursor-not-allowed">
                    <span x-show="!loading">
                        <i class="fas fa-sign-in-alt mr-2"></i>Sign In
                    </span>
                    <span x-show="loading" class="flex items-center justify-center">
                        <i class="fas fa-spinner fa-spin mr-2"></i>Signing In...
                    </span>
                </button>
            </form>

            <!-- Demo Accounts -->
            <div class="mt-8 pt-6 border-t border-white/20">
                <p class="text-sm text-white/80 text-center mb-4">Demo Accounts:</p>
                <div class="space-y-2 text-xs text-white/70">
                    <div class="flex justify-between">
                        <span>Admin:</span>
                        <span>admin@smartenergy.lab / elektro2024</span>
                    </div>
                    <div class="flex justify-between">
                        <span>Operator:</span>
                        <span>operator@smartenergy.lab / operator2024</span>
                    </div>
                    <div class="flex justify-between">
                        <span>Viewer:</span>
                        <span>viewer@smartenergy.lab / viewer2024</span>
                    </div>
                </div>
            </div>
        </div>

        <!-- Footer -->
        <div class="text-center mt-8">
            <p class="text-indigo-100 text-sm">
                © {{ date('Y') }} Smart Energy Management System
            </p>
            <p class="text-indigo-200 text-xs mt-1">
                Lab Teknik Tegangan Tinggi - Universitas
            </p>
        </div>
    </div>

    <!-- Success Message -->
    @if(session('success'))
        <div x-data="{ show: true }" 
             x-show="show" 
             x-init="setTimeout(() => show = false, 5000)"
             class="fixed top-4 right-4 bg-green-500 text-white px-6 py-3 rounded-lg shadow-lg z-50">
            <div class="flex items-center">
                <i class="fas fa-check-circle mr-2"></i>
                {{ session('success') }}
                <button @click="show = false" class="ml-4 text-white/80 hover:text-white">
                    <i class="fas fa-times"></i>
                </button>
            </div>
        </div>
    @endif
</body>
</html>