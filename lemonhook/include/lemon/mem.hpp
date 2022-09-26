#ifndef LEMON_MEMORY_HPP
#define LEMON_MEMORY_HPP

#include <cstddef>
#include <cstdint>
#include <windows.h>

namespace lemon::mem {
	class unprot {
	public:
		unprot(std::uintptr_t addr, std::size_t size);
		unprot(void *ptr, std::size_t size);
		~unprot();

	private:
		std::uintptr_t addr_ = 0;
		std::size_t	   size_ = 0;

		DWORD old_prot_ = 0;
	};

	void safe_copy(void *dst, const void *src, std::size_t size);
	void safe_copy(std::uintptr_t dst, std::uintptr_t src, std::size_t size);

	void safe_set(void *dst, std::uint8_t value, std::size_t size);
	void safe_set(std::uintptr_t dst, std::uint8_t value, std::size_t size);

	bool compare_bytes(const std::uint8_t *data, const std::uint8_t *bytes, const char *mask);
	bool compare_bytes(const std::uint8_t *data, const char *bytes, const char *mask);

	std::uintptr_t find_pattern(std::uintptr_t base, std::size_t len, const std::uint8_t *bytes, const char *mask);
	std::uintptr_t find_pattern(std::uintptr_t base, std::size_t len, const char *bytes, const char *mask);
} // namespace lemon::mem

#endif // LEMON_MEMORY_HPP
