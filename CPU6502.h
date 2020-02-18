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
	void connect(Bus* bus_ptr) { bus_ptr_ = bus_ptr; }
	void ticktock();
	void rst();
	void irq();
	void nmi();

private:
	//status
	void test_zero(const uint16_t& result);
	void test_sign(const uint16_t& result);
	void test_carry(const uint16_t& result);
	void test_overflow(const uint16_t& old_a, const uint16_t& m, const uint16_t& result);


private:
	//addressing 
	void acc();		void imm();
	void abso();	void zp();
	void zpx();		void zpy();
	void absx();	void absy();
	void imp();		void rel();
	void indx();	void indy();
	void ind();
	

private:
	//operations
	void adc();	void and();	void asl();	void bcc();
	void bcs();	void beq();	void bit();	void bmi();
	void bne();	void bpl();	void brk();	void bvc();
	void bvs();	void clc();	void cld();	void cli();
	void clv();	void cmp();	void cpx();	void cpy();
	void dec();	void dex();	void dey();	void eor();
	void inc();	void inx();	void iny();	void jmp();
	void jsr();	void lda();	void ldx();	void ldy();
	void lsr();	void nop();	void ora();	void pha();
	void php();	void pla();	void plp();	void rol();
	void ror();	void rti();	void rts();	void sbc();
	void sec();	void sed();	void sei();	void sta();
	void stx();	void sty();	void tax();	void tay();
	void tsx();	void txa();	void txs();	void tya();
	void slo();	void rla();	void sre();	void rra();
	void sax();	void lax();	void dcp();	void isb();

private:
	//user op
	//stack ops
	void push8(uint8_t val);
	uint8_t pull8();
	void push16(uint16_t val);
	uint16_t pull16();

private:
	//config
	uint16_t stack_base_addr_;

	//cpu registers
	//flag for status : CZIDBUVN
	enum :uint8_t		
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
	uint16_t	pc_;	//program counter
	uint8_t		sp_, a_, x_, y_, status_;

	//durations
	uint64_t	total_cycles_;	//count of the execuated cycles since the last reset
	uint64_t	total_instructions_; //count of the execuated instructions since the last reset
	uint8_t		cycles_left_on_ins_;	//how many cycles left for the last executed instruction

	//helper values
	uint16_t	oldpc, ea, reladdr, value, result;
	uint8_t		opcode, oldstatus;
	uint8_t		penalty_op, penalty_addr;

	//devices
	Bus			*bus_ptr_;

private:
	typedef void(CPU6502::* addr_ptr)(void);
	typedef void(CPU6502::* op_ptr)(void);
	addr_ptr	addr_table_[256];
	op_ptr		op_table_[256];
	uint32_t	cycle_table_[256];


};