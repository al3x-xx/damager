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

	settings::i.reset();
	dmgFuncs::i.reset();
	sampFuncs::i.reset();
}

void main::hookedMainloop(oMain orig) {
	if (static bool state; !state) {
		if (sampFuncs::sampState()) {
			if (static auto sampFuncs{ std::make_unique<decltype(sampFuncs::i)::element_type>() }; sampFuncs->gameStateIsConnected()) {
				sampFuncs::i = std::move(sampFuncs);
				dmgFuncs::i = std::make_unique<decltype(dmgFuncs::i)::element_type>();
				settings::i = std::make_unique<decltype(settings::i)::element_type>();

				static const rakClient rakClient_;
				static const visuals visuals_;
				static const wndProc wndProc_;

				state = true;
			}
		}
	}
	else {
		if (settings::i->main.sMode && (settings::i->main.d3menu || settings::i->main.imguiMenu)) {
			static auto tgWeapon{ std::numeric_limits<std::uint8_t>::max() };
			if (const auto gun{ sampFuncs::i->getCurrentWeapon() }; tgWeapon != gun) {
				if (const auto isWeapon{ dmgFuncs::i->isWeapon(gun) }; isWeapon || dmgFuncs::i->isFist(gun)) {
					settings::i->main.isWeapon = isWeapon;
					settings::i->loadWeaponSettings(gun, true);
				}
				tgWeapon = gun;
			}
		}

		if (settings::i->main.d3menu) {
			if (static std::chrono::steady_clock::time_point t; dmgFuncs::i->checkActivate()) {
				if (const auto t2{ std::chrono::steady_clock::now() }; t2 >= t) {
					if (const auto dmgId{ dmgFuncs::i->getPlayerIdForDmg() }; dmgFuncs::i->remotePlayerIsExist(dmgId)) {
						settings::i->main.dmgId = dmgId;
						dmgFuncs::i->processDmg(dmgId);
						settings::i->main.dmgStr = std::format(
							"Damage to: {}[{}] | HP: {:.2f}",
							sampFuncs::i->getRemotePlayerName(dmgId), dmgId, sampFuncs::i->getRemotePlayerHealth(dmgId)
						);
						t = t2 + dmgFuncs::i->getDelay();
					}
					else {
						settings::i->main.dmgId = std::numeric_limits<decltype(settings::i->main.dmgId)>::max();
						settings::i->main.dmgStr.clear();
					}
				}
			}
			else {
				settings::i->main.dmgId = std::numeric_limits<decltype(settings::i->main.dmgId)>::max();
				settings::i->main.dmgStr.clear();
				t = {};
			}
		}

		if (settings::i->main.d3menuEdit) {
			dmgFuncs::i->toggleCursorState(true);
			if (POINT cursorPos; GetCursorPos(&cursorPos)) {
				auto& screenRes{ dmgFuncs::i->getScreenRes() };
				if (cursorPos.x + 166 < screenRes.x) settings::i->main.d3menuX = static_cast<float>(cursorPos.x);
				if (cursorPos.y + 166 < screenRes.y) settings::i->main.d3menuY = static_cast<float>(cursorPos.y);
			}
		}
	}
	return orig();
}