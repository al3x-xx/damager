#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "json/json.hpp"

class settings {
public:
	settings();

	static inline std::unique_ptr<settings> i;

	[[nodiscard]] std::tuple<bool, nlohmann::json> getJson(const std::string& path = {}) const noexcept;
	void saveJson(const nlohmann::json& j) const;
private:
	void processWeaponSettings(const nlohmann::json& j, std::uint8_t gun, bool mode);
public:
	void loadWeaponSettings(const nlohmann::json& j, std::uint8_t gun, bool mode);
	void loadWeaponSettings(std::uint8_t gun, bool mode);

	void processWeaponSettings(nlohmann::json& j) const;

	void saveMenuSettings() const;
	void saveMenuVisuals() const;
	void saveMenuMisc() const;
private:
	void loadSettings(nlohmann::json& j);
	void saveSettings(nlohmann::json& j) const;
public:
	struct {
		bool packetVehPlayerPass{ true };
		bool packetAim{ true }, packetAimAnimVeh{ true };
		bool packetBullet{ true };
		bool packetWeapon{ true };
		bool packetRpcdmg{ true }, packetRpcdmgFromFile;
		float packetRpcGetDmg;

		std::uint8_t packetAimExtZoom, packetAimWeaponState;

		std::uint32_t key{ 82 };
		std::string keyName{ "R" };
		bool keyChange, keyPressed, keyMode;

		bool check, checkAfk{ true };

		std::uint8_t mode{ 4 };
		const char* imguiModeItem;
		bool modeOnv, modeBCheckBuildings{ true }, modeBCheckVehicles{ true }, modeBCheckObjects{ true };
		int fov{ 200 };

		std::string fracStr{ "None" };
		std::vector<std::string> fracName;
		std::vector<std::uint16_t> skin;
		int frac;
		bool af;

		bool sMode{ true }, settingsForServer, reduceAmmo{ true };
		//
		std::uint8_t gun, weaponSlot;
		std::string_view imguiWeaponItem;
		
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
		float d3menuX, d3menuY;
	} main{};

	struct {
		bool showFov{ true };
		bool renderLine{ true };
		bool wallHack, showSkinId;
		bool showIgnoreList{ true }, showBlackList{ true };
	} vis{};

	struct {
		std::uint32_t menuKey{ 113 };
		std::string menuKeyName{ "F2" };
		bool menuKeyChange;

		bool invis;
		int invisz{ 5 };
	} misc{};
};

#endif