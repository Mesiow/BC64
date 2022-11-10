#include "C64.h"

void c64_init(struct C64* c64)
{
	mem_init(&c64->mem);
}

void c64_load_software(struct C64* c64, const char* kb_path, const char* char_set_path)
{
	mem_load_firmware(&c64->mem, kb_path);
	mem_load_chrrom(&c64->mem, char_set_path);
}

void c64_run(struct C64* c64)
{

}

void c64_free(struct C64* c64)
{
	mem_free(&c64->mem);
}
