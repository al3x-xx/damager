#ifndef LEMON_DETAILS_HOOK_CALLBACK_HPP
#define LEMON_DETAILS_HOOK_CALLBACK_HPP

#include <deque>
#include <functional>
#include <tuple>
#include <variant>
#include "hook_base.hpp"

namespace lemon::details {
	template<typename... Args>
	class hook_callback {
		using cb_empty_t   = std::function<void()>;
		using cb_default_t = std::function<void(Args...)>;
		using cb_cpu_t	   = std::function<void(hook_cpu &, Args...)>;
		using cb_info_t	   = std::function<void(hook_info &, Args...)>;

		using cb_emptiness_t = std::variant<cb_default_t, cb_cpu_t, cb_info_t>;
		using cb_full_t		 = std::variant<cb_empty_t, cb_default_t, cb_cpu_t, cb_info_t>;

	public:
		using cb_t = std::conditional_t<sizeof...(Args) != 0, cb_full_t, cb_emptiness_t>;

		explicit hook_callback() = default;
		hook_callback(const hook_callback &other) {
			cbs_ = other.cbs_;
		}
		hook_callback(hook_callback &&other) noexcept {
			cbs_ = std::deque<cb_t>();
			swap(std::move(cbs_), std::move(other.cb_));
		}

		void clear() {
			cbs_.clear();
		}

		hook_callback &operator=(const hook_callback &other) {
			cbs_ = other.cbs_;
			return *this;
		}
		hook_callback &operator=(hook_callback &&other) noexcept {
			cbs_ = std::deque<cb_t>();
			swap(std::move(cbs_), std::move(other.cb_));
			return *this;
		}

		template<typename Callback>
		std::size_t operator+=(const Callback &cb) {
			return connect(cb);
		}
		void operator-=(std::size_t id) {
			if (cbs_.size() <= id) return;
			cbs_.erase(cbs_.begin() + id);
		}

		void operator()(hook_info &info, Args... args) {
			for (auto &&cb : cbs_) {
				if constexpr (sizeof...(Args) != 0) {
					if (std::get_if<cb_empty_t>(&cb)) {
						std::get<cb_empty_t>(cb)();
					}
				}
				if (std::get_if<cb_default_t>(&cb)) {
					std::get<cb_default_t>(cb)(std::forward<Args>(args)...);
				} else if (std::get_if<cb_cpu_t>(&cb)) {
					std::get<cb_cpu_t>(cb)(info.cpu, std::forward<Args>(args)...);
				} else {
					std::get<cb_info_t>(cb)(info, std::forward<Args>(args)...);
				}
			}
		}

	private:
		std::deque<cb_t> cbs_;

		std::size_t connect(const cb_t &cb) {
			cbs_.push_back(cb);
			return cbs_.size() - 1;
		}

		template<class C, typename... CArgs>
		std::size_t connect(C *obj, void (C::*func)(CArgs...)) {
			return connect([obj, func](CArgs... args) {
				return (obj->*func)(std::forward<CArgs>(args)...);
			});
		}
		template<class C, typename... CArgs>
		std::size_t connect(const std::tuple<C *, void (C::*)(CArgs...)> &method) {
			auto [obj, func] = method;
			return connect(obj, func);
		}
	};
} // namespace lemon::details

#endif // LEMON_DETAILS_HOOK_CALLBACK_HPP
