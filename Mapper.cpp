#include "Mapper.h"

Mapper::Mapper(uint8_t prg_banks_num, uint8_t chr_banks_num)
{
	prg_banks_num_ = prg_banks_num;
	chr_banks_num_ = chr_banks_num;
	reset();
}


Mapper::~Mapper()
{
}

void Mapper::reset()
{

}
