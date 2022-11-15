#include "Vic.h"
#include "../Memory/Memory.h"

void vic_init(struct VicII* vic, struct Memory* mem)
{
	vic->mem = mem;
	vic->memory_bank = 0; //default
}

void vic_set_memory_bank(struct VicII* vic, u8 bank)
{
	//Vic can only read from a 16K window of memory at a time
	//This function is called from the CIA
	switch (bank) {
		case 0b00: vic->memory_bank = 3; break; //C000 - FFFF
		case 0b01: vic->memory_bank = 2; break; //8000 - BFFF
		case 0b10: vic->memory_bank = 1; break; //4000 - 7FFF
		case 0b11: vic->memory_bank = 0; break; //0000 - 3FFF
	}
}

void vic_write_register(struct VicII* vic, u16 address, u8 value)
{
}

u8 vic_read_register(struct VicII* vic, u16 address)
{

}

void vic_write_u8(struct VicII* vic, u16 address, u8 value)
{
}

u8 vic_read_u8(struct VicII* vic, u16 address)
{
	switch (vic->memory_bank) {
		case 0: return mem_read_u8(vic->mem, (address & 0x3FFF));
		case 1: {
			//Mirror the address if the address isn't in the correct block
			//(dont know if I even need to to do this but i'll keep it here anyway)
			/*if (address < 0x4000 || address > 0x7FFF)
				address = (address & 0x7FFF) + 0x4000;*/

			return mem_read_u8(vic->mem, (address & 0x7FFF));
		}
		case 2: {
			/*if (address < 0x8000 || address > 0xBFFF)
				address = (address & 0xBFFF) + 0x4000;*/

			return mem_read_u8(vic->mem, (address & 0xBFFF));
		}
		case 3:
			return mem_read_u8(vic->mem, (address & 0xFFFF));
	}
}
