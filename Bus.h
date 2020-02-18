#pragma once
#include<cstdint>
class Bus
{
public:
	bool Read(uint16_t address, uint8_t &value) { return true; }
	bool Write(uint16_t address, uint8_t value) { return true; }
};

