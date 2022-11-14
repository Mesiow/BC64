#pragma once
#include "../Util.h"

struct VicII {
	struct Memory* mem;
};

void vic_init(struct VicII* vic, struct Memory *mem);
void vic_write_register(struct VicII* vic, u16 address, u8 value);
u8 vic_read_register(struct VicII* vic, u16 address);
