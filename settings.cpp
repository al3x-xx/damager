#include "settings.hpp"

#include <fstream>

#include "dmg_funcs.hpp"

constexpr const char* fileName{ "dmg.json" };

set::set() {
	if (const auto [res, _] { getJSON() }; !res) {
		saveSettings();
	}
	loadSettings();
}

set& set::i() {
	static set instance;
	return instance;
}

std::tuple<bool, nlohmann::json> set::getJSON(const std::string& path) const {
	try {
		nlohmann::json j;
		std::ifstream{ fileName } >> j;
		return { true, j.at(nlohmann::json_pointer<std::string>(path)) };
	}
	catch (...) {
		return { false, {} };
	}
}

void set::saveJSON(const nlohmann::json& j) const {
	std::ofstream{ fileName } << std::setw(4) << j;
}

void set::loadWeaponSettings(std::uint8_t gun, bool mode) {
	if (const auto [res, j] { getJSON("/settings/weapons/" + std::to_string(gun) + "/" + (main.settingsForServer ? sampFuncs::i().getServerIP() : "default")) }; res && !j.empty()) {
		j["delay"][0].get_to(main.delay);
		j["delay"][1].get_to(main.delay2);
		j["delay"][2].get_to(main.delayMode);

		j["radius"].get_to(main.radius);

		j["miss"][0].get_to(main.smiss);
		j["miss"][1].get_to(main.smiss2);
		j["miss"][2].get_to(main.missMode);
		j["miss"][3].get_to(main.miss);

		j["nokill"][0].get_to(main.snokill);
		j["nokill"][1].get_to(main.nokill);

		main.packetRPCGetDmg = main.packetRPCdmgFromFile ? j["dmg"].get<float>() : dmgFuncs::i().getWeaponDamage(gun);
	}
	else {
		main.delay = 0;
		main.delay2 = 0;
		main.delayMode = false;

		main.radius = 0;

		main.smiss = 0;
		main.smiss2 = 0;
		main.missMode = false;
		main.miss = false;

		main.snokill = 0;
		main.nokill = false;

		main.packetRPCGetDmg = dmgFuncs::i().getWeaponDamage(gun);
	}

	main.imguiWeaponItem = dmgFuncs::i().getWeaponName(gun);
	main.weaponSlot = dmgFuncs::i().getWeaponSlot(gun);
	main.packetAimExtZoom = dmgFuncs::i().getExtZoom(gun);
	main.packetAimWeaponState = dmgFuncs::i().getWeaponState(gun);
	main.isWeaponForAimZ = dmgFuncs::i().isWeaponForfAimZ(gun);

	main.gun = gun;
	main.sMode = mode;
}

void set::processWeaponSettings(nlohmann::json& j) const {
	const auto gunStr{ std::to_string(main.sMode ? sampFuncs::i().getCurrentWeapon() : main.gun) };
	const auto setMode{ main.settingsForServer ? sampFuncs::i().getServerIP() : "default" };

	j["settings"]["weapons"][gunStr][setMode]["delay"] = { main.delay, main.delay2, main.delayMode };
	j["settings"]["weapons"][gunStr][setMode]["radius"] = main.radius;
	j["settings"]["weapons"][gunStr][setMode]["miss"] = { main.smiss, main.smiss2, main.missMode, main.miss };
	j["settings"]["weapons"][gunStr][setMode]["nokill"] = { main.snokill, main.nokill };
	j["settings"]["weapons"][gunStr][setMode]["dmg"] = main.packetRPCGetDmg;
}

void set::saveMenuSettings() const {
	if (auto [res, j] { getJSON() }; res) {
		j["settings"]["default"]["packets"] = { main.packetVehPlayerPass, main.packetAim, main.packetBullet, main.packetWeapon, main.packetRPCdmg };
		j["settings"]["default"]["packet_aim_anim"] = main.packetAimAnimVeh;
		j["settings"]["default"]["packet_rpc_file"] = main.packetRPCdmgFromFile;
		j["settings"]["default"]["key"] = { main.key, main.keyMode };
		j["settings"]["default"]["check"] = { main.check, main.checkAFK };
		j["settings"]["default"]["mode"] = { main.mode, main.modeOnv, main.modeBCheckBuildings, main.modeBCheckObjects, main.modeBCheckVehicles, main.fov };

		j["settings"]["default"]["af"] = main.af;

		j["settings"]["default"]["as"] = main.sMode;
		j["settings"]["default"]["sfs"] = main.settingsForServer;
		j["settings"]["default"]["ra"] = main.reduceAmmo;

		processWeaponSettings(j);

		j["settings"]["default"]["ignorelist"] = main.ignoreList;
		j["settings"]["default"]["blacklist"] = main.blackList;

		saveJSON(j);
	}
}

void set::saveMenuVisuals() const {
	if (auto [res, j] { getJSON() }; res) {
		j["visuals"]["showfov"] = vis.showFov;
		j["visuals"]["renderline"] = vis.renderLine;
		j["visuals"]["wallhack"] = vis.wallHack;
		j["visuals"]["showskinid"] = vis.showSkinID;
		j["visuals"]["showignorelist"] = vis.showIgnoreList;
		j["visuals"]["showblacklist"] = vis.showBlackList;

		saveJSON(j);
	}
}

void set::saveMenuMisc() const {
	if (auto [res, j] { getJSON() }; res) {
		j["misc"]["menu_activation_key"] = misc.menuKey;
		j["misc"]["invisible"] = misc.inviZ;

		saveJSON(j);
	}
}

void set::loadSettings() {
	if (const auto [res, j] { getJSON() }; res) {
		try {
			j["settings"]["default"]["packets"][0].get_to(main.packetVehPlayerPass);
			j["settings"]["default"]["packets"][1].get_to(main.packetAim);
			j["settings"]["default"]["packets"][2].get_to(main.packetBullet);
			j["settings"]["default"]["packets"][3].get_to(main.packetWeapon);
			j["settings"]["default"]["packets"][4].get_to(main.packetRPCdmg);
			j["settings"]["default"]["packet_aim_anim"].get_to(main.packetAimAnimVeh);
			j["settings"]["default"]["packet_rpc_file"].get_to(main.packetRPCdmgFromFile);
			j["settings"]["default"]["key"][0].get_to(main.key);			
			j["settings"]["default"]["key"][1].get_to(main.keyMode);
			j["settings"]["default"]["check"][0].get_to(main.check);
			j["settings"]["default"]["check"][1].get_to(main.checkAFK);
			j["settings"]["default"]["mode"][0].get_to(main.mode);
			j["settings"]["default"]["mode"][1].get_to(main.modeOnv);
			j["settings"]["default"]["mode"][2].get_to(main.modeBCheckBuildings);
			j["settings"]["default"]["mode"][3].get_to(main.modeBCheckObjects);
			j["settings"]["default"]["mode"][4].get_to(main.modeBCheckVehicles);
			j["settings"]["default"]["mode"][5].get_to(main.fov);
			j["settings"]["default"]["af"].get_to(main.af);
			j["settings"]["default"]["as"].get_to(main.sMode);
			j["settings"]["default"]["sfs"].get_to(main.settingsForServer);
			j["settings"]["default"]["ra"].get_to(main.reduceAmmo);
			j["settings"]["default"]["ignorelist"].get_to(main.ignoreList);
			j["settings"]["default"]["blacklist"].get_to(main.blackList);
			j["settings"]["default"]["pos"][0].get_to(main.d3menuX);
			j["settings"]["default"]["pos"][1].get_to(main.d3menuY);

			j["visuals"]["showfov"].get_to(vis.showFov);
			j["visuals"]["renderline"].get_to(vis.renderLine);
			j["visuals"]["wallhack"].get_to(vis.wallHack);
			j["visuals"]["showskinid"].get_to(vis.showSkinID);
			j["visuals"]["showignorelist"].get_to(vis.showIgnoreList);
			j["visuals"]["showblacklist"].get_to(vis.showBlackList);

			j["misc"]["menu_activation_key"].get_to(misc.menuKey);
			j["misc"]["invisible"].get_to(misc.inviZ);
		}
		catch (...) {
			saveSettings();
		}

		//settings
		const auto screenRes{ dmgFuncs::i().getScreenRes() };
		if ((main.d3menuX + 166.f) > screenRes.x) main.d3menuX = screenRes.x - 166.f;
		if ((main.d3menuY + 166.f) > screenRes.y) main.d3menuY = screenRes.y - 166.f;

		if (const auto kName{ dmgFuncs::i().getKeyNameById(main.key) }; kName.empty()) {
			main.key = 82;
		}
		else {
			main.keyName = kName;
		}

		if (main.mode < 0 || main.mode > 4) main.mode = 0;

		main.imguiModeItem = dmgFuncs::i().getModeStr(main.mode);

		main.ignoreListStr = dmgFuncs::i().dobavitZapyatuyuIindex(main.ignoreList);
		main.blackListStr = dmgFuncs::i().dobavitZapyatuyuIindex(main.blackList);

		if (!main.sMode) loadWeaponSettings(0, false);

		//visuals
		if (vis.wallHack) sampFuncs::i().disableNameTags(true);

		//misc
		if (const auto kName{ dmgFuncs::i().getKeyNameById(misc.menuKey) }; kName.empty()) {
			misc.menuKey = 113;
		}
		else {
			misc.menuKeyName = kName;
		}
	}
}

void set::saveSettings() const {
	nlohmann::json j;
	j["settings"]["default"]["pos"] = { 333, 333 };
	j["settings"]["default"]["packets"] = { true, true, true, true, true };
	j["settings"]["default"]["packet_aim_anim"] = true;
	j["settings"]["default"]["packet_rpc_file"] = false;
	j["settings"]["default"]["key"] = { 82, false };
	j["settings"]["default"]["check"] = { false, true };
	j["settings"]["default"]["mode"] = { 4, false, true, true, true, 200 };
	j["settings"]["default"]["af"] = false;
	j["settings"]["default"]["as"] = true;
	j["settings"]["default"]["sfs"] = false;
	j["settings"]["default"]["ra"] = true;
	j["settings"]["default"]["ignorelist"] = nlohmann::json::array_t{};
	j["settings"]["default"]["blacklist"] = nlohmann::json::array_t{};

	j["settings"]["frac"]["army"] = { 61, 140, 179, 191, 252, 255, 287 };
	j["settings"]["frac"]["aztec"] = { 41, 44, 48, 114, 115, 116, 141, 190, 292 };
	j["settings"]["frac"]["ballas"] = { 21, 102, 103, 104, 195 };
	j["settings"]["frac"]["biker"] = { 100, 247, 248, 254 };
	j["settings"]["frac"]["fbi"] = { 141, 163, 164, 165, 166, 286 };
	j["settings"]["frac"]["grove"] = { 56, 86, 105, 106, 107, 149, 269, 270, 271, 297 };
	j["settings"]["frac"]["lcn"] = { 91, 113, 127, 124, 223, 258 };
	j["settings"]["frac"]["pd"] = { 265, 266, 267, 280, 281, 282, 283, 284, 285, 288, 300, 301, 302, 303, 304, 305, 306, 307, 309, 310, 311 };
	j["settings"]["frac"]["rifa"] = { 119, 173, 174, 175, 226, 273 };
	j["settings"]["frac"]["rm"] = { 111, 112, 125, 126, 214, 272 };
	j["settings"]["frac"]["vagos"] = { 41, 47, 108, 109, 110, 190 };
	j["settings"]["frac"]["yakuza"] = { 117, 118, 120, 122, 123, 169, 186 };

	for (std::uint8_t i{}; i < 16; i++) {
		const auto str{ std::to_string(i) };
		j["settings"]["weapons"][str]["default"]["delay"] = { 300, 0, false };
		j["settings"]["weapons"][str]["default"]["dmg"] = i == 9 ? 27.060001f : 5.280000f;
		j["settings"]["weapons"][str]["default"]["miss"] = { 0, 0, false, false };
		j["settings"]["weapons"][str]["default"]["nokill"] = { 0, false };
		j["settings"]["weapons"][str]["default"]["radius"] = 5;
	}

	j["settings"]["weapons"]["18"]["default"]["delay"] = { 300, 0, false };
	j["settings"]["weapons"]["18"]["default"]["dmg"] = 0.495000f;
	j["settings"]["weapons"]["18"]["default"]["miss"] = { 0, 0, false, false };
	j["settings"]["weapons"]["18"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["18"]["default"]["radius"] = 5;

	j["settings"]["weapons"]["22"]["default"]["delay"] = { 100, 0, false };
	j["settings"]["weapons"]["22"]["default"]["dmg"] = 8.250000f;
	j["settings"]["weapons"]["22"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["22"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["22"]["default"]["radius"] = 35;

	j["settings"]["weapons"]["23"]["default"]["delay"] = { 100, 0, false };
	j["settings"]["weapons"]["23"]["default"]["dmg"] = 13.200001f;
	j["settings"]["weapons"]["23"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["23"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["23"]["default"]["radius"] = 35;


	j["settings"]["weapons"]["24"]["default"]["delay"] = { 100, 0, false };
	j["settings"]["weapons"]["24"]["default"]["dmg"] = 46.200001f;
	j["settings"]["weapons"]["24"]["default"]["miss"] = { 3, 5, true, false };
	j["settings"]["weapons"]["24"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["24"]["default"]["radius"] = 35;

	j["settings"]["weapons"]["25"]["default"]["delay"] = { 800, 0, false };
	j["settings"]["weapons"]["25"]["default"]["dmg"] = 49.500004f;
	j["settings"]["weapons"]["25"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["25"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["25"]["default"]["radius"] = 40;

	j["settings"]["weapons"]["26"]["default"]["delay"] = { 100, 0, false };
	j["settings"]["weapons"]["26"]["default"]["dmg"] = 49.500004f;
	j["settings"]["weapons"]["26"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["26"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["26"]["default"]["radius"] = 35;

	j["settings"]["weapons"]["27"]["default"]["delay"] = { 100, 0, false };
	j["settings"]["weapons"]["27"]["default"]["dmg"] = 39.600002f;
	j["settings"]["weapons"]["27"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["27"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["27"]["default"]["radius"] = 40;

	j["settings"]["weapons"]["28"]["default"]["delay"] = { 50, 0, false };
	j["settings"]["weapons"]["28"]["default"]["dmg"] = 6.60000f;
	j["settings"]["weapons"]["28"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["28"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["28"]["default"]["radius"] = 35;

	j["settings"]["weapons"]["29"]["default"]["delay"] = { 100, 0, false };
	j["settings"]["weapons"]["29"]["default"]["dmg"] = 8.250000f;
	j["settings"]["weapons"]["29"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["29"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["29"]["default"]["radius"] = 45;

	j["settings"]["weapons"]["30"]["default"]["delay"] = { 100, 0, false };
	j["settings"]["weapons"]["30"]["default"]["dmg"] = 9.900001f;
	j["settings"]["weapons"]["30"]["default"]["miss"] = { 7, 10, true, false };
	j["settings"]["weapons"]["30"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["30"]["default"]["radius"] = 70;

	j["settings"]["weapons"]["31"]["default"]["delay"] = { 100,0,false };
	j["settings"]["weapons"]["31"]["default"]["dmg"] = 9.900001f;
	j["settings"]["weapons"]["31"]["default"]["miss"] = { 7,10,true,false };
	j["settings"]["weapons"]["31"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["31"]["default"]["radius"] = 85;

	j["settings"]["weapons"]["32"]["default"]["delay"] = { 100, 0, false };
	j["settings"]["weapons"]["32"]["default"]["dmg"] = 6.60000f;
	j["settings"]["weapons"]["32"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["32"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["32"]["default"]["radius"] = 35;

	j["settings"]["weapons"]["33"]["default"]["delay"] = { 1000, 0, false };
	j["settings"]["weapons"]["33"]["default"]["dmg"] = 24.750002f;
	j["settings"]["weapons"]["33"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["33"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["33"]["default"]["radius"] = 90;

	j["settings"]["weapons"]["34"]["default"]["delay"] = { 1000, 0, false };
	j["settings"]["weapons"]["34"]["default"]["dmg"] = 41.25000f;
	j["settings"]["weapons"]["34"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["34"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["34"]["default"]["radius"] = 300;

	j["settings"]["weapons"]["37"]["default"]["delay"] = { 300, 0, false };
	j["settings"]["weapons"]["37"]["default"]["dmg"] = 0.495000f;
	j["settings"]["weapons"]["37"]["default"]["miss"] = { 0, 0, false, false };
	j["settings"]["weapons"]["37"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["37"]["default"]["radius"] = 5;

	j["settings"]["weapons"]["38"]["default"]["delay"] = { 50, 0, false };
	j["settings"]["weapons"]["38"]["default"]["dmg"] = 46.200001f;
	j["settings"]["weapons"]["38"]["default"]["miss"] = { 4, 7, true, false };
	j["settings"]["weapons"]["38"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["38"]["default"]["radius"] = 70;

	j["settings"]["weapons"]["41"]["default"]["delay"] = { 300, 0, false };
	j["settings"]["weapons"]["41"]["default"]["dmg"] = 0.330000f;
	j["settings"]["weapons"]["41"]["default"]["miss"] = { 0, 0, false, false };
	j["settings"]["weapons"]["41"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["41"]["default"]["radius"] = 14;

	j["settings"]["weapons"]["42"]["default"]["delay"] = { 300, 0, false };
	j["settings"]["weapons"]["42"]["default"]["dmg"] = 0.330000f;
	j["settings"]["weapons"]["42"]["default"]["miss"] = { 0, 0, false, false };
	j["settings"]["weapons"]["42"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["42"]["default"]["radius"] = 14;

	j["settings"]["weapons"]["46"]["default"]["delay"] = { 300, 0, false };
	j["settings"]["weapons"]["46"]["default"]["dmg"] = 6.600000f;
	j["settings"]["weapons"]["46"]["default"]["miss"] = { 0, 0, false, false };
	j["settings"]["weapons"]["46"]["default"]["nokill"] = { 0, false };
	j["settings"]["weapons"]["46"]["default"]["radius"] = 14;

	j["visuals"]["showfov"] = true;
	j["visuals"]["renderline"] = true;
	j["visuals"]["wallhack"] = false;
	j["visuals"]["showskinid"] = false;
	j["visuals"]["showignorelist"] = true;
	j["visuals"]["showblacklist"] = true;

	j["misc"]["menu_activation_key"] = 113;
	j["misc"]["invisible"] = 5;

	saveJSON(j);
}