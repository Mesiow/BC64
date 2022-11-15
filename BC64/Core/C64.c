#include "C64.h"

void c64_init(struct C64* c64)
{
	c64->emu_running = 0;
	
	mmio_init(&c64->mmio, &c64->vic, &c64->cia);
	mem_init(&c64->mem, &c64->mmio);
	cpu_init(&c64->cpu, &c64->mem);
	vic_init(&c64->vic, &c64->mem);
	cia_init(&c64->cia, &c64->vic);
}

void c64_load_software(struct C64* c64, const char* kb_path, const char* char_set_path)
{
	mem_load_firmware(&c64->mem, kb_path);
	mem_load_chrrom(&c64->mem, char_set_path);

	cpu_handle_reset(&c64->cpu); //set cpu to reset vector
}

void c64_run(struct C64* c64)
{
	c64->emu_running = 1;
	while (c64->emu_running) {
		cpu_clock(&c64->cpu);
		if (c64->cpu.pc == 0xE5A2) {
			printf("basic boots\n");
			break;
		}
		cpu_handle_irq(&c64->cpu);
	}
}

void c64_free(struct C64* c64)
{
	mem_free(&c64->mem);
}
