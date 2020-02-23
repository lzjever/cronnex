#pragma once
#include <cstdint>

class Mapper
{
public:
	Mapper(uint8_t prg_banks_num, uint8_t chr_banks_num);
	~Mapper();
public:
	virtual bool prg_addr(uint16_t addr, uint32_t &mapped_addr)	 = 0;
	virtual bool chr_addr(uint16_t addr, uint32_t &mapped_addr)	 = 0;
	virtual void reset() = 0;

protected:
	uint8_t prg_banks_num_ = 0;
	uint8_t chr_banks_num_ = 0;
};

