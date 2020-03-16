#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <memory>

class CPU6502;
class CPU6502Disassembler
{
public:
	class Instruction
	{
	public:
		uint16_t pc_;
		uint8_t opcode_;
		uint8_t oper1_;
		uint8_t oper2_;
		uint8_t instruction_cycles_;	//duration of this instruction
		uint8_t elapsed_instruction_cycles_;
		uint64_t current_cpu_cycle_;	// curent cpu cycle.
		std::string instruction_name_;
		std::string addressing_mode_;
		std::string assembly_code_;
		std::string register_snapshot_;
	};
	//return the current pc
	uint16_t Disassemble_current_instruction(CPU6502* cpu);

	//address / instruction
	std::map<uint16_t, std::shared_ptr<Instruction> > instructions_;
};