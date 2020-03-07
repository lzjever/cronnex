#include "APU2A03.h"

Bus* APU2A03::bus_ptr = NULL;
Nes_Apu APU2A03::apu;
Blip_Buffer APU2A03::buf;

blip_sample_t APU2A03::outBuf[OUT_SIZE];
//Sound_Queue* APU2A03::soundQueue = NULL;