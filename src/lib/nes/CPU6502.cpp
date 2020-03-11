#include "CPU6502.h"
#include "Bus.h"
#include "Common.h"
#include <doctest/doctest.h>


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
		&a::jsr, &a::and_, &a::nop, &a::rla, &a::bit, &a::and_, &a::rol, &a::rla, &a::plp, &a::and_, &a::rol, &a::nop, &a::bit, &a::and_, &a::rol, &a::rla,
		&a::bmi, &a::and_, &a::nop, &a::rla, &a::nop, &a::and_, &a::rol, &a::rla, &a::sec, &a::and_, &a::nop, &a::rla, &a::nop, &a::and_, &a::rol, &a::rla,
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
	if (result & 0x0100)
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
void CPU6502::test_overflow(const uint16_t& a, const uint16_t& fetched, const uint16_t& result)
{
	uint16_t v = ~((uint16_t)a ^ (uint16_t)fetched) & ((uint16_t)a ^ (uint16_t)result);
	if (v & 0x0080)
		status_ |= flag_overflow;
	else
		status_ &= ~flag_overflow;
}

void CPU6502::clock()
{
	assert(bus_ptr_);
	if (0 == cycles_left_on_ins_)
	{
		opcode_ = bus_read8(pc_++);
		status_ |= flag_constant;
		penalty_op_ = penalty_addr_ = 0;
		(this->*addr_table_[opcode_])();
		(this->*op_table_[opcode_])();
		cycles_left_on_ins_ = cycle_table_[opcode_];
		total_instructions_++;

		if (penalty_addr_ && penalty_op_)	cycles_left_on_ins_++;
	}
	cycles_left_on_ins_--;
	total_cycles_++;
}
void CPU6502::reset()
{
	assert(bus_ptr_);
	pc_ = bus_read16(0xfffc) | (bus_read16(0xfffd) << 8);
	//pc_ = 0x0400;  //debug
	a_ = x_ = y_ = 0;
	sp_ = 0xfd;
	status_ = flag_constant;
	//manually set the cycle_ counts.
	cycles_left_on_ins_ = 8;
	total_cycles_ = 0;
	total_instructions_ = 0;
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
	pc_ = bus_read16(0xfffe) | (bus_read16(0xffff) << 8);
	//manually set the cycle_ counts.
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
	pc_ = bus_read16(0xfffa) | (bus_read16(0xfffb) << 8);
	//manually set the cycle_ counts.
	cycles_left_on_ins_ = 8;

}

////////////////////////////////
// addr
void CPU6502::acc()
{
}
void CPU6502::imp()
{
}
void CPU6502::imm()
{
	addr_abs_ = pc_++;
}
void CPU6502::zp()
{
	addr_abs_ = bus_read8(pc_++);
	addr_abs_ &= 0x00ff;
}
void CPU6502::zpx()
{
	addr_abs_ = bus_read8(pc_++) + x_;
	addr_abs_ &= 0x00ff;
}
void CPU6502::zpy()
{
	addr_abs_ = bus_read8(pc_++) + y_;
	addr_abs_ &= 0x00ff;
}
void CPU6502::abso()
{
	addr_abs_ = bus_read8(pc_++);
	addr_abs_ |= bus_read8(pc_++) << 8;
}
void CPU6502::absx()
{
	uint16_t	ori_page;
	addr_abs_ = bus_read8(pc_++);
	addr_abs_ |= bus_read8(pc_++) << 8;
	ori_page = addr_abs_ & 0xff00;
	addr_abs_ += (uint16_t)x_;
	ori_page != (addr_abs_ & 0xff00) ? penalty_addr_ = 1 : penalty_addr_ = 0;
}
void CPU6502::absy()
{
	uint16_t	ori_page;
	addr_abs_ = bus_read8(pc_++);
	addr_abs_ |= bus_read8(pc_++) << 8;
	ori_page = addr_abs_ & 0xff00;
	addr_abs_ += (uint16_t)y_;
	ori_page != (addr_abs_ & 0xff00) ? penalty_addr_ = 1 : penalty_addr_ = 0;
}
void CPU6502::rel()
{
	addr_rel_ = bus_read16(pc_++);
	if (addr_rel_ & 0x80) addr_rel_ |= 0xff00;
}
void CPU6502::ind()
{
	uint16_t ptr_lo = bus_read16(pc_++);
	uint16_t ptr_hi = bus_read16(pc_++);
	uint16_t ptr = ptr_hi << 8 | ptr_lo;
	
	if (0x00ff == ptr_lo) //hardware bug
		addr_abs_ = bus_read16(ptr & 0xff00) << 8 | bus_read16(ptr);
	else
		addr_abs_ = bus_read16(ptr + 1) << 8 | bus_read16(ptr);
}
void CPU6502::indx()
{
	uint16_t zp_loc = bus_read16(pc_++);
	uint16_t ptr_lo = bus_read16((uint16_t)(zp_loc + (uint16_t)x_) & 0x00ff);
	uint16_t ptr_hi = bus_read16((uint16_t)(zp_loc + (uint16_t)x_ + 1) & 0x00ff);
	addr_abs_ = (ptr_hi << 8) | ptr_lo;
}
void CPU6502::indy()
{
	uint16_t zp_loc = bus_read16(pc_++);
	uint16_t ptr_lo = bus_read16(zp_loc & 0x00ff);
	uint16_t ptr_hi = bus_read16((zp_loc + 1) & 0x00ff);
	addr_abs_ = ((ptr_hi << 8) | ptr_lo) + y_;
	//cross page test
	if ((ptr_hi << 8) != (addr_abs_ & 0xff00))
		penalty_addr_ = 1;
}


///////////////////////////////////
//op
void CPU6502::adc()
{
	uint8_t v = fetch();
	uint16_t result = v + a_ + ((status_ & flag_carry) ? 1 : 0);
	test_zero(result);

#ifdef CPU_SUPPORT_DECIMAL
	bool decimal_mode_enabled = true;
#else
	bool decimal_mode_enabled = false;
#endif
	if ((status_ & flag_decimal) && decimal_mode_enabled)
	{
		if (((a_ & 0xf) + (v & 0xf) + ((status_ & flag_carry) ? 1 : 0)) > 0x0009) result += 0x0006;
		test_sign(result);
		test_overflow(a_, v, result);
		if (result > 0x99)
		{
			result += 0x60;
		}
		result > 0x0099 ? status_ |= flag_carry : status_ &= ~flag_carry;
	}
	else
	{
		test_sign(result);
		test_overflow(a_, v, result);
		test_carry(result);
	}

	a_ = result & 0xff;
	penalty_op_ = 1;
}


void CPU6502::sbc()
{
	uint8_t v = fetch();
	uint16_t result = a_ - v - ((status_ & flag_carry) ? 0 : 1);
	test_sign(result);
	test_zero(result);
	(((a_ ^ result) & 0x80) && ((a_ ^ v) & 0x80)) ? 
		status_ |= flag_overflow : status_ &= ~flag_overflow;

#ifdef CPU_SUPPORT_DECIMAL
	bool decimal_mode_enabled = true;
#else
	bool decimal_mode_enabled = false;
#endif
	if ((status_ & flag_decimal) && decimal_mode_enabled)
	{
		if (((a_ & 0x0f) - ((status_ & flag_carry) ? 0 : 1)) < (v & 0x0f)) result -= 0x0006;
		if (result > 0x99)
		{
			result -= 0x60;
		}
	}
	result < 0x0100 ? status_ |= flag_carry : status_ &= ~flag_carry;
	a_ = (result & 0xff);
	penalty_op_ = 1;
}

void CPU6502::and_()
{
	uint8_t v = fetch();
	a_ = a_ & v;
	test_zero((uint16_t)a_);
	test_sign((uint16_t)a_);
	penalty_op_ = 1;
}
void CPU6502::asl()
{
	uint16_t v = fetch();
	v = v << 1;
	test_carry(v);
	test_zero(v);
	test_sign(v);
	put(v & 0x00ff);
}
void CPU6502::bcc()
{
	if (0 == (status_ & flag_carry))
	{
		cycles_left_on_ins_++;
		addr_abs_ = pc_ + addr_rel_;
		if ((addr_abs_ & 0xff00) != (pc_ & 0xff00))
			cycles_left_on_ins_++;
		pc_ = addr_abs_;
	}
}

void CPU6502::bcs()
{
	if (flag_carry  == (status_ & flag_carry))
	{
		cycles_left_on_ins_++;
		addr_abs_ = pc_ + addr_rel_;
		if ((addr_abs_ & 0xff00) != (pc_ & 0xff00))
			cycles_left_on_ins_++;
		pc_ = addr_abs_;
	}
}
void CPU6502::beq()
{
	if (flag_zero  == (status_ & flag_zero))
	{
		cycles_left_on_ins_++;
		addr_abs_ = pc_ + addr_rel_;
		if ((addr_abs_ & 0xff00) != (pc_ & 0xff00))
			cycles_left_on_ins_++;
		pc_ = addr_abs_;
	}
}
void CPU6502::bit()
{
	uint8_t v = fetch();
	test_zero(a_ & v);
	status_ &= ~flag_sign;	status_ |= (v & flag_sign);
	status_ &= ~flag_overflow;	status_ |= (v & (1 << 6));
}
void CPU6502::bmi()
{
	if (flag_sign == (status_ & flag_sign))
	{
		cycles_left_on_ins_++;
		addr_abs_ = pc_ + addr_rel_;
		if ((addr_abs_ & 0xff00) != (pc_ & 0xff00))
			cycles_left_on_ins_++;
		pc_ = addr_abs_;
	}
}

void CPU6502::bne()
{
	if (0 == (status_ & flag_zero))
	{
		cycles_left_on_ins_++;
		addr_abs_ = pc_ + addr_rel_;
		if ((addr_abs_ & 0xff00) != (pc_ & 0xff00))
			cycles_left_on_ins_++;
		pc_ = addr_abs_;
	}
}
void CPU6502::bpl()
{
	if (0 == (status_ & flag_sign))
	{
		cycles_left_on_ins_++;
		addr_abs_ = pc_ + addr_rel_;
		if ((addr_abs_ & 0xff00) != (pc_ & 0xff00))
			cycles_left_on_ins_++;
		pc_ = addr_abs_;
	}
}
void CPU6502::brk()
{
	pc_++;
	push16(pc_);
	push8(status_ | flag_break);
	status_ |= flag_interrupt;
	pc_ = bus_read16(0xfffe) | (bus_read16(0xffff) << 8);
}
void CPU6502::bvc()
{
	if (0 == (status_ & flag_overflow))
	{
		cycles_left_on_ins_++;
		addr_abs_ = pc_ + addr_rel_;
		if ((addr_abs_ & 0xff00) != (pc_ & 0xff00))
			cycles_left_on_ins_++;
		pc_ = addr_abs_;
	}
}

void CPU6502::bvs()
{
	if (flag_overflow == (status_ & flag_overflow))
	{
		cycles_left_on_ins_++;
		addr_abs_ = pc_ + addr_rel_;
		if ((addr_abs_ & 0xff00) != (pc_ & 0xff00))
			cycles_left_on_ins_++;
		pc_ = addr_abs_;
	}
}
void CPU6502::clc()
{
	status_ &= ~flag_carry;
}
void CPU6502::cld()
{
	status_ &= ~flag_decimal;
}
void CPU6502::cli()
{
	status_ &= ~flag_interrupt;
}

void CPU6502::clv()
{
	status_ &= ~flag_overflow;
}
void CPU6502::cmp()
{
	uint16_t v = fetch();
	((uint16_t)a_) >= v ? status_ |= flag_carry : status_ &= ~flag_carry;
	v = (uint16_t)a_ - v;
	test_zero(v);
	test_sign(v);
	penalty_op_ = 1;
}
void CPU6502::cpx()
{
	uint16_t v = fetch();
	((uint16_t)x_) >= v ? status_ |= flag_carry : status_ &= ~flag_carry;
	v = (uint16_t)x_ - v;
	test_zero(v);
	test_sign(v);
	penalty_op_ = 1;
}
void CPU6502::cpy()
{
	uint16_t v = fetch();
	((uint16_t)y_) >= v ? status_ |= flag_carry : status_ &= ~flag_carry;
	v = (uint16_t)y_ - v;
	test_zero(v);
	test_sign(v);
	penalty_op_ = 1;
}

void CPU6502::dec()
{
	uint16_t v = fetch() - 1;
	put((uint8_t)(v & 0x00ff));
	test_zero(v);
	test_sign(v);
}
void CPU6502::dex()
{
	x_--;
	test_zero(x_);
	test_sign(x_);
}
void CPU6502::dey()
{
	y_--;
	test_zero(y_);
	test_sign(y_);
}
void CPU6502::eor()
{
	uint8_t v = fetch();
	a_ = a_ ^ v;
	test_zero(a_);
	test_sign(a_);
}

void CPU6502::inc()
{
	uint16_t v = fetch() + 1;
	put((uint8_t)(v & 0x00ff));
	test_zero(v);
	test_sign(v);
}
void CPU6502::inx()
{
	x_++;
	test_zero(x_);
	test_sign(x_);
}
void CPU6502::iny()
{
	y_++;
	test_zero(y_);
	test_sign(y_);
}
void CPU6502::jmp()
{
	pc_ = addr_abs_;
}

void CPU6502::jsr()
{
	push16(pc_ - 1);
	pc_ = addr_abs_;
}
void CPU6502::lda()
{
	a_ = fetch();
	test_zero(a_);
	test_sign(a_);
	penalty_op_ = 1;
}
void CPU6502::ldx()
{
	x_ = fetch();
	test_zero(x_);
	test_sign(x_);
	penalty_op_ = 1;
}
void CPU6502::ldy()
{
	y_ = fetch();
	test_zero(y_);
	test_sign(y_);
	penalty_op_ = 1;
}

void CPU6502::lsr()
{
	uint16_t v = fetch();
	status_ &= ~flag_carry; status_ |= (v & flag_carry);
	v = v >> 1;
	test_zero(v);
	test_sign(v);
	put(v & 0x00ff);
}
void CPU6502::nop()
{
	switch (opcode_)
	{
	case 0x1c:
	case 0x3c:
	case 0x5c:
	case 0x7c:
	case 0xdc:
	case 0xfc:
		penalty_op_ = 1;
		break;
	}
}
void CPU6502::ora()
{
	uint8_t v = fetch();
	a_ = a_ | v;
	test_zero(a_);
	test_sign(a_);
}
void CPU6502::pha()
{
	push8(a_);
}

void CPU6502::php()
{
	push8(status_ | flag_break);
}
void CPU6502::pla()
{
	a_ = pull8();
	test_zero(a_);
	test_sign(a_);
}
void CPU6502::plp()
{
	status_ = pull8() | flag_constant;
}
void CPU6502::rol()
{
	uint16_t v = fetch();
	uint16_t result = (v << 1) | (status_ & flag_carry);
	test_carry(result);
	test_zero(result);
	test_sign(result);
	put(result & 0x00ff);
}

void CPU6502::ror()
{
	uint16_t v = fetch();
	uint16_t result = (v >> 1) | ((status_ & flag_carry) << 7);	
	(v & 0x0001) ? status_ |= flag_carry : status_ &= ~flag_carry;
	test_zero(result);
	test_sign(result);
	put(result & 0x00ff);
}
void CPU6502::rti()
{
	status_ = pull8();
	status_ &= ~flag_break;
	status_ &= ~flag_constant;
	pc_ = pull16();
}
void CPU6502::rts()
{
	pc_ = pull16() + 1;
}

void CPU6502::sec()
{
	status_ |= flag_carry;
}
void CPU6502::sed()
{
	status_ |= flag_decimal;
}
void CPU6502::sei()
{
	status_ |= flag_interrupt;
}
void CPU6502::sta()
{
	put(a_);
}

void CPU6502::stx()
{
	put(x_);
}
void CPU6502::sty()
{
	put(y_);
}
void CPU6502::tax()
{
	x_ = a_;
	test_zero(x_);
	test_sign(x_);
}
void CPU6502::tay()
{
	y_ = a_;
	test_zero(y_);
	test_sign(y_);
}

void CPU6502::tsx()
{
	x_ = sp_;
	test_zero(x_);
	test_sign(x_);
}
void CPU6502::txa()
{
	a_ = x_;
	test_zero(a_);
	test_sign(a_);
}
void CPU6502::txs()
{
	sp_ = x_;
}
void CPU6502::tya()
{
	a_ = y_;
	test_zero(a_);
	test_sign(a_);
}

void CPU6502::slo()
{
	asl();
	ora();
}
void CPU6502::rla()
{
	rol();
	and_();
}
void CPU6502::sre()
{
	lsr();
	eor();
}
void CPU6502::rra()
{
	ror();
	adc();
}

void CPU6502::sax()
{
	sta();
	stx();
	put(a_ & x_);
}
void CPU6502::lax()
{
	lda();
	ldx();
}
void CPU6502::dcp()
{
	dec();
	cmp();
}
void CPU6502::isb()
{
	inc();
	sbc();
}


void CPU6502::push8(uint8_t val)
{
	assert(bus_ptr_);
	bus_ptr_->write(stack_base_addr_ + sp_--,val);
}
uint8_t CPU6502::pull8()
{
	assert(bus_ptr_);
	return bus_read8(stack_base_addr_ + ++sp_);
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
	uint16_t val = bus_read16(stack_base_addr_ + ++sp_);
	val |= (bus_read16(stack_base_addr_ + ++sp_) & 0x00ff) << 8;
	return val;
}

uint8_t CPU6502::fetch()
{
	if (addr_table_[opcode_] == &a::acc ||
		addr_table_[opcode_] == &a::imp)
		return a_;
	else
		return bus_read8(addr_abs_);
}
void  CPU6502::put(uint8_t val)
{
	if (addr_table_[opcode_] == &a::acc ||
		addr_table_[opcode_] == &a::imp)
		a_ = val;
	else
		bus_ptr_->write(addr_abs_, val);
}


uint8_t CPU6502::bus_read8(uint16_t addr)
{
	uint8_t data;
	bool succ = bus_ptr_->read(addr, data);
	assert(succ);
	return data;
}
uint16_t CPU6502::bus_read16(uint16_t addr)
{
	uint8_t data;
	bool succ = bus_ptr_->read(addr, data);
	assert(succ);
	return (uint16_t)data;
}

char* CPU6502::cpu_status()
{
	cpu_status_buffer_[511] = 0x00;
	snprintf(cpu_status_buffer_,511, "%llu:%llu # PC:%04X A:%02X X:%02X Y:%02X %s%s%s%s%s%s%s%s sp:%02X\n",
		(long long unsigned int)total_cycles_, (long long unsigned int)total_instructions_, pc_,  a_, x_, y_,
		status_ & flag_sign ? "N" : ".", status_ & flag_overflow ? "V" : ".", status_ & flag_constant ? "U" : ".",
		status_ & flag_break ? "B" : ".", status_ & flag_decimal ? "D" : ".", status_ & flag_interrupt ? "I" : ".",
		status_ & flag_zero ? "Z" : ".", status_ & flag_carry ? "C" : ".", sp_);
	return cpu_status_buffer_;
}





//doctest
TEST_CASE("CPU6502.instructions.6502_functional_test") 
{
	CPU6502 cpu;
}
