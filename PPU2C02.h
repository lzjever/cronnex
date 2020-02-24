#pragma once
#include <cstdint>
#include <memory>

#include "olcPixelGameEngine.h"


class Bus;
class Cartridge;
class PPU2C02
{
public:
	PPU2C02();
	~PPU2C02();
	olc::Sprite& screen();
	olc::Pixel& get_render_pixel(uint8_t palette, uint8_t pixel);
	bool is_frame_complete_ = false;
public:
	//registers
	/*
	PPUCTRL	$2000	VPHB SINN	NMI enable(V), PPU master / slave(P), sprite height(H), background tile select(B), sprite tile select(S), increment mode(I), nametable select(NN)
	PPUMASK	$2001	BGRs bMmG	color emphasis(BGR), sprite enable(s), background enable(b), sprite left column enable(M), background left column enable(m), greyscale(G)
	PPUSTATUS	$2002	VSO - ----vblank(V), sprite 0 hit(S), sprite overflow(O); read resets write pair for $2005 / $2006
	OAMADDR	$2003	aaaa aaaa	OAM read / write address
	OAMDATA	$2004	dddd dddd	OAM data read / write
	PPUSCROLL	$2005	xxxx xxxx	fine scroll position(two writes : X scroll, Y scroll)
	PPUADDR	$2006	aaaa aaaa	PPU read / write address(two writes : most significant byte, least significant byte)
	PPUDATA	$2007	dddd dddd	PPU data read / write
	OAMDMA	$4014	aaaa aaaa	OAM DMA high address
	*/
	union PPUCTRL
	{
		struct
		{
			//uint8_t nametable_x : 1;
			//uint8_t nametable_y : 1;
			uint8_t	NN	: 2;
			uint8_t I : 1;
			uint8_t S : 1;
			uint8_t B : 1;
			uint8_t H : 1;
			uint8_t P : 1; // unused
			uint8_t V : 1;
		};

		uint8_t byte_;
	} control_;

	union
	{
		struct
		{
			uint8_t grayscale : 1;
			uint8_t render_background_left : 1;
			uint8_t render_sprites_left : 1;
			uint8_t render_background : 1;
			uint8_t render_sprites : 1;
			uint8_t enhance_red : 1;
			uint8_t enhance_green : 1;
			uint8_t enhance_blue : 1;
		};
		uint8_t byte_;
	} mask_;

	union
	{
		struct
		{
			uint8_t unused : 5;
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};

		uint8_t byte_;
	} status_;

	uint8_t oam_addr_ = 0x00;

	//SCROLL
	union loopy_register
	{
		struct
		{

			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t NN : 2;
			//uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};
		uint16_t byte_ = 0x0000;
	};
	loopy_register vram_addr; // Active "pointer" address into nametable to extract background tile info
	loopy_register tram_addr; // Temporary store of information to be "transferred" into "pointer" at various times

	// Pixel offset horizontally
	uint8_t fine_x = 0x00;

	// Internal communications
	uint8_t address_latch = 0x00;
	uint8_t ppu_data_buffer = 0x00;

	// Pixel "dot" position information
	int16_t scanline = 0;
	int16_t cycle = 0;

	// Background rendering =========================================
	uint8_t bg_next_tile_id = 0x00;
	uint8_t bg_next_tile_attrib = 0x00;
	uint8_t bg_next_tile_lsb = 0x00;
	uint8_t bg_next_tile_msb = 0x00;
	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_attrib_lo = 0x0000;
	uint16_t bg_shifter_attrib_hi = 0x0000;

private:
	uint8_t     name_table_[2][1024];
	uint8_t     pattern_table_[2][4096];
	uint8_t		palette_table_[32];

private:
	olc::Pixel  pixel_colors_[64];
	olc::Sprite screen_ = olc::Sprite(256, 240);

public:
	struct ObjectAttributeMemoryItem
	{
		uint8_t y;			// Y position of sprite
		uint8_t id;			// ID of tile from pattern memory
		uint8_t attribute;	// Flags define how sprite should be rendered
		uint8_t x;			// X position of sprite
	} OAM_[64];


	ObjectAttributeMemoryItem spriteScanline[8];
	uint8_t sprite_count;
	uint8_t sprite_shifter_pattern_lo[8];
	uint8_t sprite_shifter_pattern_hi[8];

	// Sprite Zero Collision Flags
	bool bSpriteZeroHitPossible = false;
	bool bSpriteZeroBeingRendered = false;


public:
	void connect_bus(Bus* bus_ptr) { bus_ptr_ = bus_ptr; }
	void connect_cartridge(Cartridge* cart) { cart_ptr_ = cart; }
	void clock();
	void reset();

	bool register_read(uint16_t addr, uint8_t &data);
	bool register_write(uint16_t addr, uint8_t data);
	//read, write cart,pattern_table,palette_table,name_table.
	bool read(uint16_t addr, uint8_t& data);
	bool write(uint16_t addr, uint8_t data);

	bool on_nmi_ = false;
	//devices
	Bus* bus_ptr_;
	Cartridge* cart_ptr_;
};