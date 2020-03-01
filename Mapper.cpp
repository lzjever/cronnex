#include "Mapper.h"

Mapper::Mapper(uint8_t prg_banks_num, uint8_t chr_banks_num)
{
	prg_banks_num_ = prg_banks_num;
	chr_banks_num_ = chr_banks_num;
}


bool Mapper::copy_memory(uint16_t bank_id, uint8_t* origin, uint16_t data_size)
{
	std::vector<uint8_t> segment(data_size);
	std::memcpy(segment.data(), origin, data_size);
	memory_[bank_id] = segment;
	return true;
}
bool Mapper::zero_memory(uint16_t bank_id, uint16_t data_size)
{
	std::vector<uint8_t> segment(data_size);
	std::memset(segment.data(), 0x00, data_size);
	memory_[bank_id] = segment;
	return true;
}

Mapper::~Mapper()
{
}

void Mapper::reset()
{

}
