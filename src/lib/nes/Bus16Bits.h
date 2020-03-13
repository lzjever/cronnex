#pragma once
#include <cstdint>

class Bus16Bits
{
public:
	virtual bool read(uint16_t addr, uint8_t& data, bool read_only = false) = 0;
	virtual bool write(uint16_t addr, uint8_t data) = 0;
};