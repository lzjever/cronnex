#include "Mapper.h"

Mapper::Mapper(uint8_t prg_banks_num, uint8_t chr_banks_num)
{
	prg_banks_num_ = prg_banks_num;
	chr_banks_num_ = chr_banks_num;
	mirror_type_ = Mapper::MIRROR::flag_vertical;
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


bool Mapper::nt_addr(uint16_t addr, uint16_t& mapped_addr)
{
	if (addr < 0x2000 || addr > 0x3EFF)
		return false;
	switch (mirror_type_)
	{
	case Mapper::MIRROR::flag_vertical:    
		//return addr % 0x800;
		mapped_addr = addr % 0x800 + 0x2000;
		break;
	case Mapper::MIRROR::flag_horizontal:  
		//return ((addr / 2) & 0x400) + (addr % 0x400);
		mapped_addr = ((addr / 2) & 0x400) + (addr % 0x400) + 0x2000;
		break;
	default:
		//return addr - 0x2000;
		return false;
	}
	return true;
}

Mapper::~Mapper()
{
}

void Mapper::reset()
{

}
