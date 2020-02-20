#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>

class Cartridge
{
public:
	enum class MirroringType
	{
		Horizontal,
		Vertical,
		ScreenAOnly,
		ScreenBOnly,
		FourScreens
	};

	Cartridge(const std::string& rom_file);
	~Cartridge();
private:

	uint8_t mapper_id_ = 0;
	uint8_t prg_banks_ = 0;
	uint8_t chr_banks_ = 0;

	std::vector<uint8_t> prg_memory_;
	std::vector<uint8_t> chr_memory_;
};

