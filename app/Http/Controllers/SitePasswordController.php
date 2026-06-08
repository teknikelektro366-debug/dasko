<?php

namespace App\Http\Controllers;

use Illuminate\Http\RedirectResponse;
use Illuminate\Http\Request;
use Illuminate\View\View;

class SitePasswordController extends Controller
{
    private const SESSION_KEY = 'site_password_unlocked_at';
    private const PASSWORD_HASH = '$2y$10$0ZAVASNpxpE8yEoJjGNDLukOfTlE2Qrrjj52r3oYQcd/P14qF1r66';

    public function show(Request $request): View|RedirectResponse
    {
        if ($this->isUnlocked($request)) {
            return redirect()->route('dashboard');
        }

        return view('site-password');
    }

    public function unlock(Request $request): RedirectResponse
    {
        $request->validate([
            'password' => ['required', 'string'],
        ], [
            'password.required' => 'Password wajib diisi.',
        ]);

        if (!password_verify((string) $request->input('password'), self::PASSWORD_HASH)) {
            return back()
                ->withErrors(['password' => 'Password  tidak sesuai.'])
                ->onlyInput();
        }

        $request->session()->regenerate();
        $request->session()->put(self::SESSION_KEY, now()->timestamp);

        return redirect()->intended(route('dashboard'));
    }

    private function isUnlocked(Request $request): bool
    {
        $unlockedAt = (int) $request->session()->get(self::SESSION_KEY, 0);

        return $unlockedAt > 0 && now()->timestamp - $unlockedAt < 86400;
    }
}
