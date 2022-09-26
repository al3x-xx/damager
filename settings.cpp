#include "settings.hpp"

#include <fstream>

#include "dmg_funcs.hpp"

const char* const fileName{ "dmg.json" };

settings::settings() {
	auto [res, j] { getJson() };
	if (!res) saveSettings(j);
	loadSettings(j);
}

std::tuple<bool, nlohmann::json> settings::getJson(const std::string& path) const noexcept {
	try {
		nlohmann::json j;
		std::ifstream{ fileName } >> j;
		return { true, j.at(nlohmann::json_pointer<std::remove_cvref_t<decltype(path)>>{ path }) };
	}
	catch (...) {
		return {};
	}
}

void settings::saveJson(const nlohmann::json& j) const {
	std::ofstream{ fileName } << std::setw(4) << j;
}

void settings::processWeaponSettings(const nlohmann::json& j, std::uint8_t gun, bool mode) {
	if (!j.empty()) {
		const auto& delay{ j["delay"] };

		delay[0].get_to(main.delay);
		delay[1].get_to(main.delay2);
		delay[2].get_to(main.delayMode);

		j["radius"].get_to(main.radius);

		const auto& miss{ j["miss"] };

		miss[0].get_to(main.smiss);
		miss[1].get_to(main.smiss2);
		miss[2].get_to(main.missMode);
		miss[3].get_to(main.miss);

		const auto& nokill{ j["nokill"] };

		nokill[0].get_to(main.snokill);
		nokill[1].get_to(main.nokill);

		main.packetRpcGetDmg = main.packetRpcdmgFromFile ? j["dmg"].get<float>() : dmgFuncs::i->getWeaponDamage(gun);
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

		main.packetRpcGetDmg = dmgFuncs::i->getWeaponDamage(gun);
	}

	main.imguiWeaponItem = dmgFuncs::i->getWeaponName(gun);
	main.weaponSlot = dmgFuncs::i->getWeaponSlot(gun);
	main.packetAimExtZoom = dmgFuncs::i->getExtZoom(gun);
	main.packetAimWeaponState = dmgFuncs::i->getWeaponState(gun);
	main.isWeaponForAimZ = dmgFuncs::i->isWeaponForfAimZ(gun);

	main.gun = gun;
	main.sMode = mode;
}

void settings::loadWeaponSettings(const nlohmann::json& j, std::uint8_t gun, bool mode) {
	const auto& res{ j["settings"]["weapons"][std::to_string(gun)][main.settingsForServer ? sampFuncs::i->getServerIp() : "default"] };
	processWeaponSettings(res, gun, mode);
}

void settings::loadWeaponSettings(std::uint8_t gun, bool mode) {
	if (const auto [res, j] { getJson("/settings/weapons/" + std::to_string(gun) + "/" + (main.settingsForServer ? sampFuncs::i->getServerIp() : "default")) }; res) {
		processWeaponSettings(j, gun, mode);
	}
}

void settings::processWeaponSettings(nlohmann::json& j) const {
	const auto gunStr{ std::to_string(main.sMode ? sampFuncs::i->getCurrentWeapon() : main.gun) };
	const auto setMode{ main.settingsForServer ? sampFuncs::i->getServerIp() : "default" };

	auto& settings{ j["weapons"][gunStr][setMode] };

	settings["delay"] = { main.delay, main.delay2, main.delayMode };
	settings["radius"] = main.radius;
	settings["miss"] = { main.smiss, main.smiss2, main.missMode, main.miss };
	settings["nokill"] = { main.snokill, main.nokill };
	settings["dmg"] = main.packetRpcGetDmg;
}

void settings::saveMenuSettings() const {
	if (auto [res, j] { getJson() }; res) {
		auto& settings{ j["settings"] }, &default_{ settings["default"] };

		default_["packets"] = { main.packetVehPlayerPass, main.packetAim, main.packetBullet, main.packetWeapon, main.packetRpcdmg };
		default_["packet_aim_anim"] = main.packetAimAnimVeh;
		default_["packet_rpc_file"] = main.packetRpcdmgFromFile;
		default_["key"] = { main.key, main.keyMode };
		default_["check"] = { main.check, main.checkAfk };
		default_["mode"] = { main.mode, main.modeOnv, main.modeBCheckBuildings, main.modeBCheckObjects, main.modeBCheckVehicles, main.fov };

		default_["af"] = main.af;

		default_["as"] = main.sMode;
		default_["sfs"] = main.settingsForServer;
		default_["ra"] = main.reduceAmmo;

		processWeaponSettings(settings);

		default_["ignorelist"] = main.ignoreList;
		default_["blacklist"] = main.blackList;

		saveJson(j);
	}
}

void settings::saveMenuVisuals() const {
	if (auto [res, j] { getJson() }; res) {
		auto& visuals{ j["visuals"] };

		visuals["showfov"] = vis.showFov;
		visuals["renderline"] = vis.renderLine;
		visuals["wallhack"] = vis.wallHack;
		visuals["showskinid"] = vis.showSkinId;
		visuals["showignorelist"] = vis.showIgnoreList;
		visuals["showblacklist"] = vis.showBlackList;

		saveJson(j);
	}
}

void settings::saveMenuMisc() const {
	if (auto [res, j] { getJson() }; res) {
		auto& misc_{ j["misc"] };

		misc_["menu_activation_key"] = misc.menuKey;
		misc_["invisible"] = misc.invisz;

		saveJson(j);
	}
}

void settings::loadSettings(nlohmann::json& j) {
	try {
		const auto& settings{ j["settings"]["default"] };

		const auto& packets{ settings["packets"] };

		packets[0].get_to(main.packetVehPlayerPass);
		packets[1].get_to(main.packetAim);
		packets[2].get_to(main.packetBullet);
		packets[3].get_to(main.packetWeapon);
		packets[4].get_to(main.packetRpcdmg);

		settings["packet_aim_anim"].get_to(main.packetAimAnimVeh);
		settings["packet_rpc_file"].get_to(main.packetRpcdmgFromFile);

		const auto& key{ settings["key"] };

		key[0].get_to(main.key);
		key[1].get_to(main.keyMode);

		const auto& check{ settings["check"] };

		check[0].get_to(main.check);
		check[1].get_to(main.checkAfk);

		const auto& mode{ settings["mode"] };

		mode[0].get_to(main.mode);
		mode[1].get_to(main.modeOnv);
		mode[2].get_to(main.modeBCheckBuildings);
		mode[3].get_to(main.modeBCheckObjects);
		mode[4].get_to(main.modeBCheckVehicles);
		mode[5].get_to(main.fov);

		settings["af"].get_to(main.af);
		settings["as"].get_to(main.sMode);
		settings["sfs"].get_to(main.settingsForServer);
		settings["ra"].get_to(main.reduceAmmo);
		settings["ignorelist"].get_to(main.ignoreList);
		settings["blacklist"].get_to(main.blackList);

		const auto& pos{ settings["pos"] };

		pos[0].get_to(main.d3menuX);
		pos[1].get_to(main.d3menuY);

		const auto& visuals{ j["visuals"] };

		visuals["showfov"].get_to(vis.showFov);
		visuals["renderline"].get_to(vis.renderLine);
		visuals["wallhack"].get_to(vis.wallHack);
		visuals["showskinid"].get_to(vis.showSkinId);
		visuals["showignorelist"].get_to(vis.showIgnoreList);
		visuals["showblacklist"].get_to(vis.showBlackList);

		const auto& misc_{ j["misc"] };

		misc_["menu_activation_key"].get_to(misc.menuKey);
		misc_["invisible"].get_to(misc.invisz);
	}
	catch (...) {
		saveSettings(j);
	}

	//settings
	auto& screenRes{ dmgFuncs::i->getScreenRes() };
	if (main.d3menuX + 166.f > screenRes.x) main.d3menuX = screenRes.x - 166.f;
	if (main.d3menuY + 166.f > screenRes.y) main.d3menuY = screenRes.y - 166.f;

	if (const auto keyName{ dmgFuncs::i->getKeyNameById(main.key) }; keyName.empty()) main.key = 82;
	else main.keyName = keyName;

	if (main.mode < 0 || main.mode > 4) main.mode = 0;

	main.imguiModeItem = dmgFuncs::i->getModeStr(main.mode);

	main.ignoreListStr = dmgFuncs::i->dobavitZapyatuyuIindex(main.ignoreList);
	main.blackListStr = dmgFuncs::i->dobavitZapyatuyuIindex(main.blackList);

	if (!main.sMode) loadWeaponSettings(j, 0, false);

	//visuals
	if (vis.wallHack) sampFuncs::i->disableNameTags(true);

	//misc
	if (const auto keyName{ dmgFuncs::i->getKeyNameById(misc.menuKey) }; keyName.empty()) misc.menuKey = 113;
	else misc.menuKeyName = keyName;
}

void settings::saveSettings(nlohmann::json& j) const {
	auto& settings{ j["settings"] };

	auto& default_{ settings["default"] };

	default_["pos"] = { 333, 333 };
	default_["packets"] = { true, true, true, true, true };
	default_["packet_aim_anim"] = true;
	default_["packet_rpc_file"] = false;
	default_["key"] = { 82, false };
	default_["check"] = { false, true };
	default_["mode"] = { 4, false, true, true, true, 200 };
	default_["af"] = false;
	default_["as"] = true;
	default_["sfs"] = false;
	default_["ra"] = true;
	default_["ignorelist"] = nlohmann::json::array_t{};
	default_["blacklist"] = nlohmann::json::array_t{};

	auto& frac{ settings["frac"] };

	frac["army"] = { 61, 140, 179, 191, 252, 255, 287 };
	frac["aztec"] = { 41, 44, 48, 114, 115, 116, 141, 190, 292 };
	frac["ballas"] = { 21, 102, 103, 104, 195 };
	frac["biker"] = { 100, 247, 248, 254 };
	frac["fbi"] = { 141, 163, 164, 165, 166, 286 };
	frac["grove"] = { 56, 86, 105, 106, 107, 149, 269, 270, 271, 297 };
	frac["lcn"] = { 91, 113, 127, 124, 223, 258 };
	frac["pd"] = { 265, 266, 267, 280, 281, 282, 283, 284, 285, 288, 300, 301, 302, 303, 304, 305, 306, 307, 309, 310, 311 };
	frac["rifa"] = { 119, 173, 174, 175, 226, 273 };
	frac["rm"] = { 111, 112, 125, 126, 214, 272 };
	frac["vagos"] = { 41, 47, 108, 109, 110, 190 };
	frac["yakuza"] = { 117, 118, 120, 122, 123, 169, 186 };

	auto& weapons{ settings["weapons"] };

	for (const std::uint8_t i : std::ranges::views::iota(0, 16)) {
		auto& weapon{ weapons[std::to_string(i)]["default"] };

		weapon["delay"] = { 300, 0, false };
		weapon["dmg"] = i == 9 ? 27.060001f : 5.280000f;
		weapon["miss"] = { 0, 0, false, false };
		weapon["nokill"] = { 0, false };
		weapon["radius"] = 5;
	}

	for (static const std::tuple<const char*, int, float, int, int, bool, int> weapons_[]{
		{ "18", 300, 0.495000f, 0, 0, false, 5 },
		{ "22", 100, 8.250000f, 4, 7, true, 35 },
		{ "23", 100, 13.200001f, 4, 7, true, 35 },
		{ "24", 100, 46.200001f, 3, 5, true, 35 },
		{ "25", 800, 49.500004f, 4, 7, true, 40 },
		{ "26", 100, 49.500004f, 4, 7, true, 35 },
		{ "27", 100, 39.600002f, 4, 7, true, 40 },
		{ "28", 50, 6.60000f, 4, 7, true, 35 },
		{ "29", 100, 8.250000f, 4, 7, true, 45 },
		{ "30", 100, 9.900001f, 7, 10, true, 70 },
		{ "31", 100, 9.900001f, 7, 10, true, 85 },
		{ "32", 100, 6.60000f, 4, 7, true, 35 },
		{ "33", 1000, 24.750002f, 4, 7, true, 90 },
		{ "34", 1000, 41.25000f, 4, 7, true, 300 },
		{ "37", 300, 0.495000f, 0, 0, false, 5 },
		{ "38", 50, 46.200001f, 4, 7, true, 70 },
		{ "41", 300, 0.330000f, 0, 0, false, 14 },
		{ "42", 300, 0.330000f, 0, 0, false, 14 },
		{ "46", 300, 6.600000f, 0, 0, false, 14 }
	}; const auto& [id, delay, dmg, miss, miss2, miss3, radius] : weapons_) {
		auto& weapon{ weapons[id]["default"] };

		weapon["delay"] = { delay, 0, false };
		weapon["dmg"] = dmg;
		weapon["miss"] = { miss, miss2, miss3, false };
		weapon["nokill"] = { 0, false };
		weapon["radius"] = radius;
	}

	auto& visuals{ j["visuals"] };

	visuals["showfov"] = true;
	visuals["renderline"] = true;
	visuals["wallhack"] = false;
	visuals["showskinid"] = false;
	visuals["showignorelist"] = true;
	visuals["showblacklist"] = true;

	auto& misc_{ j["misc"] };

	misc_["menu_activation_key"] = 113;
	misc_["invisible"] = 5;

	saveJson(j);
}