#ifndef MISC_FUNCS_HPP
#define MISC_FUNCS_HPP

#include <vector>
#include <sstream>
#include <random>

#include "samp_funcs.hpp"

class miscFuncs {
public:
	template <typename T>
	[[nodiscard]] bool findInVector(const std::vector<T>& vec, const T& value) const;
	template <typename T>
	void removeInVectorByValue(std::vector<T>& vec, const T& value) const;

	template <typename T>
	[[nodiscard]] std::string dobavitZapyatuyu(const std::vector<T>& vec) const;
	[[nodiscard]] std::string dobavitZapyatuyuIindex(const std::vector<std::string>& vec) const;

	void setKeyTable(std::uint8_t key, bool res);
	[[nodiscard]] std::string getKeyNameById(std::uint32_t id) const;

	[[nodiscard]] bool checkIDLimit(int id) const;

	[[nodiscard]] std::uint32_t calcNameColor(std::uint32_t color) const;

	POINT getScreenRes() const;
	vector calcScreenCoords(const vector& vecWorld) const;

	[[nodiscard]] float getWeaponDamage(std::uint8_t gun) const;

	[[nodiscard]] std::uint8_t getWeaponSlot(std::uint8_t gun) const;

	[[nodiscard]] std::uint8_t getWeaponId(const std::string& str) const;
	[[nodiscard]] std::string getWeaponName(std::uint8_t gun) const;

	[[nodiscard]] bool isFist(std::uint8_t gun) const;
	[[nodiscard]] bool isWeapon(std::uint8_t gun) const;

	[[nodiscard]] bool isWeaponForfAimZ(std::uint8_t gun) const;

	void toggleCursorState(bool state) const;
protected:
	[[nodiscard]] std::vector<std::string> splitString(const std::string& str) const;

	[[nodiscard]] int strToInt(const std::string& str) const;

	[[nodiscard]] float getAngleFromXY(float x, float y) const;

	[[nodiscard]] std::tuple<float, float> calcQuaternion(float angle) const;

	template <typename T>
	T randomValue(T a, T b) const;

	[[nodiscard]] float getVecLen(const vector& vec) const;
	[[nodiscard]] float getDistance(const vector& vec, const vector& vec2) const;
	[[nodiscard]] float getDistance(float x, float y, float xx, float yy) const;

	char getWeaponSkill(std::uint8_t gun) const;

	bool isLineOfSightClear(const vector* orig, const vector* target, bool build, bool veh, bool obj) const;

	bool bKeyTable_[256];
};

template <typename T>
bool miscFuncs::findInVector(const std::vector<T>& vec, const T& value) const {
	return std::find(vec.begin(), vec.end(), value) != vec.end();
}

template <typename T>
void miscFuncs::removeInVectorByValue(std::vector<T>& vec, const T& value) const {
	vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}

template <typename T>
std::string miscFuncs::dobavitZapyatuyu(const std::vector<T>& vec) const {
	if (!vec.empty()) {
		std::ostringstream oss;
		std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T>{ oss, ", " });
		oss << vec.back();
		return oss.str();
	}
	return {};
}

template <typename T>
T miscFuncs::randomValue(T a, T b) const {
	static std::random_device rd;
	static std::default_random_engine e1{ rd() };
	if constexpr (std::is_same_v<T, int>) {
		std::uniform_int_distribution uniformDist{ a > b ? b : a, b < a ? a : b };
		return uniformDist(e1);
	}
	else if constexpr (std::is_same_v<T, float>) {
		std::uniform_real_distribution uniformDist{ a > b ? b : a, b < a ? a : b };
		return uniformDist(e1);
	}
	else {
		static_assert(std::_Always_false<T>, "unk type");
	}
}

#endif // MISC_FUNCS_HPP