#include "Cpu.h"
#include "Memory.h"

static u8 cyc_lookup[0x100] = {
	7, 6, 4, 5, 5

};

//addressing mode func ptrs for each instruction
u8 (*addr_mode_lookup[0x100])(struct Cpu6510*) = {
	NULL, indirect_x, NULL, NULL, NULL, zeropage, NULL, NULL, NULL, immediate, NULL, NULL, NULL, absolute, absolute, NULL,

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

u8 cpu_is_signed(u8 value)
{
	s8 signed_value = (s8)value;
	return (((signed_value >> 7) & 0x1) == 0x1);
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
	u8 addr_mode_result = addr_mode_lookup[opcode];
	u8 cycles = cyc_lookup[opcode];

	cpu_execute_instruction(cpu, opcode, addr_mode_result);

	return cycles;
}

void cpu_execute_instruction(struct Cpu6510* cpu, u8 opcode, u8 addr_mode_result)
{
	switch (opcode) {
		case 0x00: brk(cpu); break;
		case 0x01: ora(cpu, addr_mode_result); break;
		case 0x05: ora(cpu, addr_mode_result); break;
		case 0x08: php(cpu); break;
		case 0x09: ora(cpu, addr_mode_result); break;
		case 0x0D: ora(cpu, addr_mode_result); break;
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
	cpu_set_flag(cpu, FLAG_I);
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

void ora(struct Cpu6510* cpu, u8 addr_mode_result)
{
	u8 result = cpu->acc | addr_mode_result;
	cpu->acc |= addr_mode_result;

	cpu_affect_flag(cpu, result == 0, FLAG_Z);
	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
}

u8 immediate(struct Cpu6510* cpu)
{
	return cpu_fetch_u8(cpu);
}

u8 zeropage(struct Cpu6510* cpu)
{
	u8 zeropage_addr = cpu_fetch_u8(cpu);
	u8 value = cpu_read_u8(cpu, zeropage_addr);

	return value;
}

u8 absolute(struct Cpu6510* cpu)
{
	return cpu_fetch_u16(cpu);
}

u8 indirect_x(struct Cpu6510* cpu)
{
	u8 zeropage_addr = cpu_fetch_u8(cpu);
	u8 vector = cpu->x + zeropage_addr;

	u16 effective_address = cpu_read_u16(cpu, vector);
	u8 value = cpu_read_u8(cpu, effective_address);

	return value;
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
