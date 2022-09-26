#ifndef WND_PROC_HPP
#define WND_PROC_HPP

#include "lemon/detour.hpp"

class wndProc {
public:
	wndProc();
	~wndProc();
private:
	using oWndProc = LRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM);
	lemon::detour<oWndProc> hookWindowProc_{ 0x747EB0 };

	LRESULT hookedWindowProc(oWndProc orig, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif