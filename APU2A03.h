#pragma once
#include <cstdint>
#include <memory>
#include "Bus.h"
#include "3rdpatry/NesSndEmu/Nes_Apu.h"

namespace APU2A03
{
    Bus* bus_ptr;
    Nes_Apu apu;
    Blip_Buffer buf;

    const int OUT_SIZE = 4096;
    blip_sample_t outBuf[OUT_SIZE];


    int dmc_read(void*, cpu_addr_t addr)
    {
        uint8_t data;
        bus_ptr->read(addr, data);
        return data;
    }


    void init(Bus* bus)
    {
        bus_ptr = bus;
        buf.sample_rate(96000);
        buf.clock_rate(1789773);

        apu.output(&buf);
        apu.dmc_reader(dmc_read);
    }

    void reset()
    {
        apu.reset();
        buf.clear();
    }

    bool register_write(int elapsed, uint16_t addr, uint8_t data)
    {
        if (addr >= apu.start_addr && addr <= apu.end_addr)
        {
            apu.write_register(elapsed, addr, data);
            return true;
        }
        return false;
    }
    bool register_read(int elapsed, uint16_t addr, uint8_t data)
    {
        if (addr == apu.status_addr)
        {
            data = apu.read_status(elapsed);
            return true;
        }
        return false;
    }

    void run_frame(int elapsed)
    {
        apu.end_frame(elapsed);
        buf.end_frame(elapsed);

        if (buf.samples_avail() >= OUT_SIZE)
            //GUI::new_samples(outBuf, buf.read_samples(outBuf, OUT_SIZE));
        { }
    }

};

