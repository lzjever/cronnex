#pragma once
#include <cstdint>
#include <functional>
#include "config.h"
#include "Bus16Bits.h"
#include "nes_apu/Nes_Apu.h"

#define SAMPLE_OUT_SIZE 4096
class APU2A03
{
public:
    static int dmc_read(void* user_data, cpu_addr_t addr);

    APU2A03(std::function<bool(int16_t*, int32_t)>  sample_callback);

    void connect_bus(Bus16Bits* bus_ptr) { bus_ptr_ = bus_ptr; }
    void reset();
    int on_dmc_read(cpu_addr_t addr);
    bool register_write(int elapsed, uint16_t addr, uint8_t data);
    bool register_read(int elapsed, uint16_t addr, uint8_t data);
    void run_frame(int elapsed);

public:

    Bus16Bits* bus_ptr_;
    Nes_Apu apu;
    Blip_Buffer buf;
    blip_sample_t outBuf[SAMPLE_OUT_SIZE];
    std::function<bool(int16_t*, int32_t)>  sample_callback_;

};