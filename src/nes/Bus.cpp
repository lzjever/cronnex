#include "Bus.h"
#include "CPU6502.h"
#include "PPU2C02.h"
#include "APU2A03.h"
#include "Cartridge.h"
#include "Common.h"

Bus::Bus()
{
	total_cycles_ = 0;
	dma_page_index_ = 0x00;
	dma_inpage_addr_ = 0x00;
	dma_data_on_transfer_ = 0x00;
	is_dma_mode_ = false;
	cycles_on_dma_ = 0;
	current_frame_cpu_cycles_ = 0;

}
Bus::~Bus()
{
}

bool Bus::write(uint16_t addr, uint8_t data)
{	
	bool ret = false;
	cart_->prg_addr(addr, addr);
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		onboard_ram_[addr & 0x07FF] = data;
		ret = true;
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		controller_state_[addr & 0x0001] = controller_[addr & 0x0001];
		ret = true;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) //to ppu memory-mapped registers.
	{
		ret = ppu_->register_write(addr, data);
	}
	else if (addr == 0x4014)
	{
		dma_page_index_ = data;
		dma_inpage_addr_ = 0x00;

		is_dma_mode_ = true;
		cycles_on_dma_ = 0;
		ret = true;
	}
	else if (addr >= 0x4000 && addr <= 0x401f)
	{
		/*
		$4000 - $4017	$0018	NES APU and I / O registers
		$4018 - $401F	$0008	APU and I / O functionality that is normally disabled.See CPU Test Mode.
		*/
		apu_->register_write(current_frame_elapsed(), addr, data);
		ret = true;
	}

	assert_ex(ret, std::cerr << "write addr = "<< addr << std::endl);
	return ret;
}

bool Bus::read(uint16_t addr, uint8_t &data, bool read_only)
{
	bool ret = false;
	cart_->prg_addr(addr, addr);
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		data = onboard_ram_[addr & 0x07FF];
		ret = true;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		ret = ppu_->register_read(addr, data, read_only);
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		data = (controller_state_[addr & 0x0001] & 0x80) > 0;
		controller_state_[addr & 0x0001] <<= 1;
		ret = true;
	}
	else if (addr >= 0x4020)
	{
		ret = cart_->prg_read(addr, data);
	}
	else if (addr >= 0x4000 && addr <= 0x401f)
	{
		/*
		$4000 - $4017	$0018	NES APU and I / O registers
		$4018 - $401F	$0008	APU and I / O functionality that is normally disabled.See CPU Test Mode.
		*/
		apu_->register_read(current_frame_elapsed(), addr, data);
		ret = true;
	}
	assert_ex(ret, std::cerr << "addr = "<< addr << std::endl);
	return ret;
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

bool Bus::connect_apu(std::shared_ptr<APU2A03> apu)
{
	if (!apu)
	{
		return false;
	}
	apu_ = apu;
	apu_->connect_bus(this);
	return true;
}

void Bus::reset()
{
	cart_->reset();
	cpu_->reset();
	ppu_->reset();


	total_cycles_ = 0;
	dma_page_index_ = 0x00;
	dma_inpage_addr_ = 0x00;
	dma_data_on_transfer_ = 0x00;
	is_dma_mode_ = false;
	cycles_on_dma_ = 0;

	current_frame_cpu_cycles_ = 0;
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
		((uint8_t*)ppu_->oam_)[dma_inpage_addr_++] = dma_data_on_transfer_;
		if (dma_inpage_addr_ == 0x00)
		{
			is_dma_mode_ = false;
		}
	}
	cycles_on_dma_++;
	return true;
}

bool Bus::clock()
{
	//todo: check cpu ppu cart
	ppu_->clock();
	if (total_cycles_ % 3 == 0)
	{
		if(!dma())
		{
			cpu_->clock();
		}
		current_frame_cpu_cycles_ ++;
	}
	if (ppu_->on_nmi_)
	{
		ppu_->on_nmi_ = false;
		cpu_->nmi();
	}
	total_cycles_++;

	if(ppu_->is_frame_complete_ == true)
	{
		apu_->run_frame(current_frame_elapsed());
		current_frame_cpu_cycles_ = 0;
		ppu_->is_frame_complete_ = false;  //new frame
		return true;
	}
	return false;

}

int Bus::current_frame_elapsed() 
{ 
	return current_frame_cpu_cycles_; 
}
void Bus::run_frame()
{
	while (!clock()){}
}
