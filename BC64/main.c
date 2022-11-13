#include <stdio.h>
#include "Core\C64.h"

int main(void) {
	struct C64 c64;
	c64_init(&c64);
	c64_load_software(&c64, "Software/c64_kernal_basic.bin",
		"Software/characters.bin");

	c64_run(&c64);
	
	c64_free(&c64);
	//load_cpu_test_rom(&cpu, "test_roms/AllSuiteA.bin"); //test passed
	//load_cpu_test_rom(&cpu, "test_roms/single_tests/6502_functional_test.bin");
	//u16 prev_pc = 0;
	//while (1) {
	//	cpu_clock(&cpu);
	//	
	//	/*if (prev_pc == cpu.pc) {
	//		if (cpu.pc == 0x3469) {
	//			printf("PASS");
	//			break;
	//		}
	//		printf("FAIL (trapped at 0x%04X)", cpu.pc);
	//		break;
	//	}
	//	prev_pc = cpu.pc;*/
 //   	cpu_handle_irq(&cpu);
	//}

	//cpu_free_test(&cpu);

	return 0;
}