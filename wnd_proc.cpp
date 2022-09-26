#include "wnd_proc.hpp"

#include "dmg_funcs.hpp"
#include "settings.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

wndProc::wndProc() {
	hookWindowProc_.install(std::make_tuple(this, &wndProc::hookedWindowProc));
}

wndProc::~wndProc() {
	hookWindowProc_.remove();
}

LRESULT wndProc::hookedWindowProc(oWndProc orig, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (set::i().main.imguiMenu) ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	switch (uMsg) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
		if (wParam < 256) {
			dmgFuncs::i().setKeyState(wParam, true);
		}
	} break;
	case WM_KEYUP:
	case WM_SYSKEYUP: {
		if (wParam < 256) {
			dmgFuncs::i().setKeyState(wParam, false);
		}

		if (set::i().main.keyChange) {
			if (const auto keyName{ dmgFuncs::i().getKeyNameById(wParam) }; !keyName.empty()) {
				set::i().main.key = wParam;
				set::i().main.keyName = keyName;
				set::i().main.keyChange = false;
			}
		}
		else if (wParam == set::i().main.key && set::i().main.keyMode && !sampFuncs::i().getInput()->iInputEnabled) {
			set::i().main.keyPressed ^= true;
		}

		if (set::i().misc.menuKeyChange) {
			if (const auto keyName{ dmgFuncs::i().getKeyNameById(wParam) }; !keyName.empty()) {
				set::i().misc.menuKey = wParam;
				set::i().misc.menuKeyName = keyName;
				set::i().misc.menuKeyChange = false;
			}
		}
		else if (wParam == set::i().misc.menuKey) {
			set::i().main.imguiMenu ^= true;
			dmgFuncs::i().toggleCursorState(set::i().main.imguiMenu);
		}
	} break;
	case WM_LBUTTONUP: {
		if (set::i().main.d3menuEdit) {
			if (auto [res, j] { set::i().getJSON() }; res) {
				j["settings"]["default"]["pos"] = { set::i().main.d3menuX, set::i().main.d3menuY };
				set::i().saveJSON(j);
			}
			dmgFuncs::i().toggleCursorState(false);
			set::i().main.d3menuEdit = false;
		}
	} break;
	}
	return orig(hWnd, uMsg, wParam, lParam);
}