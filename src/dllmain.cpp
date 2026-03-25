#include "CoHModSDK.hpp"

namespace {
	constexpr const char* kXpKickerCheckPattern = "E8 ?? ?? ?? ?? 84 C0 75 18 8B 54 24 10";

    bool SetupPatch() {
        const auto tXpKickerCheckResult = ModSDK::Memory::FindPattern("WW2Mod.dll", kXpKickerCheckPattern);
        if (!tXpKickerCheckResult.has_value()) {
            ModSDK::Dialogs::ShowError("Failed to find XpKickerCheck address");
            return false;
        }

        // Patch the 5-byte `call` instruction to `xor al, al; nop; nop; nop` so this
        // specific XP-kicker check behaves as if the original function returned false.
        constexpr unsigned char patchBytes[] = { 0x32, 0xC0, 0x90, 0x90, 0x90 };
		const auto tXpKickerCheck = reinterpret_cast<void*>(tXpKickerCheckResult.value());
        ModSDK::Memory::PatchMemory(tXpKickerCheck, patchBytes, sizeof(patchBytes));

        return true;
    }

    bool OnInitialize() {
        return true;
    }

    bool OnModsLoaded() {
        return SetupPatch();
    }

    void OnShutdown() {}

    const CoHModSDKModuleV1 kModule = {
        .abiVersion = COHMODSDK_ABI_VERSION,
        .size = sizeof(CoHModSDKModuleV1),
        .modId = "de.tosox.persistentxpkickers",
        .name = "Persistent XP Kickers",
        .version = "1.4.0",
        .author = "Tosox",
        .OnInitialize = &OnInitialize,
        .OnModsLoaded = &OnModsLoaded,
        .OnShutdown = &OnShutdown
    };
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    DisableThreadLibraryCalls(hModule);
    return TRUE;
}

COHMODSDK_EXPORT_MODULE(kModule);
