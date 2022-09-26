#ifndef LEMON_DETOUR_HPP
#define LEMON_DETOUR_HPP

#include <string>
#include <string_view>
#include <windows.h>
#include "details/detour_base.hpp"
#include "details/detour_callback.hpp"

namespace lemon {
	template<typename T>
	class detour;

	template<typename R, typename... Args>
	class detour<R(__cdecl *)(Args...)> : public details::detour_base {
	public:
		using orig_t	 = R(__cdecl *)(Args...);
		using callback_t = details::detour_callback<R(orig_t, Args...)>;

		explicit detour(std::uintptr_t addr, std::string_view mod = "") noexcept
			: details::detour_base(addr, true, mod) {}

		void install(const callback_t &func) {
			func_ = func;
			details::detour_base::install(reinterpret_cast<void *>(&relay), reinterpret_cast<std::uintptr_t>(this));
		}

		[[nodiscard]] orig_t orig_func() {
			return details::detour_base::trampoline<orig_t>();
		}

	private:
		callback_t func_;

		static R __cdecl relay(detour<R(__cdecl *)(Args...)> *this_ptr, Args... args) {
			return this_ptr->func_(this_ptr->orig_func(), std::forward<Args>(args)...);
		}
	};

	template<typename R, typename... Args>
	class detour<R(__stdcall *)(Args...)> : public details::detour_base {
	public:
		using orig_t	 = R(__stdcall *)(Args...);
		using callback_t = details::detour_callback<R(orig_t, Args...)>;

		explicit detour(std::uintptr_t addr, std::string_view mod = "") noexcept
			: details::detour_base(addr, false, mod) {}

		void install(const callback_t &func) {
			func_ = func;
			details::detour_base::install(reinterpret_cast<void *>(&relay), reinterpret_cast<std::uintptr_t>(this));
		}

		[[nodiscard]] orig_t orig_func() {
			return details::detour_base::trampoline<orig_t>();
		}

	private:
		callback_t func_;

		static R __stdcall relay(detour<R(__stdcall *)(Args...)> *this_ptr, Args... args) {
			return this_ptr->func_(this_ptr->orig_func(), std::forward<Args>(args)...);
		}
	};

	template<typename R, typename C, typename... Args>
	class detour<R(__thiscall *)(C *, Args...)> : public details::detour_base {
	public:
		using orig_t	 = R(__thiscall *)(C *, Args...);
		using callback_t = details::detour_callback<R(orig_t, C *, Args...)>;

		explicit detour(std::uintptr_t addr, std::string_view mod = "") noexcept
			: details::detour_base(addr, false, mod) {}

		void install(const callback_t &func) {
			func_ = func;
			details::detour_base::install(reinterpret_cast<void *>(&relay), reinterpret_cast<std::uintptr_t>(this));
		}

		[[nodiscard]] orig_t orig_func() {
			return details::detour_base::trampoline<orig_t>();
		}

	private:
		callback_t func_;

		static R __fastcall relay(C *ecx, void *, detour<R(__thiscall *)(C *, Args...)> *this_ptr, Args... args) {
			return this_ptr->func_(this_ptr->orig_func(), std::forward<C *>(ecx), std::forward<Args>(args)...);
		}
	};
} // namespace lemon

#endif // LEMON_DETOUR_HPP
