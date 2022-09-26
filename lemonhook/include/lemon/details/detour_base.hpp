#ifndef LEMON_DETAILS_BASE_DETOUR_HPP
#define LEMON_DETAILS_BASE_DETOUR_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <xbyak/xbyak.h>

namespace lemon::details {
	class detour_base {
		struct context {
			std::uint32_t  eax		= 0;
			std::uintptr_t ret_addr = 0;
		};

	public:
		explicit detour_base(std::uintptr_t addr, bool clean_context, std::string_view mod = "") noexcept;
		virtual ~detour_base() noexcept;

		[[nodiscard]] std::uintptr_t	 addr() const;
		[[nodiscard]] const std::string &mod() const;

		void set_addr(std::uintptr_t addr);
		void set_mod(std::string_view mod);

		[[nodiscard]] std::uintptr_t original_addr() const;
		[[nodiscard]] std::size_t	 original_size() const;

		void reset();
		void remove();

		[[nodiscard]] bool installed() const;

	protected:
		void install(void *detour, std::uintptr_t add_var_ptr = 0);

		[[nodiscard]] std::uint8_t *trampoline();

		template<typename F>
		[[nodiscard]] F trampoline() {
			return reinterpret_cast<F>(trampoline());
		}

	private:
		std::uintptr_t addr_		= 0;
		bool		   clean_stuck_ = false;
		std::string	   mod_;

		std::uintptr_t original_addr_ = 0;
		std::uint8_t	 *original_code_ = nullptr;
		std::size_t	   original_size_ = 0;

		Xbyak::CodeGenerator *trampoline_		 = nullptr;
		std::size_t			  patched_code_size_ = 0;
		bool				  installed_		 = false;

		struct context context_;
	};
} // namespace lemon::details

#endif // LEMON_DETAILS_BASE_DETOUR_HPP