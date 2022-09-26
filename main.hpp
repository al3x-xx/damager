#ifndef MAIN_HPP
#define MAIN_HPP

#include "lemon/detour.hpp"

class main {
public:
	main();
	~main();
private:
	using oMain = void(__cdecl*)();
	lemon::detour<oMain> hookMainloop_{ 0x53BEE0 };

	void hookedMainloop(oMain orig);
};

#endif