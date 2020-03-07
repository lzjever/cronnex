#pragma once
#include <cstdint>
#include <memory>
#include "Mapper_000.h"

using namespace std;
class Cartridge
{
public:
	Cartridge(const std::string& file_name);
	~Cartridge();


	bool prg_addr(uint16_t addr, uint16_t& mapped_addr);
	bool chr_addr(uint16_t addr, uint16_t& mapped_addr);
	bool nt_addr(uint16_t addr, uint16_t& mapped_addr);

	bool prg_read(uint16_t addr, uint8_t& data);
	bool prg_write(uint16_t addr, uint8_t data);
	bool chr_read(uint16_t addr, uint8_t& data);
	bool chr_write(uint16_t addr, uint8_t data);



public:
	bool is_valid();



	void reset();
private:
	bool is_rom_valid_ = false;
	uint8_t mapper_ID_ = 0;

	uint8_t prg_banks_num_ = 0;
	uint8_t chr_banks_num_ = 0;

	std::shared_ptr<Mapper> mapper_;
};

