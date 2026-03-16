#include "CoHModSDK.hpp"

#pragma comment(lib, "CoHModSDK.lib")

void SetupPatch() {
    std::uintptr_t xpKickerCheckAddr = ModSDK::Memory::FindPattern("WW2Mod.dll", "E8 ?? ?? ?? ?? 84 C0 75 18 8B 54 24 10");
    if (!xpKickerCheckAddr) {
        MessageBoxA(nullptr, "Caller pattern not found!", "Error", MB_ICONERROR);
        return;
    }

    // Patch the 5-byte `call` instruction to `xor al, al; nop; nop; nop` so this
    // specific XP-kicker check behaves as if the original function returned false.
    constexpr unsigned char patchBytes[] = { 0x32, 0xC0, 0x90, 0x90, 0x90 };
    ModSDK::Memory::PatchMemory(reinterpret_cast<void*>(xpKickerCheckAddr), patchBytes, sizeof(patchBytes));
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
        SetupPatch();
    }

    __declspec(dllexport) void OnGameShutdown() {
        // Unused
    }

    __declspec(dllexport) const char* GetModName() {
        return "Persistent XP Kickers";
    }

    __declspec(dllexport) const char* GetModVersion() {
        return "1.3.0";
    }

    __declspec(dllexport) const char* GetModAuthor() {
        return "Tosox";
    }
}
