#include "pch.h"
#include "CPU6502.h"



CPU6502::CPU6502(uint16_t stack_base_addr):
	stack_base_addr_(stack_base_addr), bus_ptr_(NULL),
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
		 },
	op_table_{
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
	cycle_table_{
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



void CPU6502::test_zero(const uint16_t& result)
{
	if (result & 0x00ff)
		status_ &= ~flag_zero;
	else
		status_ |= flag_zero;
}

void CPU6502::test_sign(const uint16_t& result)
{
	if (result & 0x0080)
		status_ |= flag_sign;
	else
		status_ &= ~flag_sign;
}

void CPU6502::test_carry(const uint16_t& result)
{
	if (result & 0xff00)
		status_ |= flag_carry;
	else
		status_ &= ~flag_carry;
}

// A  M  R | V | A^R | A^M |~(A^M) | 
// 0  0  0 | 0 |  0  |  0  |   1   |
// 0  0  1 | 1 |  1  |  0  |   1   |
// 0  1  0 | 0 |  0  |  1  |   0   |
// 0  1  1 | 0 |  1  |  1  |   0   |  so V = ~(A^M) & (A^R)
// 1  0  0 | 0 |  1  |  1  |   0   |
// 1  0  1 | 0 |  0  |  1  |   0   |
// 1  1  0 | 1 |  1  |  0  |   1   |
// 1  1  1 | 0 |  0  |  0  |   1   |
void CPU6502::test_overflow(const uint16_t& old_a, const uint16_t& m, const uint16_t& result)
{
	uint16_t v = ~((uint16_t)old_a ^ (uint16_t)m) & ((uint16_t)old_a ^ (uint16_t)result);
	if (v & 0x0080)
		status_ |= flag_overflow;
	else
		status_ &= ~flag_overflow;
}




void CPU6502::ticktock()
{
	assert(bus_ptr_);
	if (0 == cycles_left_on_ins_)
	{
		uint8_t opcode = bus_ptr_->read(pc_++);
		status_ |= flag_constant;
		penalty_op = penalty_addr = 0;
		(this->*addr_table_[opcode])();
		(this->*op_table_[opcode])();
		cycles_left_on_ins_ = cycle_table_[opcode];
		total_instructions_++;

		if (penalty_addr && penalty_op)	cycles_left_on_ins_++;
	}
	cycles_left_on_ins_--;
	total_cycles_++;
}
void CPU6502::rst()
{
	assert(bus_ptr_);
	pc_ = (uint16_t)bus_ptr_->read(0xfffc) | ((uint16_t)bus_ptr_->read(0xfffd) << 8);
	a_ = x_ = y_ = 0;
	sp_ = 0xfd;
	status_ = flag_constant;
	//manually set the cycle counts.
	cycles_left_on_ins_ = 8;
}
void CPU6502::irq()
{
	assert(bus_ptr_);
	if (status_ & flag_interrupt) // if disable interrupt flag is set
		return;
	push16(pc_);
	push8(status_);
	status_ &= ~flag_break;
	status_ |= flag_interrupt;
	status_ |= flag_constant;
	pc_ = (uint16_t)bus_ptr_->read(0xfffe) | ((uint16_t)bus_ptr_->read(0xffff) << 8);
	//manually set the cycle counts.
	cycles_left_on_ins_ = 7;
}
void CPU6502::nmi()
{
	assert(bus_ptr_);
	push16(pc_);
	push8(status_);
	status_ &= ~flag_break;
	status_ |= flag_interrupt;
	status_ |= flag_constant;
	pc_ = (uint16_t)bus_ptr_->read(0xfffa) | ((uint16_t)bus_ptr_->read(0xfffb) << 8);
	//manually set the cycle counts.
	cycles_left_on_ins_ = 8;

}

////////////////////////////////
// addr
void CPU6502::acc()
{
	return;
}
void CPU6502::imm()
{
	return;
}
void CPU6502::abso()
{
	return;
}
void CPU6502::zp()
{
	return;
}
void CPU6502::zpx()
{
	return;
}
void CPU6502::zpy()
{
	return;
}
void CPU6502::absx()
{
	return;
}
void CPU6502::absy()
{
	return;
}
void CPU6502::imp()
{
	return;
}
void CPU6502::rel()
{
	return;
}
void CPU6502::indx()
{
	return;
}
void CPU6502::indy()
{
	return;
}
void CPU6502::ind()
{
	return;
}



///////////////////////////////////
//op
void CPU6502::adc()
{
	return;
}
void CPU6502::and()
{
	return;
}
void CPU6502::asl()
{
	return;
}
void CPU6502::bcc()
{
	return;
}

void CPU6502::bcs()
{
	return;
}
void CPU6502::beq()
{
	return;
}
void CPU6502::bit()
{
	return;
}
void CPU6502::bmi()
{
	return;
}

void CPU6502::bne()
{
	return;
}
void CPU6502::bpl()
{
	return;
}
void CPU6502::brk()
{
	return;
}
void CPU6502::bvc()
{
	return;
}

void CPU6502::bvs()
{
	return;
}
void CPU6502::clc()
{
	return;
}
void CPU6502::cld()
{
	return;
}
void CPU6502::cli()
{
	return;
}

void CPU6502::clv()
{
	return;
}
void CPU6502::cmp()
{
	return;
}
void CPU6502::cpx()
{
	return;
}
void CPU6502::cpy()
{
	return;
}

void CPU6502::dec()
{
	return;
}
void CPU6502::dex()
{
	return;
}
void CPU6502::dey()
{
	return;
}
void CPU6502::eor()
{
	return;
}

void CPU6502::inc()
{
	return;
}
void CPU6502::inx()
{
	return;
}
void CPU6502::iny()
{
	return;
}
void CPU6502::jmp()
{
	return;
}

void CPU6502::jsr()
{
	return;
}
void CPU6502::lda()
{
	return;
}
void CPU6502::ldx()
{
	return;
}
void CPU6502::ldy()
{
	return;
}

void CPU6502::lsr()
{
	return;
}
void CPU6502::nop()
{
	return;
}
void CPU6502::ora()
{
	return;
}
void CPU6502::pha()
{
	return;
}

void CPU6502::php()
{
	return;
}
void CPU6502::pla()
{
	return;
}
void CPU6502::plp()
{
	return;
}
void CPU6502::rol()
{
	return;
}

void CPU6502::ror()
{
	return;
}
void CPU6502::rti()
{
	return;
}
void CPU6502::rts()
{
	return;
}
void CPU6502::sbc()
{
	return;
}

void CPU6502::sec()
{
	return;
}
void CPU6502::sed()
{
	return;
}
void CPU6502::sei()
{
	return;
}
void CPU6502::sta()
{
	return;
}

void CPU6502::stx()
{
	return;
}
void CPU6502::sty()
{
	return;
}
void CPU6502::tax()
{
	return;
}
void CPU6502::tay()
{
	return;
}

void CPU6502::tsx()
{
	return;
}
void CPU6502::txa()
{
	return;
}
void CPU6502::txs()
{
	return;
}
void CPU6502::tya()
{
	return;
}

void CPU6502::slo()
{
	return;
}
void CPU6502::rla()
{
	return;
}
void CPU6502::sre()
{
	return;
}
void CPU6502::rra()
{
	return;
}

void CPU6502::sax()
{
	return;
}
void CPU6502::lax()
{
	return;
}
void CPU6502::dcp()
{
	return;
}
void CPU6502::isb()
{
	return;
}


void CPU6502::push8(uint8_t val)
{
	assert(bus_ptr_);
	bus_ptr_->write(stack_base_addr_ + sp_--,val);
}
uint8_t CPU6502::pull8()
{
	assert(bus_ptr_);
	return bus_ptr_->read(stack_base_addr_ + ++sp_);
}
void CPU6502::push16(uint16_t val)
{
	assert(bus_ptr_);
	bus_ptr_->write(stack_base_addr_ + sp_--, (val>>8) & 0x00ff );
	bus_ptr_->write(stack_base_addr_ + sp_--, val & 0x00ff);
}
uint16_t CPU6502::pull16()
{
	assert(bus_ptr_);
	uint16_t val = bus_ptr_->read(stack_base_addr_ + ++sp_);
	val |= ((uint16_t)bus_ptr_->read(stack_base_addr_ + ++sp_) & 0x00ff) << 8;
	return val;
}