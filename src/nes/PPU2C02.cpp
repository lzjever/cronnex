#include <cstring>
#include "PPU2C02.h"
#include "Cartridge.h"

static uint8_t reverse_byte(uint8_t n) 
{
	static unsigned char lookup[16] = {
	   0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
	   0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };
	return (lookup[n & 0b1111] << 4) | lookup[n >> 4];
}

PPU2C02::PPU2C02()
	/*
	:rgb_colors_
	{ 0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,
	  0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
	  0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
	  0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,
	  0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
	  0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
	  0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
	  0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000 
	}
	*/
	:rgb_colors_
	{ 0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,
	  0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
	  0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
	  0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,
	  0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
	  0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
	  0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
	  0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xD8D8D8, 0x000000, 0x000000
	}
{
	on_nmi_ = false;
	bus_ptr_ = NULL;
	cart_ptr_ = NULL;
}

PPU2C02::~PPU2C02()
{
}

uint32_t* PPU2C02::get_video_buffer()
{
	return video_buffer_;
}
uint32_t  PPU2C02::get_pixel_rgb(uint8_t palette, uint8_t pixel)
{
	uint8_t data = 0;
	read(0x3F00 + (palette << 2) + pixel, data);
	uint32_t abgr = rgb_colors_[data & 0x3F] << 8 | 0xff;

	return (abgr << 24) | ((abgr & 0xFF00) << 8) |
		((abgr & 0xFF0000) >> 8) | (abgr >> 24);


	//return rgb_colors_[data & 0x3F]<<2 | 0xff;
}

bool PPU2C02::register_read(uint16_t addr, uint8_t& data, bool read_only)
{
	//addr >= 0x2000 && addr <= 0x3FFF
	addr = addr & 0x0007;	//all are mirrors of the first 8 bytes.
	data = 0x00;
	switch (addr)
	{
	case 0x0000: break;
	case 0x0001: break;
	case 0x0002:
		data =(ppu_data_buffer_ & 0x1F) | (status_.byte_ & 0xE0);
		if (!read_only)
		{
			status_.V = 0;
			address_latch_ = 0;
		}
		break;
	case 0x0003: break;
	case 0x0004:
		data = ((uint8_t*)oam_)[oam_addr_];
		break;

	case 0x0005: break;
	case 0x0006: break;
	case 0x0007:
		data = ppu_data_buffer_;
		read(loopy_v_.byte_, ppu_data_buffer_);
		if (loopy_v_.byte_ >= 0x3F00) 
			data = ppu_data_buffer_;
		if (!read_only)
		{
			loopy_v_.byte_ += (control_.I ? 32 : 1);
		}
		break;
	default:
		return false; // impossible
	}
	return true;
}

bool PPU2C02::register_write(uint16_t addr, uint8_t data)
{
	//addr >= 0x2000 && addr <= 0x3FFF
	addr = addr & 0x0007;	//all are mirrors of the first 8 bytes.
	switch (addr)
	{
	case 0x0000: // Control
		control_.byte_ = data;
		loopy_t_.nametable_select = control_.NN;
		break;
	case 0x0001: // Mask
		mask_.byte_ = data;
		break;
	case 0x0002: // Status
		break;
	case 0x0003: // OAM Address
		oam_addr_ = data;
		break;
	case 0x0004: // OAM Data
		((uint8_t*)oam_)[oam_addr_] = data;
		break;
	case 0x0005: // Scroll
		if (!address_latch_)
		{
			fine_x_ = data & 0x07;
			loopy_t_.coarse_x = data >> 3;
		}
		else
		{
			loopy_t_.fine_y = data & 0x07;
			loopy_t_.coarse_y = data >> 3;
		}
		address_latch_ = !address_latch_;
		break;
	case 0x0006: // PPU Address
		if (!address_latch_)
		{
			loopy_t_.h = data & 0x3F;
		}
		else
		{
			loopy_t_.l = data;
			loopy_v_.byte_ = loopy_t_.byte_; 
		}
		address_latch_ = !address_latch_;
		break;
	case 0x0007:
		write(loopy_v_.byte_, data);
		loopy_v_.byte_ += (control_.I ? 32 : 1);
		break;
	default:
		return false; // impossible
	}
	return true;
}


bool PPU2C02::read(uint16_t addr, uint8_t& data)
{
	data = 0x00;
	if (cart_ptr_->chr_addr(addr, addr))
		return cart_ptr_->chr_read(addr, data);
	else if (cart_ptr_->nt_addr(addr, addr))
		data = name_table_[addr - 0x2000];
	else if (addr >= 0x3F00 && addr <= 0x3FFF) // may write pt_addr in the future
	{
		if ((addr & 0x13) == 0x10)
			addr &= ~0x10;
		data = palette_table_[addr & 0x1F] & (mask_.G ? 0x30 : 0xFF);
	}
	else
		return false;
	return true;
}

bool PPU2C02::write(uint16_t addr, uint8_t data)
{
	if (cart_ptr_->chr_addr(addr, addr))
		return cart_ptr_->chr_write(addr, data);
	else if (cart_ptr_->nt_addr(addr, addr))
		name_table_[addr - 0x2000] = data;
	else if (addr >= 0x3F00 && addr <= 0x3FFF) // may write pt_addr in the future
	{
		if ((addr & 0x13) == 0x10)
			addr &= ~0x10;
		palette_table_[addr & 0x1F] = data;
	}
	else
		return false;
	return true;
}

void PPU2C02::reset()
{
	fine_x_ = 0x00;
	address_latch_ = 0x00;
	ppu_data_buffer_ = 0x00;
	scanline_ = 0;
	cycle_ = 0;
	status_.byte_ = 0x00;
	mask_.byte_ = 0x00;
	control_.byte_ = 0x00;
	loopy_v_.byte_ = 0x0000;
	loopy_t_.byte_ = 0x0000;

	bg_next_tile_id_ = 0x00;
	bg_next_tile_attr_ = 0x00;
	bg_next_tile_lsb_ = 0x00;
	bg_next_tile_msb_ = 0x00;
	bg_shifter_pattern_lo_ = 0x0000;
	bg_shifter_pattern_hi_ = 0x0000;
	bg_shifter_attrib_lo_ = 0x0000;
	bg_shifter_attrib_hi_ = 0x0000;

	sprite_zero_hit_ = false;
	sprite_zero_being_rendered_ = false;
	is_frame_complete_ = false;
	std::memset(video_buffer_, 0x00, 256 * 240 * sizeof(uint32_t));
}

bool PPU2C02::test_render_enable()
{
	return (mask_.b || mask_.s);
}

void PPU2C02::scroll_x()
{
	if (!test_render_enable())
		return;

	if (loopy_v_.coarse_x == 31)
	{
		loopy_v_.coarse_x = 0;
		loopy_v_.nametable_select ^= 1;
	}
	else
	{
		loopy_v_.coarse_x++;
	}
}

void PPU2C02::scroll_y()
{
	if (!test_render_enable())
		return;

	if (loopy_v_.fine_y < 7)
	{
		loopy_v_.fine_y++;
	}
	else
	{
		loopy_v_.fine_y = 0;
		if (loopy_v_.coarse_y == 29)
		{
			loopy_v_.coarse_y = 0;
			loopy_v_.nametable_select ^= 2;
		}
		else if (loopy_v_.coarse_y == 31)
		{
			loopy_v_.coarse_y = 0;
		}
		else
		{
			loopy_v_.coarse_y++;
		}
	}
}

void PPU2C02::set_loopy_v_x()
{
	if (!test_render_enable())
		return;
	loopy_v_.nametable_select = (loopy_t_.nametable_select & 1) | (loopy_v_.nametable_select & 2);
	loopy_v_.coarse_x = loopy_t_.coarse_x;
}

void PPU2C02::set_loopy_v_y()
{
	if (!test_render_enable())
		return;
	loopy_v_.fine_y = loopy_t_.fine_y;
	loopy_v_.nametable_select = (loopy_t_.nametable_select & 2) | (loopy_v_.nametable_select & 1);
	loopy_v_.coarse_y = loopy_t_.coarse_y;
}


void PPU2C02::load_background_shifters()
{
	bg_shifter_pattern_lo_ = (bg_shifter_pattern_lo_ & 0xFF00) | bg_next_tile_lsb_;
	bg_shifter_pattern_hi_ = (bg_shifter_pattern_hi_ & 0xFF00) | bg_next_tile_msb_;

	//for palettes
	bg_shifter_attrib_lo_ = (bg_shifter_attrib_lo_ & 0xFF00) | ((bg_next_tile_attr_ & 0b01) ? 0xFF : 0x00);
	bg_shifter_attrib_hi_ = (bg_shifter_attrib_hi_ & 0xFF00) | ((bg_next_tile_attr_ & 0b10) ? 0xFF : 0x00);
}


void PPU2C02::load_foreground_shifters()
{
	for (uint8_t i = 0; i < sprite_count_; i++)
	{
		uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
		uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

		//step1: process vertical flipping
		if (!control_.H)
		{
			sprite_pattern_addr_lo = (control_.S << 12) | (oam_on_scanline_[i].id << 4);
			sprite_pattern_addr_lo |= ((oam_on_scanline_[i].attribute & 0x80) ?
				(7 - (scanline_ - oam_on_scanline_[i].y)) : (scanline_ - oam_on_scanline_[i].y));

		}
		else  //8x16
		{
			if (!(oam_on_scanline_[i].attribute & 0x80)) //no vertical flipping
			{
				if (scanline_ - oam_on_scanline_[i].y < 8)  // 1st tile
				{
					sprite_pattern_addr_lo =
						((oam_on_scanline_[i].id & 0x01) << 12)
						| ((oam_on_scanline_[i].id & 0xFE) << 4)
						| ((scanline_ - oam_on_scanline_[i].y) & 0x07); 
				}
				else
				{
					// 2nd tile
					sprite_pattern_addr_lo =
						((oam_on_scanline_[i].id & 0x01) << 12)
						| (((oam_on_scanline_[i].id & 0xFE) + 1) << 4)
						| ((scanline_ - oam_on_scanline_[i].y) & 0x07);
				}
			}
			else
			{
				//vertical flipping , same as above but reverse y.
				if (scanline_ - oam_on_scanline_[i].y < 8)
				{
					sprite_pattern_addr_lo =
						((oam_on_scanline_[i].id & 0x01) << 12)
						| (((oam_on_scanline_[i].id & 0xFE) + 1) << 4)
						| (7 - (scanline_ - oam_on_scanline_[i].y) & 0x07);
				}
				else
				{
					sprite_pattern_addr_lo =
						((oam_on_scanline_[i].id & 0x01) << 12)
						| ((oam_on_scanline_[i].id & 0xFE) << 4)
						| (7 - (scanline_ - oam_on_scanline_[i].y) & 0x07); 
				}
			}
		}
		sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

		read(sprite_pattern_addr_lo, sprite_pattern_bits_lo);
		read(sprite_pattern_addr_hi, sprite_pattern_bits_hi);

		//horizontal flipping
		if (oam_on_scanline_[i].attribute & 0x40)
		{
			sprite_pattern_bits_lo = reverse_byte(sprite_pattern_bits_lo);
			sprite_pattern_bits_hi = reverse_byte(sprite_pattern_bits_hi);
		}

		sprite_shifter_pattern_lo_[i] = sprite_pattern_bits_lo;
		sprite_shifter_pattern_hi_[i] = sprite_pattern_bits_hi;
	}
}


void PPU2C02::update_shifters()
{
	if (mask_.b)
	{
		bg_shifter_pattern_lo_ <<= 1;
		bg_shifter_pattern_hi_ <<= 1;

		bg_shifter_attrib_lo_ <<= 1;
		bg_shifter_attrib_hi_ <<= 1;
	}

	if (mask_.s && cycle_ >= 1 && cycle_ < 258)
	{
		for (int i = 0; i < sprite_count_; i++)
		{
			if (oam_on_scanline_[i].x > 0)
			{
				oam_on_scanline_[i].x--;
			}
			else
			{
				sprite_shifter_pattern_lo_[i] <<= 1;
				sprite_shifter_pattern_hi_[i] <<= 1;
			}
		}
	}
}


void PPU2C02::load_oam_on_next_scanline()
{
	sprite_count_ = 0;

	std::memset(oam_on_scanline_, 0xFF, 8 * sizeof(ObjectAttributeMemoryItem));
	std::memset(sprite_shifter_pattern_lo_, 0x00, 8 * sizeof(uint8_t));
	std::memset(sprite_shifter_pattern_hi_, 0x00, 8 * sizeof(uint8_t));


	uint8_t oam_index = 0;

	sprite_zero_hit_ = false;

	while (oam_index < 64 && sprite_count_ < 9)
	{
		int16_t diff = ((int16_t)scanline_ - (int16_t)oam_[oam_index].y);
		if (diff >= 0 && diff < (control_.H ? 16 : 8))
		{
			if (sprite_count_ < 8)
			{
				if (oam_index == 0)
				{
					sprite_zero_hit_ = true;
				}

				std::memcpy(&oam_on_scanline_[sprite_count_], &oam_[oam_index], sizeof(ObjectAttributeMemoryItem));
				sprite_count_++;
			}
		}
		oam_index++;
	}
	status_.O = (sprite_count_ > 8);
}

void PPU2C02::scanline_onscreen()
{
	if (scanline_ == 0 && cycle_ == 0)
	{
		cycle_ = 1;
	}

	if (scanline_ == -1 && cycle_ == 1)
	{
		status_.V = status_.O = status_.S = 0;

		std::memset(sprite_shifter_pattern_lo_, 0x00, 8 * sizeof(uint8_t));
		std::memset(sprite_shifter_pattern_hi_, 0x00, 8 * sizeof(uint8_t));
	}

	if ((cycle_ >= 2 && cycle_ < 258) || (cycle_ >= 321 && cycle_ < 338))
	{
		update_shifters();
		switch ((cycle_ - 1) % 8)	//1...9....17  . every 8 pixels
		{
		case 0:
			load_background_shifters();
			read((loopy_v_.byte_ & 0x0FFF) | 0x2000, bg_next_tile_id_);
			break;
		case 2:
			read(0x23C0 | (loopy_v_.nametable_select << 10)
				| ((loopy_v_.coarse_y >> 2) << 3)
				| (loopy_v_.coarse_x >> 2), bg_next_tile_attr_);

			if (loopy_v_.coarse_y & 0x02) bg_next_tile_attr_ >>= 4;
			if (loopy_v_.coarse_x & 0x02) bg_next_tile_attr_ >>= 2;
			bg_next_tile_attr_ &= 0x03;
			break;
		case 4:
			read(((uint16_t)control_.B << 12)
				| ((uint16_t)bg_next_tile_id_ << 4)
				| (loopy_v_.fine_y) + 0, bg_next_tile_lsb_);

			break;
		case 6:
			read(((uint16_t)control_.B << 12)
				| ((uint16_t)bg_next_tile_id_ << 4)
				| (loopy_v_.fine_y) + 8, bg_next_tile_msb_);
			break;
		case 7:
			scroll_x();
			break;
		}
	}

	if (cycle_ == 256)
	{
		scroll_y();
	}

	if (cycle_ == 257)
	{
		load_background_shifters();
		set_loopy_v_x();
	}

	if (cycle_ == 338 || cycle_ == 340)
	{
		read(0x2000 | (loopy_v_.byte_ & 0x0FFF), bg_next_tile_id_);
	}

	if (scanline_ == -1 && cycle_ >= 280 && cycle_ < 305)
	{
		set_loopy_v_y();
	}


	if (cycle_ == 257 && scanline_ >= 0)
	{
		load_oam_on_next_scanline();
	}

	if (cycle_ == 340)
	{
		load_foreground_shifters();
	}
}
void PPU2C02::scanline_post()
{
	return;
}
void PPU2C02::scanline_mni()
{
	if (scanline_ == 241 && cycle_ == 1)
	{
		status_.V = 1;
		if (control_.V)
			on_nmi_ = true;
	}
}

void PPU2C02::render(uint8_t& palette, uint8_t& pixel)
{
	uint8_t bg_pixel = 0x00;
	uint8_t bg_palette = 0x00;

	//background
	if (mask_.b)
	{
		uint16_t bit_mux = 0x8000 >> fine_x_;
		uint8_t bg_pixel_lo = (bg_shifter_pattern_lo_ & bit_mux) ? 1 : 0;
		uint8_t bg_pixel_hi = (bg_shifter_pattern_hi_ & bit_mux) ? 1 : 0;

		bg_pixel = (bg_pixel_hi << 1) | bg_pixel_lo;

		// Get palette
		uint8_t bg_palette_lo = (bg_shifter_attrib_lo_ & bit_mux) ? 1 : 0;
		uint8_t bg_palette_hi = (bg_shifter_attrib_hi_ & bit_mux) ? 1 : 0;
		bg_palette = (bg_palette_hi << 1) | bg_palette_lo;
	}

	//foreground
	uint8_t fg_pixel = 0x00;  
	uint8_t fg_palette = 0x00;
	uint8_t fg_priority = 0x00;
	if (mask_.s)
	{
		for (uint8_t i = 0; i < sprite_count_; i++)
		{
			if (oam_on_scanline_[i].x == 0)
			{
				uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo_[i] & 0x80) > 0;
				uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi_[i] & 0x80) > 0;
				fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

				fg_palette = (oam_on_scanline_[i].attribute & 0x03) + 0x04;
				fg_priority = (oam_on_scanline_[i].attribute & 0x20) == 0;

				if (fg_pixel != 0)
				{
					sprite_zero_being_rendered_ = (i == 0);
					break;
				}
			}
		}
	}

	palette = pixel = 0x00;
	if (bg_pixel == 0 && fg_pixel == 0)
	{
		pixel = 0x00;
		palette = 0x00;
	}
	else if (bg_pixel == 0 && fg_pixel > 0)
	{
		pixel = fg_pixel;
		palette = fg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel == 0)
	{
		pixel = bg_pixel;
		palette = bg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel > 0)
	{
		if (fg_priority)
		{
			pixel = fg_pixel;
			palette = fg_palette;
		}
		else
		{
			pixel = bg_pixel;
			palette = bg_palette;
		}

		//copy from olc , no idea of this
		if (sprite_zero_hit_ && sprite_zero_being_rendered_)
		{
			if (mask_.b & mask_.s)
			{
				if (~(mask_.m | mask_.M))
				{
					if (cycle_ >= 9 && cycle_ < 258)
					{
						status_.S = 1;
					}
				}
				else
				{
					if (cycle_ >= 1 && cycle_ < 258)
					{
						status_.S = 1;
					}
				}
			}
		}
	}
	
}

void PPU2C02::clock()
{
	if (scanline_ >= -1 && scanline_ < 240)
	{
		scanline_onscreen();
	}
	if (scanline_ == 240)
	{
		scanline_post();
	}
	if (scanline_ >= 241 && scanline_ < 261)
	{
		scanline_mni();
	}
	uint8_t palette, pixel;
	render(palette, pixel);


	int x, y;
	x = cycle_ - 1 ;
	y = scanline_;
	if (x < 256 && y < 240 && x >= 0 && y >= 0)
		video_buffer_[y * 256 + x] = get_pixel_rgb(palette, pixel);

	cycle_++;
	if (cycle_ >= 341)
	{
		cycle_ = 0;
		scanline_++;
		if (scanline_ >= 261)
		{
			scanline_ = -1;
			is_frame_complete_ = true;
		}
	}
}