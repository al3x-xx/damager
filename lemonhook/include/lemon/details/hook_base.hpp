#ifndef LEMON_DETAILS_HOOK_BASE_HPP
#define LEMON_DETAILS_HOOK_BASE_HPP

#include <cstdint>

#define CPURX(reg)                   \
	union {                          \
		union {                      \
			struct {                 \
				std::uint8_t reg##L; \
				std::uint8_t reg##H; \
			};                       \
			std::uint16_t reg##X;    \
		};                           \
		std::uint32_t E##reg##X;     \
	}
#define CPUR(reg)             \
	union {                   \
		std::uint16_t reg;    \
		std::uint32_t E##reg; \
	}

namespace lemon {
	struct hook_cpu {
		CPURX(A);
		CPURX(B);
		CPURX(C);
		CPURX(D);

		CPUR(SP);
		CPUR(BP);
		CPUR(SI);
		CPUR(DI);

		union {
			struct {
				union {
					struct {
						std::uint8_t CF			: 1;
						std::uint8_t RESERVED1	: 1;
						std::uint8_t PF			: 1;
						std::uint8_t RESERVED3	: 1;
						std::uint8_t AF			: 1;
						std::uint8_t RESERVED5	: 1;
						std::uint8_t ZF			: 1;
						std::uint8_t SF			: 1;
						std::uint8_t TF			: 1;
						std::uint8_t IF			: 1;
						std::uint8_t DF			: 1;
						std::uint8_t OF			: 1;
						std::uint8_t IOPL		: 2;
						std::uint8_t NT			: 1;
						std::uint8_t RESERVED15 : 1;
					};
					std::uint16_t FLAGS;
				};
				std::uint8_t  RF		 : 1;
				std::uint8_t  VM		 : 1;
				std::uint8_t  AC		 : 1;
				std::uint8_t  VIF		 : 1;
				std::uint8_t  VIP		 : 1;
				std::uint8_t  ID		 : 1;
				std::uint16_t RESERVED22 : 10;
			};
			std::uint32_t EFLAGS;
		};
	};

	class hook_info {
	public:
		struct hook_cpu &cpu;
		std::uintptr_t  &ret_addr;
		bool			 skip_original = false;

		hook_info(struct hook_cpu &cpu, std::uintptr_t &ret_addr)
			: cpu(cpu), ret_addr(ret_addr) {}
	};
} // namespace lemon::details

#undef CPURX
#undef CPUR

#endif // LEMON_DETAILS_HOOK_BASE_HPP
