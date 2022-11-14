#include "Vic.h"
#include "../Memory/Memory.h"

void vic_init(struct VicII* vic, struct Memory* mem)
{
	vic->mem = mem;
}

void vic_write_register(struct VicII* vic, u16 address, u8 value)
{
}

u8 vic_read_register(struct VicII* vic, u16 address)
{
	//return u8();
}
