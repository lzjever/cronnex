#pragma once
#include<cstdint>

class DASM6502
{
public:
	class DissamEntry
	{
		char inst_[4];
		char address_mod_[5];
		uint16_t abs_addr_;

	};
	using a = DASM6502;
	DASM6502();

	//addressing 
	void acc();		void imp();		void imm();
	void zp();		void zpx();		void zpy();
	void abso();	void absx();	void absy();
	void rel();		void indx();	void indy();
	void ind();



	typedef void(DASM6502::* addr_ptr)(void);
	addr_ptr	addr_table_[256];
	const char	instruction_table_[256][4];
};

