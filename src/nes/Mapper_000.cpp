#include "Mapper_000.h"

Mapper_000::Mapper_000(uint8_t prg_banks_num, uint8_t chr_banks_num) 
	: Mapper(prg_banks_num, chr_banks_num)
{
	extend_space_.resize(0x8000 - 0x4020, 0); // mapping 0x4020 to 7ffff
}


Mapper_000::~Mapper_000()
{
}

void Mapper_000::reset()
{

}

bool Mapper_000::prg_addr(uint16_t addr, uint16_t &mapped_addr)
{
	// if PRGROM is 16KB
	//     CPU Address Bus          PRG ROM
	//     0x8000 -> 0xBFFF: Map    0x8000 -> 0xBFFF
	//     0xC000 -> 0xFFFF: Mirror 0x8000 -> 0xBFFF
	// if PRGROM is 32KB
	//     CPU Address Bus          PRG ROM
	//     0x8000 -> 0xFFFF: Map    0x8000 -> 0xFFFF
	if (addr < 0x4020)
		return false;
	mapped_addr = addr;
	if (addr >= 0x8000 && addr <= 0xFFFF)
	{
		if (prg_banks_num_ == 1)
		{
			mapped_addr = addr & 0xBFFF;
		}
	}
	return true;
}
bool Mapper_000::chr_addr(uint16_t addr, uint16_t &mapped_addr)
{
	if (addr > 0x1FFF)
		return false;	
	mapped_addr = addr;
	return true;

}

bool Mapper_000::prg_read(uint16_t addr, uint8_t& data)
{
	if (addr < 0x4020)
		return false;
	if (addr <= 0x7fff)
	{
		data = extend_space_[addr - 0x4020];
	}
	else // (addr >= 0x8000)
	{
		uint16_t local_addr = addr - 0x8000;
		data = memory_[local_addr / 0x4000][addr & 0x3fff];
	}
	return true;
}
bool Mapper_000::prg_write(uint16_t addr, uint8_t data)
{
	if (addr < 0x4020)
		return false;
	if (addr <= 0x7fff)
	{
		extend_space_[addr - 0x4020] = data;
	}
	else // (addr >= 0x8000)
	{
		uint16_t local_addr = addr - 0x8000;
		memory_[local_addr / 0x4000][addr & 0x3fff] = data;
	}
	return true;
}
bool Mapper_000::chr_read(uint16_t addr, uint8_t& data)
{
	if (addr > 0x1FFF)
		return false;
	data = memory_[0xff00][addr];
	return true;
}
bool Mapper_000::chr_write(uint16_t addr, uint8_t data)
{
	if (addr > 0x1FFF)
		return false;
	memory_[0xff00][addr] = data;
	return true;
}
