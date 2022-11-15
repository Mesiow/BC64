#pragma once
#include "Util.h"

struct RegisterState {
	u8 acc;
	u8 x;
	u8 y;
	u8 sr;
	u16 sp;
	u16 pc;
};

struct Log {
	char* log;
	FILE* file;
};

void open_log(struct Log* log, const char* path);
void parse_log(struct Log* log, struct RegisterState* rs);
void close_log(struct Log* log);