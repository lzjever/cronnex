#pragma once
#include<cstdint>
class Bus
{
public:
	Bus();
	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);
private:
	uint8_t	memory_[0xffff];
};

