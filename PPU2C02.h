#pragma once
#include <cstdint>
#include <memory>


class Bus;
class Cartridge;
class PPU2C02
{
public:
	PPU2C02();
	~PPU2C02();

	uint32_t* get_video_buffer();
	uint32_t  get_pixel_rgb(uint8_t palette, uint8_t pixel);
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
	union
	{
		struct
		{
			uint8_t	NN: 2;	//nametable select
			uint8_t I: 1;	//increment mode
			uint8_t S: 1;	//sprite tile select
			uint8_t B: 1;	//background tile select
			uint8_t H: 1;	//sprite height
			uint8_t P: 1;	//PPU master / slave
			uint8_t V: 1;	//NMI enable
		};
		uint8_t byte_;
	} control_;

	union
	{
		struct
		{
			uint8_t G: 1;	//greyscale
			uint8_t m: 1;	//background left column enable
			uint8_t M: 1;	//sprite left column enable
			uint8_t b: 1;	//background enable
			uint8_t s: 1;	//sprite enable
			uint8_t BGR: 3;	//color emphasis
		};
		uint8_t byte_;
	} mask_;

	union
	{
		struct
		{
			uint8_t unused : 5;
			uint8_t O : 1;	//sprite overflow
			uint8_t S : 1;	//sprite 0 hit
			uint8_t V : 1;	//vblank
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
			uint16_t nametable_select : 2;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};
		uint16_t byte_ = 0x0000;
	}loopy_v_, loopy_t_;
	uint8_t fine_x_ = 0x00;
	
	//for 1 clock cycle delay
	uint8_t address_latch_ = 0x00;
	uint8_t ppu_data_buffer_ = 0x00;


	int16_t scanline_ = 0; // y
	int16_t cycle_ = 0; //x


	uint8_t bg_next_tile_id_;
	uint8_t bg_next_tile_attr_;
	uint8_t bg_next_tile_lsb_;
	uint8_t bg_next_tile_msb_;
	uint16_t bg_shifter_pattern_lo_;
	uint16_t bg_shifter_pattern_hi_;
	uint16_t bg_shifter_attrib_lo_;
	uint16_t bg_shifter_attrib_hi_;

	bool test_render_enable();
	void load_oam_on_next_scanline();
	void scroll_x();
	void scroll_y();
	void set_loopy_v_x();
	void set_loopy_v_y();
	void load_background_shifters();
	void load_foreground_shifters();
	void update_shifters();
	void scanline_onscreen();
	void scanline_post();
	void scanline_mni();
	void render(uint8_t  & palette, uint8_t &pixel);

private:
	uint8_t     name_table_[2 * 1024];
	uint8_t		palette_table_[32];

private:
	uint32_t	rgb_colors_[64]; //pre-defined colors for palette to choose from.
	uint32_t	video_buffer_[256 * 240]; // the screen.

public:
	//http://wiki.nesdev.com/w/index.php/PPU_OAM
	struct ObjectAttributeMemoryItem
	{
		uint8_t y;			// Y position of top of sprite 
		uint8_t id;			// Tile index number 

		/* attribute
		76543210
		||||||||
		||||||++- Palette (4 to 7) of sprite
		|||+++--- Unimplemented
		||+------ Priority (0: in front of background; 1: behind background)
		|+------- Flip sprite horizontally
		+-------- Flip sprite vertically
		*/
		uint8_t attribute;
		uint8_t x;			// X position of left side of sprite.
	} oam_[64];
	uint8_t oam_addr_ = 0x00;

	ObjectAttributeMemoryItem oam_on_scanline_[8];
	uint8_t sprite_count_;
	uint8_t sprite_shifter_pattern_lo_[8];
	uint8_t sprite_shifter_pattern_hi_[8];

	bool sprite_zero_hit_; // to set the value in status_
	bool sprite_zero_being_rendered_;

public:
	void connect_bus(Bus* bus_ptr) { bus_ptr_ = bus_ptr; }
	void connect_cartridge(Cartridge* cart) { cart_ptr_ = cart; }
	void clock();
	void reset();

	bool register_read(uint16_t addr, uint8_t &data);
	bool register_write(uint16_t addr, uint8_t data);

	//read, write cart,pattern_table,palette_table,name_table.
	uint16_t unmirror_nametable(uint16_t addr, uint8_t mirror);
	bool read(uint16_t addr, uint8_t& data);
	bool write(uint16_t addr, uint8_t data);

	bool on_nmi_ = false;
	//devices
	Bus* bus_ptr_;
	Cartridge* cart_ptr_;
};