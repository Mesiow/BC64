#include "Log.h"
#include "Cpu\Cpu.h"
#include <errno.h>

void open_log(struct Log* log, const char* path)
{
	log->file = fopen(path, "r");
	if (log->file == NULL) {
		printf("%s\n", strerror(errno));
		return;
	}
}

void parse_log(struct Log* log, struct RegisterState* rs)
{
	if (log != NULL) {
		//get line
		char buf[256];
		fgets(buf, 256, log->file);
		printf("%s\n", buf);

		//Parse program counter
		u32 pc;
		rs->pc = strtoul(buf, NULL, 16);
	
		printf("pc: 0x%02X\n", rs->pc);

		char a;
		//while ((a = fgetc(log->file)) != ':') {} //move to a register
		rs->acc = strtoul(buf, NULL, 36);
		//sprintf()
		//fgets(buf, 255, log->file);
		//rs->acc = strtoul(buf, NULL, 8);
		//printf("%s\n", buf);
		//scanf(a, "%x", &rs->acc);
		printf("acc: %02X\n", rs->acc);

		//sprintf()
	}
}

void close_log(struct Log* log) {
	if (log != NULL) {
		if (log->file != NULL) fclose(log->file);
	}
}