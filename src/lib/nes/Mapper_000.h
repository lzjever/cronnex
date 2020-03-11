#pragma once
#include <cstdint>
#include "config.h"
#include "Mapper.h"


class Mapper_000 : public Mapper
{
public:
	Mapper_000(uint8_t prg_banks_num, uint8_t chr_banks_num);
	~Mapper_000();

public:
	bool prg_addr(uint16_t addr, uint16_t &mapped_addr);
	bool chr_addr(uint16_t addr, uint16_t &mapped_addr);

	void reset() override;

	// No local equipment required

public:
	bool prg_read(uint16_t addr, uint8_t& data);
	bool prg_write(uint16_t addr, uint8_t data);
	bool chr_read(uint16_t addr, uint8_t& data);
	bool chr_write(uint16_t addr, uint8_t data);

};

