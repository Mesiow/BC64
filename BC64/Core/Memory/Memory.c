#include "Memory.h"

void mem_init(struct Memory* mem)
{
	mem->ram = malloc(0x10000 * sizeof(u8));
	mem->kernal = malloc(0x2000 * sizeof(u8));
	mem->basic_int = malloc(0x2000 * sizeof(u8));

	memset(mem->ram, 0x0, 0x10000);
	memset(mem->stack, 0x0, 0xFF);
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
		u32 file_size = ftell(file);
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

			printf("--Firmware loaded--\n");
			free(firmware);
		}
		fclose(file);
	}
}

void mem_load_chrrom(struct Memory* mem, const char* path)
{
	//Load both 2k character sets
	FILE* file = fopen(path, "rb");
	if (file != NULL) {
		//Determine rom size
		fseek(file, 0, SEEK_END);
		u32 file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		if (file_size > 0x1000 || file_size < 0x1000) {
			//char set shouldnt be larger or less than 4KB
			printf("Char Set failed to load (Invalid rom size).\n");
			fclose(file);
			return;
		}

		u8* char_set = malloc(file_size);
		if (char_set != NULL) {
			fread(char_set, sizeof(u8), file_size, file);
			for (s32 i = 0; i < 0x800; i++)
				mem->chr2[i] = char_set[i + 0x800];
			for (s32 i = 0; i < 0x800; i++)
				mem->chr1[i] = char_set[i];

			printf("--Char Set loaded--\n");
			free(char_set);
		}
		fclose(file);
	}
}

void mem_write_u8(struct Memory* mem, u8 value, u16 address)
{
	//If rom visible during write, just write to the
	//underlying ram anyway
	if (address <= 0x1) {
		mmio_write_u8(mem->mmio, address, value);
	}
	else if (address >= 0x2 && address <= 0xFF) {
		mem->ram[address & 0xFF] = value;
	}
	else if (address >= 0x100 && address <= 0x1FF) {
		mem->stack[address & 0xFF] = value;
	}
	else if (address >= 0x200 && address <= 0xFFFF) {
		mem->ram[address & 0xFFFF] = value;
	}
}

u8 mem_read_u8(struct Memory* mem, u16 address)
{
	if (address >= 0xA000 && address <= 0xBFFF) {
		if (mmio_read_port_register(mem->mmio, LORAM))
			return mem->basic_int[address & 0x1FFF];
		else
			return mem->ram[address & 0x1FFF];
	}
	else if (address >= 0xD000 && address <= 0xDFFF) {
		if (mmio_read_port_register(mem->mmio, CHAREN)) {
			//io devices mapped
			return mmio_read_u8(mem->mmio, address);
		}
		else {
			//character rom mapped
			if (address <= 0xD7FF) {
				return mem->chr1[address & 0x800];
			}
			else if (address >= 0xD800) {
				return mem->chr2[address & 0x800];
			}
		}
	}
	else if (address >= 0xE000 && address <= 0xFFFF) {
		if (mmio_read_port_register(mem->mmio, HIRAM))
			return mem->kernal[address & 0x1FFF];
		else
			return mem->ram[address & 0x1FFF];
	}
	else {
		//2 Important registers reside at address 0 and 1
		if (address <= 0x1) {
			mmio_read_u8(mem->mmio, address);
		}
		//Zeropage
		if (address >= 0x2 && address <= 0xFF) {
			return mem->ram[address & 0xFF];
		}
		//Stack
		else if (address >= 0x100 && address <= 0x1FF) {
			return mem->stack[address & 0xFF];
		}
		//The rest of ram
		//This range has no bankswitching of any other rom
		else if (address >= 0x200 && address <= 0x7FFF) {
			return mem->ram[address & 0x7FFF];
		}
		//This range also has no other rom bankswitched here
		else if (address >= 0xC000 && address <= 0xCFFF) {
			return mem->ram[address];
		}
	}
}
