#include "pch.h"
#include "Cartridge.h"

Cartridge::Cartridge(const std::string& sFileName)
{
	// iNES Format Header
	struct sHeader
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
	} header;

	is_rom_valid_ = false;


	streampos size;
	ifstream file(sFileName, ios::in | ios::binary);
	if (file.is_open())
	{
		file.read((char*)&header, sizeof(sHeader));
		if (header.mapper1 & 0x04)
			file.seekg(512, ios_base::cur);
		mapper_ID_ = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
		mirror_type_ = (header.mapper1 & 0x01) ? flag_vertical : flag_horizontal;
		uint8_t nFileType = 1;

		if (memcmp(header.name, "NES\x1a", 4) == 0) 
		{
			prg_banks_num_ = header.prg_rom_chunks;
			grp_memory_.resize(prg_banks_num_ * 16384);
			file.read((char*)grp_memory_.data(), grp_memory_.size());
			chr_banks_num_ = header.chr_rom_chunks;
			if (chr_banks_num_ == 0)
			{
				// Create CHR RAM
				chr_memory_.resize(8192);
			}
			else
			{
				// Allocate for ROM
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
			//case   3: pMapper = std::make_shared<Mapper_003>(nPRGBanks, nCHRBanks); break;
			//case  66: pMapper = std::make_shared<Mapper_066>(nPRGBanks, nCHRBanks); break;
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

bool Cartridge::cpuRead(uint16_t addr, uint8_t &data)
{
	uint32_t mapped_addr = 0;
	if (mapper_->cpuMapRead(addr, mapped_addr))
	{
		data = grp_memory_[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;
	if (mapper_->cpuMapWrite(addr, mapped_addr, data))
	{
		grp_memory_[mapped_addr] = data;
		return true;
	}
	else
		return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t & data)
{
	uint32_t mapped_addr = 0;
	if (mapper_->ppuMapRead(addr, mapped_addr))
	{
		data = chr_memory_[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;
	if (mapper_->ppuMapWrite(addr, mapped_addr))
	{
		chr_memory_[mapped_addr] = data;
		return true;
	}
	else
		return false;
}


void Cartridge::reset()
{
	// Note: This does not reset the ROM contents,
	// but does reset the mapper.
	if (mapper_ != nullptr)
		mapper_->reset();
}