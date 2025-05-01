#include <intrin.h>

#include "CoHModSDK.hpp"

#pragma comment(lib, "CoHModSDK.lib")

// Method used to check if XP kickers should be displayed
typedef bool(*Sub2641C0_t)();
Sub2641C0_t oFnSub2641C0 = nullptr;

// Address of the instruction calling the check method
std::uintptr_t expectedCallerAddr = 0;

bool CallOriginal2641C0(void* thisPtr) {
    // We can't call oFnSub2641C0(_this) directly, even if declared as __thiscall,
    // because MSVC doesn't fully support __thiscall for function pointers,
    // and MinHook treats it like a raw call (usually __cdecl).
    // This means ECX won't be set correctly, which crashes the game.
    // To safely mimic the correct behavior, we manually set ECX with inline assembly.

    bool result;
    __asm {
        mov ecx, thisPtr
        call oFnSub2641C0
        mov result, al
    }
    return result;
}

bool __cdecl HookedSub2641C0(void* _this) {
    // Check if this method was called by the xp kicker check code
    if (expectedCallerAddr == (std::uintptr_t)_ReturnAddress()) {
        return false;
    }

    return CallOriginal2641C0(_this);
}

bool SetupHooks() {
    std::uintptr_t sub2641C0Addr = ModSDK::Memory::FindPattern("WW2Mod.dll", "56 E8 ?? ?? ?? ?? 85 C0 75 03 32 C0");
    if (!sub2641C0Addr) {
        MessageBoxA(nullptr, "Pattern not found", "Error", MB_ICONERROR);
        return false;
    }

    std::uintptr_t xpKickerCheckAddr = ModSDK::Memory::FindPattern("WW2Mod.dll", "E8 ?? ?? ?? ?? 84 C0 75 18 8B 54 24 10");
    if (!xpKickerCheckAddr) {
        MessageBoxA(nullptr, "Caller pattern not found!", "Error", MB_ICONERROR);
        return false;
    }

    // This is the return address seen by _ReturnAddress()
    expectedCallerAddr = xpKickerCheckAddr + 5; // Skip to after the call instruction

    // --- Install hook ---
	if (!ModSDK::Hooks::CreateHook((void*)sub2641C0Addr, &HookedSub2641C0, (void**)&oFnSub2641C0)) return false;
    if (!ModSDK::Hooks::EnableAllHooks()) return false;

    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    DisableThreadLibraryCalls(hModule);
    return TRUE;
}

extern "C" {
    __declspec(dllexport) void OnSDKLoad() {
        // Unused
    }

    __declspec(dllexport) void OnGameStart() {
        SetupHooks();
    }

    __declspec(dllexport) void OnGameShutdown() {
        // Unused
    }

    __declspec(dllexport) const char* GetModName() {
        return "Persistent XP Kickers";
    }

    __declspec(dllexport) const char* GetModVersion() {
        return "1.0.0";
    }

    __declspec(dllexport) const char* GetModAuthor() {
        return "Tosox";
    }
}
