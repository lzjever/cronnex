#include "Cartridge.h"
#include <fstream>
#include <cstring>

Cartridge::Cartridge(const std::string& file_name)
{
	// iNES Format Header
	struct RomHeader
	{
		char name[4];
		uint8_t prg_rom_chunks;
		uint8_t chr_rom_chunks;
		uint8_t mapper1;
		uint8_t mapper2;
		uint8_t prg_ram_size;
		uint8_t tv_system1;
		uint8_t tv_system2;
		char unused[5];
	} header_;

	is_rom_valid_ = false;


	streampos size;
	ifstream file(file_name, ios::in | ios::binary);
	if (file.is_open())
	{
		file.read((char*)&header_, sizeof(RomHeader));
		if (header_.mapper1 & 0x04)
			file.seekg(512, ios_base::cur);
		mapper_ID_ = ((header_.mapper2 >> 4) << 4) | (header_.mapper1 >> 4);
		mirror_type_ = (header_.mapper1 & 0x01) ? flag_vertical : flag_horizontal;
		uint8_t nFileType = 1;

		if (std::memcmp(header_.name, "NES\x1a", 4) == 0) 
		{
			prg_banks_num_ = header_.prg_rom_chunks;
			grp_memory_.resize(prg_banks_num_ * 16384);
			file.read((char*)grp_memory_.data(), grp_memory_.size());
			chr_banks_num_ = header_.chr_rom_chunks;
			if (chr_banks_num_ == 0)
			{
				chr_memory_.resize(8192);
			}
			else
			{
				chr_memory_.resize(chr_banks_num_ * 8192);
			}
			file.read((char*)chr_memory_.data(), chr_memory_.size());
			is_rom_valid_ = true;
		}
		else
		{
			//not supported yet.
			return;
		}
		file.close();


		switch (mapper_ID_)
		{
		case   0: mapper_ = std::make_shared<Mapper_000>(prg_banks_num_, chr_banks_num_); break;
			//case   2: pMapper = std::make_shared<Mapper_002>(nPRGBanks, nCHRBanks); break;
		}
	}

}


Cartridge::~Cartridge()
{
}

bool Cartridge::is_valid()
{
	return is_rom_valid_;
}

bool Cartridge::prg_read(uint16_t addr, uint8_t &data)
{
	uint32_t mapped_addr = 0;
	if (mapper_->prg_addr(addr, mapped_addr))
	{
		data = grp_memory_[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Cartridge::prg_write(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;
	if (mapper_->prg_addr(addr, mapped_addr))
	{
		grp_memory_[mapped_addr] = data;
		return true;
	}
	else
		return false;
}

bool Cartridge::chr_read(uint16_t addr, uint8_t & data)
{
	uint32_t mapped_addr = 0;
	if (mapper_->chr_addr(addr, mapped_addr))
	{
		data = chr_memory_[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Cartridge::chr_write(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;
	if (mapper_->chr_addr(addr, mapped_addr))
	{
		chr_memory_[mapped_addr] = data;
		return true;
	}
	else
		return false;
}


void Cartridge::reset()
{
	if (mapper_ != nullptr)
		mapper_->reset();
}