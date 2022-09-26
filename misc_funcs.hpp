#ifndef MISC_FUNCS_HPP
#define MISC_FUNCS_HPP

#include <random>
#include <ranges>

#include "samp_funcs.hpp"

class miscFuncs {
public:
	[[nodiscard]] bool findValue(const auto& buf, const auto& value_) const;
	void removeValue(auto& buf, const auto& value_) const;

	[[nodiscard]] std::string dobavitZapyatuyu(const auto& buf) const;
	[[nodiscard]] std::string dobavitZapyatuyuIindex(const std::vector<std::string>& vec) const;

	void setKeyState(std::uint8_t key, bool state);
	[[nodiscard]] std::string getKeyNameById(std::uint32_t id) const;

	[[nodiscard]] bool checkIDLimit(int id) const;

	[[nodiscard]] std::uint32_t calcNameColor(std::uint32_t color) const;

	[[nodiscard]] const POINT& getScreenRes() const;
	[[nodiscard]] vector3 calcScreenCoords(const vector3& vecWorld) const;

	[[nodiscard]] float getWeaponDamage(std::uint8_t gun) const;

	[[nodiscard]] std::uint8_t getWeaponSlot(std::uint8_t gun) const;

	[[nodiscard]] std::uint8_t getWeaponId(const std::string& str) const;
	[[nodiscard]] std::string getWeaponName(std::uint8_t gun) const;

	[[nodiscard]] bool isFist(std::uint8_t gun) const;
	[[nodiscard]] bool isWeapon(std::uint8_t gun) const;

	[[nodiscard]] bool isWeaponForfAimZ(std::uint8_t gun) const;

	void toggleCursorState(bool state) const;
protected:
	[[nodiscard]] std::optional<int> strToInt(const std::string& str) const;

	[[nodiscard]] std::tuple<float, float> calcQuaternion(float angle) const;

	template <typename T>
	[[nodiscard]] T randomValue(T a, T b) const;

	[[nodiscard]] float getVecLen(const vector3& vec) const;
	[[nodiscard]] float getDistance(const vector3& vec, const vector3& vec2) const;
	[[nodiscard]] float getDistance(float x, float y, float x2, float y2) const;

	[[nodiscard]] char getWeaponSkill(std::uint8_t gun) const;

	[[nodiscard]] bool isLineOfSightClear(const vector3* orig, const vector3* target, bool build, bool veh, bool obj) const;

	bool keyState_[256];
};

bool miscFuncs::findValue(const auto& buf, const auto& value_) const {
	return std::find(buf.begin(), buf.end(), value_) != buf.end();
}

void miscFuncs::removeValue(auto& buf, const auto& value_) const {
	buf.erase(std::remove(buf.begin(), buf.end(), value_), buf.end());
}

std::string miscFuncs::dobavitZapyatuyu(const auto& buf) const {
	using namespace std::literals::string_view_literals;

	if constexpr (std::is_arithmetic_v<std::remove_reference_t<decltype(buf[0])>>) {
		return buf | std::ranges::views::transform([](const auto value_) {
			return std::to_string(value_);
		}) | std::ranges::views::join_with(", "sv) | std::ranges::to<std::string>();
	}
	else {
		return buf | std::ranges::views::join_with(", "sv) | std::ranges::to<std::string>();
	}
}

template <typename T>
T miscFuncs::randomValue(T a, T b) const {
	static std::random_device rd;
	static std::default_random_engine dre{ rd() };
	if constexpr (std::is_integral_v<T>) {
		std::uniform_int_distribution uid{ a > b ? b : a, b < a ? a : b };
		return uid(dre);
	}
	else if constexpr (std::is_floating_point_v<T>) {
		std::uniform_real_distribution urd{ a, b };
		return urd(dre);
	}
	else {
		static_assert(std::_Always_false<T>, "unk type");
	}
}

#endif // MISC_FUNCS_HPP