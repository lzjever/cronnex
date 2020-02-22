#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include "Mapper_000.h"

using namespace std;
class Cartridge
{
public:
	Cartridge(const std::string& sFileName);
	~Cartridge();


public:
	bool is_valid();

	enum MIRROR
	{
		flag_horizontal,
		flag_vertical
	} mirror_type_ = flag_horizontal;

private:
	bool is_rom_valid_ = false;

	uint8_t mapper_ID_ = 0;
	uint8_t prg_banks_num_ = 0;
	uint8_t chr_banks_num_ = 0;

	std::vector<uint8_t> grp_memory_;
	std::vector<uint8_t> chr_memory_;
	std::shared_ptr<Mapper> mapper_;

public:
	// Communication with Main Bus
	bool cpuRead(uint16_t addr, uint8_t& data);
	bool cpuWrite(uint16_t addr, uint8_t data);

	// Communication with PPU Bus
	bool ppuRead(uint16_t addr, uint8_t& data);
	bool ppuWrite(uint16_t addr, uint8_t data);

	// Permits system rest of mapper to know state
	void reset();
};

