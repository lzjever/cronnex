#include "CPU6502Disassembler.h"
#include "common/Assert.h"
#include "CPU6502.h"


CPU6502Disassembler::Instruction::Instruction(uint16_t pc, uint8_t opcode, uint8_t oper1, uint8_t oper2, uint8_t inst_cycles,
	uint8_t elapsed_inst_cycle, uint64_t cpu_cycle)
	:pc_(pc), opcode_(opcode), oper1_(oper1), oper2_(oper2),
	instruction_cycles_(inst_cycles), elapsed_instruction_cycles_(elapsed_inst_cycle), current_cpu_cycle_(cpu_cycle)
{
}

uint16_t CPU6502Disassembler::Disassemble_current_instruction(CPU6502* cpu)
{
	assert(cpu);
	return 0;
}