#include "Mapper_000.h"

Mapper_000::Mapper_000(uint8_t prg_banks_num, uint8_t chr_banks_num) 
	: Mapper(prg_banks_num, chr_banks_num)
{
}


Mapper_000::~Mapper_000()
{
}

void Mapper_000::reset()
{

}

bool Mapper_000::prg_addr(uint16_t addr, uint32_t &mapped_addr)
{
	// if PRGROM is 16KB
	//     CPU Address Bus          PRG ROM
	//     0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
	//     0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
	// if PRGROM is 32KB
	//     CPU Address Bus          PRG ROM
	//     0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF
	if (addr >= 0x8000 && addr <= 0xFFFF)
	{
		mapped_addr = addr & (prg_banks_num_ > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}
bool Mapper_000::chr_addr(uint16_t addr, uint32_t &mapped_addr)
{
	// There is no mapping required for PPU
	// PPU Address Bus          CHR ROM
	// 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		mapped_addr = addr;
		return true;
	}

	return false;
}