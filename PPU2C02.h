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

	uint32_t* get_video_buffer();
	uint32_t  get_pixel_rgb(uint8_t palette, uint8_t pixel);
	void connect_bus(Bus* bus_ptr) { bus_ptr_ = bus_ptr; }
	void connect_cartridge(Cartridge* cart) { cart_ptr_ = cart; }
	void clock();
	void reset();
	bool register_read(uint16_t addr, uint8_t &data);
	bool register_write(uint16_t addr, uint8_t data);

	//read, write cart,pattern_table,palette_table,name_table.
	bool read(uint16_t addr, uint8_t& data);
	bool write(uint16_t addr, uint8_t data);
	uint16_t unmirror_nametable(uint16_t addr, uint8_t mirror);


	void scanline_cycle_visible();
	void scanline_cycle_post();
	void scanline_cycle_nmi();
	void scanline_cycle_pre();
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
	union
	{
		struct
		{
			uint8_t	NN: 2;
			uint8_t I: 1;
			uint8_t S: 1;
			uint8_t B: 1;
			uint8_t H: 1;
			uint8_t P: 1;
			uint8_t V: 1;
		};

		uint8_t byte_;
	} control_;

	union
	{
		struct
		{
			uint8_t G: 1;
			uint8_t m: 1;
			uint8_t M: 1;
			uint8_t b: 1;
			uint8_t s: 1;
			uint8_t BGR: 3;
		};
		uint8_t byte_;
	} mask_;

	union
	{
		struct
		{
			uint8_t unused : 5;
			uint8_t O : 1;
			uint8_t S : 1;
			uint8_t V : 1;
		};

		uint8_t byte_;
	} status_;


	//SCROLL
	union
	{
		struct
		{

			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t NN : 2;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};
		uint16_t byte_ = 0x0000;
	}loopy_v_, loopy_t_;

	//http://wiki.nesdev.com/w/index.php/PPU_OAM
	struct ObjectAttributeMemoryItem
	{
		uint8_t y;
		uint8_t id;	
		uint8_t attribute;
		uint8_t x;
	} OAM_[64];
	uint8_t oam_addr_ = 0x00;


	struct RenderObjectAttributeMemoryItem
	{
	    uint8_t id;     // Index in OAM.
	    uint8_t x;      // X position.
	    uint8_t y;      // Y position.
	    uint8_t tile;   // Tile index.
	    uint8_t attr;   // Attributes.
	    uint8_t dataL;  // Tile data (low).
	    uint8_t dataH;  // Tile data (high).
	} render_oam[8], render_oam2[8];



	inline bool rendering() { return mask_.s || mask_.b; }
	inline int spr_height() { return control_.H ? 16 : 8; }
	void clear_oam();
	void eval_sprites();
	void load_sprites();
	void pixel();


	
	// Rendering vars:
	int scanline_, dot_;
	bool odd_frame_;

	// Pixel offset horizontally
	uint8_t fine_x = 0x00;
	// Internal communications
	uint8_t address_latch_ = 0x00;
	uint8_t ppu_data_buffer_ = 0x00;




	uint8_t     name_table_[0x0800]; 	//2k
	uint8_t		palette_table_[0x20];	//32
	uint32_t	rgb_colors_[0x40]; //pre-defined colors for palette to choose from. 64
	uint32_t	video_buffer_[0xf000]; // the screen. 256*240
	bool frame_complete_;
	bool on_nmi_;
	Bus* bus_ptr_;
	Cartridge* cart_ptr_;
};