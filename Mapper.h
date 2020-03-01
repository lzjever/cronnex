#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>


class Mapper
{
public:
	enum class MIRROR : uint8_t
	{
		flag_horizontal,
		flag_vertical
	};

	Mapper(uint8_t prg_banks_num, uint8_t chr_banks_num);
	bool copy_memory(uint16_t bank_id, uint8_t* origin, uint16_t data_size);
	bool zero_memory(uint16_t bank_id, uint16_t data_size);
	~Mapper();
public:
	virtual bool prg_addr(uint16_t addr, uint16_t &mapped_addr)	 = 0;
	virtual bool chr_addr(uint16_t addr, uint16_t &mapped_addr)	 = 0;
	virtual bool nt_addr(uint16_t addr, uint16_t&mapped_addr);

	virtual bool prg_read(uint16_t addr, uint8_t& data) = 0;
	virtual bool prg_write(uint16_t addr, uint8_t data) = 0;
	virtual bool chr_read(uint16_t addr, uint8_t& data) = 0;
	virtual bool chr_write(uint16_t addr, uint8_t data) = 0;

	virtual void reset() = 0;

	MIRROR mirror_type_;
protected:
	uint8_t prg_banks_num_ = 0;
	uint8_t chr_banks_num_ = 0;


	/*
	Some parts of the 2 KiB of internal RAM at $0000-$07FF have predefined purposes 
	dictated by the 6502 architecture. The zero page is $0000-$00FF, and the stack always 
	uses some part of the $0100-$01FF page. Games may divide up the rest however 
	the programmer deems useful. See Sample RAM map for an example allocation strategy for this RAM.
	Note: Most common boards and iNES mappers address ROM and Save/Work RAM in this format:
	$6000-$7FFF = Battery Backed Save or Work RAM
	$8000-$FFFF = Usual ROM, commonly with Mapper Registers (see MMC1 and UxROM for example)
	*/
	
	std::vector<uint8_t> extend_space_;	// some games will access 0x4020 - 0x5fff.  mapping the address from 0x4020 to 7ffff
	std::unordered_map<uint16_t, std::vector<uint8_t> >  memory_;
};

