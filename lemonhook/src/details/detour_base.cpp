#include "details/detour_base.hpp"
#include <cstring>
#include <stdexcept>
#include "details/details.hpp"
#include "mem.hpp"

#define RESET_AND_THROW(e)           \
	{                                \
		reset();                     \
		throw std::runtime_error(e); \
	}

lemon::details::detour_base::detour_base(std::uintptr_t addr, bool clean_context, std::string_view mod) noexcept
	: addr_(addr), clean_stuck_(clean_context), mod_(mod) {}

lemon::details::detour_base::~detour_base() noexcept {
	reset();
}

std::uintptr_t lemon::details::detour_base::addr() const {
	return addr_;
}

const std::string &lemon::details::detour_base::mod() const {
	return mod_;
}

void lemon::details::detour_base::set_addr(std::uintptr_t addr) {
	if (!installed()) addr_ = addr;
}

void lemon::details::detour_base::set_mod(std::string_view mod) {
	if (!installed()) mod_ = mod;
}

std::uintptr_t lemon::details::detour_base::original_addr() const {
	return original_addr_;
}

std::size_t lemon::details::detour_base::original_size() const {
	return original_size_;
}

void lemon::details::detour_base::reset() {
	remove();
	if (trampoline_) trampoline_ = nullptr;
	if (original_code_) delete[] original_code_, original_code_ = nullptr;
	original_addr_ = original_size_ = patched_code_size_ = 0;
}

void lemon::details::detour_base::install(void *detour, std::uintptr_t add_var_ptr) {
	using namespace Xbyak::util;

	if (installed()) return;

	bool mem_exists = trampoline_ != nullptr;

	if (!mem_exists) {
		if (!mod_.empty()) {
			auto mod_handle = GetModuleHandleA(mod_.c_str());
			if (mod_handle == nullptr || mod_handle == INVALID_HANDLE_VALUE) RESET_AND_THROW("module is invalid");
			original_addr_ = addr_ + reinterpret_cast<std::uintptr_t>(mod_handle);
		} else {
			original_addr_ = addr_;
		}

		original_size_ = detect_hook_size(original_addr_, 0);
		if (!original_size_) RESET_AND_THROW("function is too small for hooking");
		original_code_ = new std::uint8_t[original_size_];
		std::memcpy(original_code_, reinterpret_cast<void *>(original_addr_), original_size_);
		trampoline_ = new Xbyak::CodeGenerator();
	} else {
		trampoline_->reset();
	}

	trampoline_->mov(ptr[&context_.eax], eax); // save eax
	// save ret address
	trampoline_->pop(eax);
	trampoline_->mov(ptr[&context_.ret_addr], eax);
	trampoline_->mov(eax, ptr[&context_.eax]); // restore eax
	// push additional variable if exists
	if (add_var_ptr) {
		trampoline_->push(dword, add_var_ptr);
	} else {
		trampoline_->nop(5);
	}

	trampoline_->call(detour);
	// restore orig stack
	if (clean_stuck_ && add_var_ptr) {
		trampoline_->add(esp, 4);
	} else {
		trampoline_->nop(3);
	}
	trampoline_->mov(ptr[&context_.eax], eax); // save eax
	// push original return address
	trampoline_->mov(eax, ptr[&context_.ret_addr]);
	trampoline_->push(eax);
	trampoline_->mov(eax, ptr[&context_.eax]); // restore eax
	trampoline_->ret();

	patched_code_size_ = add_patched_code(trampoline_, original_code_, original_size_, original_addr_);
	if (!patched_code_size_) RESET_AND_THROW("can't patch original code");
	trampoline_->jmp(reinterpret_cast<void *>(original_addr_ + original_size_));

	auto		rel_addr = reinterpret_cast<std::uintptr_t>(trampoline_->getCode()) - (original_addr_ + 5);
	mem::unprot unprot(original_addr_, original_size_);
	*reinterpret_cast<std::uint8_t *>(original_addr_)		= 0xE9;
	*reinterpret_cast<std::uintptr_t *>(original_addr_ + 1) = rel_addr;
	if (original_size_ > 5) {
		std::memset(reinterpret_cast<void *>(original_addr_ + 5), 0x90, original_size_ - 5);
	}

	installed_ = true;
}

void lemon::details::detour_base::remove() {
	if (!installed()) return;

	auto rel_addr = trampoline_->getCode<std::uintptr_t>() - (original_addr_ + 5);

	auto op	  = *reinterpret_cast<std::uint8_t *>(original_addr_);
	auto addr = *reinterpret_cast<std::uintptr_t *>(original_addr_ + 1);
	if (op == 0xE9 && addr == rel_addr) {
		mem::safe_copy(reinterpret_cast<void *>(original_addr_), original_code_, original_size_);

		delete trampoline_, trampoline_			= nullptr;
		delete[] original_code_, original_code_ = nullptr;
		original_addr_ = original_size_ = patched_code_size_ = 0;
	} else {
		trampoline_->resetSize();
		trampoline_->nop(46);
		trampoline_->setSize(trampoline_->getSize() + patched_code_size_ + 46);
	}

	installed_ = false;
}

bool lemon::details::detour_base::installed() const {
	return installed_;
}

std::uint8_t *lemon::details::detour_base::trampoline() {
	if (!installed()) return nullptr;
	return trampoline_->getCode<std::uint8_t *>() + 46;
}

#undef RESET_AND_THROW