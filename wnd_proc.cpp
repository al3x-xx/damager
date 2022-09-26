#include "wnd_proc.hpp"

#include "settings.hpp"
#include "dmg_funcs.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

wndProc::wndProc() {
	hookWindowProc_.install(std::make_tuple(this, &wndProc::hookedWindowProc));
}

wndProc::~wndProc() {
	hookWindowProc_.remove();
}

LRESULT wndProc::hookedWindowProc(oWndProc orig, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (settings::i->main.imguiMenu) ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	switch (uMsg) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < miscFuncs::keyStateSize) dmgFuncs::i->setKeyState(wParam, true);
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < miscFuncs::keyStateSize) {
			dmgFuncs::i->setKeyState(wParam, false);

			if (settings::i->main.keyChange) {
				if (const auto keyName{ dmgFuncs::i->getKeyNameById(wParam) }; !keyName.empty()) {
					settings::i->main.key = wParam;
					settings::i->main.keyName = keyName;
					settings::i->main.keyChange = false;
				}
			}
			else if (wParam == settings::i->main.key && settings::i->main.keyMode && !sampFuncs::i->getInput()->iInputEnabled) settings::i->main.keyPressed ^= true;

			if (settings::i->misc.menuKeyChange) {
				if (const auto keyName{ dmgFuncs::i->getKeyNameById(wParam) }; !keyName.empty()) {
					settings::i->misc.menuKey = wParam;
					settings::i->misc.menuKeyName = keyName;
					settings::i->misc.menuKeyChange = false;
				}
			}
			else if (wParam == settings::i->misc.menuKey) {
				settings::i->main.imguiMenu ^= true;
				dmgFuncs::i->toggleCursorState(settings::i->main.imguiMenu);
			}
		}
		break;
	case WM_LBUTTONUP:
		if (settings::i->main.d3menuEdit) {
			if (auto [res, j] { settings::i->getJson() }; res) {
				j["settings"]["default"]["pos"] = { settings::i->main.d3menuX, settings::i->main.d3menuY };
				settings::i->saveJson(j);
			}
			dmgFuncs::i->toggleCursorState(false);
			settings::i->main.d3menuEdit = false;
		}
	}
	return orig(hWnd, uMsg, wParam, lParam);
}