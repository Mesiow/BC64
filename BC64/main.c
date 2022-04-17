#include <stdio.h>
#include "Core\Memory.h"

int main(void) {

	struct Memory mem;
	memLoadFirmware(&mem, "Software/c64_kernal_basic.bin");

	return 0;
}