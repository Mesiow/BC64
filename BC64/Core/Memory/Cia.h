#pragma once
#include "../Util.h"

struct Cia2 {
	u8 port_register_a;
	u8 port_register_b;
};

void cia_init(struct Cia2* cia);
void cia_write_register(struct Cia2* cia, u16 address, u8 value);
u8 cia_read_register(struct Cia2* cia, u16 address);
