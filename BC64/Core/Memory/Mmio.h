#pragma once
#include "../Util.h"

//Cpu control lines for bankswitching
enum Control {
	LORAM = 0,
	HIRAM,
	CHAREN
};

struct Mmio {
	//0x0 and 0x1
	u8 data_direction_register;
	u8 port_register;
};

void mmio_init(struct Mmio* mmio);
void mmio_write_u8(struct Mmio* mmio, u16 mmio_address, u8 value);
u8 mmio_read_u8(struct Mmio* mmio, u16 mmio_address);
u8 mmio_read_port_register(struct Mmio* mmio,  enum Control cl);
