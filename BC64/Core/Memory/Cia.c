#include "Cia.h"
#include "../Video/Vic.h"

void cia_init(struct Cia2* cia, struct VicII *vic)
{
	cia->vic = vic;

	cia->port_register_a = 0x3;
	cia->port_register_b = 0x0;
}

void cia_write_register(struct Cia2* cia, u16 address, u8 value)
{
	switch (address & 0xFF) {
		case 0x00: 
			cia->port_register_a = value;
			vic_set_memory_bank(cia->vic, (cia->port_register_a & 0x3));
			break;
		case 0x01: cia->port_register_b = value; break;
	}
}

u8 cia_read_register(struct Cia2* cia, u16 address)
{
	switch (address & 0xFF) {
		case 0x00: return cia->port_register_a;
		case 0x01: return cia->port_register_b;
	}
}
