#pragma once
#include "Util.h"

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
    u8 kernal[0x2000];
    u8 basic_int[0x2000];
    u8 chr1[0x800];
    u8 chr2[0x800];
};

void memInit(struct Memory* mem);
void memLoadFirmware(struct Memory* mem, const char* path);
void memLoadChrRom(struct Memory* mem, const char* path);
void memWriteU8(struct Memory* mem, u16 address, u8 value);
u8 memReadU8(struct Memory* mem, u16 address);