#include <cstring>
#include "PPU2C02.h"
#include "Cartridge.h"
PPU2C02::PPU2C02()
	:rgb_colors_
	{ 0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,
	  0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,
	  0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,
	  0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,
	  0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,
	  0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,
	  0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,
	  0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000 }
{
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
	return rgb_colors_[data & 0x3F];
}

bool PPU2C02::register_read(uint16_t addr, uint8_t &data)
{
	//addr >= 0x2000 && addr <= 0x3FFF
	addr = addr & 0x0007;	//all are mirrors of the first 8 bytes.
	data = 0x00;
	switch (addr)
	{
	case 0x0000: break;
	case 0x0001: break;
	case 0x0002:
		data = (status_.byte_ & 0xE0) | (ppu_data_buffer_ & 0x1F);
		status_.V = 0;
		address_latch_ = 0;
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
		if (loopy_v_.byte_ >= 0x3F00) data = ppu_data_buffer_;
		loopy_v_.byte_ += (control_.I ? 32 : 1);
		break;
	default:
		return false;
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
		if (address_latch_ == 0)
		{
			fine_x_ = data & 0x07;
			loopy_t_.coarse_x = data >> 3;
			address_latch_ = 1;
		}
		else
		{
			loopy_t_.fine_y = data & 0x07;
			loopy_t_.coarse_y = data >> 3;
			address_latch_ = 0;
		}
		break;
	case 0x0006: // PPU Address
		if (address_latch_ == 0)
		{
			loopy_t_.byte_ = (uint16_t)((data & 0x3F) << 8) | (loopy_t_.byte_ & 0x00FF);
			address_latch_ = 1;
		}
		else
		{
			loopy_t_.byte_ = (loopy_t_.byte_ & 0xFF00) | data;
			loopy_v_ = loopy_t_;
			address_latch_ = 0;
		}
		break;
	case 0x0007: 
		write(loopy_v_.byte_, data);
		loopy_v_.byte_ += (control_.I ? 32 : 1);
		break;
	default:
		return false;
	}
	return true;
}


uint16_t PPU2C02::unmirror_nametable(uint16_t addr, uint8_t mirror)
{
    switch (mirror)
    {
        case Cartridge::MIRROR::flag_vertical:    return addr % 0x800;
        case Cartridge::MIRROR::flag_horizontal:  return ((addr / 2) & 0x400) + (addr % 0x400);
        default:
        	return addr - 0x2000;
    }
    //impossible to return from here.
}

bool PPU2C02::read(uint16_t addr, uint8_t &data)
{
	data = 0x00;

	if (addr >= 0x0000 && addr <= 0x1FFF)
		return cart_ptr_->chr_read(addr, data);
	else if (addr >= 0x2000 && addr <= 0x3EFF)
		data = name_table_[unmirror_nametable(addr, cart_ptr_->mirror_type_)];
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
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
	if (addr >= 0x0000 && addr <= 0x1FFF)
        	return cart_ptr_->chr_write(addr, data);
	else if (addr >= 0x2000 && addr <= 0x3EFF)
        	name_table_[unmirror_nametable(addr,cart_ptr_->mirror_type_ ) ] = data;
	else if (addr >= 0x3F00 && addr <= 0x3FFF)
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
	bg_next_tile_id = 0x00;
	bg_next_tile_attrib = 0x00;
	bg_next_tile_lsb = 0x00;
	bg_next_tile_msb = 0x00;
	bg_shifter_pattern_lo = 0x0000;
	bg_shifter_pattern_hi = 0x0000;
	bg_shifter_attrib_lo = 0x0000;
	bg_shifter_attrib_hi = 0x0000;
	status_.byte_ = 0x00;
	mask_.byte_ = 0x00;
	control_.byte_ = 0x00;
	loopy_v_.byte_ = 0x0000;
	loopy_t_.byte_ = 0x0000;
}



bool PPU2C02::test_render_enable()
{
	return (mask_.b || mask_.s);
}

void PPU2C02::IncrementScrollX()
{
	if(!test_render_enable())
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


void PPU2C02::IncrementScrollY()
{
	if(!test_render_enable())
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

// ==============================================================================
// Transfer the temporarily stored horizontal nametable access information
// into the "pointer". Note that fine x scrolling is not part of the "pointer"
// addressing mechanism
void PPU2C02::TransferAddressX()
{
	if(!test_render_enable())
		return;
	loopy_v_.nametable_select = (loopy_t_.nametable_select & 1) | (loopy_v_.nametable_select & 2);
	loopy_v_.coarse_x = loopy_t_.coarse_x;
	
}

// ==============================================================================
// Transfer the temporarily stored vertical nametable access information
// into the "pointer". Note that fine y scrolling is part of the "pointer"
// addressing mechanism
void PPU2C02::TransferAddressY()
{
	if(!test_render_enable())
		return;
	loopy_v_.fine_y = loopy_t_.fine_y;
	loopy_v_.nametable_select = (loopy_t_.nametable_select & 2) | (loopy_v_.nametable_select & 1);
	loopy_v_.coarse_y = loopy_t_.coarse_y;

}


void PPU2C02::LoadBackgroundShifters()
{
	bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
	bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

	//for palettes
	bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
	bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
}


void PPU2C02::LoadForegroundShifters()
{
	for (uint8_t i = 0; i < sprite_count_; i++)
	{

		uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
		uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

		if (!control_.H)
		{
			if (!(oam_on_scanline_[i].attribute & 0x80))
			{
				// Sprite is NOT flipped vertically, i.e. normal    
				sprite_pattern_addr_lo =
					(control_.S << 12)  // Which Pattern Table? 0KB or 4KB offset
					| (oam_on_scanline_[i].id << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
					| (scanline_ - oam_on_scanline_[i].y); // Which Row in cell? (0->7)

			}
			else
			{
				// Sprite is flipped vertically, i.e. upside down
				sprite_pattern_addr_lo =
					(control_.S << 12)  // Which Pattern Table? 0KB or 4KB offset
					| (oam_on_scanline_[i].id << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
					| (7 - (scanline_ - oam_on_scanline_[i].y)); // Which Row in cell? (7->0)
			}

		}
		else
		{
			// 8x16 Sprite Mode - The sprite attribute determines the pattern table
			if (!(oam_on_scanline_[i].attribute & 0x80))
			{
				// Sprite is NOT flipped vertically, i.e. normal
				if (scanline_ - oam_on_scanline_[i].y < 8)
				{
					// Reading Top half Tile
					sprite_pattern_addr_lo =
						((oam_on_scanline_[i].id & 0x01) << 12)  // Which Pattern Table? 0KB or 4KB offset
						| ((oam_on_scanline_[i].id & 0xFE) << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
						| ((scanline_ - oam_on_scanline_[i].y) & 0x07); // Which Row in cell? (0->7)
				}
				else
				{
					// Reading Bottom Half Tile
					sprite_pattern_addr_lo =
						((oam_on_scanline_[i].id & 0x01) << 12)  // Which Pattern Table? 0KB or 4KB offset
						| (((oam_on_scanline_[i].id & 0xFE) + 1) << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
						| ((scanline_ - oam_on_scanline_[i].y) & 0x07); // Which Row in cell? (0->7)
				}
			}
			else
			{
				// Sprite is flipped vertically, i.e. upside down
				if (scanline_ - oam_on_scanline_[i].y < 8)
				{
					// Reading Top half Tile
					sprite_pattern_addr_lo =
						((oam_on_scanline_[i].id & 0x01) << 12)    // Which Pattern Table? 0KB or 4KB offset
						| (((oam_on_scanline_[i].id & 0xFE) + 1) << 4)    // Which Cell? Tile ID * 16 (16 bytes per tile)
						| (7 - (scanline_ - oam_on_scanline_[i].y) & 0x07); // Which Row in cell? (0->7)
				}
				else
				{
					// Reading Bottom Half Tile
					sprite_pattern_addr_lo =
						((oam_on_scanline_[i].id & 0x01) << 12)    // Which Pattern Table? 0KB or 4KB offset
						| ((oam_on_scanline_[i].id & 0xFE) << 4)    // Which Cell? Tile ID * 16 (16 bytes per tile)
						| (7 - (scanline_ - oam_on_scanline_[i].y) & 0x07); // Which Row in cell? (0->7)
				}
			}
		}

		// Phew... XD I'm absolutely certain you can use some fantastic bit 
		// manipulation to reduce all of that to a few one liners, but in this
		// form it's easy to see the processes required for the different
		// sizes and vertical orientations

		// Hi bit plane equivalent is always offset by 8 bytes from lo bit plane
		sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

		// Now we have the address of the sprite patterns, we can read them
		read(sprite_pattern_addr_lo, sprite_pattern_bits_lo);
		read(sprite_pattern_addr_hi, sprite_pattern_bits_hi);

		// If the sprite is flipped horizontally, we need to flip the 
		// pattern bytes. 
		if (oam_on_scanline_[i].attribute & 0x40)
		{
			// This little lambda function "flips" a byte
			// so 0b11100000 becomes 0b00000111. It's very
			// clever, and stolen completely from here:
			// https://stackoverflow.com/a/2602885
			auto flipbyte = [](uint8_t b)
			{
				b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
				b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
				b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
				return b;
			};

			// Flip Patterns Horizontally
			sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
			sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
		}

		// Finally! We can load the pattern into our sprite shift registers
		// ready for rendering on the next scanline_
		sprite_shifter_pattern_lo_[i] = sprite_pattern_bits_lo;
		sprite_shifter_pattern_hi_[i] = sprite_pattern_bits_hi;
	}
}


void PPU2C02::UpdateShifters()
{
	if (mask_.b)
	{
		bg_shifter_pattern_lo <<= 1;
		bg_shifter_pattern_hi <<= 1;

		bg_shifter_attrib_lo <<= 1;
		bg_shifter_attrib_hi <<= 1;
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
	std::memset(sprite_shifter_pattern_lo_,0x00,8 * sizeof(uint8_t));
	std::memset(sprite_shifter_pattern_hi_,0x00,8 * sizeof(uint8_t));


	uint8_t nOAMEntry = 0;

	bSpriteZeroHitPossible_ = false;

	while (nOAMEntry < 64 && sprite_count_ < 9)
	{
		int16_t diff = ((int16_t)scanline_ - (int16_t)oam_[nOAMEntry].y);
		if (diff >= 0 && diff < (control_.H ? 16 : 8))
		{
			if (sprite_count_ < 8)
			{
				if (nOAMEntry == 0)
				{
					bSpriteZeroHitPossible_ = true;
				}

				std::memcpy(&oam_on_scanline_[sprite_count_], &oam_[nOAMEntry], sizeof(ObjectAttributeMemoryItem));
				sprite_count_++;
			}
		}
		nOAMEntry++;
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

		std::memset(sprite_shifter_pattern_lo_,0x00,8 * sizeof(uint8_t));
		std::memset(sprite_shifter_pattern_hi_,0x00,8 * sizeof(uint8_t));
	}

	if ((cycle_ >= 2 && cycle_ < 258) || (cycle_ >= 321 && cycle_ < 338))
	{
		UpdateShifters();
		switch ((cycle_ - 1) % 8)	//1...9....17  . every 8 pixels
		{
		case 0:
			LoadBackgroundShifters();
			read((loopy_v_.byte_ & 0x0FFF) | 0x2000, bg_next_tile_id);
			break;
		case 2:	
			read(0x23C0 | (loopy_v_.nametable_select << 10)
				| ((loopy_v_.coarse_y >> 2) << 3)
				| (loopy_v_.coarse_x >> 2), bg_next_tile_attrib);

			if (loopy_v_.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
			if (loopy_v_.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
			bg_next_tile_attrib &= 0x03;
			break;
		case 4:
			read(((uint16_t)control_.B << 12)
				| ((uint16_t)bg_next_tile_id << 4)
				| (loopy_v_.fine_y) + 0, bg_next_tile_lsb);

			break;
		case 6:
			read(((uint16_t)control_.B << 12)
				| ((uint16_t)bg_next_tile_id << 4)
				| (loopy_v_.fine_y) + 8, bg_next_tile_msb);
			break;
		case 7:
			IncrementScrollX();
			break;
		}
	}

	if (cycle_ == 256)
	{
		IncrementScrollY();
	}

	if (cycle_ == 257)
	{
		LoadBackgroundShifters();
		TransferAddressX();
	}

	if (cycle_ == 338 || cycle_ == 340)
	{
		read(0x2000 | (loopy_v_.byte_ & 0x0FFF), bg_next_tile_id);
	}

	if (scanline_ == -1 && cycle_ >= 280 && cycle_ < 305)
	{
		TransferAddressY();
	}


	if (cycle_ == 257 && scanline_ >= 0)
	{
		load_oam_on_next_scanline();
	}

	if (cycle_ == 340)
	{
		LoadForegroundShifters();
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
	// Composition - We now have background & foreground pixel information for this cycle_

// Background =============================================================
	uint8_t bg_pixel = 0x00;   // The 2-bit pixel to be rendered
	uint8_t bg_palette = 0x00; // The 3-bit index of the palette the pixel indexes

	// We only render backgrounds if the PPU is enabled to do so. Note if 
	// background rendering is disabled, the pixel and palette combine
	// to form 0x00. This will fall through the colour tables to yield
	// the current background colour in effect
	if (mask_.b)
	{
		// Handle Pixel Selection by selecting the relevant bit
		// depending upon fine x scolling. This has the effect of
		// offsetting ALL background rendering by a set number
		// of pixels, permitting smooth scrolling
		uint16_t bit_mux = 0x8000 >> fine_x_;

		// Select Plane pixels by extracting from the shifter 
		// at the required location. 
		uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
		uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;

		// Combine to form pixel index
		bg_pixel = (p1_pixel << 1) | p0_pixel;

		// Get palette
		uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
		uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
		bg_palette = (bg_pal1 << 1) | bg_pal0;
	}

	// Foreground =============================================================
	uint8_t fg_pixel = 0x00;   // The 2-bit pixel to be rendered
	uint8_t fg_palette = 0x00; // The 3-bit index of the palette the pixel indexes
	uint8_t fg_priority = 0x00;// A bit of the sprite attribute indicates if its
							   // more important than the background
	if (mask_.s)
	{
		// Iterate through all sprites for this scanline_. This is to maintain
		// sprite priority. As soon as we find a non transparent pixel of
		// a sprite we can abort

		bSpriteZeroBeingRendered_ = false;

		for (uint8_t i = 0; i < sprite_count_; i++)
		{
			// Scanline cycle_ has "collided" with sprite, shifters taking over
			if (oam_on_scanline_[i].x == 0)
			{
				// Note Fine X scrolling does not apply to sprites, the game
				// should maintain their relationship with the background. So
				// we'll just use the MSB of the shifter

				// Determine the pixel value...
				uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo_[i] & 0x80) > 0;
				uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi_[i] & 0x80) > 0;
				fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

				// Extract the palette from the bottom two bits. Recall
				// that foreground palettes are the latter 4 in the 
				// palette memory.
				fg_palette = (oam_on_scanline_[i].attribute & 0x03) + 0x04;
				fg_priority = (oam_on_scanline_[i].attribute & 0x20) == 0;

				// If pixel is not transparent, we render it, and dont
				// bother checking the rest because the earlier sprites
				// in the list are higher priority
				if (fg_pixel != 0)
				{
					if (i == 0) // Is this sprite zero?
					{
						bSpriteZeroBeingRendered_ = true;
					}

					break;
				}
			}
		}
	}

	// Now we have a background pixel and a foreground pixel. They need
	// to be combined. It is possible for sprites to go behind background
	// tiles that are not "transparent", yet another neat trick of the PPU
	// that adds complexity for us poor emulator developers...

	//uint8_t pixel = 0x00;   // The FINAL Pixel...
	//uint8_t palette = 0x00; // The FINAL Palette...

	palette = pixel = 0x00;

	if (bg_pixel == 0 && fg_pixel == 0)
	{
		// The background pixel is transparent
		// The foreground pixel is transparent
		// No winner, draw "background" colour
		pixel = 0x00;
		palette = 0x00;
	}
	else if (bg_pixel == 0 && fg_pixel > 0)
	{
		// The background pixel is transparent
		// The foreground pixel is visible
		// Foreground wins!
		pixel = fg_pixel;
		palette = fg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel == 0)
	{
		// The background pixel is visible
		// The foreground pixel is transparent
		// Background wins!
		pixel = bg_pixel;
		palette = bg_palette;
	}
	else if (bg_pixel > 0 && fg_pixel > 0)
	{
		// The background pixel is visible
		// The foreground pixel is visible
		// Hmmm...
		if (fg_priority)
		{
			// Foreground cheats its way to victory!
			pixel = fg_pixel;
			palette = fg_palette;
		}
		else
		{
			// Background is considered more important!
			pixel = bg_pixel;
			palette = bg_palette;
		}

		// Sprite Zero Hit detection
		if (bSpriteZeroHitPossible_ && bSpriteZeroBeingRendered_)
		{
			// Sprite zero is a collision between foreground and background
			// so they must both be enabled
			if (mask_.b & mask_.s)
			{
				// The left edge of the screen has specific switches to control
				// its appearance. This is used to smooth inconsistencies when
				// scrolling (since sprites x coord must be >= 0)
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
	x = cycle_ - 1;
	y = scanline_;
	if(x < 256 && y < 240 && x >=0 && y >= 0)
		video_buffer_[y * 256 + x] = get_pixel_rgb(palette, pixel);
	// Advance renderer - it never stops, it's relentless
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