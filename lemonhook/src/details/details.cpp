#include "details/details.hpp"
#include <cstring>
#include <Zydis/Zydis.h>

std::size_t lemon::details::detect_hook_size(std::uintptr_t addr, std::size_t min_size) {
	static constexpr ZyanUSize max_ops_length = 5 * 15;
	min_size								  = min_size >= 5 ? min_size : 5;

	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32);
	auto					data   = reinterpret_cast<std::uint8_t *>(addr);
	ZyanUSize				offset = 0;
	ZydisDecodedInstruction instruction;
	while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data + offset, max_ops_length - offset, &instruction)) && offset < min_size) {
		offset += instruction.length;
	}

	return offset < min_size ? 0 : offset;
}

std::size_t lemon::details::get_patched_code_size(std::uint8_t *code, std::size_t size) {
	std::size_t patched_code_size = 0;

	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32);
	ZydisDecodedInstruction instruction;
	for (auto i = 0u; i < size;) {
		if (!ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, code + i, size, &instruction))) continue;

		if (instruction.raw.imm[0].is_relative) {
			// short JMP or 1 byte JCC
			if ((instruction.opcode == 0xEB || (instruction.opcode & 0xF0) == 0x70) &&
				instruction.operands[0].imm.value.u <= size - i) {
				patched_code_size += instruction.length;
			} else {
				ZyanUSize new_instruction_size = 0;

				if ((instruction.opcode & 0xF0) == 0x70 ||
					(instruction.opcode_map == ZYDIS_OPCODE_MAP_0F && (instruction.opcode & 0xF0) == 0x80)) {
					new_instruction_size = 6;
				} else {
					new_instruction_size = 5;
				}
				patched_code_size += new_instruction_size;
			}
		} else {
			patched_code_size += instruction.length;
		}

		i += instruction.length;
	}

	return patched_code_size;
}

std::size_t lemon::details::add_patched_code(Xbyak::CodeGenerator *code_gen, std::uint8_t *code, std::size_t size, std::uintptr_t runtime_addr) {
	auto before_code_size = code_gen->getSize();

	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32);
	ZydisDecodedInstruction instruction;
	for (auto i = 0u; i < size;) {
		if (!ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, code + i, size, &instruction))) continue;

		if (instruction.raw.imm[0].is_relative) {
			// short JMP or 1 byte JCC
			if ((instruction.opcode == 0xEB || (instruction.opcode & 0xF0) == 0x70) &&
				instruction.operands[0].imm.value.u <= size - i) {
				code_gen->db(code + i, instruction.length);
			} else {
				ZyanU64 curr_runtime_addr = runtime_addr + i;
				ZyanU64 abs_addr		  = 0;
				ZydisCalcAbsoluteAddress(&instruction, instruction.operands, curr_runtime_addr, &abs_addr);

				ZyanUSize	  new_instruction_size	= 0;
				std::uint8_t *new_instruction		= nullptr;
				ZyanUSize	  new_instruction_index = 0;

				if ((instruction.opcode & 0xF0) == 0x70 ||
					(instruction.opcode_map == ZYDIS_OPCODE_MAP_0F && (instruction.opcode & 0xF0) == 0x80)) {
					new_instruction_size					 = 6;
					new_instruction							 = new std::uint8_t[new_instruction_size];
					new_instruction[new_instruction_index++] = 0x0F;
					new_instruction[new_instruction_index++] = 0x80 | (instruction.opcode & 0x0F);
				} else {
					new_instruction_size					 = 5;
					new_instruction							 = new std::uint8_t[new_instruction_size];
					new_instruction[new_instruction_index++] = instruction.opcode == 0xEB ? 0xE9 : instruction.opcode;
				}
				auto new_rel_addr = abs_addr - (code_gen->getCurr<std::uintptr_t>() + new_instruction_size);
				*reinterpret_cast<ZyanU32 *>(new_instruction + new_instruction_index) = static_cast<ZyanU32>(new_rel_addr);
				code_gen->db(new_instruction, new_instruction_size);
				delete[] new_instruction;
			}
		} else {
			code_gen->db(code + i, instruction.length);
		}

		i += instruction.length;
	}

	return code_gen->getSize() - before_code_size;
}