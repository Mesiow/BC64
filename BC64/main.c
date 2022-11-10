#include <stdio.h>
#include "Core/Cpu/Cpu.h"
#include "Core/Log.h"

int main(void) {

	/*struct C64 c64;
	c64_init(&c64);

	c64_free(&c64);*/
	struct Cpu6510 cpu;
	cpu_init(&cpu);
	load_cpu_test_rom(&cpu, "test_roms/AllSuiteA.bin"); //test passed

	while (1) {
		cpu_clock(&cpu);
		
    	//cpu_handle_interrupts(&cpu);
	}

	cpu_free_test(&cpu);

	//struct Log log;
	//struct RegisterState state;
	//open_log(&log, "test_roms/nestest.log");

	//while (log_is_open(&log)) {
	//parse_log(&log, &state);
	//}

	//close_log(&log);

	return 0;
}