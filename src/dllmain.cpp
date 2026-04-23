#include "CoHModSDK.hpp"

#include <cstring>

namespace {
	constexpr const char* kXpKickerCheckPattern = "E8 ?? ?? ?? ?? 84 C0 75 18 8B 54 24 10";

    constexpr unsigned char kXpKickerCheckPatchedBytes[] = { 0x32, 0xC0, 0x90, 0x90, 0x90 };

    void* tXpKickerCheck = nullptr;
    char xpKickerCheckOriginalBytes[5] = {};

    bool SetupPatch() {
        const auto tXpKickerCheckResult = ModSDK::Memory::FindPattern("WW2Mod.dll", kXpKickerCheckPattern);
        if (!tXpKickerCheckResult.has_value()) {
            ModSDK::Dialogs::ShowError("Failed to find XpKickerCheck address");
            return false;
        }

		tXpKickerCheck = reinterpret_cast<void*>(tXpKickerCheckResult.value());
        std::memcpy(xpKickerCheckOriginalBytes, tXpKickerCheck, sizeof(xpKickerCheckOriginalBytes));

        // Patch the 5-byte `call` instruction to `xor al, al; nop; nop; nop` so this
        // specific XP-kicker check behaves as if the original function returned false.
        ModSDK::Memory::PatchMemory(
            tXpKickerCheck,
            kXpKickerCheckPatchedBytes,
            sizeof(kXpKickerCheckPatchedBytes)
        );

        return true;
    }

    bool OnInitialize() {
        return SetupPatch();
    }

    void OnShutdown() {
        if (tXpKickerCheck != nullptr) {
            ModSDK::Memory::PatchMemory(
                tXpKickerCheck,
                xpKickerCheckOriginalBytes,
                sizeof(xpKickerCheckOriginalBytes)
            );
        }
    }

    const CoHModSDKModuleV1 kModule = {
        .abiVersion = COHMODSDK_ABI_VERSION,
        .size = sizeof(CoHModSDKModuleV1),
        .modId = "de.tosox.persistentxpkickers",
        .name = "Persistent XP Kickers",
        .version = "1.5.0",
        .author = "Tosox",
        .OnInitialize = &OnInitialize,
        .OnShutdown = &OnShutdown
    };
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    DisableThreadLibraryCalls(hModule);
    return TRUE;
}

COHMODSDK_EXPORT_MODULE(kModule);
