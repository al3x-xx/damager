#include "dmg_funcs.hpp"

#include "settings.hpp"

dmgFuncs::dmgFuncs() {
	sampFuncs::i->addChatMessage(-1, "damager by al3x_ loaded vk.com/0x3078");
	sampFuncs::i->registerChatCommand("dmg", commandFunc);
}

dmgFuncs::~dmgFuncs() {
	sampFuncs::i->unregisterChatCommand(commandFunc);
}

void dmgFuncs::commandFunc(const char* buf) {
	if (!buf || !*buf) {
		settings::i->main.keyPressed = false;
		settings::i->main.d3menu ^= true;
		return;
	}

	auto [res, j] { settings::i->getJson() };
	if (!res) return;

	const std::string_view buf_{ buf };

	auto tsv{ [](const auto& buf) {
		return std::string_view{ buf };
	} };

	std::string_view p, p2, p3;

	for (const auto& buf : buf_ | std::ranges::views::split(' ') | std::ranges::views::take(3) | std::ranges::views::transform(tsv)) {
		if (p.empty()) p = buf;
		else if (p2.empty()) p2 = buf;
		else if (p3.empty()) p3 = buf;
	}

	if (p == "menu") {
		settings::i->main.imguiMenu ^= true;
		i->toggleCursorState(settings::i->main.imguiMenu);
	}
	else if (settings::i->main.d3menu) {
		if (p == "key") {
			if (p2.empty()) {
				settings::i->main.keyMode ^= true;
				j["settings"]["default"]["key"] = { settings::i->main.key, settings::i->main.keyMode };
			}
			else if (const auto value_{ i->strToInt(p2) }) {
				if (const auto keyName{ i->getKeyNameById(*value_) }; !keyName.empty()) {
					settings::i->main.key = *value_;
					settings::i->main.keyName = keyName;
					settings::i->main.keyChange = false;
					j["settings"]["default"]["key"] = { settings::i->main.key, settings::i->main.keyMode };
				}
			}
		}
		else if (p == "check") {
			settings::i->main.check ^= true;
			j["settings"]["default"]["check"] = { settings::i->main.check, settings::i->main.checkAfk };
		}
		else if (p == "afk") {
			settings::i->main.checkAfk ^= true;
			j["settings"]["default"]["check"] = { settings::i->main.check, settings::i->main.checkAfk };
		}
		else if (p == "mode" && !p2.empty()) {
			if (const auto value_{ i->strToInt(p2) }; value_ && *value_ <= 4 && *value_ >= 0) {
				settings::i->main.mode = *value_;
				settings::i->main.imguiModeItem = i->getModeStr(*value_);
				j["settings"]["default"]["mode"] = { settings::i->main.mode, settings::i->main.modeOnv, settings::i->main.modeBCheckBuildings, settings::i->main.modeBCheckObjects, settings::i->main.modeBCheckVehicles, settings::i->main.fov };
			}
		}
		else if (p == "onv") {
			settings::i->main.modeOnv ^= true;
			j["settings"]["default"]["mode"] = { settings::i->main.mode, settings::i->main.modeOnv, settings::i->main.modeBCheckBuildings, settings::i->main.modeBCheckObjects, settings::i->main.modeBCheckVehicles, settings::i->main.fov };
		}
		else if (p == "fov" && !p2.empty()) {
			if (const auto value_{ i->strToInt(p2) }) {
				settings::i->main.fov = *value_;
				j["settings"]["default"]["mode"] = { settings::i->main.mode, settings::i->main.modeOnv, settings::i->main.modeBCheckBuildings, settings::i->main.modeBCheckObjects, settings::i->main.modeBCheckVehicles, settings::i->main.fov };
			}
		}
		else if (p == "frac" && !p2.empty()) {
			if (p2 == "none") {
				settings::i->main.frac = 0;
				settings::i->main.fracStr = "None";
			}
			else if (p2 == "all") {
				settings::i->main.frac = 1;
				settings::i->main.fracStr = "All";
			}
			else {
				settings::i->main.skin.clear();
				settings::i->main.fracName.clear();

				for (const auto& buf : buf_ | std::ranges::views::split(' ') | std::ranges::views::transform(tsv)) {
					const auto& vb{ j["settings"]["frac"][buf] };
					if (vb.empty()) continue;
					if (i->findValue(settings::i->main.fracName, buf)) continue;
					settings::i->main.fracName.emplace_back(buf);
					settings::i->main.skin.append_range(vb);
				}

				if (!settings::i->main.fracName.empty()) {
					settings::i->main.frac = 2;
					settings::i->main.fracStr = i->dobavitZapyatuyu(settings::i->main.fracName);
				}
			}
		}
		else if (p == "af") {
			settings::i->main.af ^= true;
			j["settings"]["default"]["af"] = settings::i->main.af;
		}
		else if (p == "as") {
			settings::i->main.sMode ^= true;
			j["settings"]["default"]["as"] = settings::i->main.sMode;
			sampFuncs::i->addChatMessage(-1, settings::i->main.sMode ? "AutoSettings: True" : "AutoSettings: False");
		}
		else if (p == "sfs") {
			settings::i->main.settingsForServer ^= true;
			if (const auto isWeapon_{ i->isWeapon(settings::i->main.gun) }; isWeapon_ || i->isFist(settings::i->main.gun)) {
				settings::i->main.isWeapon = isWeapon_;
				settings::i->loadWeaponSettings(j, settings::i->main.gun, settings::i->main.sMode);
			}
			j["settings"]["default"]["sfs"] = settings::i->main.settingsForServer;
			sampFuncs::i->addChatMessage(-1, settings::i->main.settingsForServer ? "SettingsForServer: True" : "SettingsForServer: False");
		}
		else if (p == "ra") {
			settings::i->main.reduceAmmo ^= true;
			j["settings"]["default"]["ra"] = settings::i->main.reduceAmmo;
			sampFuncs::i->addChatMessage(-1, settings::i->main.reduceAmmo ? "ReduceAmmo: True" : "ReduceAmmo: False");
		}
		else if (p == "gun" && !p2.empty()) {
			if (const auto value2{ i->strToInt(p2) }; value2 && *value2 >= 0 && *value2 <= 46) {
				if (const auto isWeapon_{ i->isWeapon(settings::i->main.gun) }; isWeapon_ || i->isFist(settings::i->main.gun)) {
					settings::i->main.isWeapon = isWeapon_;
					settings::i->loadWeaponSettings(j, *value2, false);
				}
			}
		}
		else if (const auto gunStr{ std::to_string(settings::i->main.sMode ? sampFuncs::i->getCurrentWeapon() : settings::i->main.gun) }; p == "delay" && !p2.empty()) {
			if (const auto value2{ i->strToInt(p2) }) {
				settings::i->main.delay = *value2;
				if (p3.empty()) settings::i->main.delayMode = false;
				else if (const auto value3{ i->strToInt(p3) }) {
					settings::i->main.delay2 = *value3;
					settings::i->main.delayMode = true;
				}

				if (settings::i->main.settingsForServer) settings::i->processWeaponSettings(j["settings"]);
				else j["settings"]["weapons"][gunStr]["default"]["delay"] = { settings::i->main.delay, settings::i->main.delay2, settings::i->main.delayMode };
			}
		}
		else if (p == "radius" && !p2.empty()) {
			if (const auto value2{ i->strToInt(p2) }) {
				settings::i->main.radius = *value2;

				if (settings::i->main.settingsForServer) settings::i->processWeaponSettings(j["settings"]);
				else j["settings"]["weapons"][gunStr]["default"]["radius"] = settings::i->main.radius;
			}
		}
		else if (p == "miss") {
			if (p2.empty()) settings::i->main.miss ^= true;
			else if (const auto value2{ i->strToInt(p2) }) {
				settings::i->main.smiss = *value2;
				if (p3.empty()) settings::i->main.missMode = false;
				else if (const auto value3{ i->strToInt(p3) }) {
					settings::i->main.smiss2 = *value3;
					settings::i->main.missMode = true;
				}
			}

			if (settings::i->main.settingsForServer) settings::i->processWeaponSettings(j["settings"]);
			else j["settings"]["weapons"][gunStr]["default"]["miss"] = { settings::i->main.smiss, settings::i->main.smiss2, settings::i->main.missMode, settings::i->main.miss };
		}
		else if (p == "nokill") {
			if (p2.empty()) settings::i->main.nokill ^= true;
			else if (const auto value2{ i->strToInt(p2) }) settings::i->main.snokill = *value2;

			if (settings::i->main.settingsForServer) settings::i->processWeaponSettings(j["settings"]);
			else j["settings"]["weapons"][gunStr]["default"]["nokill"] = { settings::i->main.snokill, settings::i->main.nokill };
		}
		else if (p == "ig" && !p2.empty()) {
			if (const auto value2{ i->strToInt(p2) }; value2 && i->checkIdLimit(*value2) && i->remotePlayerIsListed(*value2)) {
				if (auto& name{ sampFuncs::i->getRemotePlayerName(*value2) }; !i->findValue(settings::i->main.ignoreList, name)) {
					settings::i->main.ignoreList.push_back(name);
					settings::i->main.ignoreListStr = i->dobavitZapyatuyuIindex(settings::i->main.ignoreList);
					j["settings"]["default"]["ignorelist"] = settings::i->main.ignoreList;
				}
			}
		}
		else if (p == "dig") {
			if (p2.empty()) {
				settings::i->main.ignoreList.clear();
				settings::i->main.ignoreListStr.clear();
				j["settings"]["default"]["ignorelist"] = settings::i->main.ignoreList;
			}
			else if (auto value2{ i->strToInt(p2) }; value2 && --*value2 >= 0 && static_cast<decltype(settings::i->main.ignoreList.size())>(*value2) < settings::i->main.ignoreList.size()) {
				settings::i->main.ignoreList.erase(settings::i->main.ignoreList.begin() + *value2);
				settings::i->main.ignoreListStr = i->dobavitZapyatuyuIindex(settings::i->main.ignoreList);
				j["settings"]["default"]["ignorelist"] = settings::i->main.ignoreList;
			}
		}
		else if (p == "bl" && !p2.empty()) {
			if (const auto value2{ i->strToInt(p2) }; value2 && i->checkIdLimit(*value2) && i->remotePlayerIsListed(*value2)) {
				if (auto& name{ sampFuncs::i->getRemotePlayerName(*value2) }; !i->findValue(settings::i->main.blackList, name)) {
					settings::i->main.blackList.push_back(name);
					settings::i->main.blackListStr = i->dobavitZapyatuyuIindex(settings::i->main.blackList);
					j["settings"]["default"]["blacklist"] = settings::i->main.blackList;
				}
			}
		}
		else if (p == "dbl") {
			if (p2.empty()) {
				settings::i->main.blackList.clear();
				settings::i->main.blackListStr.clear();
				j["settings"]["default"]["blacklist"] = settings::i->main.blackList;
			}
			else if (auto value2{ i->strToInt(p2) }; value2 && --*value2 >= 0 && static_cast<decltype(settings::i->main.blackList.size())>(*value2) < settings::i->main.blackList.size()) {
				settings::i->main.blackList.erase(settings::i->main.blackList.begin() + *value2);
				settings::i->main.blackListStr = i->dobavitZapyatuyuIindex(settings::i->main.blackList);
				j["settings"]["default"]["blacklist"] = settings::i->main.blackList;
			}
		}
		else if (p == "pos") {
			settings::i->main.d3menuEdit ^= true;
			i->toggleCursorState(settings::i->main.d3menuEdit);
			if (!settings::i->main.d3menuEdit) j["settings"]["default"]["pos"] = { settings::i->main.d3menuX, settings::i->main.d3menuY };
		}
		
		settings::i->saveJson(j);
	}
}

bool dmgFuncs::checkActivate() const noexcept {
	return settings::i->main.d3menu ? settings::i->main.keyMode ? settings::i->main.keyPressed : keyState_[settings::i->main.key] && !sampFuncs::i->getInput()->iInputEnabled : false;
}

const char* dmgFuncs::getModeStr(std::uint8_t mode) const noexcept {
	static const char* const m[]{ "Little id", "Little health", "Little distance", "Little ping", "FOV" };
	return m[mode];
}

bool dmgFuncs::remotePlayerIsListed(std::uint16_t id) const noexcept {
	return sampFuncs::i->remotePlayerIsListed(id) && sampFuncs::i->getRemotePlayer(id) && sampFuncs::i->getRemotePlayerData(id);
}

bool dmgFuncs::remotePlayerIsExist(std::uint16_t id, bool dead) const noexcept {
	if (id == std::numeric_limits<decltype(id)>::max() || !remotePlayerIsListed(id)) return false;
	const auto ped{ sampFuncs::i->getRemotePlayerSampPed(id) };
	if (!ped || !ped->pGTA_Ped) return false;
	return dead ? settings::i->main.check ? ped->pGTA_Ped->state != 54 && ped->pGTA_Ped->state != 55 : sampFuncs::i->getRemotePlayerHealth(id) > 0.f : true;
}

vector3 dmgFuncs::getLocalPlayerPos(bool invis) const noexcept {
	const auto matrix{ sampFuncs::i->getLocalSampPed()->pGTA_Ped->base.matrix };
	return { matrix[12], matrix[13], invis ? matrix[14] - settings::i->misc.invisz : matrix[14] };
}

vector3 dmgFuncs::getRemotePlayerPos(std::uint16_t id) const noexcept {
	const auto matrix{ sampFuncs::i->getRemotePlayerSampPed(id)->pGTA_Ped->base.matrix };
	return { matrix[12], matrix[13], matrix[14] };
}

vector3 dmgFuncs::getRemotePlayerBonePos(std::uint16_t id, int boneId) const {
	vector3 ret;
	reinterpret_cast<void(__thiscall*)(void*, vector3*, int, bool)>(0x5E01C0)(sampFuncs::i->getRemotePlayerSampPed(id)->pGTA_Ped, &ret, boneId, true);
	return ret;
}

std::uint8_t dmgFuncs::getExtZoom(std::uint8_t gun) const {
	static const std::unordered_map<std::uint8_t, std::uint8_t> extZoom{
		{ 30, 27 }, { 31, 27 },
		{ 33, 0 },
	};
	const auto res{ extZoom.find(gun) };
	return res != extZoom.end() ? res->second : 63;
}

std::uint8_t dmgFuncs::getWeaponState(std::uint8_t gun) const {
	static const std::unordered_map<std::uint8_t, std::uint8_t> weaponState{
		{ 22, 2 }, { 23, 2 }, { 24, 2 }, { 26, 2 }, { 27, 2 }, { 28, 2 }, { 29, 2 }, { 30, 2 }, { 31, 2 }, { 32, 2 }, { 37, 2 }, { 38, 2 }, { 41, 2 }, { 42, 2 },
		{ 18, 3 }, { 25, 3 }, { 33, 3 }, { 34, 3 }
	};
	const auto res{ weaponState.find(gun) };
	return res != weaponState.end() ? res->second : 0;
}

std::chrono::nanoseconds dmgFuncs::getDelay() const {
	return std::chrono::milliseconds{ settings::i->main.delayMode ? randomValue(settings::i->main.delay, settings::i->main.delay2) : settings::i->main.delay };
}

std::uint16_t dmgFuncs::getPlayerIdForDmg() const {
	if (!settings::i->main.blackList.empty()) {
		if (const auto id{ processPlayerId(true) }; id != std::numeric_limits<decltype(id)>::max()) {
			return id;
		}
	}
	return processPlayerId();
}

void dmgFuncs::processDmg(std::uint16_t id) const {
	if (settings::i->main.miss) {
		static bool tg;
		static int count, randValue;
		if (settings::i->main.missMode && !tg) randValue = randomValue(settings::i->main.smiss, settings::i->main.smiss2);
		const auto noMiss{ count < (settings::i->main.missMode ? randValue : settings::i->main.smiss) };
		processPackets(id, !noMiss);
		count = noMiss ? count + 1 : 0;
		tg = noMiss;
	}
	else processPackets(id);
}

bool dmgFuncs::localPlayerInVehicle() const noexcept {
	return *reinterpret_cast<void**>(0xBA18FC) != nullptr;
}

bool dmgFuncs::localPlayerIsPassenger() const noexcept {
	return sampFuncs::i->getLocalSampPed()->pGTA_Ped->vehicle->passengers[0] != sampFuncs::i->getLocalSampPed()->pGTA_Ped;
}

std::uint8_t dmgFuncs::getCamMode(std::uint8_t gun) const {
	if (localPlayerInVehicle()) return settings::i->main.packetAimAnimVeh && localPlayerIsPassenger() ? 55 : 18;
	static const std::unordered_map<std::uint8_t, std::uint8_t> camMode{
		{ 22, 53 }, { 23, 53 }, { 24, 53 }, { 25, 53 }, { 26, 53 }, { 27, 53 }, { 28, 53 }, { 29, 53 }, { 30, 53 }, { 31, 53 }, { 32, 53 }, { 33, 53 }, { 37, 53 }, { 38, 53 },
		{ 34, 7 }
	};
	const auto res{ camMode.find(gun) };
	return res != camMode.end() ? res->second : 4;
}

vector3 dmgFuncs::calcVecAimPos(const vector3& targetPos) const noexcept {
	const auto myPos{ getLocalPlayerPos(settings::i->misc.invis) };
	const auto angle{ std::atan2f(targetPos.x - myPos.x, targetPos.y - myPos.y) };
	return { myPos.x - std::sinf(angle) * 3.5f, myPos.y - std::cosf(angle) * 3.5f, myPos.z };
}

float dmgFuncs::calcfAimZ(float aim) const noexcept {
	return localPlayerInVehicle() ? 0.f : settings::i->main.isWeaponForAimZ ? std::abs(aim) : 0.f;
}

void dmgFuncs::processPlayerVehPassSync(std::uint16_t id) const {
	const auto myPos{ getLocalPlayerPos(settings::i->misc.invis) };

	BitStream bs;
	if (localPlayerInVehicle()) {
		if (localPlayerIsPassenger()) {
			auto data{ sampFuncs::i->getLocalPassengerData() };
			data.fPosition[2] = myPos.z;
			data.sKeys = 4;
			data.byteCurrentWeapon = settings::i->main.gun;
			bs.Write(PacketEnumeration::ID_PASSENGER_SYNC);
			bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
		}
		else {
			auto data{ sampFuncs::i->getLocalInCarData() };
			data.fPosition[2] = myPos.z;
			data.sKeys = 4;
			data.byteCurrentWeapon = settings::i->main.gun;
			bs.Write(PacketEnumeration::ID_VEHICLE_SYNC);
			bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
		}
	}
	else {
		auto data{ sampFuncs::i->getLocalOnFootData() };
		data.fPosition[2] = myPos.z;

		const auto remotePos{ getRemotePlayerPos(id) };
		const auto angle{ std::atan2f(remotePos.x - myPos.x, remotePos.y - myPos.y) };
		std::tie(data.fQuaternion[0], data.fQuaternion[3]) = calcQuaternion(angle);

		if (settings::i->main.gun == 24) {
			static bool plusC;
			data.byteSpecialAction = plusC;
			data.sAnimFlags = plusC ? -32764 : -32760;
			data.sCurrentAnimationID = plusC ? 1274 : 1167;
			data.sKeys = plusC ? 134 : 132;
			plusC ^= true;
		}
		else {
			data.byteSpecialAction = 0;
			data.sAnimFlags = -32760;
			data.sCurrentAnimationID = 1167;
			data.sKeys = 132;
		}
		data.byteCurrentWeapon = settings::i->main.gun;
		bs.Write(PacketEnumeration::ID_PLAYER_SYNC);
		bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
	}
	sampFuncs::i->sendPacket(&bs);
}

void dmgFuncs::processAimSync(std::uint16_t id, const vector3& targetPos) const {
	const auto vecAimPos{ calcVecAimPos(targetPos) };

	stAimData data{
		.byteCamMode{ getCamMode(settings::i->main.gun) },
		.vecAimPos{ vecAimPos.x, vecAimPos.y, vecAimPos.z + randomValue(0.7f, 0.8f) },
		.byteCamExtZoom{ settings::i->main.packetAimExtZoom },
		.byteWeaponState{ settings::i->main.packetAimWeaponState },
		.bUnk{ 85 }
	};

	const vector3 aimVec{ targetPos.x - data.vecAimPos[0], targetPos.y - data.vecAimPos[1], targetPos.z - data.vecAimPos[2] };

	const auto vecLen{ getVecLen(aimVec) };

	data.vecAimf1[0] = aimVec.x / vecLen;
	data.vecAimf1[1] = aimVec.y / vecLen;
	data.vecAimf1[2] = aimVec.z / vecLen;

	data.fAimZ = calcfAimZ(data.vecAimf1[2]);

	BitStream bs;
	bs.Write(PacketEnumeration::ID_AIM_SYNC);
	bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
	sampFuncs::i->sendPacket(&bs);
}

void dmgFuncs::processBulletSync(std::uint16_t id, const vector3& targetPos, bool isMiss) const {
	const auto myPos{ getLocalPlayerPos(settings::i->misc.invis) }, remotePos{ getRemotePlayerPos(id) };
	const auto angle{ std::atan2f(remotePos.x - myPos.x, remotePos.y - myPos.y) };

	const auto localPedInVeh{ localPlayerInVehicle() };

	const vector3 vecOrigin{ myPos.x + std::sinf(angle) * (localPedInVeh ? randomValue(0.63f, 0.64f) : randomValue(0.29f, 0.30f)), myPos.y + std::cosf(angle) * (localPedInVeh ? randomValue(0.63f, 0.64f) : randomValue(0.29f, 0.30f)), myPos.z + (localPedInVeh ? randomValue(1.f, 1.1f) : randomValue(0.7f, 0.8f)) };

	const stBulletData data{
		!isMiss,
		isMiss ? std::numeric_limits<decltype(data.sTargetID)>::max() : id,
		vecOrigin.x + std::sinf(angle + 1.57f) * (localPedInVeh ? randomValue(0.19f, 0.20f) : randomValue(0.18f, 0.19f)), vecOrigin.y + std::cosf(angle + 1.57f) * (localPedInVeh ? randomValue(0.19f, 0.20f) : randomValue(0.18f, 0.19f)), vecOrigin.z,
		targetPos.x, targetPos.y, targetPos.z,
		targetPos.x - remotePos.x, targetPos.y - remotePos.y, targetPos.z - remotePos.z,
		settings::i->main.gun
	};

	BitStream bs;
	bs.Write(PacketEnumeration::ID_BULLET_SYNC);
	bs.Write(reinterpret_cast<const char*>(&data), sizeof(data));
	sampFuncs::i->sendPacket(&bs);
}

void dmgFuncs::processGiveDamage(std::uint16_t id) const {
	const stGiveTakeDamage data{
		id,
		settings::i->main.gun == 24 && !settings::i->main.packetRpcdmgFromFile ? !getWeaponSkill(24) ? 23.100000f : settings::i->main.packetRpcGetDmg : settings::i->main.packetRpcGetDmg,
		settings::i->main.gun,
		randomValue(3u, 5u)
	};

	BitStream bs;
	bs.Write(false);
	bs.Write(reinterpret_cast<const char*>(&data), sizeof(data));
	sampFuncs::i->sendRpc(RPCEnumeration::RPC_GiveTakeDamage, &bs);
}

void dmgFuncs::processReduceAmmo() const noexcept {
	auto& weapon{ sampFuncs::i->getLocalSampPed()->pGTA_Ped->weapon[settings::i->main.weaponSlot] };
	if (weapon.ammo_clip > 0 && !(weapon.ammo - weapon.ammo_clip)) weapon.ammo_clip--;
	if (weapon.ammo > 0) weapon.ammo--;
}

void dmgFuncs::processWeaponSync(std::uint16_t id, bool isMiss) const {
	const stWeaponsData data{
		remotePlayerInVehicle(id) || isMiss ? std::numeric_limits<decltype(data.playerTarget)>::max() : id,
		std::numeric_limits<decltype(data.actorTarget)>::max(),
		settings::i->main.weaponSlot,
		settings::i->main.gun,
		static_cast<decltype(data.ammo)>(sampFuncs::i->getLocalSampPed()->pGTA_Ped->weapon[settings::i->main.weaponSlot].ammo)
	};

	BitStream bs;
	bs.Write(PacketEnumeration::ID_WEAPONS_UPDATE);
	bs.Write(reinterpret_cast<const char*>(&data), sizeof(data));
	sampFuncs::i->sendPacket(&bs);
}

void dmgFuncs::processPackets(std::uint16_t id, bool isMiss) const {
	auto targetPos{ getRemotePlayerPos(id) };

	const auto x{ isMiss ? randomValue(0.5f, 0.6f) : randomValue(0.05f, 0.10f) };
	targetPos.x = randomValue(0, 1) ? targetPos.x + x : targetPos.x - x;

	const auto y{ isMiss ? randomValue(0.5f, 0.6f) : randomValue(0.05f, 0.10f) };
	targetPos.y = randomValue(0, 1) ? targetPos.y + y : targetPos.y - y;

	targetPos.z += isMiss ? randomValue(0.1f, 0.5f) : remotePlayerInVehicle(id) ? randomValue(0.4f, 0.5f) : randomValue(0.1f, 0.2f);

	if (settings::i->main.packetVehPlayerPass) processPlayerVehPassSync(id);
	if (settings::i->main.packetAim) processAimSync(id, targetPos);
	if (settings::i->main.packetBullet && settings::i->main.isWeapon) processBulletSync(id, targetPos, isMiss);
	if (settings::i->main.packetRpcdmg && !isMiss) processGiveDamage(id);
	if (settings::i->main.reduceAmmo) processReduceAmmo();
	if (settings::i->main.packetWeapon) processWeaponSync(id, isMiss);
}

std::uint16_t dmgFuncs::processPlayerId(bool blackList) const {
	auto id{ std::numeric_limits<std::uint16_t>::max() };

	float health{};
	float dist{};
	int ping{};

	for (const std::uint16_t i : std::ranges::views::iota(0, sampFuncs::playerLimit)) {
		if (!remotePlayerIsExist(i)) continue;
		if (!settings::i->main.checkAfk && sampFuncs::i->getRemotePlayerAfkState(i)) continue;
		const auto myPos{ getLocalPlayerPos() }, remotePos{ getRemotePlayerPos(i) };
		const auto distRes{ getDistance(myPos, remotePos) };
		if (distRes > settings::i->main.radius) continue;
		if (!blackList) {
			if (findValue(settings::i->main.ignoreList, sampFuncs::i->getRemotePlayerName(i))) continue;
			const auto skinRes{ checkSkin(i) };
			if (!settings::i->main.af && !skinRes) continue;
			if (settings::i->main.af && skinRes) continue;
			if (settings::i->main.nokill && sampFuncs::i->getRemotePlayerHealth(i) < settings::i->main.snokill) continue;
			if (settings::i->main.modeOnv && !isLineOfSightClear(&myPos, &remotePos, settings::i->main.modeBCheckBuildings, settings::i->main.modeBCheckVehicles, settings::i->main.modeBCheckObjects)) continue;
		}
		else if (!findValue(settings::i->main.blackList, sampFuncs::i->getRemotePlayerName(i))) continue;

		switch (settings::i->main.mode) {
		case 0: return i;
		case 1:
			if (const auto res{ sampFuncs::i->getRemotePlayerHealth(i) + sampFuncs::i->getRemotePlayerArmour(i) }; res < health || id == std::numeric_limits<decltype(id)>::max()) {
				health = res;
				id = i;
			}
			break;
		case 2:
			if (distRes < dist) {
				dist = distRes;
				id = i;
			}
			break;
		case 3:
			if (const auto res{ sampFuncs::i->getRemotePlayerPing(i) }; res < ping || id == std::numeric_limits<decltype(id)>::max()) {
				ping = res;
				id = i;
			}
			break;
		case 4:
			const auto screenPos{ calcScreenCoords(remotePos) };
			if (screenPos.z < 1.f) continue;

			auto& screenRes{ getScreenRes() };

			const auto x{ screenRes.x * 0.53f };
			const auto y{ screenRes.y * 0.4f };

			if (screenPos.x > x + settings::i->main.fov || screenPos.y > y + settings::i->main.fov || screenPos.x < x - settings::i->main.fov || screenPos.y < y - settings::i->main.fov) continue;

			if (const auto res{ getDistance(screenPos.x, screenPos.y, x, y) }; res < dist || id == std::numeric_limits<decltype(id)>::max()) {
				dist = res;
				id = i;
			}
		}
	}
	return id;
}

bool dmgFuncs::checkSkin(std::uint16_t id) const {
	return settings::i->main.frac == 1 ? true : settings::i->main.frac == 2 ? findValue(settings::i->main.skin, sampFuncs::i->getRemotePlayerSampPed(id)->pGTA_Ped->base.model_alt_id) : false;
}

bool dmgFuncs::remotePlayerInVehicle(std::uint16_t id) const noexcept {
	return sampFuncs::i->getRemotePlayerSampPed(id)->pGTA_Ped->pedFlags[1] & 1;
}