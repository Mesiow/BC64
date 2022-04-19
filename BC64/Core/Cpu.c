#include "Cpu.h"
#include "Memory.h"

static u8 cyc_lookup[0x100] = {
	7, 6, 4, 5, 5

};

void cpu_init(struct Cpu6510* cpu)
{
	cpu->acc = 0x0;
	cpu->x = 0x0;
	cpu->y = 0x0;
	cpu->sr = 0x0;
	cpu->sp = 0x1FF; //stack lives at 0x100 - 0x1FF
	cpu->pc = RESET_VECTOR;
}

void cpu_set_flag(struct Cpu6510* cpu, u8 flags)
{
	cpu->sr |= (flags & 0b1101'1111);
}

void cpu_clear_flag(struct Cpu6510* cpu, u8 flags)
{
	cpu->sr &= ~(flags & 0b1101'1111);
}

void cpu_affect_flag(struct Cpu6510* cpu, u8 condition, u8 flags)
{
	if (condition) {
		cpu->sr |= (flags & 0b1101'1111);
	}
	else {
		cpu->sr &= ~(flags & 0b1101'1111);
	}
}

void cpu_write_mem_u8(struct Cpu6510* cpu, u8 value, u16 address)
{
	mem_write_u8(cpu->mem, value, address);
}

u16 cpu_fetch_u16(struct Cpu6510* cpu)
{
	u8 lo = mem_read_u8(cpu->mem, cpu->pc++);
	u8 hi = mem_read_u8(cpu->mem, cpu->pc++);

	return ((hi << 8) | lo);
}

u16 cpu_read_u16(struct Cpu6510* cpu, u16 address)
{
	u8 lo = mem_read_u8(cpu->mem, address);
	u8 hi = mem_read_u8(cpu->mem, address + 1);

	return ((hi << 8) | lo);
}

u8 cpu_fetch_u8(struct Cpu6510* cpu)
{
	u8 data = mem_read_u8(cpu->mem, cpu->pc++);
	return data;
}

u8 cpu_read_u8(struct Cpu6510* cpu, u16 address)
{
	u8 data = mem_read_u8(cpu->mem, address);
	return data;
}

u8 cpu_clock(struct Cpu6510* cpu)
{
	u8 opcode = cpu_read_u8(cpu, cpu->pc++);
	u8 cycles = cyc_lookup[opcode];

	cpu_execute_instruction(cpu, opcode);

	return cycles;
}

void cpu_execute_instruction(struct Cpu6510* cpu, u8 opcode)
{
	switch (opcode) {
		case 0x00: brk(cpu); break;
		case 0x08: php(cpu); break;
		case 0x20: jsr(cpu); break;
		case 0x60: rts(cpu); break;
	}
}

void brk(struct Cpu6510* cpu)
{
	//return address is pc + 2, + 1 for extra byte of spacing for a break mark
	cpu->pc++;
	push_u16(cpu, cpu->pc);

	//push status reg as well
	cpu_set_flag(cpu, FLAG_B);
	push_u8(cpu, cpu->sr);
}

void jsr(struct Cpu6510* cpu)
{
	push_u16(cpu, cpu->pc + 2); //push the address of the instruction after this one
	cpu->pc = cpu_fetch_u16(cpu);
}

void rts(struct Cpu6510* cpu)
{
	pop_u16(cpu, &cpu->pc);
}

void php(struct Cpu6510* cpu)
{
	//push status reg with break flag and bit 5 set
	cpu_set_flag(cpu, FLAG_B);
	cpu->sr |= (1 << 5);

	push_u8(cpu, cpu->sr);
}

void plp(struct Cpu6510* cpu)
{
	pop_u8(cpu, &cpu->sr);
	cpu->sr &= 0xCF; //break flag and bit 5 ignored
}

void push_u16(struct Cpu6510* cpu, u16 value)
{
	cpu->sp--;
	cpu_write_mem_u8(cpu, ((value) >> 8) & 0xFF, cpu->sp);
	cpu->sp--;
	cpu_write_mem_u8(cpu, value & 0xFF, cpu->sp);
}

void push_u8(struct Cpu6510* cpu, u8 value)
{
	cpu->sp--;
	cpu_write_mem_u8(cpu, value, cpu->sp);
}

void pop_u16(struct Cpu6510* cpu, u16 *reg)
{
	u8 lo = cpu_read_u8(cpu, cpu->sp);
	cpu->sp++;
	u8 hi = cpu_read_u8(cpu, cpu->sp);
	cpu->sp++;

	*reg = ((hi << 8) | lo);
}

void pop_u8(struct Cpu6510* cpu, u8* reg)
{
	u8 value = cpu_read_u8(cpu, cpu->sp++);
	*reg = value;
}
