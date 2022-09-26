#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "json/json.hpp"

class set {
	set();
public:
	static set& i();

	[[nodiscard]] std::tuple<bool, nlohmann::json> getJSON(const std::string& path = "") const;
	void saveJSON(const nlohmann::json& j) const;

	void loadWeaponSettings(std::uint8_t gun, bool mode);

	void processWeaponSettings(nlohmann::json& j) const;

	void saveMenuSettings() const;
	void saveMenuVisuals() const;
	void saveMenuMisc() const;

	void loadSettings();
	void saveSettings() const;
private:
	struct main_ {
		bool packetVehPlayerPass{ true };
		bool packetAim{ true }, packetAimAnimVeh{ true };
		bool packetBullet{ true };
		bool packetWeapon{ true };
		bool packetRPCdmg{ true }, packetRPCdmgFromFile;
		float packetRPCGetDmg;

		std::uint8_t packetAimExtZoom, packetAimWeaponState;

		std::uint32_t key{ 82 };
		std::string keyName{ "R" };
		bool keyChange, keyPressed, keyMode;

		bool check, checkAFK{ true };

		std::uint8_t mode{ 4 };
		std::string imguiModeItem;
		bool modeOnv, modeBCheckBuildings{ true }, modeBCheckVehicles{ true }, modeBCheckObjects{ true };
		int fov{ 200 };

		std::string fracStr{ "None" };
		std::vector<std::string> fracName;
		std::vector<std::uint16_t> skin;
		int frac;
		bool af;

		bool sMode{ true }, settingsForServer{ false }, reduceAmmo{ true };
		//
		std::uint8_t gun, weaponSlot;
		std::string imguiWeaponItem;
		
		int delay, delay2;
		bool delayMode;

		int radius;

		int smiss, smiss2;
		bool miss, missMode;
		
		int snokill;
		bool nokill;

		bool isWeapon;
		bool isWeaponForAimZ;
		//
		std::vector<std::string> ignoreList, blackList;
		std::string ignoreListStr, blackListStr;

		std::uint16_t dmgId;
		std::string dmgStr;

		bool imguiMenu, d3menu, d3menuEdit;
		float imguiMenuPosX, d3menuX, d3menuY;
	};
	struct vis_ {
		bool showFov{ true };
		bool renderLine{ true };
		bool wallHack, showSkinID;
		bool showIgnoreList{ true }, showBlackList{ true };
	};
	struct misc_ {
		std::uint32_t menuKey{ 113 };
		std::string menuKeyName{ "F2" };
		bool menuKeyChange;

		int inviZ{ 5 };
		bool invi;		
	};
public:
	main_ main;
	vis_ vis;
	misc_ misc;
};

#endif