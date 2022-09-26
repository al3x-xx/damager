#ifndef LEMON_HOOK_HPP
#define LEMON_HOOK_HPP

#include <cstring>
#include <stdexcept>
#include <xbyak/xbyak.h>
#include "details/details.hpp"
#include "details/hook_callback.hpp"
#include "mem.hpp"

#define RESET_AND_THROW(e)           \
	{                                \
		reset();                     \
		throw std::runtime_error(e); \
	}

namespace lemon {
	template<typename... Args>
	class hook {
	public:
		enum class stage { wait,
						   install,
						   remove,
						   before,
						   after };

		explicit hook(std::uintptr_t addr, std::string_view mod = "") noexcept
			: addr_(addr), mod_(mod) {}
		explicit hook(std::uintptr_t addr, std::size_t size, std::string_view mod = "") noexcept
			: addr_(addr), size_(size), mod_(mod) {}
		virtual ~hook() noexcept {
			reset();
		}

		[[nodiscard]] std::uintptr_t addr() const {
			return addr_;
		}
		[[nodiscard]] std::size_t size() const {
			return size_;
		}
		[[nodiscard]] const std::string &mod() const {
			return mod_;
		}

		void set_addr(std::uintptr_t addr) {
			if (!installed()) addr_ = addr;
		}
		void set_size(std::size_t size) {
			if (!installed()) size_ = size;
		}
		void set_mod(std::string_view mod) {
			if (!installed()) mod_ = mod;
		}

		[[nodiscard]] std::uintptr_t original_addr() const {
			return original_addr_;
		}
		[[nodiscard]] std::size_t original_size() const {
			return original_size_;
		}

		void reset() {
			remove();
			if (original_code_) delete[] original_code_, original_code_ = nullptr;
			original_addr_ = original_size_ = 0;
			if (code_) code_ = nullptr;
			stage_ = stage::wait;
		}

		void install(std::size_t before_stack_off = 0, std::size_t after_stack_off = 0) {
			using namespace Xbyak::util;

			if (stage_ != stage::wait) return;
			if (installed()) return;
			stage_ = stage::install;

			bool mem_exists = code_ != nullptr;

			if (!mem_exists) {
				if (!mod_.empty()) {
					auto mod_handle = GetModuleHandleA(mod_.c_str());
					if (mod_handle == nullptr || mod_handle == INVALID_HANDLE_VALUE) RESET_AND_THROW("module is invalid");
					original_addr_ = addr_ + reinterpret_cast<std::uintptr_t>(mod_handle);
				} else {
					original_addr_ = addr_;
				}

				original_size_ = details::detect_hook_size(original_addr_, size_);
				if (!original_size_) RESET_AND_THROW("function is too small for hooking");
				original_code_ = new std::uint8_t[original_size_];
				std::memcpy(original_code_, reinterpret_cast<void *>(original_addr_), original_size_);
				code_ = new Xbyak::CodeGenerator();
			} else {
				code_->reset();
			}

			// save EAX
			code_->mov(ptr[&cpu_.EAX], eax);
			// save return address
			code_->pop(eax);
			code_->mov(ptr[&ret_addr_], eax);
			save_cpu_no_eax();

			// repush args
			if constexpr (sizeof...(Args) > 0) {
				code_->mov(edx, esp);
				for (int i = sizeof...(Args) - 1; i >= 0; --i) {
					auto off = before_stack_off + i * 4;
					code_->add(edx, off);
					code_->push(edx);
					code_->sub(edx, off);
				}
			}
			// call on before
			code_->mov(ecx, reinterpret_cast<std::uintptr_t>(this));
			auto before_method = &hook<Args...>::before;
			code_->call(reinterpret_cast<void *&>(before_method));

			// skip original code
			auto patched_code_size = details::get_patched_code_size(original_code_, original_size_);
			if (!patched_code_size) RESET_AND_THROW("can't patch original code");
			code_->test(eax, eax);
			std::uint8_t jnz[6]							= { 0x0F, 0x85, 0, 0, 0, 0 };
			*reinterpret_cast<std::uint32_t *>(jnz + 2) = 108 + patched_code_size;
			code_->db(jnz, 6);

			// restore cpu
			restore_cpu_no_eax();
			code_->mov(eax, ptr[&cpu_.EAX]);
			// push original code
			details::add_patched_code(code_, original_code_, original_size_, original_addr_);
			// save cpu
			code_->mov(ptr[&cpu_.EAX], eax);
			save_cpu_no_eax();

			// repush args
			if constexpr (sizeof...(Args) > 0) {
				code_->mov(edx, esp);
				for (int i = sizeof...(Args) - 1; i >= 0; --i) {
					auto off = after_stack_off + i * 4;
					code_->add(edx, off);
					code_->push(edx);
					code_->sub(edx, off);
				}
			}
			// call on after
			code_->mov(ecx, reinterpret_cast<std::uintptr_t>(this));
			auto after_method = &hook<Args...>::after;
			code_->call(reinterpret_cast<void *&>(after_method));

			// restore cpu
			restore_cpu_no_eax();
			// restore ret addr
			code_->mov(eax, ptr[&ret_addr_]);
			code_->push(eax);
			code_->mov(eax, ptr[&cpu_.EAX]);
			code_->ret();

			if (!mem_exists) {
				auto		rel_addr = reinterpret_cast<std::uintptr_t>(code_->getCode()) - (original_addr_ + 5);
				mem::unprot unprot(original_addr_, original_size_);
				*reinterpret_cast<std::uint8_t *>(original_addr_)		= 0xE8;
				*reinterpret_cast<std::uintptr_t *>(original_addr_ + 1) = rel_addr;
				if (original_size_ > 5) {
					std::memset(reinterpret_cast<void *>(original_addr_ + 5), 0x90, original_size_ - 5);
				}
			}

			stage_	   = stage::wait;
			installed_ = true;
		}
		void remove() {
			using namespace Xbyak::util;

			while (stage_ != stage::wait) {}
			if (!installed()) return;
			stage_ = stage::remove;

			auto rel_addr = reinterpret_cast<std::uintptr_t>(code_->getCode()) - (original_addr_ + 5);

			auto op	  = *reinterpret_cast<std::uint8_t *>(original_addr_);
			auto addr = *reinterpret_cast<std::uintptr_t *>(original_addr_ + 1);
			if (op == 0xE8 && addr == rel_addr) {
				mem::safe_copy(reinterpret_cast<void *>(original_addr_), original_code_, original_size_);

				delete[] original_code_, original_code_ = nullptr;
				original_addr_ = original_size_ = 0;
				delete code_, code_ = nullptr;
			} else {
				code_->resetSize();

				auto save_EAX	   = new std::uint32_t(0);
				auto save_ret_addr = new std::uintptr_t(0);

				// save EAX
				code_->mov(ptr[save_EAX], eax);
				// save return address
				code_->pop(eax);
				code_->mov(ptr[save_ret_addr], eax);

				// restore EAX
				code_->mov(eax, ptr[save_EAX]);
				// add patched original code
				auto patched_code_size = details::add_patched_code(code_, original_code_, original_size_, original_addr_);
				// save EAX
				code_->mov(ptr[save_EAX], eax);

				// restore return address
				code_->mov(eax, ptr[save_ret_addr]);
				code_->push(eax);
				// restore EAX
				code_->mov(eax, ptr[save_EAX]);
				// ret
				code_->ret();
				// nop
				code_->nop(289 + patched_code_size);
			}

			stage_	   = stage::wait;
			installed_ = false;
		}

		[[nodiscard]] bool installed() const {
			return installed_;
		}

		details::hook_callback<Args &...> on_before;
		details::hook_callback<Args &...> on_after;

	private:
		std::uintptr_t addr_ = 0;
		std::size_t	   size_ = 0;
		std::string	   mod_;

		std::uintptr_t original_addr_ = 0;
		std::uint8_t  *original_code_ = nullptr;
		std::size_t	   original_size_ = 0;
		stage		   stage_		  = stage::wait;

		Xbyak::CodeGenerator *code_		 = nullptr;
		bool				  installed_ = false;

		struct hook_cpu cpu_;
		std::uintptr_t	ret_addr_ = 0;

		bool before(Args &...args) {
			stage_ = stage::before;
			hook_info info(cpu_, ret_addr_);
			on_before(info, std::forward<Args &>(args)...);
#if defined(_GLIBCXX_STRING) || defined(_MSC_VER)
			[[maybe_unused]] std::function<void()> gcc10_cruch = [] {
			};
#endif
			stage_ = stage::wait;
			return info.skip_original;
		}
		void after(Args &...args) {
			stage_ = stage::after;
			hook_info info(cpu_, ret_addr_);
			on_after(info, std::forward<Args &>(args)...);
#if defined(_GLIBCXX_STRING) || defined(_MSC_VER)
			[[maybe_unused]] std::function<void()> gcc10_cruch = [] {
			};
#endif
			stage_ = stage::wait;
		}

		void save_cpu_no_eax() {
			using namespace Xbyak::util;

			// Save registers
			code_->mov(ptr[&cpu_.EBX], ebx);
			code_->mov(ptr[&cpu_.ECX], ecx);
			code_->mov(ptr[&cpu_.EDX], edx);

			code_->mov(ptr[&cpu_.ESP], esp);
			code_->mov(ptr[&cpu_.EBP], ebp);
			code_->mov(ptr[&cpu_.ESI], esi);
			code_->mov(ptr[&cpu_.EDI], edi);

			// Save flags
			code_->pushf();
			code_->pop(eax);
			code_->mov(ptr[&cpu_.EFLAGS], eax);
		}
		void restore_cpu_no_eax() {
			using namespace Xbyak::util;

			// Restore flags
			code_->mov(eax, ptr[&cpu_.EFLAGS]);
			code_->push(eax);
			code_->popf();

			// Restore registers
			code_->mov(edi, ptr[&cpu_.EDI]);
			code_->mov(esi, ptr[&cpu_.ESI]);
			code_->mov(ebp, ptr[&cpu_.EBP]);
			code_->mov(esp, ptr[&cpu_.ESP]);

			code_->mov(edx, ptr[&cpu_.EDX]);
			code_->mov(ecx, ptr[&cpu_.ECX]);
			code_->mov(ebx, ptr[&cpu_.EBX]);
		}
	};
} // namespace lemon

#undef RESET_AND_THROW

#endif // LEMON_HOOK_HPP
