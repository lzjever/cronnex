#include <memory>
#include <list>
#include <vector>
#include <string>
#include <cstring>
#include <doctest/doctest.h>
#include <common/Assert.h>
#include "common/FileUtils.h"
#include "nes/NesBus.h"
#include "nes/APU2A03.h"
#include "nes/PPU2C02.h"
#include "nes/CPU6502.h"
#include "nes/Cartridge.h"


TEST_SUITE("nes.cpu")
{
	TEST_CASE("nestest")
	{
		std::shared_ptr<NesBus> nes = std::make_shared<NesBus>();
	    nes->connect_cpu(std::make_shared<CPU6502>());
	    nes->connect_ppu(std::make_shared<PPU2C02>());

	    std::shared_ptr<Cartridge> cart = std::make_shared<Cartridge>(
	        std::string("../../../test_bin/nestest.nes"));
	    REQUIRE( cart->is_valid() );
	    nes->insert_cartridge(cart);

		nes->reset();
		nes->cpu_->pc_ = 0xc000;
		size_t i = 0;
		uint8_t res02 = 0xff;
		uint8_t res03 = 0xff;
		size_t cycle = 0;
		while ( nes->cpu_->pc_ != 0xc66e)
		{
			nes->clock();
			nes->read(0x02,res02);
			nes->read(0x03,res03);
			if(res02 || res03)
				break;
		}
		CHECK(res02 == 0);
		CHECK(res03 == 0);
	}
}