#pragma once
// uncomment to disable assert()
// #define NDEBUG
#include <cassert>
#include<cstdint>
#include <string>
#include "Bus.h"
class CPU6502
{
public:
	using a = CPU6502;
	CPU6502(uint16_t stack_base_addr = 0x0100);
	void ConnectToBus(Bus* bus_ptr) { bus_ptr_ = bus_ptr; }
	void TickTock();
	void RST();
	void IRQ();
	void NMI();

private:
	//CZIDBUVN
	enum:uint8_t
	{
		flag_carry = 0x01,
		flag_zero = 0x02,
		flag_interrupt = 0x04,
		flag_decimal = 0x08,
		flag_break = 0x10,
		flag_constant = 0x20,
		flag_overflow = 0x40,
		flag_sign = 0x80,
	};

	inline void TestZero(const uint16_t& result)
	{
		if (result & 0x00ff)
			status_ &= ~flag_zero;
		else
			status_ |= flag_zero;
	}

	inline void TestSign(const uint16_t& result)
	{
		if (result & 0x0080)
			status_ |= flag_sign;
		else
			status_ &= ~flag_sign;
	}

	inline void TestCarry(const uint16_t& result)
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
	inline void TestOverflow(const uint16_t& old_a, const uint16_t& m, const uint16_t& result)
	{
		uint16_t v = ~((uint16_t)old_a ^ (uint16_t)m) & ((uint16_t)old_a ^ (uint16_t)result);
		if (v & 0x0080)
			status_ |= flag_overflow;
		else
			status_ &= ~flag_overflow;
	}


private:
	uint8_t acc();	uint8_t imm();
	uint8_t abso();	uint8_t zp();
	uint8_t zpx();	uint8_t zpy();
	uint8_t absx();	uint8_t absy();
	uint8_t imp();	uint8_t rel();
	uint8_t indx();	uint8_t indy();
	uint8_t ind();
	

private:
	uint8_t adc();	uint8_t and();	uint8_t asl();	uint8_t bcc();
	uint8_t bcs();	uint8_t beq();	uint8_t bit();	uint8_t bmi();
	uint8_t bne();	uint8_t bpl();	uint8_t brk();	uint8_t bvc();
	uint8_t bvs();	uint8_t clc();	uint8_t cld();	uint8_t cli();
	uint8_t clv();	uint8_t cmp();	uint8_t cpx();	uint8_t cpy();
	uint8_t dec();	uint8_t dex();	uint8_t dey();	uint8_t eor();
	uint8_t inc();	uint8_t inx();	uint8_t iny();	uint8_t jmp();
	uint8_t jsr();	uint8_t lda();	uint8_t ldx();	uint8_t ldy();
	uint8_t lsr();	uint8_t nop();	uint8_t ora();	uint8_t pha();
	uint8_t php();	uint8_t pla();	uint8_t plp();	uint8_t rol();
	uint8_t ror();	uint8_t rti();	uint8_t rts();	uint8_t sbc();
	uint8_t sec();	uint8_t sed();	uint8_t sei();	uint8_t sta();
	uint8_t stx();	uint8_t sty();	uint8_t tax();	uint8_t tay();
	uint8_t tsx();	uint8_t txa();	uint8_t txs();	uint8_t tya();
	uint8_t slo();	uint8_t rla();	uint8_t sre();	uint8_t rra();
	uint8_t sax();	uint8_t lax();	uint8_t dcp();	uint8_t isb();


private:
	uint16_t stack_base_addr_;

	//cpu registers
	uint16_t	pc_;	//program counter
	uint8_t		sp_, a_, x_, y_, status_;

	//ticks
	uint64_t	cycles_;	//count of the excuated cycles since the last reset
	uint64_t	instructions_; //count of the excuated instructions since the last reset

	//helper values
	uint16_t	oldpc, ea, reladdr, value, result;
	uint8_t		opcode, oldstatus;

	//devices
	Bus			*bus_ptr_;

private:
	typedef uint8_t(CPU6502::* addr_ptr)(void);
	typedef uint8_t(CPU6502::* op_ptr)(void);
	addr_ptr	addrtable[256];
	op_ptr		optable[256];
	uint32_t	ticktable[256];

	


};