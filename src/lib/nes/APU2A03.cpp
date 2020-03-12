#include <memory>
#include "APU2A03.h"


APU2A03::APU2A03(std::function<bool(int16_t*, int32_t)>  sample_callback)
    :sample_callback_(sample_callback)
{
    //std::function<bool(int16_t, int32_t)> callback_sample_out
    
    buf.sample_rate(44100);
    buf.clock_rate(1789773);

    apu.output(&buf);
    apu.dmc_reader(dmc_read, (void*)this);

}

int APU2A03::dmc_read(void* user_data, cpu_addr_t addr)
{
    APU2A03* apu_ptr = (APU2A03*)user_data;
    return apu_ptr->on_dmc_read(addr);
}

void APU2A03::reset()
{
    apu.reset();
    buf.clear();
}

int APU2A03::on_dmc_read(cpu_addr_t addr)
{
    uint8_t data;
    bus_ptr_->read(addr, data);
    return data;
}

bool APU2A03::register_write(int elapsed, uint16_t addr, uint8_t data)
{
    if (addr >= apu.start_addr && addr <= apu.end_addr)
    {
        apu.write_register(elapsed, addr, data);
        return true;
    }
    return false;
}
bool APU2A03::register_read(int elapsed, uint16_t addr, uint8_t data)
{
    if (addr == apu.status_addr)
    {
        data = apu.read_status(elapsed);
        return true;
    }
    return false;
}

void APU2A03::run_frame(int elapsed)
{
    apu.end_frame(elapsed);
    buf.end_frame(elapsed);
    if (buf.samples_avail() >= SAMPLE_OUT_SIZE)
    {
        sample_callback_((int16_t*)outBuf, (int32_t)buf.read_samples(outBuf, SAMPLE_OUT_SIZE));
    }
}


