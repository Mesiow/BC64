#include "Memory.h"

void mem_init(struct Memory* mem)
{
	mem->ram = malloc(0x10000 * sizeof(u8));
	mem->kernal = malloc(0x2000 * sizeof(u8));
	mem->basic_int = malloc(0x2000 * sizeof(u8));

	memset(mem->kernal, 0x0, 0x2000);
	memset(mem->basic_int, 0x0, 0x2000);
	memset(mem->chr1, 0x0, 0x800);
	memset(mem->chr2, 0x0, 0x800);
}

void mem_free(struct Memory* mem)
{
	if (mem != NULL) {
		if (mem->ram != NULL) free(mem->ram);
		if (mem->kernal != NULL) free(mem->kernal);
		if (mem->basic_int != NULL) free(mem->basic_int);
	}
}

void mem_load_firmware(struct Memory* mem, const char* path)
{
	//Load kernal and basic interpreter
	FILE* file = fopen(path, "rb");
	if (file != NULL) {
		//Determine rom size
		fseek(file, 0, SEEK_END);
		u16 file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		if (file_size > 0x4000 || file_size < 0x4000) {
			//firmware shouldnt be larger than 16KB
			printf("Firmware failed to load (Invalid rom size).\n");
			fclose(file);
			return;
		}

		u8* firmware = malloc(file_size);
		if (firmware != NULL) {
			fread(firmware, sizeof(u8), file_size, file);
			for (s32 i = 0; i < 0x2000; i++)
				mem->kernal[i] = firmware[i + 0x2000];
			for (s32 i = 0; i < 0x2000; i++)
				mem->basic_int[i] = firmware[i];

			free(firmware);
		}
		fclose(file);
	}
}

void mem_load_chrrom(struct Memory* mem, const char* path)
{
	//load both 2k character sets
}

void mem_write_u8(struct Memory* mem, u16 address, u8 value)
{

}

u8 mem_read_u8(struct Memory* mem, u16 address)
{
	if (address >= 0xA000 && address <= 0xBFFF) {
		return mem->basic_int[address & 0x1FFF];
	}
	else if (address >= 0xD000 && address <= 0xDFFF) {
		if (address <= 0xD7FF) {
			return mem->chr1[address & 0x800];
		}
		else if (address >= 0xD800) {
			return mem->chr2[address & 0x800];
		}
	}
	else if (address >= 0xE000 && address <= 0xFFFF) {
		return mem->kernal[address & 0x1FFF];
	}
}
