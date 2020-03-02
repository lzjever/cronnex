#include "DASM6502.h"

DASM6502::DASM6502()
	:instruction_table_{
		"BRK","ORA","NOP","SLO","NOP","ORA","ASL","SLO","PHP","ORA","ASL","NOP","NOP","ORA","ASL","SLO",
		"BPL","ORA","NOP","SLO","NOP","ORA","ASL","SLO","CLC","ORA","NOP","SLO","NOP","ORA","ASL","SLO",
		"JSR","AND","NOP","RLA","BIT","AND","ROL","RLA","PLP","AND","ROL","NOP","BIT","AND","ROL","RLA",
		"BMI","AND","NOP","RLA","NOP","AND","ROL","RLA","SEC","AND","NOP","RLA","NOP","AND","ROL","RLA",
		"RTI","EOR","NOP","SRE","NOP","EOR","LSR","SRE","PHA","EOR","LSR","NOP","JMP","EOR","LSR","SRE",
		"BVC","EOR","NOP","SRE","NOP","EOR","LSR","SRE","CLI","EOR","NOP","SRE","NOP","EOR","LSR","SRE",
		"RTS","ADC","NOP","RRA","NOP","ADC","ROR","RRA","PLA","ADC","ROR","NOP","JMP","ADC","ROR","RRA",
		"BVS","ADC","NOP","RRA","NOP","ADC","ROR","RRA","SEI","ADC","NOP","RRA","NOP","ADC","ROR","RRA",
		"NOP","STA","NOP","SAX","STY","STA","STX","SAX","DEY","NOP","TXA","NOP","STY","STA","STX","SAX",
		"BCC","STA","NOP","NOP","STY","STA","STX","SAX","TYA","STA","TXS","NOP","NOP","STA","NOP","NOP",
		"LDY","LDA","LDX","LAX","LDY","LDA","LDX","LAX","TAY","LDA","TAX","NOP","LDY","LDA","LDX","LAX",
		"BCS","LDA","NOP","LAX","LDY","LDA","LDX","LAX","CLV","LDA","TSX","LAX","LDY","LDA","LDX","LAX",
		"CPY","CMP","NOP","DCP","CPY","CMP","DEC","DCP","INY","CMP","DEX","NOP","CPY","CMP","DEC","DCP",
		"BNE","CMP","NOP","DCP","NOP","CMP","DEC","DCP","CLD","CMP","NOP","DCP","NOP","CMP","DEC","DCP",
		"CPX","SBC","NOP","ISB","CPX","SBC","INC","ISB","INX","SBC","NOP","SBC","CPX","SBC","INC","ISB",
		"BEQ","SBC","NOP","ISB","NOP","SBC","INC","ISB","SED","SBC","NOP","ISB","NOP","SBC","INC","ISB"
},

addr_table_{
	 &a::imp, &a::indx, &a::imp, &a::indx, &a::zp , &a::zp , &a::zp , &a::zp , &a::imp, &a::imm, &a::acc, &a::imm, &a::abso,&a::abso,&a::abso,&a::abso,
	 &a::rel, &a::indy, &a::imp, &a::indy, &a::zpx, &a::zpx, &a::zpx, &a::zpx, &a::imp, &a::absy,&a::imp, &a::absy,&a::absx,&a::absx,&a::absx,&a::absx,
	 &a::abso,&a::indx, &a::imp, &a::indx, &a::zp , &a::zp , &a::zp , &a::zp , &a::imp, &a::imm, &a::acc, &a::imm, &a::abso,&a::abso,&a::abso,&a::abso,
	 &a::rel, &a::indy, &a::imp, &a::indy, &a::zpx, &a::zpx, &a::zpx, &a::zpx, &a::imp, &a::absy,&a::imp, &a::absy,&a::absx,&a::absx,&a::absx,&a::absx,
	 &a::imp, &a::indx, &a::imp, &a::indx, &a::zp , &a::zp , &a::zp , &a::zp , &a::imp, &a::imm, &a::acc, &a::imm, &a::abso,&a::abso,&a::abso,&a::abso,
	 &a::rel, &a::indy, &a::imp, &a::indy, &a::zpx, &a::zpx, &a::zpx, &a::zpx, &a::imp, &a::absy,&a::imp, &a::absy,&a::absx,&a::absx,&a::absx,&a::absx,
	 &a::imp, &a::indx, &a::imp, &a::indx, &a::zp , &a::zp , &a::zp , &a::zp , &a::imp, &a::imm, &a::acc, &a::imm, &a::ind, &a::abso,&a::abso, &a::abso,
	 &a::rel, &a::indy, &a::imp, &a::indy, &a::zpx, &a::zpx, &a::zpx, &a::zpx, &a::imp, &a::absy,&a::imp, &a::absy,&a::absx,&a::absx,&a::absx,&a::absx,
	 &a::imm, &a::indx, &a::imm, &a::indx, &a::zp , &a::zp , &a::zp , &a::zp , &a::imp, &a::imm, &a::imp, &a::imm, &a::abso,&a::abso,&a::abso,&a::abso,
	 &a::rel, &a::indy, &a::imp, &a::indy, &a::zpx, &a::zpx, &a::zpy, &a::zpy, &a::imp, &a::absy,&a::imp, &a::absy,&a::absx,&a::absx,&a::absy,&a::absy,
	 &a::imm, &a::indx, &a::imm, &a::indx, &a::zp , &a::zp , &a::zp , &a::zp , &a::imp, &a::imm, &a::imp, &a::imm, &a::abso,&a::abso,&a::abso,&a::abso,
	 &a::rel, &a::indy, &a::imp, &a::indy, &a::zpx, &a::zpx, &a::zpy, &a::zpy, &a::imp, &a::absy,&a::imp, &a::absy,&a::absx,&a::absx,&a::absy,&a::absy,
	 &a::imm, &a::indx, &a::imm, &a::indx, &a::zp , &a::zp , &a::zp , &a::zp , &a::imp, &a::imm, &a::imp, &a::imm, &a::abso,&a::abso,&a::abso,&a::abso,
	 &a::rel, &a::indy, &a::imp, &a::indy, &a::zpx, &a::zpx, &a::zpx, &a::zpx, &a::imp, &a::absy,&a::imp, &a::absy,&a::absx,&a::absx,&a::absx,&a::absx,
	 &a::imm, &a::indx, &a::imm, &a::indx, &a::zp , &a::zp , &a::zp , &a::zp , &a::imp, &a::imm, &a::imp, &a::imm, &a::abso,&a::abso,&a::abso,&a::abso,
	 &a::rel, &a::indy, &a::imp, &a::indy, &a::zpx, &a::zpx, &a::zpx, &a::zpx, &a::imp, &a::absy,&a::imp, &a::absy,&a::absx,&a::absx,&a::absx,&a::absx
}
{
}