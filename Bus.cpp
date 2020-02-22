#include "pch.h"
#include "Bus.h"
#include "CPU6502.h"
#include "PPU2C02.h"
#include "Cartridge.h"

Bus::Bus()
{
	cpu_ = std::make_shared<CPU6502>();
	cpu_->connect(this);
	ppu_ = std::make_shared<PPU2C02>();
}
Bus::~Bus()
{
}

void Bus::write(uint16_t addr, uint8_t data)
{	
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		//onboard_ram_[addr % 0x0800] = data;
		onboard_ram_[addr & 0x07FF] = data;
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		controller_state_[addr & 0x0001] = controller_[addr & 0x0001];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) //to ppu memory-mapped registers.
	{
		//ppu_->cpuWrite(addr % 0x0008, data);
		ppu_->cpuWrite(addr & 0x0007, data);
	}
	
}

uint8_t Bus::read(uint16_t addr, bool bReadOnly)
{
	uint8_t data = 0x00;	
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		data = onboard_ram_[addr & 0x07FF];
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		data = (controller_state_[addr & 0x0001] & 0x80) > 0;
		controller_state_[addr & 0x0001] <<= 1;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		data = ppu_->cpuRead(addr & 0x0007, bReadOnly);
	}
	else 
	{
		// Cartridge Address Range
		cart_->cpuRead(addr, data);
	}
	return data;
}

void Bus::insert_cartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	// Connects cartridge to both Main Bus and CPU Bus
	this->cart_ = cartridge;
	ppu_->ConnectCartridge(cartridge);
}

void Bus::reset()
{
	cart_->reset();
	cpu_->reset();
	ppu_->reset();
	total_cycles_ = 0;
}

void Bus::clock()
{
	ppu_->clock();
	if (total_cycles_ % 3 == 0)
	{
		cpu_->clock();
	}

	// The PPU is capable of emitting an interrupt to indicate the
	// vertical blanking period has been entered. If it has, we need
	// to send that irq to the cpu->
	if (ppu_->nmi)
	{
		ppu_->nmi = false;
		cpu_->nmi();
	}

	total_cycles_++;
}
