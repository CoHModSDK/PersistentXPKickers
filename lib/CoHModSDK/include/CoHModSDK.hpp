#pragma once

#include <Windows.h>
#include <cstddef>
#include <cstdint>

extern "C" {
	// Called once when ModSDK loads the mod
	__declspec(dllexport) void OnSDKLoad();

	// Called when the game is starting (after mod load)
	__declspec(dllexport) void OnGameStart();

	// Called when the game is shutting down
	__declspec(dllexport) void OnGameShutdown();

	// Mod metadata
	__declspec(dllexport) const char* GetModName();
	__declspec(dllexport) const char* GetModVersion();
	__declspec(dllexport) const char* GetModAuthor();
}

namespace ModSDK {
	namespace Memory {
		std::uintptr_t FindPattern(const char* moduleName, const char* signature, bool reportError = true);
		void PatchMemory(void* destination, const void* source, std::size_t size);
	}

	namespace Hooks {
		bool CreateHook(void* targetFunction, void* detourFunction, void** originalFunction);
		bool EnableHook(void* targetFunction);
		bool EnableAllHooks();
		bool DisableHook(void* targetFunction);
		bool DisableAllHooks();
	}
}
