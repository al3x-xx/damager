#include "visuals.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

#include "dmg_funcs.hpp"
#include "settings.hpp"

visuals::visuals() noexcept {
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
	std::call_once(of, [this, pDevice]() noexcept {
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

void visuals::imguiRender() const noexcept {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (set::i().main.imguiMenu) {
		static auto imguiMenuPosX{ 394.f };
		ImGui::SetNextWindowSize({ 330.f, imguiMenuPosX });

		static std::once_flag of;
		std::call_once(of, [] {
			auto& screenRes{ dmgFuncs::i().getScreenRes() };
			ImGui::SetNextWindowPos({ screenRes.x / 2.f, screenRes.y / 2.5f });
		});

		ImGui::Begin("Damager by al3x_ | vk.com/0x3078", &set::i().main.imguiMenu, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings);
		
		dmgFuncs::i().toggleCursorState(set::i().main.imguiMenu);

		if (set::i().main.imguiMenu) {
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

void visuals::imguiMenuSettings() const noexcept {
	ImGui::PushItemWidth(85.f);
	if (ImGui::BeginCombo("##1", "      Packets", ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, { 0.007f, 0.007f, 0.007f, 1.f });
		ImGui::Checkbox("ID_PLAYER_SYNC, ID_VEHICLE_SYNC, ID_PASSENGER_SYNC", &set::i().main.packetVehPlayerPass);
		ImGui::Checkbox("ID_AIM_SYNC", &set::i().main.packetAim);
		if (set::i().main.packetAim) {
			ImGui::SameLine(0.f, 47.f);
			ImGui::Checkbox("AnimOnPassenger", &set::i().main.packetAimAnimVeh);
		}
		ImGui::Checkbox("ID_BULLET_SYNC", &set::i().main.packetBullet);
		ImGui::Checkbox("ID_WEAPONS_UPDATE", &set::i().main.packetWeapon);
		ImGui::Checkbox("RPC_GiveTakeDamage", &set::i().main.packetRpcdmg);
		if (set::i().main.packetRpcdmg) {
			ImGui::SameLine();
			if (ImGui::Checkbox("DamageFromFile", &set::i().main.packetRpcdmgFromFile)) {
				if (set::i().main.packetRpcdmgFromFile) {
					const auto [res, j] { set::i().getJson("/settings/weapons/" + std::to_string(set::i().main.gun) + "/" + (set::i().main.settingsForServer ? sampFuncs::i().getServerIp() : "default") + "/dmg") };
					set::i().main.packetRpcGetDmg = !res || j.empty() ? dmgFuncs::i().getWeaponDamage(set::i().main.gun) : j.get<float>();
				}
				else set::i().main.packetRpcGetDmg = dmgFuncs::i().getWeaponDamage(set::i().main.gun);
			}
		}
		ImGui::PopStyleColor();
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::TextUnformatted("Packets");

	if (ImGui::Button(set::i().main.keyChange ? "Press any key" : set::i().main.keyName.data(), { 85.f, 20.f }); ImGui::IsItemClicked()) {
		set::i().main.keyPressed = false;
		set::i().main.keyChange = true;
	}
	ImGui::SameLine();
	ImGui::Checkbox("OnPressed", &set::i().main.keyMode);
	if (ImGui::Button(set::i().main.check ? "Status" : "Health", { 85.f, 20.f }); ImGui::IsItemClicked()) set::i().main.check ^= true;
	ImGui::SameLine();
	ImGui::Checkbox("DamageAFK", &set::i().main.checkAfk);

	ImGui::PushItemWidth(85.f);
	if (ImGui::BeginCombo("##2", set::i().main.imguiModeItem, ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		for (const std::uint8_t i : std::ranges::views::iota(0, 5)) {
			const auto modeStr{ dmgFuncs::i().getModeStr(i) };
			const auto selected{ set::i().main.imguiModeItem == modeStr };
			if (ImGui::Selectable(modeStr, selected)) {
				set::i().main.imguiModeItem = modeStr;
				set::i().main.mode = i;
			}
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(85.f);
	if (ImGui::BeginCombo("##3", set::i().main.modeOnv ? "OnVisibility: ON" : "OnVisibility: OFF", ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, { 0.007f, 0.007f, 0.007f, 1.f });
		ImGui::Checkbox("OnVisibility", &set::i().main.modeOnv);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Separator, { 0.02f, 0.02f, 0.02f, 1.f });
		ImGui::Separator();
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, { 0.007f, 0.007f, 0.007f, 1.f });
		ImGui::Checkbox("bCheckBuildings", &set::i().main.modeBCheckBuildings);
		ImGui::Checkbox("bCheckVehicles", &set::i().main.modeBCheckVehicles);
		ImGui::Checkbox("bCheckObjects", &set::i().main.modeBCheckObjects);
		ImGui::PopStyleColor();
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	if (set::i().main.mode == 4) {
		ImGui::SameLine();
		ImGui::PushItemWidth(84.f);
		ImGui::InputInt("##4", &set::i().main.fov);
		ImGui::PopItemWidth();
	}
	ImGui::SameLine();
	ImGui::TextUnformatted("Mode");

	ImGui::PushItemWidth(85.f);
	if (static bool tg; ImGui::BeginCombo("##5", set::i().main.fracStr.data(), ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		static std::vector<std::string> vecFrac;
		static std::vector<std::uint8_t> vecFracBool;
		if (!tg) {
			vecFrac.clear();
			vecFracBool.clear();
			set::i().main.fracName.clear();
			set::i().main.skin.clear();
			set::i().main.frac = 0;
			set::i().main.fracStr = "None";
			if (const auto [res, j] { set::i().getJson("/settings/frac") }; res) {
				for (auto& buf : j.get_ref<const nlohmann::json::object_t&>()) {
					if (!buf.second.empty()) {
						vecFrac.push_back(const_cast<decltype(vecFrac)::value_type&&>(buf.first));
						vecFracBool.push_back(0);
					}
				}
			}
			tg = true;
		}

		if (set::i().main.frac >= 0 && set::i().main.frac <= 2) {
			static const char* const fracStr[]{ "None", "All", "Frac" };
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, { 0.007f, 0.007f, 0.007f, 1.f });
			ImGui::Button(fracStr[set::i().main.frac], { 85.f, 18.f });
			if (ImGui::IsItemClicked()) {
				if (set::i().main.frac == 0 || set::i().main.frac == 1) {
					set::i().main.frac ^= 1;
					set::i().main.fracStr = set::i().main.frac ? "All" : "None";
				}
				else {
					set::i().main.frac = 0;
					set::i().main.fracName.clear();
					set::i().main.skin.clear();
					std::ranges::fill(vecFracBool, 0);
					set::i().main.fracStr = "None";
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
					if (const auto [res, j] { set::i().getJson("/settings/frac/" + buf) }; res && !j.empty()) {
						set::i().main.skin.append_range(j);
						set::i().main.fracName.push_back(buf);
						set::i().main.frac = 2;
						set::i().main.fracStr = dmgFuncs::i().dobavitZapyatuyu(set::i().main.fracName);
					}
				}
				else {
					if (const auto res{ std::ranges::find(set::i().main.fracName, buf) }; res != set::i().main.fracName.end()) {
						set::i().main.fracName.erase(res);
						if (const auto [res2, j] { set::i().getJson("/settings/frac/" + buf) }; res2 && !j.empty()) {
							for (const auto& buf : j) {
								dmgFuncs::i().removeValue(set::i().main.skin, buf);
							}
						}
					}
					set::i().main.frac = dmgFuncs::i().findValue(vecFracBool, 1u) ? 2 : 0;
					set::i().main.fracStr = set::i().main.frac ? dmgFuncs::i().dobavitZapyatuyu(set::i().main.fracName) : "None";
				}
			}
		}
		ImGui::PopStyleColor();
		ImGui::EndCombo();
	}
	else tg = false;
	ImGui::PopItemWidth();
	if (ImGui::IsItemHovered()) imguiShowTooltip(set::i().main.fracStr);
	ImGui::SameLine();
	ImGui::Checkbox("AntiFrac", &set::i().main.af);

	ImGui::Checkbox("AutoSettings", &set::i().main.sMode);
	ImGui::SameLine(0.f, 10.f);
	if (ImGui::Checkbox("SettingsForServer", &set::i().main.settingsForServer)) {
		if (const auto isWeapon{ dmgFuncs::i().isWeapon(set::i().main.gun) }; isWeapon || dmgFuncs::i().isFist(set::i().main.gun)) {
			set::i().main.isWeapon = isWeapon;
			set::i().loadWeaponSettings(set::i().main.gun, set::i().main.sMode);
		}
	}
	ImGui::SameLine(0.f, 10.f);
	ImGui::Checkbox("ReduceAmmo", &set::i().main.reduceAmmo);
	ImGui::Separator();

	ImGui::PushItemWidth(85.f);
	if (static const char* const weaponName[]{
		"Fist", "Brass Knuckles", "Golf Club", "Nitestick", "Knife", "Baseball Bat", "Shovel", "Pool Cue", "Katana", "Chainsaw", "Dildo 1", "Dildo 2", "Vibe 1", "Vibe 2", "Flowers", "Cane", "Molotov Cocktail",
		"Pistol", "Silenced Pistol", "Desert Eagle", "Shotgun", "Sawn-Off Shotgun", "SPAZ12", "Micro UZI", "MP5", "AK47", "M4", "Tech9", "Country Rifle", "Sniper Rifle",
		"Flame Thrower",
		"Minigun",
		"Spray Can", "Fire Extinguisher", "Parachute"
	}; ImGui::BeginCombo("##6", set::i().main.imguiWeaponItem.data(), ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		for (const auto buf : weaponName) {
			const auto selected{ set::i().main.imguiWeaponItem == buf };
			if (ImGui::Selectable(buf, selected)) {
				const auto gun{ dmgFuncs::i().getWeaponId(buf) };
				set::i().main.isWeapon = dmgFuncs::i().isWeapon(gun);
				set::i().main.sMode = false;
				set::i().loadWeaponSettings(gun, false);
			}
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (set::i().main.packetRpcdmg && set::i().main.packetRpcdmgFromFile) {
		ImGui::PushItemWidth(105.f);
		ImGui::InputFloat("Weapon / Damage", &set::i().main.packetRpcGetDmg, 1.f, 1.f, "%.6f");
		ImGui::PopItemWidth();
	}
	else ImGui::TextUnformatted("Weapon");
	
	ImGui::PushItemWidth(85.f);
	ImGui::InputInt("##7", &set::i().main.delay);
	ImGui::PopItemWidth();
	if (set::i().main.delayMode) {
		ImGui::SameLine();
		ImGui::PushItemWidth(87.f);
		ImGui::InputInt("##8", &set::i().main.delay2);
		ImGui::PopItemWidth();
	}
	ImGui::SameLine();
	ImGui::Checkbox("DelayMode", &set::i().main.delayMode);
	ImGui::PushItemWidth(85.f);
	ImGui::InputInt("##9", &set::i().main.radius, 1, 1);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::TextUnformatted("Radius");

	ImGui::PushItemWidth(85.f);
	ImGui::InputInt("##10", &set::i().main.smiss);
	ImGui::PopItemWidth();
	if (set::i().main.missMode) {
		ImGui::SameLine();
		ImGui::PushItemWidth(85.f);
		ImGui::InputInt("##11", &set::i().main.smiss2);
		ImGui::PopItemWidth();
	}
	ImGui::SameLine();
	ImGui::Checkbox("MissMode", &set::i().main.missMode);
	ImGui::SameLine();
	ImGui::Checkbox("Miss", &set::i().main.miss);

	ImGui::PushItemWidth(85.f);
	ImGui::InputInt("##12", &set::i().main.snokill, 1, 1);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::Checkbox("NoKill", &set::i().main.nokill);
	ImGui::Separator();

	ImGui::PushItemWidth(85.f);
	static int localId;
	ImGui::InputInt("##13", &localId, 1, 1);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Add", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		if (dmgFuncs::i().checkIdLimit(localId) && dmgFuncs::i().remotePlayerIsListed(localId)) {
			if (auto& name{ sampFuncs::i().getRemotePlayerName(localId) }; !dmgFuncs::i().findValue(set::i().main.ignoreList, name)) {
				set::i().main.ignoreList.push_back(name);
				set::i().main.ignoreListStr = dmgFuncs::i().dobavitZapyatuyuIindex(set::i().main.ignoreList);
			}
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Remove", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		if (const auto id{ localId - 1 }; id >= 0 && static_cast<decltype(set::i().main.ignoreList.size())>(id) < set::i().main.ignoreList.size()) {
			set::i().main.ignoreList.erase(set::i().main.ignoreList.begin() + id);
			set::i().main.ignoreListStr = dmgFuncs::i().dobavitZapyatuyuIindex(set::i().main.ignoreList);
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Clear", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		set::i().main.ignoreList.clear();
		set::i().main.ignoreListStr.clear();
	}
	ImGui::SameLine();
	if (ImGui::TextUnformatted("IgnoreList"); ImGui::IsItemHovered() && !set::i().main.ignoreListStr.empty()) imguiShowTooltip(set::i().main.ignoreListStr);

	ImGui::PushItemWidth(85.f);
	static int localId2;
	ImGui::InputInt("##14", &localId2, 1, 1);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Add", { 45.f, 20.f }); ImGui::IsItemClicked() && dmgFuncs::i().checkIdLimit(localId2) && dmgFuncs::i().remotePlayerIsListed(localId2)) {
		if (auto& name{ sampFuncs::i().getRemotePlayerName(localId2) }; !dmgFuncs::i().findValue(set::i().main.blackList, name)) {
			set::i().main.blackList.push_back(name);
			set::i().main.blackListStr = dmgFuncs::i().dobavitZapyatuyuIindex(set::i().main.blackList);
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Remove", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		if (const auto id{ localId2 - 1 }; id >= 0 && static_cast<decltype(set::i().main.blackList.size())>(id) < set::i().main.blackList.size()) {
			set::i().main.blackList.erase(set::i().main.blackList.begin() + id);
			set::i().main.blackListStr = dmgFuncs::i().dobavitZapyatuyuIindex(set::i().main.blackList);
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Clear", { 45.f, 20.f }); ImGui::IsItemClicked()) {
		set::i().main.blackList.clear();
		set::i().main.blackListStr.clear();
	}
	ImGui::SameLine();
	if (ImGui::TextUnformatted("BlackList"); ImGui::IsItemHovered() && !set::i().main.blackListStr.empty()) imguiShowTooltip(set::i().main.blackListStr);

	if (ImGui::Button("Save settings", { 313.f, 20.f }); ImGui::IsItemClicked()) set::i().saveMenuSettings();
}

void visuals::imguiMenuVisuals() const noexcept {
	ImGui::Checkbox("ShowFOV", &set::i().vis.showFov);
	ImGui::Checkbox("RenderLineToDamagePlayer", &set::i().vis.renderLine);
	if (ImGui::Checkbox("Wallhack", &set::i().vis.wallHack)) sampFuncs::i().disableNameTags(set::i().vis.wallHack);
	ImGui::Checkbox("ShowSkinID", &set::i().vis.showSkinId);
	ImGui::Checkbox("ShowIgnoreList", &set::i().vis.showIgnoreList);
	ImGui::Checkbox("ShowBlackList", &set::i().vis.showBlackList);

	if (ImGui::Button("Save settings", { 313.f, 20.f }); ImGui::IsItemClicked()) set::i().saveMenuVisuals();
}

void visuals::imguiMenuMisc() const noexcept {
	if (ImGui::Button(set::i().misc.menuKeyChange ? "Press any key" : set::i().misc.menuKeyName.data(), { 85.f, 20.f }); ImGui::IsItemClicked()) set::i().misc.menuKeyChange = true;
	ImGui::SameLine();
	ImGui::TextUnformatted("MenuActivationKey");

	ImGui::PushItemWidth(85.f);
	static char bufFrac[50];
	ImGui::InputText("##15", bufFrac, sizeof(bufFrac));
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Add Frac", { 92.f, 20.f }); ImGui::IsItemClicked() && bufFrac[0] != '\0') {
		if (auto [res, j] { set::i().getJson() }; res) {
			j["settings"]["frac"][bufFrac] = nlohmann::json::array_t{};
			set::i().saveJson(j);
		}
	}
	ImGui::SameLine(0.f, 2.f);
	static std::string_view fracList{ "None" };
	if (ImGui::Button("Remove Frac", { 92.f, 20.f }); ImGui::IsItemClicked() && bufFrac[0] != '\0') {
		if (auto [res, j] { set::i().getJson() }; res) {
			j["settings"]["frac"].erase(bufFrac);
			if (fracList == bufFrac) fracList = "None";
			set::i().saveJson(j);
		}
	}

	ImGui::PushItemWidth(85.f);
	if (static bool tg; ImGui::BeginCombo("##16", fracList.data(), ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton)) {
		static std::vector<std::string> frac;
		if (!tg) {
			fracList = "None";
			frac.clear();
			if (const auto [res, j] { set::i().getJson("/settings/frac") }; res) {
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
					if (const auto [res, j] { set::i().getJson("/settings/frac") }; res) {
						if (const auto& vb{ j[fracList] }; !vb.empty()) {
							skinIdStr = dmgFuncs::i().dobavitZapyatuyu(vb.get<std::vector<int>>());
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
		if (auto [res, j] { set::i().getJson() }; res) {
			if (auto& vb{ j["settings"]["frac"][fracList] }; !dmgFuncs::i().findValue(vb, skinId)) {
				vb.push_back(skinId);
				set::i().saveJson(j);
			}
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Remove", { 45.f, 20.f }); ImGui::IsItemClicked() && !fracList.empty() && fracList != "None") {
		if (auto [res, j] { set::i().getJson() }; res) {
			if (auto& vb{ j["settings"]["frac"][fracList] }; !vb.empty()) {
				dmgFuncs::i().removeValue(vb, skinId);
				set::i().saveJson(j);
			}
		}
	}
	ImGui::SameLine(0.f, 2.f);
	if (ImGui::Button("Clear", { 45.f, 20.f }); ImGui::IsItemClicked() && !fracList.empty() && fracList != "None") {
		if (auto [res, j] { set::i().getJson() }; res) {
			j["settings"]["frac"][fracList].clear();
			set::i().saveJson(j);
		}
	}
	ImGui::PushItemWidth(85.f);
	ImGui::SliderInt("##18", &set::i().misc.invisz, 0, 10);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::Checkbox("UndergroundInvisible", &set::i().misc.invis);

	if (ImGui::Button("Save settings", { 313.f, 20.f }); ImGui::IsItemClicked()) set::i().saveMenuMisc();
}

void visuals::d3Initialize(IDirect3DDevice9* pDevice) const {
	pFontNick_->Initialize(pDevice);
	pFontNickBar_->Initialize(pDevice);
	pRender_->Initialize(pDevice);
	pBox_->Initialize(pDevice);
}

void visuals::d3Invalidate() const {
	pFontNick_->Invalidate();
	pFontNickBar_->Invalidate();
	pRender_->Invalidate();
	pBox_->Invalidate();
}

void visuals::d3Render() const noexcept {
	if (set::i().main.d3menu) {
		d3Menu();
		if (set::i().vis.showFov && set::i().main.mode == 4) fov();
		if (set::i().vis.renderLine) line();
	}
	if (set::i().vis.wallHack) wallhack();
}

void visuals::d3Menu() const noexcept {
	pRender_->Print(std::format(
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
		set::i().main.packetVehPlayerPass ? "+" : "-", set::i().main.packetAim ? "+" : "-", set::i().main.packetBullet ? "+" : "-", set::i().main.packetWeapon ? "+" : "-", set::i().main.packetRpcdmg ? "+" : "-",
		set::i().main.keyName, set::i().main.keyMode ? "Pressed" : "Down", set::i().main.keyMode ? (set::i().main.keyPressed ? "[True]" : "[False]") : "",
		set::i().main.check ? "Status" : "Health", set::i().main.checkAfk ? "True" : "False",
		dmgFuncs::i().getModeStr(set::i().main.mode), set::i().main.modeOnv ? "True" : "False", set::i().main.mode == 4 ? "[" + std::to_string(set::i().main.fov) + "]" : "",
		set::i().main.af ? "Anti" : "", set::i().main.fracStr,
		set::i().main.imguiWeaponItem, set::i().main.gun,
		set::i().main.delay, set::i().main.delayMode ? "-" + std::to_string(set::i().main.delay2) + "ms" : "",
		set::i().main.radius,
		set::i().main.smiss, set::i().main.missMode ? "-" + std::to_string(set::i().main.smiss2) + "pt" : "", set::i().main.miss ? "True" : "False",
		set::i().main.snokill, set::i().main.nokill ? "True" : "False",
		set::i().vis.showIgnoreList && !set::i().main.ignoreListStr.empty() ? "IG: " + set::i().main.ignoreListStr : "",
		set::i().vis.showBlackList && !set::i().main.blackListStr.empty() ? "BL: " + set::i().main.blackListStr : "",
		dmgFuncs::i().checkActivate() ? set::i().main.dmgStr : ""
	).data(), -1, set::i().main.d3menuX, set::i().main.d3menuY);
}

void visuals::fov() const {
	auto& screenRes{ dmgFuncs::i().getScreenRes() };
	pBox_->DrawPolygon(screenRes.x * 0.53f, screenRes.y * 0.4f, 60, set::i().main.fov / 1.05f, D3DCOLOR_ARGB(75, 0, 0, 0));
}

void visuals::line() const {
	if (dmgFuncs::i().checkActivate() && dmgFuncs::i().remotePlayerIsExist(set::i().main.dmgId)) {
		if (const auto myPos{ dmgFuncs::i().getLocalPlayerPos() }, remotePlayerPos{ dmgFuncs::i().getRemotePlayerPos(set::i().main.dmgId) }, screenMyPos{ dmgFuncs::i().calcScreenCoords(myPos) }, screenPlayerPos{ dmgFuncs::i().calcScreenCoords(remotePlayerPos) }; screenMyPos.z >= 1.f && screenPlayerPos.z >= 1.f) {
			pBox_->D3DLine(screenMyPos.x, screenMyPos.y, screenPlayerPos.x, screenPlayerPos.y, D3DCOLOR_XRGB(255, 0, 0));
		}
	}
}

void visuals::wallhack() const noexcept {
	for (const std::uint16_t i : std::ranges::views::iota(0, sampFuncs::playerLimit)) {
		if (!dmgFuncs::i().remotePlayerIsExist(i, false)) continue;
		const auto remoteBonePos{ dmgFuncs::i().getRemotePlayerBonePos(i, 6) };
		auto screenPos{ dmgFuncs::i().calcScreenCoords(remoteBonePos) };
		if (screenPos.z < 1.f) continue;

		screenPos.x -= 30;
		screenPos.y -= 40;

		auto& remoteName{ sampFuncs::i().getRemotePlayerName(i) };
		const auto remoteAfkState{ sampFuncs::i().getRemotePlayerAfkState(i) };
		const auto remoteColor{ sampFuncs::i().getRemotePlayerNameColor(i) };

		const auto nameStr{ remoteName + "{FFFFFF}[" + std::to_string(i) + "]" + (remoteAfkState ? "[AFK]" : "") };

		pFontNick_->Print(nameStr.data(), dmgFuncs::i().calcNameColor(remoteColor), screenPos.x, screenPos.y, false);

		auto remoteHealth{ sampFuncs::i().getRemotePlayerHealth(i) };
		auto remoteArmour{ sampFuncs::i().getRemotePlayerArmour(i) };

		const auto remoteHealth2{ static_cast<int>(remoteHealth) };
		const auto remoteArmour2{ static_cast<int>(remoteArmour) };

		if (remoteHealth > 100.f) remoteHealth = 100.f;
		if (remoteArmour > 100.f) remoteArmour = 100.f;

		pBox_->D3DBoxBorder(screenPos.x, screenPos.y + (remoteArmour > 0.f ? 19.f : 13.f), 42.f, 5.f, D3DCOLOR_ARGB(255, 0, 0, 0), D3DCOLOR_ARGB(100, 0, 0, 0));
		pBox_->D3DBox(screenPos.x + 1.f, screenPos.y + (remoteArmour > 0.f ? 20.f : 14.f), remoteHealth / 2.5f, 3.f, D3DCOLOR_ARGB(222, 227, 0, 5));
		pFontNickBar_->Print(std::to_string(remoteHealth2).data(), D3DCOLOR_ARGB(255, 255, 255, 255), screenPos.x + 41.5f, screenPos.y + (remoteArmour > 0.f ? 18.f : 10.f));

		if (remoteArmour > 0.f) {
			pBox_->D3DBoxBorder(screenPos.x, screenPos.y + 13.f, 42.f, 5.f, D3DCOLOR_ARGB(255, 0, 0, 0), D3DCOLOR_ARGB(100, 0, 0, 0));
			pBox_->D3DBox(screenPos.x + 1.f, screenPos.y + 14.f, remoteArmour / 2.5f, 3.f, D3DCOLOR_ARGB(255, 255, 255, 255));
			pFontNickBar_->Print(std::to_string(remoteArmour2).data(), D3DCOLOR_ARGB(255, 255, 255, 255), screenPos.x + 41.5f, screenPos.y + 10.f);
		}

		sampFuncs::i().updateScoreboardData();

		const auto remoteScore{ sampFuncs::i().getRemotePlayerScore(i) };
		const auto remotePing{ sampFuncs::i().getRemotePlayerPing(i) };
		const auto remoteSkin{ sampFuncs::i().getRemotePlayerSampPed(i)->pGTA_Ped->base.model_alt_id };

		const auto infoStr{ "Lvl " + std::to_string(remoteScore) + " / Ping " + std::to_string(remotePing) + (set::i().vis.showSkinId ? (" / Skin " + std::to_string(remoteSkin)) : "") };
		
		pFontNick_->Print(infoStr.data(), D3DCOLOR_XRGB(255, 255, 255), screenPos.x, screenPos.y + (remoteArmour > 0.f ? 26.f : 17.f));
	}
}