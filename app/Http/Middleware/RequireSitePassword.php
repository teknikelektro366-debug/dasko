<?php

namespace App\Http\Middleware;

use Closure;
use Illuminate\Http\Request;
use Symfony\Component\HttpFoundation\Response;

class RequireSitePassword
{
    private const SESSION_KEY = 'site_password_unlocked_at';
    private const EXPIRES_AFTER_SECONDS = 86400;

    public function handle(Request $request, Closure $next): Response
    {
        $unlockedAt = (int) $request->session()->get(self::SESSION_KEY, 0);

        if ($unlockedAt > 0 && now()->timestamp - $unlockedAt < self::EXPIRES_AFTER_SECONDS) {
            return $next($request);
        }

        $request->session()->forget(self::SESSION_KEY);

        if ($request->expectsJson() || $request->is('api/*')) {
            return response()->json([
                'message' => 'Akses terkunci. Masukkan Password  terlebih dahulu.',
            ], 423);
        }

        return redirect()->route('site-password.show');
    }
}
