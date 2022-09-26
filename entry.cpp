#include "main.hpp"

#include "samp_funcs.hpp"

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		if (const auto [_, ver] { sampFuncs::getSampBaseAndVer() }; ver == decltype(ver)::unk) return FALSE;
		static const main main_;
	}
	return TRUE;
}