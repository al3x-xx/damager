#ifndef LEMON_DETAILS_CALLBACK_HPP
#define LEMON_DETAILS_CALLBACK_HPP

#include <functional>
#include <tuple>
#include <variant>

namespace lemon::details {
	template<typename T>
	class detour_callback;

	template<typename R, typename Orig, typename... Args>
	class detour_callback<R(Orig, Args...)> {
		using cb_empty_t	= std::function<R()>;
		using cb_origless_t = std::function<R(Args...)>;
		using cb_default_t	= std::function<R(Orig, Args...)>;

		using cb_emptiness_t = std::variant<cb_origless_t, cb_default_t>;
		using cb_full_t		 = std::variant<cb_empty_t, cb_origless_t, cb_default_t>;

	public:
		using cb_t = std::conditional_t<sizeof...(Args) != 0, cb_full_t, cb_emptiness_t>;

		explicit detour_callback() = default;
		detour_callback(const detour_callback &other) {
			cb_ = other.cb_;
		}
		detour_callback(detour_callback &&other) noexcept {
			cb_ = cb_t();
			swap(std::move(cb_), std::move(other.cb_));
		}
		template<typename Callback>
		detour_callback(const Callback &cb) {
			set(cb);
		}

		detour_callback &operator=(const detour_callback &other) {
			cb_ = other.cb_;
			return *this;
		}
		detour_callback &operator=(detour_callback &&other) noexcept {
			cb_ = {};
			swap(cb_, std::move(other.cb_));
			return *this;
		}
		template<typename Callback>
		detour_callback &operator=(const Callback &cb) {
			set(cb);
			return *this;
		}

		R operator()(Orig orig, Args... args) {
			if constexpr (sizeof...(Args) != 0) {
				if (std::get_if<cb_empty_t>(&cb_)) {
					return std::get<cb_empty_t>(cb_)();
				}
			}
			if (std::get_if<cb_origless_t>(&cb_)) {
				return std::get<cb_origless_t>(cb_)(std::forward<Args>(args)...);
			}
			return std::get<cb_default_t>(cb_)(std::forward<Orig>(orig), std::forward<Args>(args)...);
		}

	private:
		cb_t	   cb_;

		void set(const cb_t &cb) {
			cb_ = cb;
		}

		template<class C, typename... CArgs>
		void set(C *obj, R (C::*func)(CArgs...)) {
			set([obj, func](CArgs... args) -> R {
				return (obj->*func)(std::forward<CArgs>(args)...);
			});
		}
		template<class C, typename... CArgs>
		void set(const std::tuple<C *, R (C::*)(CArgs...)> &method) {
			auto [obj, func] = method;
			return set(obj, func);
		}
	};
} // namespace lemon::details

#endif // LEMON_DETAILS_CALLBACK_HPP
