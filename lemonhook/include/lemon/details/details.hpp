#ifndef LEMON_DETAILS_DETAILS_HPP
#define LEMON_DETAILS_DETAILS_HPP

#include <cstdint>
#include <vector>
#include <xbyak/xbyak.h>

namespace lemon::details {
	std::size_t detect_hook_size(std::uintptr_t addr, std::size_t min_size);

	[[nodiscard]] std::size_t get_patched_code_size(std::uint8_t *code, std::size_t size);
	std::size_t				  add_patched_code(Xbyak::CodeGenerator *code_gen, std::uint8_t *code, std::size_t size, std::uintptr_t runtime_addr);
} // namespace lemon::details

#endif // LEMON_DETAILS_DETAILS_HPP
