
#include <iostream>
#include <sstream>

#include "Bus.h"
#include "Cartridge.h"
#include "CPU6502.h"
#include "PPU2C02.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class PCGT_Program : public olc::PixelGameEngine
{
public:
	PCGT_Program()
	{ 
		sAppName = "PCGT emulator";
		nes = std::make_shared<Bus>();
		nes->connect_cpu(std::make_shared<CPU6502>());
		nes->connect_ppu(std::make_shared<PPU2C02>());
	}

private:
	// The NES
	std::shared_ptr<Bus> nes;
	std::shared_ptr<Cartridge> cart;
	bool bEmulationRun = false;
	float fResidualTime = 0.0f;

	uint8_t nSelectedPalette = 0x00;

private:
	// Support Utilities
	std::map<uint16_t, std::string> mapAsm;

	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	void DrawCpu(int x, int y)
	{
		std::string status_ = "status_: ";
		DrawString(x, y, "status_:", olc::WHITE);
		DrawString(x + 64, y, "N", nes->cpu_->status_ & CPU6502::flag_sign ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", nes->cpu_->status_ & CPU6502::flag_overflow ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", nes->cpu_->status_ & CPU6502::flag_constant ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B", nes->cpu_->status_ & CPU6502::flag_break ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D", nes->cpu_->status_ & CPU6502::flag_decimal ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I", nes->cpu_->status_ & CPU6502::flag_interrupt ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z", nes->cpu_->status_ & CPU6502::flag_zero ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C", nes->cpu_->status_ & CPU6502::flag_carry ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(nes->cpu_->pc_, 4));
		DrawString(x, y + 20, "A: $" + hex(nes->cpu_->a_, 2) + "  [" + std::to_string(nes->cpu_->a_) + "]");
		DrawString(x, y + 30, "X: $" + hex(nes->cpu_->x_, 2) + "  [" + std::to_string(nes->cpu_->x_) + "]");
		DrawString(x, y + 40, "Y: $" + hex(nes->cpu_->y_, 2) + "  [" + std::to_string(nes->cpu_->y_) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(nes->cpu_->sp_, 4));
	}

	void DrawCode(int x, int y, int nLines)
	{
		auto it_a = mapAsm.find(nes->cpu_->pc_);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = mapAsm.find(nes->cpu_->pc_);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}

	bool OnUserCreate()
	{
		// Load the cartridge
		//Donkey Kong (JU).nes
		//cart = std::make_shared<Cartridge>("E:\\works\\pcgt\\test_bin\\nestest.nes");
		//cart = std::make_shared<Cartridge>("E:\\works\\pcgt\\test_bin\\Donkey Kong (JU).nes");
		cart = std::make_shared<Cartridge>("C:\\games\\NESrompack\\World\\Ice Climber (JE).nes");
		
		//cart = std::make_shared<Cartridge>("../test_bin/Ice Climber (JE).nes");
		//cart = std::make_shared<Cartridge>("../test_bin/Super Mario Bros. (World).nes");
		
		if (!cart->is_valid())
			return false;

		// Insert into NES
		nes->insert_cartridge(cart);

		// Reset NES
		nes->reset();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::DARK_BLUE);

		// Sneaky peek of controller input in next video! ;P
		nes->controller_[0] = 0x00;
		nes->controller_[0] |= GetKey(olc::Key::X).bHeld ? 0x80 : 0x00;
		nes->controller_[0] |= GetKey(olc::Key::Z).bHeld ? 0x40 : 0x00;
		nes->controller_[0] |= GetKey(olc::Key::A).bHeld ? 0x20 : 0x00;
		nes->controller_[0] |= GetKey(olc::Key::S).bHeld ? 0x10 : 0x00;
		nes->controller_[0] |= GetKey(olc::Key::UP).bHeld ? 0x08 : 0x00;
		nes->controller_[0] |= GetKey(olc::Key::DOWN).bHeld ? 0x04 : 0x00;
		nes->controller_[0] |= GetKey(olc::Key::LEFT).bHeld ? 0x02 : 0x00;
		nes->controller_[0] |= GetKey(olc::Key::RIGHT).bHeld ? 0x01 : 0x00;

		if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;
		if (GetKey(olc::Key::R).bPressed) nes->reset();
		if (GetKey(olc::Key::P).bPressed) (++nSelectedPalette) &= 0x07;

		if (bEmulationRun)
		{
			if (fResidualTime > 0.0f)
				fResidualTime -= fElapsedTime;
			else
			{
				fResidualTime += (1.0f / 60.0f) - fElapsedTime;
				do { nes->clock(); } while (!nes->ppu_->is_frame_complete_);
				nes->ppu_->is_frame_complete_ = false;
			}
		}
		else
		{
			// Emulate code step-by-step
			if (GetKey(olc::Key::C).bPressed)
			{
				// Clock enough times to execute a whole CPU instruction
				do { nes->clock(); } while (nes->cpu_->cycles_left_on_ins_ > 0);
				// CPU clock runs slower than system clock, so it may be
				// complete for additional system clock cycles. Drain
				// those out
				//do { nes->clock(); } while (nes->cpu_->complete());
			}

			// Emulate one whole frame
			if (GetKey(olc::Key::F).bPressed)
			{
				// Clock enough times to draw a single frame
				do { nes->clock(); } while (!nes->ppu_->is_frame_complete_);
				// Use residual clock cycles to complete current instruction
				//do { nes->clock(); } while (!nes->cpu_->complete());
				// Reset frame completion flag
				nes->ppu_->is_frame_complete_ = false;
			}
		}




		DrawCpu(516, 2);
//		DrawCode(516, 72, 26);
		uint32_t * video_buffer = nes->ppu_->get_video_buffer();
		olc::Sprite spr(256,240);
		for(int i = 0; i < 256; i ++)
			for (int j = 0; j < 240; j++)
				spr.SetPixel(i,j,olc::Pixel( ((uint8_t*)&video_buffer[i + j * 256])[2],((uint8_t*)&video_buffer[i + j  * 256])[1],((uint8_t*)&video_buffer[i + j   * 256])[0]  ));
		DrawSprite(0, 0, &spr, 2);
		return true;
	}
};



#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
int WINAPI WinMain(HINSTANCE, HINSTANCE, char*, int cmdShow)
{
	PCGT_Program demo;
	demo.Construct(780, 480, 2, 2);
	demo.Start();
	return 0;
}
#else
int main()
{
	PCGT_Program demo;
	demo.Construct(780, 480, 2, 2);
	demo.Start();
	return 0;
}
#endif