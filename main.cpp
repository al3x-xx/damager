#include "main.hpp"

#include "dmg_funcs.hpp"
#include "settings.hpp"

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

			pRakClient_ = std::make_unique<rakClient>();
			pVisuals_ = std::make_unique<visuals>();
			pWndProc_ = std::make_unique<wndProc>();

			init = true;
		}
	}
	else {
		if ((set::i().main.imguiMenu || set::i().main.d3menu) && set::i().main.sMode) {
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
			static auto time{ GetTickCount() };
			if (dmgFuncs::i().checkActivate()) {
				if (GetTickCount() > time) {					
					if (const auto dmgId{ dmgFuncs::i().getPlayerIdForDMG() }; dmgFuncs::i().remotePlayerIsExist(dmgId)) {
						set::i().main.gDMGId = dmgId;
						dmgFuncs::i().processDMG(dmgId);					
						set::i().main.gDMGStr = std::format(
							"Damage to: {}[{}] | HP: {:.2f}",
							sampFuncs::i().getRemotePlayerName(dmgId), dmgId, sampFuncs::i().getRemotePlayerHealth(dmgId)
						);
						time = GetTickCount() + dmgFuncs::i().getDelay();
					}
					else {
						set::i().main.gDMGId = std::numeric_limits<std::uint16_t>::max();
						set::i().main.gDMGStr.clear();
					}
				}
			}
			else {
				set::i().main.gDMGId = std::numeric_limits<std::uint16_t>::max();
				set::i().main.gDMGStr.clear();
				time = 0;
			}
		}

		if (set::i().main.d3menuEdit) {	
			dmgFuncs::i().toggleCursorState(true);
			POINT cursorPos;
			if (GetCursorPos(&cursorPos)) {				
				const auto screenRes{ dmgFuncs::i().getScreenRes() };
				if ((cursorPos.x + 166) < screenRes.x) set::i().main.d3menuX = static_cast<float>(cursorPos.x);
				if ((cursorPos.y + 166) < screenRes.y) set::i().main.d3menuY = static_cast<float>(cursorPos.y);
			}
		}
	}
	return orig();
}