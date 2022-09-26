#include "main.hpp"

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		const auto [_, ver] { sampFuncs::getSampBaseAndVer() };
		if (ver == sampFuncs::sampVer::unk) return FALSE;
		static const auto pMain{ std::make_unique<main>() };
	}
	return TRUE;
}