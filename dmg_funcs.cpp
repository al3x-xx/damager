#include "dmg_funcs.hpp"

#include <d3d9.h>

#include "settings.hpp"

dmgFuncs::dmgFuncs() {
	sampFuncs::i().addChatMessage(-1, "damager by al3x_ loaded vk.com/0x3078");
	sampFuncs::i().registerChatCommand("dmg", commandFunc);
}

dmgFuncs::~dmgFuncs() {
	sampFuncs::i().unregisterChatCommand(commandFunc);
}

dmgFuncs& dmgFuncs::i() {
	static dmgFuncs instance;
	return instance;
}

void __cdecl dmgFuncs::commandFunc(char* buf) {
	if (buf[0] == '\0') {
		set::i().main.keyPressed = false;
		set::i().main.d3menu ^= true;
		return;
	}

	auto [res, j] { set::i().getJSON() };
	if (!res) {
		return;
	}

	const auto gunStr{ std::to_string(set::i().main.sMode ? sampFuncs::i().getCurrentWeapon() : set::i().main.gun) };

	std::string p, p2, p3;
	std::istringstream{ buf } >> p >> p2 >> p3;

	if (p == "menu") {
		set::i().main.imguiMenu ^= true;
		dmgFuncs::i().toggleCursorState(set::i().main.imguiMenu);
	}
	else if (set::i().main.d3menu) {
		if (p == "key") {
			if (p2.empty()) {
				set::i().main.keyMode ^= true;
			}
			else if (const auto value{ dmgFuncs::i().strToInt(p2) }; value != -1) {
				if (const auto kName{ dmgFuncs::i().getKeyNameById(value) }; !kName.empty()) {
					set::i().main.key = value;
					set::i().main.keyName = kName;
					set::i().main.keyChange = false;
				}
			}
			j["settings"]["default"]["key"] = { set::i().main.key, set::i().main.keyMode };
		}
		else if (p == "check") {
			set::i().main.check ^= true;
			j["settings"]["default"]["check"] = { set::i().main.check, set::i().main.checkAFK };
		}
		else if (p == "afk") {
			set::i().main.checkAFK ^= true;
			j["settings"]["default"]["check"] = { set::i().main.check, set::i().main.checkAFK };
		}
		else if (p == "mode" && !p2.empty()) {
			if (const auto value{ dmgFuncs::i().strToInt(p2) }; value <= 4 && value >= 0) {
				set::i().main.mode = value;
				set::i().main.imguiModeItem = dmgFuncs::i().getModeStr(value);
				j["settings"]["default"]["mode"] = { set::i().main.mode, set::i().main.modeOnv, set::i().main.modeBCheckBuildings, set::i().main.modeBCheckObjects, set::i().main.modeBCheckVehicles, set::i().main.fov };
			}
		}
		else if (p == "onv") {
			set::i().main.modeOnv ^= true;
			j["settings"]["default"]["mode"] = { set::i().main.mode, set::i().main.modeOnv, set::i().main.modeBCheckBuildings, set::i().main.modeBCheckObjects, set::i().main.modeBCheckVehicles, set::i().main.fov };
		}
		else if (p == "fov" && !p2.empty()) {
			if (const auto value{ dmgFuncs::i().strToInt(p2) }; value != -1) {
				set::i().main.fov = value;
				j["settings"]["default"]["mode"] = { set::i().main.mode, set::i().main.modeOnv, set::i().main.modeBCheckBuildings, set::i().main.modeBCheckObjects, set::i().main.modeBCheckVehicles, set::i().main.fov };
			}
		}
		else if (p == "frac" && !p2.empty()) {
			if (p2 == "none") {
				set::i().main.frac = 0;
				set::i().main.fracStr = "None";
			}
			else if (p2 == "all") {
				set::i().main.frac = 1;
				set::i().main.fracStr = "All";
			}
			else {
				set::i().main.skin.clear();
				set::i().main.fracName.clear();

				for (const auto& buf : dmgFuncs::i().splitString(buf)) {
					if (j["settings"]["frac"][buf].empty()) continue;
					if (dmgFuncs::i().findInVector(set::i().main.fracName, buf)) continue;
					set::i().main.fracName.push_back(buf);
					const auto vb{ j["settings"]["frac"][buf].get<std::vector<int>>() };
					set::i().main.skin.insert(set::i().main.skin.end(), vb.begin(), vb.end());
				}

				if (!set::i().main.fracName.empty()) {
					set::i().main.frac = 2;
					set::i().main.fracStr = dmgFuncs::i().dobavitZapyatuyu(set::i().main.fracName);
				}
			}
		}
		else if (p == "af") {
			set::i().main.af ^= true;
			j["settings"]["default"]["af"] = set::i().main.af;
		}
		else if (p == "as") {
			set::i().main.sMode ^= true;
			j["settings"]["default"]["as"] = set::i().main.sMode;
			sampFuncs::i().addChatMessage(-1, set::i().main.sMode ? "AutoSettings: True" : "AutoSettings: False");
		}
		else if (p == "sfs") {
			set::i().main.settingsForServer ^= true;
			if (const auto isWeapon_{ dmgFuncs::i().isWeapon(set::i().main.gun) }; isWeapon_ || dmgFuncs::i().isFist(set::i().main.gun)) {
				set::i().main.isWeapon = isWeapon_;
				set::i().loadWeaponSettings(set::i().main.gun, set::i().main.sMode);
			}
			j["settings"]["default"]["sfs"] = set::i().main.settingsForServer;
			sampFuncs::i().addChatMessage(-1, set::i().main.settingsForServer ? "SettingsForServer: True" : "SettingsForServer: False");
		}
		else if (p == "ra") {
			set::i().main.reduceAmmo ^= true;
			j["settings"]["default"]["ra"] = set::i().main.reduceAmmo;
			sampFuncs::i().addChatMessage(-1, set::i().main.reduceAmmo ? "ReduceAmmo: True" : "ReduceAmmo: False");
		}
		else if (p == "gun" && !p2.empty()) {
			if (const auto value2{ dmgFuncs::i().strToInt(p2) }; value2 >= 0 && value2 <= 46) {
				if (const auto isWeapon_{ dmgFuncs::i().isWeapon(set::i().main.gun) }; isWeapon_ || dmgFuncs::i().isFist(set::i().main.gun)) {
					set::i().main.isWeapon = isWeapon_;
					set::i().loadWeaponSettings(value2, false);
				}
			}
		}
		else if (p == "delay" && !p2.empty()) {
			if (const auto value2{ dmgFuncs::i().strToInt(p2) }; value2 != -1) {
				set::i().main.delay = value2;
				if (p3.empty()) {
					set::i().main.delayMode = false;
				}
				else if (const auto value3{ dmgFuncs::i().strToInt(p3) }; value3 != -1) {
					set::i().main.delay2 = value3;
					set::i().main.delayMode = true;
				}

				if (set::i().main.settingsForServer) {
					set::i().processWeaponSettings(j);
				}
				else {
					j["settings"]["weapons"][gunStr]["default"]["delay"] = { set::i().main.delay, set::i().main.delay2, set::i().main.delayMode };
				}
			}
		}
		else if (p == "radius" && !p2.empty()) {
			if (const auto value2{ dmgFuncs::i().strToInt(p2) }; value2 != -1) {
				set::i().main.radius = value2;

				if (set::i().main.settingsForServer) {
					set::i().processWeaponSettings(j);
				}
				else {
					j["settings"]["weapons"][gunStr]["default"]["radius"] = set::i().main.radius;
				}
			}
		}
		else if (p == "miss") {
			if (p2.empty()) {
				set::i().main.miss ^= true;
			}
			else if (const auto value2{ dmgFuncs::i().strToInt(p2) }; value2 != -1) {
				set::i().main.smiss = value2;
				if (p3.empty()) {
					set::i().main.missMode = false;
				}
				else if (const auto value3{ dmgFuncs::i().strToInt(p3) }; value3 != -1) {
					set::i().main.smiss2 = value3;
					set::i().main.missMode = true;
				}
			}

			if (set::i().main.settingsForServer) {
				set::i().processWeaponSettings(j);
			}
			else {
				j["settings"]["weapons"][gunStr]["default"]["miss"] = { set::i().main.smiss, set::i().main.smiss2, set::i().main.missMode, set::i().main.miss };
			}
		}
		else if (p == "nokill") {
			if (p2.empty()) {
				set::i().main.nokill ^= true;
			}
			else if (const auto value2{ dmgFuncs::i().strToInt(p2) }; value2 != -1) {
				set::i().main.snokill = value2;
			}

			if (set::i().main.settingsForServer) {
				set::i().processWeaponSettings(j);
			}
			else {
				j["settings"]["weapons"][gunStr]["default"]["nokill"] = { set::i().main.snokill, set::i().main.nokill };
			}
		}
		else if (p == "ig" && !p2.empty()) {
			if (const auto value2{ dmgFuncs::i().strToInt(p2) }; dmgFuncs::i().checkIDLimit(value2) && dmgFuncs::i().remotePlayerIsListed(value2)) {
				const auto name{ sampFuncs::i().getRemotePlayerName(value2) };
				if (!dmgFuncs::i().findInVector(set::i().main.ignoreList, name)) {
					set::i().main.ignoreList.push_back(name);
					set::i().main.ignoreListStr = dmgFuncs::i().dobavitZapyatuyuIindex(set::i().main.ignoreList);
					j["settings"]["default"]["ignorelist"] = set::i().main.ignoreList;
				}
			}
		}
		else if (p == "dig") {
			if (p2.empty()) {
				set::i().main.ignoreList.clear();
				set::i().main.ignoreListStr.clear();
			}
			else if (const auto value2{ dmgFuncs::i().strToInt(p2) - 1 }; value2 >= 0 && set::i().main.ignoreList.size() > static_cast<decltype(set::i().main.ignoreList.size())>(value2)) {
				set::i().main.ignoreList.erase(set::i().main.ignoreList.begin() + value2);
				set::i().main.ignoreListStr = dmgFuncs::i().dobavitZapyatuyuIindex(set::i().main.ignoreList);
			}
			j["settings"]["default"]["ignorelist"] = set::i().main.ignoreList;
		}
		else if (p == "bl" && !p2.empty()) {
			if (const auto value2{ dmgFuncs::i().strToInt(p2) }; dmgFuncs::i().checkIDLimit(value2) && dmgFuncs::i().remotePlayerIsListed(value2)) {
				if (const auto name{ sampFuncs::i().getRemotePlayerName(value2) }; !dmgFuncs::i().findInVector(set::i().main.blackList, name)) {
					set::i().main.blackList.push_back(name);
					set::i().main.blackListStr = dmgFuncs::i().dobavitZapyatuyuIindex(set::i().main.blackList);
					j["settings"]["default"]["blacklist"] = set::i().main.blackList;
				}
			}
		}
		else if (p == "dbl") {
			if (p2.empty()) {
				set::i().main.blackList.clear();
				set::i().main.blackListStr.clear();
			}
			else if (const auto value2{ dmgFuncs::i().strToInt(p2) - 1 }; value2 >= 0 && set::i().main.blackList.size() > static_cast<decltype(set::i().main.blackList.size())>(value2)) {
				set::i().main.blackList.erase(set::i().main.blackList.begin() + value2);
				set::i().main.blackListStr = dmgFuncs::i().dobavitZapyatuyuIindex(set::i().main.blackList);
			}
			j["settings"]["default"]["blacklist"] = set::i().main.blackList;
		}
		else if (p == "pos") {
			set::i().main.d3menuEdit ^= true;
			dmgFuncs::i().toggleCursorState(set::i().main.d3menuEdit);
			if (!set::i().main.d3menuEdit) {
				j["settings"]["default"]["pos"] = { set::i().main.d3menuX, set::i().main.d3menuY };
			}
		}
		
		set::i().saveJSON(j);
	}
}

bool dmgFuncs::checkActivate() const {
	if (set::i().main.d3menu) {
		return set::i().main.keyMode ? set::i().main.keyPressed : bKeyTable_[set::i().main.key] && !sampFuncs::i().getInput()->iInputEnabled;
	}
	return false;
}

const char* dmgFuncs::getModeStr(std::uint8_t mode) const {
	static constexpr const char* m[]{ "Little id","Little health","Little distance","Little ping", "FOV" };
	return m[mode];
}

bool dmgFuncs::remotePlayerIsListed(std::uint16_t id) const {
	if (!sampFuncs::i().remotePlayerIsListed(id)) return false;
	if (!sampFuncs::i().getRemotePlayer(id) || !sampFuncs::i().getRemotePlayerData(id)) return false;
	return true;
}

bool dmgFuncs::remotePlayerIsExist(std::uint16_t id, bool dead) const {
	if (id == std::numeric_limits<decltype(id)>::max() || !remotePlayerIsListed(id)) return false;
	const auto ped{ sampFuncs::i().getRemotePlayerSampPed(id) };
	if (!ped || !ped->pGTA_Ped) return false;
	if (dead) {
		return set::i().main.check ? ped->pGTA_Ped->state != 54 && ped->pGTA_Ped->state != 55 : sampFuncs::i().getRemotePlayerHealth(id) > 0.f;
	}
	return true;
}

vector dmgFuncs::getLocalPlayerPos(bool wInvis) const {
	const auto matrix{ sampFuncs::i().getLocalSampPed()->pGTA_Ped->base.matrix };
	return { matrix[12], matrix[13], wInvis ? matrix[14] - set::i().misc.inviZ : matrix[14] };
}

vector dmgFuncs::getRemotePlayerPos(std::uint16_t id) const {
	const auto matrix{ sampFuncs::i().getRemotePlayerSampPed(id)->pGTA_Ped->base.matrix };
	return { matrix[12], matrix[13], matrix[14] };
}

vector dmgFuncs::getRemotePlayerBonePos(std::uint16_t id, int boneID) const {
	vector ret;
	reinterpret_cast<void(__thiscall*)(void*, vector*, int, bool)>(0x5E01C0)(sampFuncs::i().getRemotePlayerSampPed(id)->pGTA_Ped, &ret, boneID, true);
	return ret;
}

std::uint8_t dmgFuncs::getExtZoom(std::uint8_t gun) const {
	static const std::unordered_map<std::uint8_t, std::uint8_t> mGetExtZoom{
		{ 30, 27 }, { 31, 27 },
		{ 33, 0 },
	};
	return mGetExtZoom.contains(gun) ? mGetExtZoom.at(gun) : 63;
}

std::uint8_t dmgFuncs::getWeaponState(std::uint8_t gun) const {
	static const std::unordered_map<std::uint8_t, std::uint8_t> mGetWeaponState{
		{ 22, 2 }, { 23, 2 }, { 24, 2 }, { 26, 2 }, { 27, 2 }, { 28, 2 }, { 29, 2 }, { 30, 2 }, { 31, 2 }, { 32, 2 }, { 37, 2 }, { 38, 2 }, { 41, 2 }, { 42, 2 },
		{ 18, 3 }, { 25, 3 }, { 33, 3 }, { 34, 3 }
	};
	return mGetWeaponState.contains(gun) ? mGetWeaponState.at(gun) : 0;
}

int dmgFuncs::getDelay() const {
	return set::i().main.delayMode ? randomValue(set::i().main.delay, set::i().main.delay2) : set::i().main.delay;
}

std::uint16_t dmgFuncs::getPlayerIdForDMG() const {
	if (!set::i().main.blackList.empty()) {
		if (const auto id{ processPlayerId(true) }; id != std::numeric_limits<decltype(id)>::max()) {
			return id;
		}
	}
	return processPlayerId();
}

void dmgFuncs::processDMG(std::uint16_t id) const {
	if (set::i().main.miss) {
		static bool tg;
		static int count, randValue;
		if (set::i().main.missMode && !tg) randValue = randomValue(set::i().main.smiss, set::i().main.smiss2);
		const auto noMiss{ set::i().main.missMode ? count < randValue : count < set::i().main.smiss };
		processPackets(id, !noMiss);
		count = noMiss ? count + 1 : 0;
		tg = noMiss;
	}
	else {
		processPackets(id);
	}
}

bool dmgFuncs::localPlayerInVehicle() const {
	return *reinterpret_cast<void**>(0xBA18FC) != nullptr;
}

bool dmgFuncs::localPlayerIsPassenger() const {
	return sampFuncs::i().getLocalSampPed()->pGTA_Ped->vehicle->passengers[0] != sampFuncs::i().getLocalSampPed()->pGTA_Ped;
}

std::uint8_t dmgFuncs::getCamMode(std::uint8_t gun) const {
	if (localPlayerInVehicle()) {
		return set::i().main.packetAimAnimVeh && localPlayerIsPassenger() ? 55 : 18;
	}
	static const std::unordered_map<std::uint8_t, std::uint8_t> mGetCamMode{
		{ 22, 53 }, { 23, 53 }, { 24, 53 }, { 25, 53 }, { 26, 53 }, { 27, 53 }, { 28, 53 }, { 29, 53 }, { 30, 53 }, { 31, 53 }, { 32, 53 }, { 33, 53 }, { 37, 53 }, { 38, 53 },
		{ 34, 7 }
	};
	return mGetCamMode.contains(gun) ? mGetCamMode.at(gun) : 4;
}

vector dmgFuncs::calcVecAimPos(const vector& targetPos) const {
	const auto myPos{ getLocalPlayerPos(set::i().misc.invi) };
	const auto angle{ getAngleFromXY(targetPos.fX - myPos.fX, targetPos.fY - myPos.fY) };
	return {
		myPos.fX - sinf(angle) * 3.5f,
		myPos.fY - cosf(angle) * 3.5f,
		myPos.fZ
	};
}

float dmgFuncs::calcfAimZ(float aim) const {
	if (localPlayerInVehicle()) {
		return 0.f;
	}
	else if (set::i().main.isWeaponForAimZ) {
		return std::abs(aim);
	}
	return 0.f;
}

void dmgFuncs::processPlayerVehPassSync(std::uint16_t id) const {
	const auto myPos{ getLocalPlayerPos(set::i().misc.invi) };

	BitStream bs;
	if (localPlayerInVehicle()) {
		if (localPlayerIsPassenger()) {
			auto data{ sampFuncs::i().getLocalPassengerData() };
			data.fPosition[2] = myPos.fZ;
			data.sKeys = 4;
			data.byteCurrentWeapon = set::i().main.gun;
			bs.Write<std::uint8_t>(PacketEnumeration::ID_PASSENGER_SYNC);
			bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
		}
		else {
			auto data{ sampFuncs::i().getLocalInCarData() };
			data.fPosition[2] = myPos.fZ;
			data.sKeys = 4;
			data.byteCurrentWeapon = set::i().main.gun;
			bs.Write<std::uint8_t>(PacketEnumeration::ID_VEHICLE_SYNC);
			bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
		}
	}
	else {
		auto data{ sampFuncs::i().getLocalOnFootData() };
		data.fPosition[2] = myPos.fZ;

		const auto remotePos{ getRemotePlayerPos(id) };
		const auto angle{ getAngleFromXY(remotePos.fX - myPos.fX, remotePos.fY - myPos.fY) };
		std::tie(data.fQuaternion[0], data.fQuaternion[3]) = calcQuaternion(angle);

		if (set::i().main.gun == 24) {
			static bool plusC{};
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
		data.byteCurrentWeapon = set::i().main.gun;
		bs.Write<std::uint8_t>(PacketEnumeration::ID_PLAYER_SYNC);
		bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
	}
	sampFuncs::i().sendPacket(&bs);
}

void dmgFuncs::processAimSync(std::uint16_t id, const vector& targetPos) const {
	stAimData data{};

	data.bUnk = 85;
	data.byteWeaponState = set::i().main.packetAimWeaponState;
	data.byteCamExtZoom = set::i().main.packetAimExtZoom;
	data.byteCamMode = dmgFuncs::i().getCamMode(set::i().main.gun);

	const auto vecAimPos{ calcVecAimPos(targetPos) };
	data.vecAimPos[0] = vecAimPos.fX;
	data.vecAimPos[1] = vecAimPos.fY;
	data.vecAimPos[2] = vecAimPos.fZ + randomValue(0.7f, 0.8f);

	const vector aimVec{
		targetPos.fX - data.vecAimPos[0],
		targetPos.fY - data.vecAimPos[1],
		targetPos.fZ - data.vecAimPos[2]
	};

	const auto vecLen{ getVecLen(aimVec) };

	data.vecAimf1[0] = aimVec.fX / vecLen;
	data.vecAimf1[1] = aimVec.fY / vecLen;
	data.vecAimf1[2] = aimVec.fZ / vecLen;

	data.fAimZ = calcfAimZ(data.vecAimf1[2]);

	BitStream bs;
	bs.Write<std::uint8_t>(PacketEnumeration::ID_AIM_SYNC);
	bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
	sampFuncs::i().sendPacket(&bs);
}

void dmgFuncs::processBulletSync(std::uint16_t id, const vector& targetPos, bool isMiss) const {
	stBulletData data{};

	data.byteType = !isMiss;
	data.byteWeaponID = set::i().main.gun;

	const auto myPos{ getLocalPlayerPos(set::i().misc.invi) }, remotePos{ getRemotePlayerPos(id) };
	const auto angle{ getAngleFromXY(remotePos.fX - myPos.fX, remotePos.fY - myPos.fY) };

	const auto localPedInVeh{ localPlayerInVehicle() };

	const vector vecOrigin{
		myPos.fX + sinf(angle) * (localPedInVeh ? randomValue(0.63f, 0.64f) : randomValue(0.29f, 0.30f)),
		myPos.fY + cosf(angle) * (localPedInVeh ? randomValue(0.63f, 0.64f) : randomValue(0.29f, 0.30f)),
		myPos.fZ + (localPedInVeh ? randomValue(1.f, 1.1f) : randomValue(0.7f, 0.8f))
	};

	data.fOrigin[0] = vecOrigin.fX + sinf(angle + 1.57f) * (localPedInVeh ? randomValue(0.19f, 0.20f) : randomValue(0.18f, 0.19f));
	data.fOrigin[1] = vecOrigin.fY + cosf(angle + 1.57f) * (localPedInVeh ? randomValue(0.19f, 0.20f) : randomValue(0.18f, 0.19f));
	data.fOrigin[2] = vecOrigin.fZ;

	data.fTarget[0] = targetPos.fX;
	data.fTarget[1] = targetPos.fY;
	data.fTarget[2] = targetPos.fZ;

	data.fCenter[0] = targetPos.fX - remotePos.fX;
	data.fCenter[1] = targetPos.fY - remotePos.fY;
	data.fCenter[2] = targetPos.fZ - remotePos.fZ;

	data.sTargetID = isMiss ? std::numeric_limits<decltype(data.sTargetID)>::max() : id;

	BitStream bs;
	bs.Write<std::uint8_t>(PacketEnumeration::ID_BULLET_SYNC);
	bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
	sampFuncs::i().sendPacket(&bs);
}

void dmgFuncs::processGiveDamage(std::uint16_t id) const {
	stGiveTakeDamage data{};

	data.id = id;
	data.damage = set::i().main.gun == 24 && !set::i().main.packetRPCdmgFromFile ? !getWeaponSkill(24) ? 23.100000f : set::i().main.packetRPCGetDmg : set::i().main.packetRPCGetDmg;
	data.weapon = set::i().main.gun;
	data.bodypart = randomValue(3, 5);

	BitStream bs;
	bs.Write(false);
	bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
	sampFuncs::i().sendRPC(RPCEnumeration::RPC_GiveTakeDamage, &bs);
}

void dmgFuncs::processReduceAmmo() const {
	auto& weapon{ sampFuncs::i().getLocalSampPed()->pGTA_Ped->weapon[set::i().main.weaponSlot] };
	if (weapon.ammo_clip > 0 && !(weapon.ammo - weapon.ammo_clip)) weapon.ammo_clip--;
	if (weapon.ammo > 0) weapon.ammo--;
}

void dmgFuncs::processWeaponSync(std::uint16_t id, bool isMiss) const {
	stWeaponsData data{};

	data.playerTarget = remotePlayerInVehicle(id) || isMiss ? std::numeric_limits<decltype(data.playerTarget)>::max() : id;
	data.actorTarget = std::numeric_limits<decltype(data.actorTarget)>::max();
	data.slot = set::i().main.weaponSlot;
	data.weapon = set::i().main.gun;
	data.ammo = sampFuncs::i().getLocalSampPed()->pGTA_Ped->weapon[set::i().main.weaponSlot].ammo;

	BitStream bs;
	bs.Write<std::uint8_t>(PacketEnumeration::ID_WEAPONS_UPDATE);
	bs.Write(reinterpret_cast<char*>(&data), sizeof(data));
	sampFuncs::i().sendPacket(&bs);
}

void dmgFuncs::processPackets(std::uint16_t id, bool isMiss) const {
	auto targetPos{ getRemotePlayerPos(id) };

	const auto rX{ isMiss ? randomValue(0.5f, 0.6f) : randomValue(0.05f, 0.10f) };
	targetPos.fX = randomValue(0, 1) ? targetPos.fX + rX : targetPos.fX - rX;

	const auto rY{ isMiss ? randomValue(0.5f, 0.6f) : randomValue(0.05f, 0.10f) };
	targetPos.fY = randomValue(0, 1) ? targetPos.fY + rY : targetPos.fY - rY;

	const auto rZ{ remotePlayerInVehicle(id) ? randomValue(0.4f, 0.5f) : randomValue(0.1f, 0.2f) };
	targetPos.fZ += isMiss ? randomValue(0.1f, 0.5f) : rZ;

	if (set::i().main.packetVehPlayerPass) processPlayerVehPassSync(id);
	if (set::i().main.packetAim) processAimSync(id, targetPos);
	if (set::i().main.packetBullet && set::i().main.isWeapon) processBulletSync(id, targetPos, isMiss);
	if (set::i().main.packetRPCdmg && !isMiss) processGiveDamage(id);
	if (set::i().main.reduceAmmo) processReduceAmmo();
	if (set::i().main.packetWeapon) processWeaponSync(id, isMiss);
}

std::uint16_t dmgFuncs::processPlayerId(bool wBlackList) const {
	auto id{ std::numeric_limits<std::uint16_t>::max() };

	auto first{ true };

	float health{};
	float dist{};
	int ping{};

	for (std::uint16_t i{}; i < limits::SAMP_MAX_PLAYERS; i++) {
		if (!remotePlayerIsExist(i)) continue;
		if (!set::i().main.checkAFK && sampFuncs::i().getRemotePlayerAFKState(i)) continue;
		const auto myPos{ getLocalPlayerPos() }, remotePos{ getRemotePlayerPos(i) };
		const auto rDist{ getDistance(myPos, remotePos) };
		if (rDist > set::i().main.radius) continue;
		if (!wBlackList) {
			if (findInVector(set::i().main.ignoreList, sampFuncs::i().getRemotePlayerName(i))) continue;
			const auto skinRes{ checkSkin(i) };
			if (!set::i().main.af && !skinRes) continue;
			if (set::i().main.af && skinRes) continue;
			if (set::i().main.nokill && sampFuncs::i().getRemotePlayerHealth(i) < set::i().main.snokill) continue;
			if (set::i().main.modeOnv && !isLineOfSightClear(&myPos, &remotePos, set::i().main.modeBCheckBuildings, set::i().main.modeBCheckVehicles, set::i().main.modeBCheckObjects)) continue;
		}
		else if (!findInVector(set::i().main.blackList, sampFuncs::i().getRemotePlayerName(i))) continue;

		switch (set::i().main.mode) {
		case 0: {
			id = i;
		} break;
		case 1: {
			const auto rHealth{ sampFuncs::i().getRemotePlayerHealth(i) + sampFuncs::i().getRemotePlayerArmour(i) };
			if (first || rHealth < health) {
				health = rHealth;
				id = i;
				first = false;
			}
		} break;
		case 2: {
			if (first || rDist < dist) {
				dist = rDist;
				id = i;
				first = false;
			}
		} break;
		case 3: {
			const auto rPing{ sampFuncs::i().getRemotePlayerPing(i) };
			if (first || rPing < ping) {
				ping = rPing;
				id = i;
				first = false;
			}
		} break;
		case 4: {
			const auto screenPos{ calcScreenCoords(remotePos) };
			if (screenPos.fZ < 1.f) continue;

			const auto screenRes{ getScreenRes() };

			const auto x{ screenRes.x * 0.5299999714f };
			const auto y{ screenRes.y * 0.4f };

			if ((screenPos.fX > x + (set::i().main.fov)) || (screenPos.fY > y + (set::i().main.fov)) || (screenPos.fX < x - (set::i().main.fov)) || (screenPos.fY < y - (set::i().main.fov))) continue;

			const auto rDist2D{ getDistance(screenPos.fX, screenPos.fY, x, y) };
			if (first || rDist2D < dist) {
				dist = rDist2D;
				id = i;
				first = false;
			}
		} break;
		}
	}
	return id;
}

bool dmgFuncs::checkSkin(std::uint16_t id) const {
	if (set::i().main.frac == 1) {
		return true;
	}
	else if (set::i().main.frac == 2) {
		return findInVector(set::i().main.skin, sampFuncs::i().getRemotePlayerSampPed(id)->pGTA_Ped->base.model_alt_id);
	}
	return false;
}

bool dmgFuncs::remotePlayerInVehicle(std::uint16_t id) const {
	return sampFuncs::i().getRemotePlayerSampPed(id)->pGTA_Ped->pedFlags[1] & 1;
}
