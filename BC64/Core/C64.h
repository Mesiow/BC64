#include "Memory/Memory.h"


struct C64 {
	struct Memory mem;
};

void c64_init(struct C64* c64);
//load original software for the system (kernal and basic interpreter)
void c64_load_software(struct C64* c64, const char* kb_path, const char *char_set_path);
void c64_run(struct C64* c64);
void c64_free(struct C64* c64);



