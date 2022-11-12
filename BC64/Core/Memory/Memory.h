#pragma once
#include "Mmio.h"

/*
	C64 memory layout

    $FFFF = 65535  
    //Kernal
    $E000 = 57344 
    //Char        
    $D000 = 53248 

    $C000 = 49152
    //Basic Rom
    $A000 = 40960 

    $0800 = 2048
    //Vram
    $0400 = 1024

    $0000
*/

struct Memory {
    u8 *ram;
    u8 stack[0x100];
    u8 *kernal;
    u8 *basic_int;
    u8 chr1[0x800];
    u8 chr2[0x800];

    struct Mmio* mmio;
};

void mem_init(struct Memory* mem);
void mem_free(struct Memory* mem);
void mem_load_firmware(struct Memory* mem, const char* path);
void mem_load_chrrom(struct Memory* mem, const char* path);
void mem_write_u8(struct Memory* mem, u8 value, u16 address);
u8 mem_read_u8(struct Memory* mem, u16 address);