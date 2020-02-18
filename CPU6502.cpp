#include "pch.h"
#include "CPU6502.h"



CPU6502::CPU6502(uint16_t stack_base_addr):
	stack_base_addr_(stack_base_addr), bus_ptr_(NULL),
	addrtable{
		&a::imp,	&a::indx,	&a::imp,	&a::indx,	&a::zp,		&a::zp,		&a::zp,		&a::zp,		&a::imp,	&a::imm,	&a::acc,	&a::imm,	&a::abso,	&a::abso,	&a::abso,	&a::abso,
		&a::rel,	&a::indy,	&a::imp,	&a::indy,	&a::zpx,	&a::zpx,	&a::zpx,	&a::zpx,	&a::imp,	&a::absy,	&a::imp,	&a::absy,	&a::absx,	&a::absx,	&a::absx,	&a::absx,
		&a::abso,	&a::indx,	&a::imp,	&a::indx,	&a::zp,		&a::zp,		&a::zp,		&a::zp,		&a::imp,	&a::imm,	&a::acc,	&a::imm,	&a::abso,	&a::abso,	&a::abso,	&a::abso,
		&a::rel,	&a::indy,	&a::imp,	&a::indy,	&a::zpx,	&a::zpx,	&a::zpx,	&a::zpx,	&a::imp,	&a::absy,	&a::imp,	&a::absy,	&a::absx,	&a::absx,	&a::absx,	&a::absx,
		&a::imp,	&a::indx,	&a::imp,	&a::indx,	&a::zp,		&a::zp,		&a::zp,		&a::zp,		&a::imp,	&a::imm,	&a::acc,	&a::imm,	&a::abso,	&a::abso,	&a::abso,	&a::abso,
		&a::rel,	&a::indy,	&a::imp,	&a::indy,	&a::zpx,	&a::zpx,	&a::zpx,	&a::zpx,	&a::imp,	&a::absy,	&a::imp,	&a::absy,	&a::absx,	&a::absx,	&a::absx,	&a::absx,
		&a::imp,	&a::indx,	&a::imp,	&a::indx,	&a::zp,		&a::zp,		&a::zp,		&a::zp,		&a::imp,	&a::imm,	&a::acc,	&a::imm,	&a::ind,	&a::abso,	&a::abso,	&a::abso,
		&a::rel,	&a::indy,	&a::imp,	&a::indy,	&a::zpx,	&a::zpx,	&a::zpx,	&a::zpx,	&a::imp,	&a::absy,	&a::imp,	&a::absy,	&a::absx,	&a::absx,	&a::absx,	&a::absx,
		&a::imm,	&a::indx,	&a::imm,	&a::indx,	&a::zp,		&a::zp,		&a::zp,		&a::zp,		&a::imp,	&a::imm,	&a::imp,	&a::imm,	&a::abso,	&a::abso,	&a::abso,	&a::abso,
		&a::rel,	&a::indy,	&a::imp,	&a::indy,	&a::zpx,	&a::zpx,	&a::zpy,	&a::zpy,	&a::imp,	&a::absy,	&a::imp,	&a::absy,	&a::absx,	&a::absx,	&a::absy,	&a::absy,
		&a::imm,	&a::indx,	&a::imm,	&a::indx,	&a::zp,		&a::zp,		&a::zp,		&a::zp,		&a::imp,	&a::imm,	&a::imp,	&a::imm,	&a::abso,	&a::abso,	&a::abso,	&a::abso,
		&a::rel,	&a::indy,	&a::imp,	&a::indy,	&a::zpx,	&a::zpx,	&a::zpy,	&a::zpy,	&a::imp,	&a::absy,	&a::imp,	&a::absy,	&a::absx,	&a::absx,	&a::absy,	&a::absy,
		&a::imm,	&a::indx,	&a::imm,	&a::indx,	&a::zp,		&a::zp,		&a::zp,		&a::zp,		&a::imp,	&a::imm,	&a::imp,	&a::imm,	&a::abso,	&a::abso,	&a::abso,	&a::abso,
		&a::rel,	&a::indy,	&a::imp,	&a::indy,	&a::zpx,	&a::zpx,	&a::zpx,	&a::zpx,	&a::imp,	&a::absy,	&a::imp,	&a::absy,	&a::absx,	&a::absx,	&a::absx,	&a::absx,
		&a::imm,	&a::indx,	&a::imm,	&a::indx,	&a::zp,		&a::zp,		&a::zp,		&a::zp,		&a::imp,	&a::imm,	&a::imp,	&a::imm,	&a::abso,	&a::abso,	&a::abso,	&a::abso,
		&a::rel,	&a::indy,	&a::imp,	&a::indy,	&a::zpx,	&a::zpx,	&a::zpx,	&a::zpx,	&a::imp,	&a::absy,	&a::imp,	&a::absy,	&a::absx,	&a::absx,	&a::absx,	&a::absx
	},
	optable{
		&a::brk, &a::ora, &a::nop, &a::slo, &a::nop, &a::ora, &a::asl, &a::slo, &a::php, &a::ora, &a::asl, &a::nop, &a::nop, &a::ora, &a::asl, &a::slo,
		&a::bpl, &a::ora, &a::nop, &a::slo, &a::nop, &a::ora, &a::asl, &a::slo, &a::clc, &a::ora, &a::nop, &a::slo, &a::nop, &a::ora, &a::asl, &a::slo,
		&a::jsr, &a::and, &a::nop, &a::rla, &a::bit, &a::and, &a::rol, &a::rla, &a::plp, &a::and, &a::rol, &a::nop, &a::bit, &a::and, &a::rol, &a::rla,
		&a::bmi, &a::and, &a::nop, &a::rla, &a::nop, &a::and, &a::rol, &a::rla, &a::sec, &a::and, &a::nop, &a::rla, &a::nop, &a::and, &a::rol, &a::rla,
		&a::rti, &a::eor, &a::nop, &a::sre, &a::nop, &a::eor, &a::lsr, &a::sre, &a::pha, &a::eor, &a::lsr, &a::nop, &a::jmp, &a::eor, &a::lsr, &a::sre,
		&a::bvc, &a::eor, &a::nop, &a::sre, &a::nop, &a::eor, &a::lsr, &a::sre, &a::cli, &a::eor, &a::nop, &a::sre, &a::nop, &a::eor, &a::lsr, &a::sre,
		&a::rts, &a::adc, &a::nop, &a::rra, &a::nop, &a::adc, &a::ror, &a::rra, &a::pla, &a::adc, &a::ror, &a::nop, &a::jmp, &a::adc, &a::ror, &a::rra,
		&a::bvs, &a::adc, &a::nop, &a::rra, &a::nop, &a::adc, &a::ror, &a::rra, &a::sei, &a::adc, &a::nop, &a::rra, &a::nop, &a::adc, &a::ror, &a::rra,
		&a::nop, &a::sta, &a::nop, &a::sax, &a::sty, &a::sta, &a::stx, &a::sax, &a::dey, &a::nop, &a::txa, &a::nop, &a::sty, &a::sta, &a::stx, &a::sax,
		&a::bcc, &a::sta, &a::nop, &a::nop, &a::sty, &a::sta, &a::stx, &a::sax, &a::tya, &a::sta, &a::txs, &a::nop, &a::nop, &a::sta, &a::nop, &a::nop,
		&a::ldy, &a::lda, &a::ldx, &a::lax, &a::ldy, &a::lda, &a::ldx, &a::lax, &a::tay, &a::lda, &a::tax, &a::nop, &a::ldy, &a::lda, &a::ldx, &a::lax,
		&a::bcs, &a::lda, &a::nop, &a::lax, &a::ldy, &a::lda, &a::ldx, &a::lax, &a::clv, &a::lda, &a::tsx, &a::lax, &a::ldy, &a::lda, &a::ldx, &a::lax,
		&a::cpy, &a::cmp, &a::nop, &a::dcp, &a::cpy, &a::cmp, &a::dec, &a::dcp, &a::iny, &a::cmp, &a::dex, &a::nop, &a::cpy, &a::cmp, &a::dec, &a::dcp,
		&a::bne, &a::cmp, &a::nop, &a::dcp, &a::nop, &a::cmp, &a::dec, &a::dcp, &a::cld, &a::cmp, &a::nop, &a::dcp, &a::nop, &a::cmp, &a::dec, &a::dcp,
		&a::cpx, &a::sbc, &a::nop, &a::isb, &a::cpx, &a::sbc, &a::inc, &a::isb, &a::inx, &a::sbc, &a::nop, &a::sbc, &a::cpx, &a::sbc, &a::inc, &a::isb,
		&a::beq, &a::sbc, &a::nop, &a::isb, &a::nop, &a::sbc, &a::inc, &a::isb, &a::sed, &a::sbc, &a::nop, &a::isb, &a::nop, &a::sbc, &a::inc, &a::isb 
	},
	ticktable{
		7,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    4,    4,    6,    6, 
		2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7, 
		6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    4,    4,    6,    6, 
		2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7, 
		6,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    3,    4,    6,    6, 
		2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7, 
		6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    5,    4,    6,    6, 
		2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7, 
		2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4, 
		2,    6,    2,    6,    4,    4,    4,    4,    2,    5,    2,    5,    5,    5,    5,    5, 
		2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4, 
		2,    5,    2,    5,    4,    4,    4,    4,    2,    4,    2,    4,    4,    4,    4,    4, 
		2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6, 
		2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7, 
		2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6, 
		2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7  
	}
{
}



void CPU6502::TickTock()
{
	assert(bus_ptr_);

}
void CPU6502::RST()
{
	assert(bus_ptr_);
}
void CPU6502::IRQ()
{
	assert(bus_ptr_);

}
void CPU6502::NMI()
{
	assert(bus_ptr_);

}

////////////////////////////////
// addr
uint8_t CPU6502::acc()
{
	return 0;
}
uint8_t CPU6502::imm()
{
	return 0;
}
uint8_t CPU6502::abso()
{
	return 0;
}
uint8_t CPU6502::zp()
{
	return 0;
}
uint8_t CPU6502::zpx()
{
	return 0;
}
uint8_t CPU6502::zpy()
{
	return 0;
}
uint8_t CPU6502::absx()
{
	return 0;
}
uint8_t CPU6502::absy()
{
	return 0;
}
uint8_t CPU6502::imp()
{
	return 0;
}
uint8_t CPU6502::rel()
{
	return 0;
}
uint8_t CPU6502::indx()
{
	return 0;
}
uint8_t CPU6502::indy()
{
	return 0;
}
uint8_t CPU6502::ind()
{
	return 0;
}



///////////////////////////////////
//op
uint8_t CPU6502::adc()
{
	return 0;
}
uint8_t CPU6502::and()
{
	return 0;
}
uint8_t CPU6502::asl()
{
	return 0;
}
uint8_t CPU6502::bcc()
{
	return 0;
}

uint8_t CPU6502::bcs()
{
	return 0;
}
uint8_t CPU6502::beq()
{
	return 0;
}
uint8_t CPU6502::bit()
{
	return 0;
}
uint8_t CPU6502::bmi()
{
	return 0;
}

uint8_t CPU6502::bne()
{
	return 0;
}
uint8_t CPU6502::bpl()
{
	return 0;
}
uint8_t CPU6502::brk()
{
	return 0;
}
uint8_t CPU6502::bvc()
{
	return 0;
}

uint8_t CPU6502::bvs()
{
	return 0;
}
uint8_t CPU6502::clc()
{
	return 0;
}
uint8_t CPU6502::cld()
{
	return 0;
}
uint8_t CPU6502::cli()
{
	return 0;
}

uint8_t CPU6502::clv()
{
	return 0;
}
uint8_t CPU6502::cmp()
{
	return 0;
}
uint8_t CPU6502::cpx()
{
	return 0;
}
uint8_t CPU6502::cpy()
{
	return 0;
}

uint8_t CPU6502::dec()
{
	return 0;
}
uint8_t CPU6502::dex()
{
	return 0;
}
uint8_t CPU6502::dey()
{
	return 0;
}
uint8_t CPU6502::eor()
{
	return 0;
}

uint8_t CPU6502::inc()
{
	return 0;
}
uint8_t CPU6502::inx()
{
	return 0;
}
uint8_t CPU6502::iny()
{
	return 0;
}
uint8_t CPU6502::jmp()
{
	return 0;
}

uint8_t CPU6502::jsr()
{
	return 0;
}
uint8_t CPU6502::lda()
{
	return 0;
}
uint8_t CPU6502::ldx()
{
	return 0;
}
uint8_t CPU6502::ldy()
{
	return 0;
}

uint8_t CPU6502::lsr()
{
	return 0;
}
uint8_t CPU6502::nop()
{
	return 0;
}
uint8_t CPU6502::ora()
{
	return 0;
}
uint8_t CPU6502::pha()
{
	return 0;
}

uint8_t CPU6502::php()
{
	return 0;
}
uint8_t CPU6502::pla()
{
	return 0;
}
uint8_t CPU6502::plp()
{
	return 0;
}
uint8_t CPU6502::rol()
{
	return 0;
}

uint8_t CPU6502::ror()
{
	return 0;
}
uint8_t CPU6502::rti()
{
	return 0;
}
uint8_t CPU6502::rts()
{
	return 0;
}
uint8_t CPU6502::sbc()
{
	return 0;
}

uint8_t CPU6502::sec()
{
	return 0;
}
uint8_t CPU6502::sed()
{
	return 0;
}
uint8_t CPU6502::sei()
{
	return 0;
}
uint8_t CPU6502::sta()
{
	return 0;
}

uint8_t CPU6502::stx()
{
	return 0;
}
uint8_t CPU6502::sty()
{
	return 0;
}
uint8_t CPU6502::tax()
{
	return 0;
}
uint8_t CPU6502::tay()
{
	return 0;
}

uint8_t CPU6502::tsx()
{
	return 0;
}
uint8_t CPU6502::txa()
{
	return 0;
}
uint8_t CPU6502::txs()
{
	return 0;
}
uint8_t CPU6502::tya()
{
	return 0;
}

uint8_t CPU6502::slo()
{
	return 0;
}
uint8_t CPU6502::rla()
{
	return 0;
}
uint8_t CPU6502::sre()
{
	return 0;
}
uint8_t CPU6502::rra()
{
	return 0;
}

uint8_t CPU6502::sax()
{
	return 0;
}
uint8_t CPU6502::lax()
{
	return 0;
}
uint8_t CPU6502::dcp()
{
	return 0;
}
uint8_t CPU6502::isb()
{
	return 0;
}
