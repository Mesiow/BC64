#include "Mmio.h"
#include "Cia.h"

void mmio_init(struct Mmio* mmio, struct VicII* vic, struct Cia2* cia)
{
	mmio->vic = vic;
	mmio->cia = cia;

	mmio->data_direction_register = 0x7; //low 3 bits default to 1
	mmio->port_register = 0x7; //control lines are set hi by default to allow reading of rom
}

void mmio_write_device(struct Mmio* mmio, u16 mmio_address, u8 value)
{
	switch (mmio_address) {
		case 0x0: mmio->data_direction_register = value; break;
		case 0x1: mmio->port_register = value; break;

		//Cia 2
		case 0xDD00: 
		case 0xDD01:
			cia_write_register(mmio->cia, mmio_address, value);
			break;
	}
}

u8 mmio_read_device(struct Mmio* mmio, u16 mmio_address)
{
	switch (mmio_address) {
		case 0x0: return mmio->data_direction_register;
		case 0x1: return mmio->port_register;

		//Cia 2
		case 0xDD00:
		case 0xDD01:
			return cia_read_register(mmio->cia, mmio_address);
	}
}

u8 mmio_read_data_register(struct Mmio* mmio)
{
	return mmio->data_direction_register;
}

u8 mmio_read_port_register(struct Mmio* mmio, enum Control cl)
{
	switch (cl) {
		case LORAM: return (mmio->port_register & 0x1);
		case HIRAM: return (mmio->port_register >> 1) & 0x1;
		case CHAREN: return (mmio->port_register >> 2) & 0x1;
	}
}
