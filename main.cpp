#include "main.hpp"

#include "dmg_funcs.hpp"
#include "settings.hpp"
#include "rak_client.hpp"
#include "visuals.hpp"
#include "wnd_proc.hpp"

main::main() {
	hookMainloop_.install(std::make_tuple(this, &main::hookedMainloop));
}

main::~main() {
	hookMainloop_.remove();
}

void main::hookedMainloop(oMain orig) {
	if (static bool init; !init) {
		if (sampFuncs::sampInit() && sampFuncs::i().gameStateIsConnected()) {
			dmgFuncs::i();
			set::i();

			static const rakClient rakClient_;
			static const visuals visuals_;
			static const wndProc wndProc_;

			init = true;
		}
	}
	else {
		if (set::i().main.sMode && (set::i().main.d3menu || set::i().main.imguiMenu)) {
			static auto tgWeapon{ std::numeric_limits<std::uint8_t>::max() };
			if (const auto gun{ sampFuncs::i().getCurrentWeapon() }; tgWeapon != gun) {
				if (const auto isWeapon{ dmgFuncs::i().isWeapon(gun) }; isWeapon || dmgFuncs::i().isFist(gun)) {
					set::i().main.isWeapon = isWeapon;
					set::i().loadWeaponSettings(gun, true);
				}
				tgWeapon = gun;
			}
		}

		if (set::i().main.d3menu) {
			if (static std::chrono::steady_clock::time_point t; dmgFuncs::i().checkActivate()) {
				if (const auto t2{ std::chrono::steady_clock::now() }; t2 >= t) {
					if (const auto dmgId{ dmgFuncs::i().getPlayerIdForDmg() }; dmgFuncs::i().remotePlayerIsExist(dmgId)) {
						set::i().main.dmgId = dmgId;
						dmgFuncs::i().processDmg(dmgId);
						set::i().main.dmgStr = std::format(
							"Damage to: {}[{}] | HP: {:.2f}",
							sampFuncs::i().getRemotePlayerName(dmgId), dmgId, sampFuncs::i().getRemotePlayerHealth(dmgId)
						);
						t = t2 + dmgFuncs::i().getDelay();
					}
					else {
						set::i().main.dmgId = std::numeric_limits<decltype(set::i().main.dmgId)>::max();
						set::i().main.dmgStr.clear();
					}
				}
			}
			else {
				set::i().main.dmgId = std::numeric_limits<decltype(set::i().main.dmgId)>::max();
				set::i().main.dmgStr.clear();
				t = {};
			}
		}

		if (set::i().main.d3menuEdit) {
			dmgFuncs::i().toggleCursorState(true);
			if (POINT cursorPos; GetCursorPos(&cursorPos)) {
				auto& screenRes{ dmgFuncs::i().getScreenRes() };
				if (cursorPos.x + 166 < screenRes.x) set::i().main.d3menuX = static_cast<float>(cursorPos.x);
				if (cursorPos.y + 166 < screenRes.y) set::i().main.d3menuY = static_cast<float>(cursorPos.y);
			}
		}
	}
	return orig();
}