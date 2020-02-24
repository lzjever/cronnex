#include "Bus.h"
#include "CPU6502.h"
#include "PPU2C02.h"
#include "Cartridge.h"

Bus::Bus()
{
	dma_page_index_ = 0x00;
	dma_inpage_addr_ = 0x00;
	dma_data_on_transfer_ = 0x00;
	is_dma_mode_ = false;
	cycles_on_dma_ = 0;
}
Bus::~Bus()
{
}

bool Bus::write(uint16_t addr, uint8_t data)
{	
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		onboard_ram_[addr & 0x07FF] = data;
		return true;
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		controller_state_[addr & 0x0001] = controller_[addr & 0x0001];
		return true;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) //to ppu memory-mapped registers.
	{
		ppu_->register_write(addr, data);
		return true;
	}
	else if (addr == 0x4014)
	{
		dma_page_index_ = data;
		dma_inpage_addr_ = 0x00;

		is_dma_mode_ = true;
		cycles_on_dma_ = 0;
		return true;
	}
	return false;
	
}

bool Bus::read(uint16_t addr, uint8_t &data)
{
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		data = onboard_ram_[addr & 0x07FF];
		return true;
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		data = (controller_state_[addr & 0x0001] & 0x80) > 0;
		controller_state_[addr & 0x0001] <<= 1;
		return true;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		return ppu_->register_read(addr, data);
	}
	else 
	{
		return cart_->prg_read(addr, data);
	}
	return false;
}

bool Bus::insert_cartridge(std::shared_ptr<Cartridge> cartridge)
{
	if (!cartridge->is_valid())
	{
		return false;
	}
	this->cart_ = cartridge;
	ppu_->connect_cartridge(this->cart_.get());
	return true;
}
bool Bus::connect_cpu(std::shared_ptr<CPU6502> cpu)
{
	if (!cpu)
	{
		return false;
	}
	cpu_ = cpu;
	cpu_->connect_bus(this);
	return true;
}
bool Bus::connect_ppu(std::shared_ptr<PPU2C02> ppu)
{
	if (!ppu)
	{
		return false;
	}
	ppu_ = ppu;
	ppu_->connect_bus(this);
	return true;
}

void Bus::reset()
{
	cart_->reset();
	cpu_->reset();
	ppu_->reset();


	dma_page_index_ = 0x00;
	dma_inpage_addr_ = 0x00;
	dma_data_on_transfer_ = 0x00;
	is_dma_mode_ = false;
	cycles_on_dma_ = 0;

	total_cycles_ = 0;
}

bool Bus::dma()
{
	if (!is_dma_mode_)
		return false;
	if ((cycles_on_dma_ % 2) == 0)
	{
		read(dma_page_index_ << 8 | dma_inpage_addr_, dma_data_on_transfer_);
	}
	else
	{
		((uint8_t*)ppu_->OAM_)[dma_inpage_addr_++] = dma_data_on_transfer_;
		if (dma_inpage_addr_ == 0x00)
		{
			is_dma_mode_ = false;
		}
	}
	cycles_on_dma_++;
	return true;
}

void Bus::clock()
{
	//todo: check cpu ppu cart
	ppu_->clock();
	if (total_cycles_ % 3 == 0)
	{
		if(!dma())
		{
			cpu_->clock();
		}
	}
	if (ppu_->on_nmi_)
	{
		ppu_->on_nmi_ = false;
		cpu_->nmi();
	}
	total_cycles_++;
}
