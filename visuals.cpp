#include "visuals.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

#include "dmg_funcs.hpp"
#include "settings.hpp"

visuals::visuals() {
	const auto device{ getDevice() };

	hookReset_.set_addr(device[16]);
	hookReset_.install(std::make_tuple(this, &visuals::hookedReset));

	hookPresent_.set_addr(device[17]);
	hookPresent_.install(std::make_tuple(this, &visuals::hookedPresent));
}

visuals::~visuals() {
	imguiDestroy();

	hookReset_.remove();
	hookPresent_.remove();
}

HRESULT visuals::hookedReset(oReset orig, IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	d3Invalidate();

	auto hr{ orig(pDevice, pPresentationParameters) };
	if (SUCCEEDED(hr)) d3Initialize(pDevice);
	return hr;
}

HRESULT visuals::hookedPresent(oPresent orig, IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hdest, const RGNDATA* pDirtyRegion) {
	static std::once_flag of;
	std::call_once(of, [this, pDevice] {
		imguiCreate(pDevice);
		d3Initialize(pDevice);
	});
	imguiRender();
	d3Render();
	return orig(pDevice, pSourceRect, pDestRect, hdest, pDirtyRegion);
}

const std::uint32_t* visuals::getDevice() const noexcept {
	return *reinterpret_cast<std::uint32_t**>(*reinterpret_cast<void**>(0xC97C28));
}

const HWND visuals::getHwnd() const noexcept {
	return **reinterpret_cast<HWND**>(0xC17054);
}

void visuals::imguiCreate(IDirect3DDevice9* pDevice) const {
	ImGui::CreateContext();
	imguiStyle();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 13.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	ImGui::GetIO().IniFilename = nullptr;
	ImGui_ImplWin32_Init(getHwnd());
	ImGui_ImplDX9_Init(pDevice);
}

void visuals::imguiStyle() const {
	const auto colors{ ImGui::GetStyle().Colors };

	colors[ImGuiCol_::ImGuiCol_Text] = { 0.8f, 0.8f, 0.83f, 1.f };
	colors[ImGuiCol_::ImGuiCol_TextDisabled] = { 0.24f, 0.23f, 0.29f, 1.f };
	colors[ImGuiCol_::ImGuiCol_WindowBg] = { 0.f, 0.f, 0.f, 0.94f };
	colors[ImGuiCol_::ImGuiCol_ChildBg] = { 0.07f, 0.07f, 0.09f, 1.f };
	colors[ImGuiCol_::ImGuiCol_PopupBg] = { 0.f, 0.f, 0.f, 0.94f };
	colors[ImGuiCol_::ImGuiCol_Border] = { 0.06f, 0.05f, 0.07f, 1.f };
	colors[ImGuiCol_::ImGuiCol_BorderShadow] = { 0.06f, 0.05f, 0.07f, 1.f };
	colors[ImGuiCol_::ImGuiCol_FrameBg] = { 0.f, 0.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_FrameBgHovered] = { 0.01f, 0.01f, 0.01f, 1.f };
	colors[ImGuiCol_::ImGuiCol_FrameBgActive] = { 0.01f, 0.01f, 0.01f, 1.f };
	colors[ImGuiCol_::ImGuiCol_TitleBg] = { 0.f, 0.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_TitleBgCollapsed] = { 0.f, 0.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_TitleBgActive] = { 0.f, 0.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_MenuBarBg] = { 0.1f, 0.09f, 0.12f, 1.f };
	colors[ImGuiCol_::ImGuiCol_ScrollbarBg] = { 0.f, 0.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_ScrollbarGrab] = { 0.02f, 0.02f, 0.02f, 1.f };
	colors[ImGuiCol_::ImGuiCol_ScrollbarGrabHovered] = { 0.02f, 0.02f, 0.02f, 1.f };
	colors[ImGuiCol_::ImGuiCol_ScrollbarGrabActive] = { 0.02f, 0.02f, 0.02f, 1.f };
	colors[ImGuiCol_::ImGuiCol_CheckMark] = { 1.f, 0.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_SliderGrab] = { 0.02f, 0.02f, 0.02f, 1.f };
	colors[ImGuiCol_::ImGuiCol_SliderGrabActive] = { 0.03f, 0.03f, 0.03f, 1.f };
	colors[ImGuiCol_::ImGuiCol_Button] = { 0.f, 0.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_ButtonHovered] = { 0.01f, 0.01f, 0.01f, 1.f };
	colors[ImGuiCol_::ImGuiCol_ButtonActive] = { 0.f, 0.f, 0.f, 0.78f };
	colors[ImGuiCol_::ImGuiCol_Header] = { 0.01f, 0.01f, 0.01f, 1.f };
	colors[ImGuiCol_::ImGuiCol_HeaderHovered] = { 0.02f, 0.02f, 0.02f, 1.f };
	colors[ImGuiCol_::ImGuiCol_HeaderActive] = { 0.007f, 0.007f, 0.007f, 1.f };
	colors[ImGuiCol_::ImGuiCol_Separator] = { 0.f, 0.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_SeparatorHovered] = { 0.24f, 0.23f, 0.29f, 1.f };
	colors[ImGuiCol_::ImGuiCol_SeparatorActive] = { 0.56f, 0.56f, 0.58f, 1.f };
	colors[ImGuiCol_::ImGuiCol_ResizeGrip] = { 0.f, 0.f, 0.f, 0.f };
	colors[ImGuiCol_::ImGuiCol_ResizeGripHovered] = { 0.56f, 0.56f, 0.58f, 1.f };
	colors[ImGuiCol_::ImGuiCol_ResizeGripActive] = { 0.06f, 0.05f, 0.07f, 1.f };
	colors[ImGuiCol_::ImGuiCol_Tab] = { 0.4f, 0.39f, 0.38f, 0.16f };
	colors[ImGuiCol_::ImGuiCol_TabHovered] = { 0.4f, 0.39f, 0.38f, 0.39f };
	colors[ImGuiCol_::ImGuiCol_TabActive] = { 0.4f, 0.39f, 0.38f, 1.f };
	colors[ImGuiCol_::ImGuiCol_PlotLines] = { 0.4f, 0.39f, 0.38f, 0.63f };
	colors[ImGuiCol_::ImGuiCol_PlotLinesHovered] = { 0.25f, 1.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_PlotHistogram] = { 0.4f, 0.39f, 0.38f, 0.63f };
	colors[ImGuiCol_::ImGuiCol_PlotHistogramHovered] = { 0.25f, 1.f, 0.f, 1.f };
	colors[ImGuiCol_::ImGuiCol_TextSelectedBg] = { 1.f, 0.f, 0.f, 1.f };
}

void visuals::imguiRender() const {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (settings::i->main.imguiMenu) {
		static auto imguiMenuPosX{ 394.f };
		ImGui::SetNextWindowSize({ 330.f, imguiMenuPosX });

		static std::once_flag of;
		std::call_once(of, [] {
			auto& screenRes{ dmgFuncs::i->getScreenRes() };
			ImGui::SetNextWindowPos({ screenRes.x / 2.f, screenRes.y / 2.5f });
		});

		ImGui::Begin("Damager by al3x_ | vk.com/0x3078", &settings::i->main.imguiMenu, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings);
		
		dmgFuncs::i->toggleCursorState(settings::i->main.imguiMenu);

		if (settings::i->main.imguiMenu) {
			static std::uint8_t imguiButton;

			if (ImGui::Button("Settings", { 103.f, 20.f }); ImGui::IsItemClicked()) {
				imguiMenuPosX = 394.f;
				imguiButton = 0;
			}
			ImGui::SameLine(0.f, 2.f);
			if (ImGui::Button("Visuals", { 103.f, 20.f }); ImGui::IsItemClicked()) {
				imguiMenuPosX = 221.f;
				imguiButton = 1;
			}
			ImGui::SameLine(0.f, 2.f);
			if (ImGui::Button("Misc", { 103.f, 20.f }); ImGui::IsItemClicked()) {
				imguiMenuPosX = 178.f;
				imguiButton = 2;
			}
			ImGui::Separator();

			switch (imguiButton) {
			case 0:
				imguiMenuSettings();
				break;
			case 1:
				imguiMenuVisuals();
				break;
			case 2: imguiMenuMisc();
			}
		}

		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void visuals::imguiDestroy() const {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void visuals::imguiShowTooltip(const std::string& str) const {
	ImGui::BeginTooltip();
	ImGui::TextUnformatted(str.data());
	ImGui::EndTooltip();
}

void visuals::imguiMenuSettings() const {
	ImGui::PushItemWidth(85.f);
	if (ImGui::BeginCombo("##1", "      Packets", ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, { 0.007f, 0.007f, 0.007f, 1.f });
		ImGui::Checkbox("ID_PLAYER_SYNC, ID_VEHICLE_SYNC, ID_PASSENGER_SYNC", &settings::i->main.packetVehPlayerPass);
		ImGui::Checkbox("ID_AIM_SYNC", &settings::i->main.packetAim);
		if (settings::i->main.packetAim) {
			ImGui::SameLine(0.f, 47.f);
			ImGui::Checkbox("AnimOnPassenger", &settings::i->main.packetAimAnimVeh);
		}
		ImGui::Checkbox("ID_BULLET_SYNC", &settings::i->main.packetBullet);
		ImGui::Checkbox("ID_WEAPONS_UPDATE", &settings::i->main.packetWeapon);
		ImGui::Checkbox("RPC_GiveTakeDamage", &settings::i->main.packetRpcdmg);
		if (settings::i->main.packetRpcdmg) {
			ImGui::SameLine();
			if (ImGui::Checkbox("DamageFromFile", &settings::i->main.packetRpcdmgFromFile)) {
				if (settings::i->main.packetRpcdmgFromFile) {
					const auto [res, j] { settings::i->getJson("/settings/weapons/" + std::to_string(settings::i->main.gun) + "/" + (settings::i->main.settingsForServer ? sampFuncs::i->getServerIp() : "default") + "/dmg") };
					settings::i->main.packetRpcGetDmg = !res || j.empty() ? dmgFuncs::i->getWeaponDamage(settings::i->main.gun) : j.get<float>();
				}
				else settings::i->main.packetRpcGetDmg = dmgFuncs::i->getWeaponDamage(settings::i->main.gun);
			}
		}
		ImGui::PopStyleColor();
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::TextUnformatted("Packets");

	if (ImGui::Button(settings::i->main.keyChange ? "Press any key" : settings::i->main.keyName.data(), { 85.f, 20.f }); ImGui::IsItemClicked()) {
		settings::i->main.keyPressed = false;
		settings::i->main.keyChange = true;
	}
	ImGui::SameLine();
	ImGui::Checkbox("OnPressed", &settings::i->main.keyMode);
	if (ImGui::Button(settings::i->main.check ? "Status" : "Health", { 85.f, 20.f }); ImGui::IsItemClicked()) settings::i->main.check ^= true;
	ImGui::SameLine();
	ImGui::Checkbox("DamageAFK", &settings::i->main.checkAfk);

	ImGui::PushItemWidth(85.f);
	if (ImGui::BeginCombo("##2", settings::i->main.imguiModeItem, ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		for (const std::uint8_t i : std::ranges::views::iota(0, 5)) {
			const auto modeStr{ dmgFuncs::i->getModeStr(i) };
			const auto selected{ settings::i->main.imguiModeItem == modeStr };
			if (ImGui::Selectable(modeStr, selected)) {
				settings::i->main.imguiModeItem = modeStr;
				settings::i->main.mode = i;
			}
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(85.f);
	if (ImGui::BeginCombo("##3", settings::i->main.modeOnv ? "OnVisibility: ON" : "OnVisibility: OFF", ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, { 0.007f, 0.007f, 0.007f, 1.f });
		ImGui::Checkbox("OnVisibility", &settings::i->main.modeOnv);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Separator, { 0.02f, 0.02f, 0.02f, 1.f });
		ImGui::Separator();
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, { 0.007f, 0.007f, 0.007f, 1.f });
		ImGui::Checkbox("bCheckBuildings", &settings::i->main.modeBCheckBuildings);
		ImGui::Checkbox("bCheckVehicles", &settings::i->main.modeBCheckVehicles);
		ImGui::Checkbox("bCheckObjects", &settings::i->main.modeBCheckObjects);
		ImGui::PopStyleColor();
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	if (settings::i->main.mode == 4) {
		ImGui::SameLine();
		ImGui::PushItemWidth(84.f);
		ImGui::InputInt("##4", &settings::i->main.fov);
		ImGui::PopItemWidth();
	}
	ImGui::SameLine();
	ImGui::TextUnformatted("Mode");

	ImGui::PushItemWidth(85.f);
	if (static bool tg; ImGui::BeginCombo("##5", settings::i->main.fracStr.data(), ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		static std::vector<std::string> vecFrac;
		static std::vector<std::uint8_t> vecFracBool;
		if (!tg) {
			vecFrac.clear();
			vecFracBool.clear();
			settings::i->main.fracName.clear();
			settings::i->main.skin.clear();
			settings::i->main.frac = 0;
			settings::i->main.fracStr = "None";
			if (const auto [res, j] { settings::i->getJson("/settings/frac") }; res) {
				for (auto& buf : j.get_ref<const nlohmann::json::object_t&>()) {
					if (!buf.second.empty()) {
						vecFrac.push_back(const_cast<decltype(vecFrac)::value_type&&>(buf.first));
						vecFracBool.push_back(0);
					}
				}
			}
			tg = true;
		}

		if (settings::i->main.frac >= 0 && settings::i->main.frac <= 2) {
			static const char* const fracStr[]{ "None", "All", "Frac" };
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.007f, 0.007f, 0.007f, 1.f });
			ImGui::Button(fracStr[settings::i->main.frac], { 85.f, 18.f });
			if (ImGui::IsItemClicked()) {
				if (settings::i->main.frac == 0 || settings::i->main.frac == 1) {
					settings::i->main.frac ^= 1;
					settings::i->main.fracStr = settings::i->main.frac ? "All" : "None";
				}
				else {
					settings::i->main.frac = 0;
					settings::i->main.fracName.clear();
					settings::i->main.skin.clear();
					std::ranges::fill(vecFracBool, 0);
					settings::i->main.fracStr = "None";
				}
			}
			ImGui::PopStyleColor();
		}

		if (!vecFrac.empty()) {
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Separator, { 0.02f, 0.02f, 0.02f, 1.f });
			ImGui::Separator();
			ImGui::PopStyleColor();
		}
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, { 0.007f, 0.007f, 0.007f, 1.f });
		for (const auto [i, buf] : vecFrac | std::ranges::views::enumerate) {
			if (ImGui::Checkbox(buf.data(), reinterpret_cast<bool*>(&vecFracBool[i]))) {
				if (vecFracBool[i]) {
					if (const auto [res, j] { settings::i->getJson("/settings/frac/" + buf) }; res && !j.empty()) {
						settings::i->main.skin.append_range(j);
						settings::i->main.fracName.push_back(buf);
						settings::i->main.frac = 2;
						settings::i->main.fracStr = dmgFuncs::i->dobavitZapyatuyu(settings::i->main.fracName);
					}
				}
				else {
					if (const auto res{ std::ranges::find(settings::i->main.fracName, buf) }; res != settings::i->main.fracName.end()) {
						settings::i->main.fracName.erase(res);
						if (const auto [res2, j] { settings::i->getJson("/settings/frac/" + buf) }; res2 && !j.empty()) {
							for (const auto& buf : j) {
								dmgFuncs::i->removeValue(settings::i->main.skin, buf);
							}
						}
					}
					settings::i->main.frac = dmgFuncs::i->findValue(vecFracBool, 1u) ? 2 : 0;
					settings::i->main.fracStr = settings::i->main.frac ? dmgFuncs::i->dobavitZapyatuyu(settings::i->main.fracName) : "None";
				}
			}
		}
		ImGui::PopStyleColor();
		ImGui::EndCombo();
	}
	else tg = false;
	ImGui::PopItemWidth();
	if (ImGui::IsItemHovered()) imguiShowTooltip(settings::i->main.fracStr);
	ImGui::SameLine();
	ImGui::Checkbox("AntiFrac", &settings::i->main.af);

	ImGui::Checkbox("AutoSettings", &settings::i->main.sMode);
	ImGui::SameLine(0.f, 10.f);
	if (ImGui::Checkbox("SettingsForServer", &settings::i->main.settingsForServer)) {
		if (const auto isWeapon{ dmgFuncs::i->isWeapon(settings::i->main.gun) }; isWeapon || dmgFuncs::i->isFist(settings::i->main.gun)) {
			settings::i->main.isWeapon = isWeapon;
			settings::i->loadWeaponSettings(settings::i->main.gun, settings::i->main.sMode);
		}
	}
	ImGui::SameLine(0.f, 10.f);
	ImGui::Checkbox("ReduceAmmo", &settings::i->main.reduceAmmo);
	ImGui::Separator();

	ImGui::PushItemWidth(85.f);
	if (static const char* const weaponName[]{
		"Fist", "Brass Knuckles", "Golf Club", "Nitestick", "Knife", "Baseball Bat", "Shovel", "Pool Cue", "Katana", "Chainsaw", "Dildo 1", "Dildo 2", "Vibe 1", "Vibe 2", "Flowers", "Cane", "Molotov Cocktail",
		"Pistol", "Silenced Pistol", "Desert Eagle", "Shotgun", "Sawn-Off Shotgun", "SPAZ12", "Micro UZI", "MP5", "AK47", "M4", "Tech9", "Country Rifle", "Sniper Rifle",
		"Flame Thrower",
		"Minigun",
		"Spray Can", "Fire Extinguisher", "Parachute"
	}; ImGui::BeginCombo("##6", settings::i->main.imguiWeaponItem.data(), ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		for (const auto buf : weaponName) {
			const auto selected{ settings::i->main.imguiWeaponItem == buf };
			if (ImGui::Selectable(buf, selected)) {
				const auto gun{ dmgFuncs::i->getWeaponId(buf) };
				settings::i->main.isWeapon = dmgFuncs::i->isWeapon(gun);
				settings::i->main.sMode = false;
				settings::i->loadWeaponSettings(gun, false);
			}
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (settings::i->main.packetRpcdmg && settings::i->main.packetRpcdmgFromFile) {
		ImGui::PushItemWidth(105.f);
		ImGui::InputFloat("Weapon / Damage", &settings::i->main.packetRpcGetDmg, 1.f, 1.f, "%.6f");
		ImGui::PopItemWidth();
	}
	else ImGui::TextUnformatted("Weapon");
	
	ImGui::PushItemWidth(85.f);
	ImGui::InputInt("##7", &settings::i->main.delay);
	ImGui::PopItemWidth();
	if (settings::i->main.delayMode) {
		ImGui::SameLine();
		ImGui::PushItemWidth(87.f);
		ImGui::InputInt("##8", &settings::i->main.delay2);
		ImGui::PopItemWidth();
	}
	ImGui::SameLine();
	ImGui::Checkbox("DelayMode", &settings::i->main.delayMode);
	ImGui::PushItemWidth(85.f);
	ImGui::InputInt("##9", &settings::i->main.radius, 1, 1);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::TextUnformatted("Radius");

	ImGui::PushItemWidth(85.f);
	ImGui::InputInt("##10", &settings::i->main.smiss);
	ImGui::PopItemWidth();
	if (settings::i->main.missMode) {
		ImGui::SameLine();
		ImGui::PushItemWidth(85.f);
		ImGui::InputInt("##11", &settings::i->main.smiss2);
		ImGui::PopItemWidth();
	}
	ImGui::SameLine();
	ImGui::Checkbox("MissMode", &settings::i->main.missMode);
	ImGui::SameLine();
	ImGui::Checkbox("Miss", &settings::i->main.miss);

	ImGui::PushItemWidth(85.f);
	ImGui::InputInt("##12", &settings::i->main.snokill, 1, 1);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::Checkbox("NoKill", &settings::i->main.nokill);
	ImGui::Separator();

	ImGui::PushItemWidth(85.f);
	static int localId;
	ImGui::InputInt("##13", &localId, 1, 1);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Add", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		if (dmgFuncs::i->checkIdLimit(localId) && dmgFuncs::i->remotePlayerIsListed(localId)) {
			if (auto& name{ sampFuncs::i->getRemotePlayerName(localId) }; !dmgFuncs::i->findValue(settings::i->main.ignoreList, name)) {
				settings::i->main.ignoreList.push_back(name);
				settings::i->main.ignoreListStr = dmgFuncs::i->dobavitZapyatuyuIindex(settings::i->main.ignoreList);
			}
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Remove", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		if (const auto id{ localId - 1 }; id >= 0 && static_cast<decltype(settings::i->main.ignoreList.size())>(id) < settings::i->main.ignoreList.size()) {
			settings::i->main.ignoreList.erase(settings::i->main.ignoreList.begin() + id);
			settings::i->main.ignoreListStr = dmgFuncs::i->dobavitZapyatuyuIindex(settings::i->main.ignoreList);
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Clear", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		settings::i->main.ignoreList.clear();
		settings::i->main.ignoreListStr.clear();
	}
	ImGui::SameLine();
	if (ImGui::TextUnformatted("IgnoreList"); ImGui::IsItemHovered() && !settings::i->main.ignoreListStr.empty()) imguiShowTooltip(settings::i->main.ignoreListStr);

	ImGui::PushItemWidth(85.f);
	static int localId2;
	ImGui::InputInt("##14", &localId2, 1, 1);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Add", { 45.f, 20.f }); ImGui::IsItemClicked() && dmgFuncs::i->checkIdLimit(localId2) && dmgFuncs::i->remotePlayerIsListed(localId2)) {
		if (auto& name{ sampFuncs::i->getRemotePlayerName(localId2) }; !dmgFuncs::i->findValue(settings::i->main.blackList, name)) {
			settings::i->main.blackList.push_back(name);
			settings::i->main.blackListStr = dmgFuncs::i->dobavitZapyatuyuIindex(settings::i->main.blackList);
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Remove", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		if (const auto id{ localId2 - 1 }; id >= 0 && static_cast<decltype(settings::i->main.blackList.size())>(id) < settings::i->main.blackList.size()) {
			settings::i->main.blackList.erase(settings::i->main.blackList.begin() + id);
			settings::i->main.blackListStr = dmgFuncs::i->dobavitZapyatuyuIindex(settings::i->main.blackList);
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Clear", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		settings::i->main.blackList.clear();
		settings::i->main.blackListStr.clear();
	}
	ImGui::SameLine();
	if (ImGui::TextUnformatted("BlackList"); ImGui::IsItemHovered() && !settings::i->main.blackListStr.empty()) imguiShowTooltip(settings::i->main.blackListStr);

	if (ImGui::Button("Save settings", { 313.f, 20.f }); ImGui::IsItemClicked()) settings::i->saveMenuSettings();
}

void visuals::imguiMenuVisuals() const {
	ImGui::Checkbox("ShowFOV", &settings::i->vis.showFov);
	ImGui::Checkbox("RenderLineToDamagePlayer", &settings::i->vis.renderLine);
	if (ImGui::Checkbox("Wallhack", &settings::i->vis.wallHack)) sampFuncs::i->disableNameTags(settings::i->vis.wallHack);
	ImGui::Checkbox("ShowSkinID", &settings::i->vis.showSkinId);
	ImGui::Checkbox("ShowIgnoreList", &settings::i->vis.showIgnoreList);
	ImGui::Checkbox("ShowBlackList", &settings::i->vis.showBlackList);

	if (ImGui::Button("Save settings", { 313.f, 20.f }); ImGui::IsItemClicked()) settings::i->saveMenuVisuals();
}

void visuals::imguiMenuMisc() const {
	if (ImGui::Button(settings::i->misc.menuKeyChange ? "Press any key" : settings::i->misc.menuKeyName.data(), { 85.f, 20.f }); ImGui::IsItemClicked()) settings::i->misc.menuKeyChange = true;
	ImGui::SameLine();
	ImGui::TextUnformatted("MenuActivationKey");

	ImGui::PushItemWidth(85.f);
	static char bufFrac[50];
	ImGui::InputText("##15", bufFrac, sizeof(bufFrac));
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Add Frac", { 92.f, 20.f }); ImGui::IsItemClicked() && bufFrac[0]) {
		if (auto [res, j] { settings::i->getJson() }; res) {
			j["settings"]["frac"][bufFrac] = nlohmann::json::array_t{};
			settings::i->saveJson(j);
		}
	}
	ImGui::SameLine(0.f, 2.f);
	static std::string_view fracList{ "None" };
	if (ImGui::Button("Remove Frac", { 92.f, 20.f }); ImGui::IsItemClicked() && bufFrac[0]) {
		if (auto [res, j] { settings::i->getJson() }; res) {
			j["settings"]["frac"].erase(bufFrac);
			if (fracList == bufFrac) fracList = "None";
			settings::i->saveJson(j);
		}
	}

	ImGui::PushItemWidth(85.f);
	if (static bool tg; ImGui::BeginCombo("##16", fracList.data(), ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		static std::vector<std::string> frac;
		if (!tg) {
			fracList = "None";
			frac.clear();
			if (const auto [res, j] { settings::i->getJson("/settings/frac") }; res) {
				if (auto& buf{ j.get_ref<const nlohmann::json::object_t&>() }; !buf.empty()) {
					for (frac.reserve(buf.size()); auto& buf : buf) {
						frac.push_back(const_cast<decltype(frac)::value_type&&>(buf.first));
					}
				}
			}
			tg = true;
		}
		for (const auto& buf : frac) {
			const auto selected{ fracList == buf };
			if (ImGui::Selectable(buf.data(), selected)) fracList = buf;
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	else tg = false;
	ImGui::PopItemWidth();

	if (static bool tg; ImGui::IsItemHovered()) {
		if (fracList != "None") {
			static std::string skinIdStr;
			if (!tg) {
				skinIdStr.clear();
				if (!fracList.empty()) {
					if (const auto [res, j] { settings::i->getJson("/settings/frac") }; res) {
						if (const auto& vb{ j[fracList] }; !vb.empty()) {
							skinIdStr = dmgFuncs::i->dobavitZapyatuyu(vb.get<std::vector<int>>());
						}
					}
				}
				tg = true;
			}
			if (!skinIdStr.empty()) imguiShowTooltip(skinIdStr);
		}
	}
	else tg = false;
	ImGui::SameLine();
	ImGui::PushItemWidth(45.f);
	static int skinId;
	ImGui::InputInt("##17", &skinId, 0);
	ImGui::PopItemWidth();
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Add", { 45.f, 20.f }); ImGui::IsItemClicked() && !fracList.empty() && fracList != "None") {
		if (auto [res, j] { settings::i->getJson() }; res) {
			if (auto& vb{ j["settings"]["frac"][fracList] }; !dmgFuncs::i->findValue(vb, skinId)) {
				vb.push_back(skinId);
				settings::i->saveJson(j);
			}
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Remove", { 45.f, 20.f }); ImGui::IsItemClicked() && !fracList.empty() && fracList != "None") {
		if (auto [res, j] { settings::i->getJson() }; res) {
			if (auto& vb{ j["settings"]["frac"][fracList] }; !vb.empty()) {
				dmgFuncs::i->removeValue(vb, skinId);
				settings::i->saveJson(j);
			}
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Clear", { 45.f, 20.f }); ImGui::IsItemClicked() && !fracList.empty() && fracList != "None") {
		if (auto [res, j] { settings::i->getJson() }; res) {
			j["settings"]["frac"][fracList].clear();
			settings::i->saveJson(j);
		}
	}
	ImGui::PushItemWidth(85.f);
	ImGui::SliderInt("##18", &settings::i->misc.invisz, 0, 10);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::Checkbox("UndergroundInvisible", &settings::i->misc.invis);

	if (ImGui::Button("Save settings", { 313.f, 20.f }); ImGui::IsItemClicked()) settings::i->saveMenuMisc();
}

void visuals::d3Initialize(IDirect3DDevice9* pDevice) const {
	fontNick_->Initialize(pDevice);
	fontNickBar_->Initialize(pDevice);
	render_->Initialize(pDevice);
	box_->Initialize(pDevice);
}

void visuals::d3Invalidate() const {
	fontNick_->Invalidate();
	fontNickBar_->Invalidate();
	render_->Invalidate();
	box_->Invalidate();
}

void visuals::d3Render() const {
	if (settings::i->main.d3menu) {
		d3Menu();
		if (settings::i->vis.showFov && settings::i->main.mode == 4) fov();
		if (settings::i->vis.renderLine) line();
	}
	if (settings::i->vis.wallHack) wallhack();
}

void visuals::d3Menu() const {
	render_->Print(std::format(
		"[Damager by al3x_ ]\n"
		"Packets: [{},{},{},{},{}]\n"
		"Key: {} [{}]{}\n"
		"Check: {} [{}]\n"
		"Mode: {} [{}]{}\n"
		"{}Frac: {}\n"
		"Weapon: {} [{}]\n"
		"Delay: {}ms{}\n"
		"Radius: {}m\n"
		"Miss: {}pt{} [{}]\n"
		"NoKill: {}hp [{}]\n"
		"{}\n"
		"{}\n"
		"{}",
		settings::i->main.packetVehPlayerPass ? "+" : "-", settings::i->main.packetAim ? "+" : "-", settings::i->main.packetBullet ? "+" : "-", settings::i->main.packetWeapon ? "+" : "-", settings::i->main.packetRpcdmg ? "+" : "-",
		settings::i->main.keyName, settings::i->main.keyMode ? "Pressed" : "Down", settings::i->main.keyMode ? (settings::i->main.keyPressed ? "[True]" : "[False]") : "",
		settings::i->main.check ? "Status" : "Health", settings::i->main.checkAfk ? "True" : "False",
		dmgFuncs::i->getModeStr(settings::i->main.mode), settings::i->main.modeOnv ? "True" : "False", settings::i->main.mode == 4 ? "[" + std::to_string(settings::i->main.fov) + "]" : "",
		settings::i->main.af ? "Anti" : "", settings::i->main.fracStr,
		settings::i->main.imguiWeaponItem, settings::i->main.gun,
		settings::i->main.delay, settings::i->main.delayMode ? "-" + std::to_string(settings::i->main.delay2) + "ms" : "",
		settings::i->main.radius,
		settings::i->main.smiss, settings::i->main.missMode ? "-" + std::to_string(settings::i->main.smiss2) + "pt" : "", settings::i->main.miss ? "True" : "False",
		settings::i->main.snokill, settings::i->main.nokill ? "True" : "False",
		settings::i->vis.showIgnoreList && !settings::i->main.ignoreListStr.empty() ? "IG: " + settings::i->main.ignoreListStr : "",
		settings::i->vis.showBlackList && !settings::i->main.blackListStr.empty() ? "BL: " + settings::i->main.blackListStr : "",
		dmgFuncs::i->checkActivate() ? settings::i->main.dmgStr : ""
	).data(), -1, settings::i->main.d3menuX, settings::i->main.d3menuY);
}

void visuals::fov() const {
	auto& screenRes{ dmgFuncs::i->getScreenRes() };
	box_->DrawPolygon(screenRes.x * 0.53f, screenRes.y * 0.4f, 60, settings::i->main.fov / 1.05f, D3DCOLOR_ARGB(75, 0, 0, 0));
}

void visuals::line() const {
	if (dmgFuncs::i->checkActivate() && dmgFuncs::i->remotePlayerIsExist(settings::i->main.dmgId)) {
		if (const auto myPos{ dmgFuncs::i->getLocalPlayerPos() }, remotePlayerPos{ dmgFuncs::i->getRemotePlayerPos(settings::i->main.dmgId) }, screenMyPos{ dmgFuncs::i->calcScreenCoords(myPos) }, screenPlayerPos{ dmgFuncs::i->calcScreenCoords(remotePlayerPos) }; screenMyPos.z >= 1.f && screenPlayerPos.z >= 1.f) {
			box_->D3DLine(screenMyPos.x, screenMyPos.y, screenPlayerPos.x, screenPlayerPos.y, D3DCOLOR_XRGB(255, 0, 0));
		}
	}
}

void visuals::wallhack() const {
	for (const std::uint16_t i : std::ranges::views::iota(0, sampFuncs::playerLimit)) {
		if (!dmgFuncs::i->remotePlayerIsExist(i, false)) continue;
		const auto remoteBonePos{ dmgFuncs::i->getRemotePlayerBonePos(i, 6) };
		auto screenPos{ dmgFuncs::i->calcScreenCoords(remoteBonePos) };
		if (screenPos.z < 1.f) continue;

		screenPos.x -= 30;
		screenPos.y -= 40;

		auto& remoteName{ sampFuncs::i->getRemotePlayerName(i) };
		const auto remoteAfkState{ sampFuncs::i->getRemotePlayerAfkState(i) };
		const auto remoteColor{ sampFuncs::i->getRemotePlayerNameColor(i) };

		const auto nameStr{ remoteName + "{FFFFFF}[" + std::to_string(i) + "]" + (remoteAfkState ? "[AFK]" : "") };

		fontNick_->Print(nameStr.data(), dmgFuncs::i->calcNameColor(remoteColor), screenPos.x, screenPos.y, false);

		auto remoteHealth{ sampFuncs::i->getRemotePlayerHealth(i) };
		auto remoteArmour{ sampFuncs::i->getRemotePlayerArmour(i) };

		const auto remoteHealth2{ static_cast<int>(remoteHealth) };
		const auto remoteArmour2{ static_cast<int>(remoteArmour) };

		if (remoteHealth > 100.f) remoteHealth = 100.f;
		if (remoteArmour > 100.f) remoteArmour = 100.f;

		box_->D3DBoxBorder(screenPos.x, screenPos.y + (remoteArmour > 0.f ? 19.f : 13.f), 42.f, 5.f, D3DCOLOR_ARGB(255, 0, 0, 0), D3DCOLOR_ARGB(100, 0, 0, 0));
		box_->D3DBox(screenPos.x + 1.f, screenPos.y + (remoteArmour > 0.f ? 20.f : 14.f), remoteHealth / 2.5f, 3.f, D3DCOLOR_ARGB(222, 227, 0, 5));
		fontNickBar_->Print(std::to_string(remoteHealth2).data(), D3DCOLOR_ARGB(255, 255, 255, 255), screenPos.x + 41.5f, screenPos.y + (remoteArmour > 0.f ? 18.f : 10.f));

		if (remoteArmour > 0.f) {
			box_->D3DBoxBorder(screenPos.x, screenPos.y + 13.f, 42.f, 5.f, D3DCOLOR_ARGB(255, 0, 0, 0), D3DCOLOR_ARGB(100, 0, 0, 0));
			box_->D3DBox(screenPos.x + 1.f, screenPos.y + 14.f, remoteArmour / 2.5f, 3.f, D3DCOLOR_ARGB(255, 255, 255, 255));
			fontNickBar_->Print(std::to_string(remoteArmour2).data(), D3DCOLOR_ARGB(255, 255, 255, 255), screenPos.x + 41.5f, screenPos.y + 10.f);
		}

		sampFuncs::i->updateScoreboardData();

		const auto remoteScore{ sampFuncs::i->getRemotePlayerScore(i) };
		const auto remotePing{ sampFuncs::i->getRemotePlayerPing(i) };
		const auto remoteSkin{ sampFuncs::i->getRemotePlayerSampPed(i)->pGTA_Ped->base.model_alt_id };

		const auto infoStr{ "Lvl " + std::to_string(remoteScore) + " / Ping " + std::to_string(remotePing) + (settings::i->vis.showSkinId ? (" / Skin " + std::to_string(remoteSkin)) : "") };
		
		fontNick_->Print(infoStr.data(), D3DCOLOR_XRGB(255, 255, 255), screenPos.x, screenPos.y + (remoteArmour > 0.f ? 26.f : 17.f));
	}
}