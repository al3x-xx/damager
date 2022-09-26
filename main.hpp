#ifndef MAIN_HPP
#define MAIN_HPP

#include "lemon/detour.hpp"

#include "samp_funcs.hpp"
#include "rak_client.hpp"
#include "visuals.hpp"
#include "wnd_proc.hpp"

class main {
	std::unique_ptr<rakClient> pRakClient_;
	std::unique_ptr<visuals> pVisuals_;
	std::unique_ptr<wndProc> pWndProc_;
public:
	main();
	~main();
private:
	using oMain = void(__cdecl*)();
	lemon::detour<oMain> hookMainloop_{ 0x53BEE0 };

	void hookedMainloop(oMain orig);
};

#endif // MAIN_HPP