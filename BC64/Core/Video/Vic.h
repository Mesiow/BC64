#pragma once
#include "../Util.h"

struct VicII {
	u8 memory_bank; //which memory bank does the vic read from
	struct Memory* mem;
};

void vic_init(struct VicII* vic, struct Memory *mem);
void vic_set_memory_bank(struct VicII* vic, u8 bank);
void vic_write_register(struct VicII* vic, u16 address, u8 value);
u8 vic_read_register(struct VicII* vic, u16 address);
void vic_write_u8(struct VicII* vic, u16 address, u8 value);
u8 vic_read_u8(struct VicII* vic, u16 address);
