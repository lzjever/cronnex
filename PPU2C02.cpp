#include <cstring>
#include "PPU2C02.h"
#include "Cartridge.h"
#include <iostream>
using namespace std;
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
	frame_complete_ = false;
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
		ppu_data_buffer_ = 0;
		break;
	case 0x0003: break;
	case 0x0004:
		data = ((uint8_t*)OAM_)[oam_addr_];
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
		loopy_t_.NN = control_.NN;
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
		((uint8_t*)OAM_)[oam_addr_] = data;
		break;
	case 0x0005: // Scroll
		if (address_latch_ == 0)
		{
			fine_x = data & 0x07;
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
	//uint8_t *name_table_ptr = &(name_table_[0][0]);
	switch (addr)
    {
        case 0x0000 ... 0x1FFF:  
        	return cart_ptr_->chr_read(addr, data);
        break;
        case 0x2000 ... 0x3EFF:  
        	data = name_table_[unmirror_nametable(addr,cart_ptr_->mirror_type_ ) ]; 
    	break;
        case 0x3F00 ... 0x3FFF:
        {
            if ((addr & 0x13) == 0x10) 
            	addr &= ~0x10;
        	data = palette_table_[addr & 0x1F] & (mask_.G ? 0x30 : 0xFF);

        }break;

        default: 
        	return false;
    }
    return true;
}

bool PPU2C02::write(uint16_t addr, uint8_t data)
{

	//uint8_t *name_table_ptr = &(name_table_[0][0]);
    switch (addr)
    {
        case 0x0000 ... 0x1FFF:  
        	return cart_ptr_->chr_write(addr, data);
        break;
        case 0x2000 ... 0x3EFF:  
        	name_table_[unmirror_nametable(addr,cart_ptr_->mirror_type_ ) ] = data;
        break;
        case 0x3F00 ... 0x3FFF:
        {
            if ((addr & 0x13) == 0x10) 
            		addr &= ~0x10;
            palette_table_[addr & 0x1F] = data;

        }break;
    }
    return true;
}

void PPU2C02::reset()
{
	fine_x = 0x00;

	scanline_ = 0;
	dot_ = 0;

	status_.byte_ = 0x00;
	mask_.byte_ = 0x00;
	control_.byte_ = 0x00;
	loopy_v_.byte_ = 0x0000;
	loopy_t_.byte_ = 0x0000;
}





/* Clear secondary OAM */
void PPU2C02::clear_oam()
{
    for (int i = 0; i < 8; i++)
    {
        render_oam2[i].id    = 64;
        render_oam2[i].y     = 0xFF;
        render_oam2[i].tile  = 0xFF;
        render_oam2[i].attr  = 0xFF;
        render_oam2[i].x     = 0xFF;
        render_oam2[i].dataL = 0;
        render_oam2[i].dataH = 0;
    }
}

/* Fill secondary OAM with the sprite infos for the next scanline */
void PPU2C02::eval_sprites()
{
    int n = 0;
    for (int i = 0; i < 64; i++)
    {
        int line = (scanline_ == 261 ? -1 : scanline_) - OAM_[i].y;
        // If the sprite is in the scanline, copy its properties into secondary OAM:
        if (line >= 0 and line < spr_height())
        {
        	
            render_oam2[n].id   = i;
            render_oam2[n].y    = OAM_[i].y;
            render_oam2[n].tile = OAM_[i].id;
            render_oam2[n].attr = OAM_[i].attribute;
            render_oam2[n].x    = OAM_[i].x;


            if (++n >= 8)
            {
                status_.O = true;
                break;
            }
        }
    }
}

/* Load the sprite info into primary OAM and fetch their tile data. */
void PPU2C02::load_sprites()
{
    uint16_t addr;
    for (int i = 0; i < 8; i++)
    {
        render_oam[i] = render_oam2[i];  // Copy secondary OAM into primary.

        // Different address modes depending on the sprite height:
        if (spr_height() == 16)
            addr = ((render_oam[i].tile & 1) * 0x1000) + ((render_oam[i].tile & ~1) * 16);
        else
            addr = ( control_.S      * 0x1000) + ( render_oam[i].tile       * 16);

        unsigned sprY = (scanline_ - render_oam[i].y) % spr_height();  // Line inside the sprite.
        if (render_oam[i].attr & 0x80) sprY ^= spr_height() - 1;      // Vertical flip.
        addr += sprY + (sprY & 8);  // Select the second tile if on 8x16.

        read(addr + 0 , render_oam[i].dataL);
        read(addr + 8 , render_oam[i].dataH);
    }
}



/* Process a pixel, draw it if it's on screen */
void PPU2C02::pixel()
{
    uint8_t palette = 0, objPalette = 0;
    bool objPriority = 0;
    int x = dot_ - 2;

    if (scanline_ < 240 && x >= 0 && x < 256)
    {
        // Sprites:
        if (mask_.s && !(!mask_.M && x < 8))
            for (int i = 7; i >= 0; i--)
            {
                if (render_oam[i].id == 64) continue;  // Void entry.
                unsigned sprX = x - render_oam[i].x;
                if (sprX >= 8) continue;            // Not in range.
                if (render_oam[i].attr & 0x40) sprX ^= 7;  // Horizontal flip.

                uint8_t sprPalette = 1;
                if (sprPalette == 0) continue;  // Transparent pixel.

                if (render_oam[i].id == 0 && palette && x != 255) status_.S = true;
                sprPalette |= (render_oam[i].attr & 3) << 2;
                objPalette  = sprPalette + 16;
                objPriority = render_oam[i].attr & 0x20;
            }
        // Evaluate priority:
        if (objPalette && (palette == 0 || objPriority == 0)) palette = objPalette;

        uint8_t color_addr;
        read(0x3F00 + (rendering() ? palette : 0),color_addr);
        video_buffer_[scanline_ * 256 + x] =  rgb_colors_[color_addr];
        //pixels[scanline*256 + x] = nesRgb[rd(0x3F00 + (rendering() ? palette : 0))];
    }
}



void PPU2C02::scanline_cycle_visible()
{
    switch (dot_)
    {
        case   1: clear_oam(); break;
        case 257: eval_sprites(); break;
        case 321: load_sprites(); break;
    }

    switch (dot_)
    {
        case 2 ... 255: case 322 ... 337:
            pixel();
        
    }

}
void PPU2C02::scanline_cycle_post()
{
	if(dot_ == 0)
	{
		frame_complete_ = true;
	}
}
void PPU2C02::scanline_cycle_nmi()
{
	if(dot_ == 1)
	{
		status_.V = 1;
		if(control_.V)
			on_nmi_ = true;
	}
}
void PPU2C02::scanline_cycle_pre()
{

    switch (dot_)
    {
        case   1: clear_oam(); status_.O = status_.S = false; break;
        case 257: eval_sprites(); break;
        case 321: load_sprites(); break;
    }
    switch (dot_)
    {
        case 2 ... 255: case 322 ... 337:
            pixel();
        
    }
}

void PPU2C02::clock()
{
	//cout << scanline_ << "  " << dot_ << endl;
    switch (scanline_)
    {
        case 0 ... 239:  scanline_cycle_visible(); break;
        case       240:  scanline_cycle_post();    break;
        case       241:  scanline_cycle_nmi();     break;
        case       261:  scanline_cycle_pre();     break;
    }
    if (++dot_ > 340)
    {
        dot_ %= 341;
        if (++scanline_ > 261)
        {
            scanline_ = 0;
            odd_frame_ ^= 1;
        }
    }
}