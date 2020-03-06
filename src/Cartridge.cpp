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
		//mirror_type_ = (header_.mapper1 & 0x01) ? flag_vertical : flag_horizontal;
		uint8_t nFileType = 1;

		if (std::memcmp(header_.name, "NES\x1a", 4) == 0) 
		{
			prg_banks_num_ = header_.prg_rom_chunks;
			chr_banks_num_ = header_.chr_rom_chunks;
			if (chr_banks_num_ == 0)
			{
				chr_banks_num_ = 1;
			}
			switch (mapper_ID_)
			{
			case   0: mapper_ = std::make_shared<Mapper_000>(prg_banks_num_, chr_banks_num_); break;
				//case   2: pMapper = std::make_shared<Mapper_002>(nPRGBanks, nCHRBanks); break;
			}

			mapper_->mirror_type_ = (header_.mapper1 & 0x01) ?
				Mapper::MIRROR::flag_vertical : Mapper::MIRROR::flag_horizontal;

			uint8_t * file_buff = (uint8_t*)malloc(1024*1024*4);	// 4M
			size_t total_size = prg_banks_num_ * (size_t)0x4000 + chr_banks_num_ * (size_t)0x2000;
			file.read((char*)file_buff, total_size);
			uint8_t *file_buffer_end = file_buff;
			for (size_t i = 0; i < prg_banks_num_; i++)
			{
				mapper_->copy_memory((uint16_t)i, file_buffer_end, 0x4000);
				file_buffer_end += 0x4000;
			}
			//0x2000
			for (size_t i = 0; i < chr_banks_num_; i++)
			{
				mapper_->copy_memory((uint16_t)i | 0xff00, file_buffer_end, 0x2000);
				file_buffer_end += 0x2000;
			}
			is_rom_valid_ = true;
			free(file_buff);
		}
		else
		{
			//not supported yet.
			return;
		}
		file.close();
	}

}


Cartridge::~Cartridge()
{
}

bool Cartridge::is_valid()
{
	return is_rom_valid_;
}


// ADDR
bool Cartridge::prg_addr(uint16_t addr, uint16_t& mapped_addr)
{
	return mapper_->prg_addr(addr, mapped_addr);
}

bool Cartridge::chr_addr(uint16_t addr, uint16_t& mapped_addr)
{
	return mapper_->chr_addr(addr, mapped_addr);
}

bool Cartridge::nt_addr(uint16_t addr, uint16_t& mapped_addr)
{
	return mapper_->nt_addr(addr, mapped_addr);
}
////////////////////////////////////////////////////


// DATA
bool Cartridge::prg_read(uint16_t addr, uint8_t &data)
{
	return mapper_->prg_read(addr, data);
}

bool Cartridge::prg_write(uint16_t addr, uint8_t data)
{
	return mapper_->prg_write(addr, data);
}

bool Cartridge::chr_read(uint16_t addr, uint8_t & data)
{
	return mapper_->chr_read(addr, data);
}

bool Cartridge::chr_write(uint16_t addr, uint8_t data)
{
	return mapper_->chr_write(addr, data);
}


void Cartridge::reset()
{
	if (mapper_ != nullptr)
		mapper_->reset();
}