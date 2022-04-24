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

	cpu->cycles = 0;
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

u8 cpu_get_flag(struct Cpu6510* cpu, u8 flag)
{
	u8 mask = (cpu->sr & flag);
	return (mask ? 1 : 0);
}

u8 cpu_is_signed(u8 value)
{
	s8 signed_value = (s8)value;
	return (((signed_value >> 7) & 0x1) == 0x1);
}

u8 cpu_overflow_from_add_u8(u8 op1, u8 op2, u8 carry)
{
	u8 total = (op1 + op2 + carry) & 0xFF;
	//check if sign are same and if they are, ~(op1 ^ op2) will evaluate to 1
	//then check results sign and then and them together and determine overflow by the msb
	return (~(op1 ^ op2) & ((op1 ^ total))) >> 7;
}

u8 cpu_overflow_from_sub_u8(u8 op1, u8 op2, u8 carry)
{
	u8 total = (op1 - op2 - carry) & 0xFF;
	//first check if both operands have a different sign,
	//then check if the result has the same sign as the second operand
	return (((s8)(op1 ^ op2) < 0) && ((s8)(op2 ^ total) >= 0));
}

u8 cpu_carry_occured_u8(u8 op1, u8 op2, u8 carry)
{
	return (((op1 & 0xFF) + (op2 & 0xFF) + (carry)) > 0xFF);
}

u8 cpu_borrow_occured_u8(u8 op1, u8 op2, u8 carry)
{
	return ((op2 + carry) > op1);
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
		case 0x01: ora_indir_x(cpu); break;
		case 0x05: ora_zpg(cpu, zeropage(cpu)); break;
		case 0x06: asl_zpg(cpu, zeropage(cpu)); break;
		case 0x08: php(cpu); break;
		case 0x09: ora_imm(cpu); break;
		case 0x0A: asla(cpu); break;
		case 0x0D: ora_abs(cpu, absolute(cpu)); break;
		case 0x0E: asl_abs(cpu, absolute(cpu)); break;
		case 0x10: bpl(cpu); break;
		case 0x11: ora_indir_y(cpu); break;
		case 0x15: ora_zpg(cpu, zeropage_x(cpu)); break;
		case 0x16: asl_zpg(cpu, zeropage_x(cpu)); break;
		case 0x19: ora_abs(cpu, absolute_y(cpu)); break;
		case 0x1D: ora_abs(cpu, absolute_x(cpu)); break;
		case 0x1E: asl_abs(cpu, absolute_x(cpu)); break;
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

void ora(struct Cpu6510* cpu, u8 value)
{
	u8 result = cpu->acc | value;
	cpu->acc |= value;

	cpu_affect_flag(cpu, result == 0, FLAG_Z);
	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
}

void ora_imm(struct Cpu6510* cpu)
{
	ora(cpu, immediate(cpu));
}

void ora_zpg(struct Cpu6510* cpu, u8 zpg_address)
{
	u8 value = cpu_read_u8(cpu, zpg_address);
	ora(cpu, value);
}

void ora_abs(struct Cpu6510* cpu, u16 abs_address)
{
	u8 value = cpu_read_u8(cpu, abs_address);
	ora(cpu, value);
}

void ora_indir_x(struct Cpu6510* cpu)
{
	ora(cpu, indirect_x(cpu));
}

void ora_indir_y(struct Cpu6510* cpu)
{
	ora(cpu, indirect_y(cpu));
}

void asl(struct Cpu6510* cpu, u8 *value)
{
	u8 msb = (*value) >> 7;
	u8 result = (*value) << 1;

	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
	cpu_affect_flag(cpu, msb, FLAG_C);

	*value <<= 1;
}

void asl_abs(struct Cpu6510* cpu, u16 abs_address)
{
	u8 value = cpu_read_u8(cpu, abs_address);
	asl(cpu, &value);

	cpu_write_mem_u8(cpu, value, abs_address);
}

void asl_zpg(struct Cpu6510* cpu, u8 zpg_address)
{
	u8 value = cpu_read_u8(cpu, zpg_address);
	asl(cpu, &value);

	cpu_write_mem_u8(cpu, value, zpg_address);
}

void asla(struct Cpu6510* cpu)
{
	asl(cpu, &cpu->acc);
}

void bpl(struct Cpu6510* cpu)
{
	if (cpu_get_flag(cpu, FLAG_N)) {
		s8 offset = (s8)cpu_fetch_u8(cpu);
		cpu->pc += offset;
		cpu->cycles += 1;
	}
	else {
		cpu->pc++;
	}
}

u8 immediate(struct Cpu6510* cpu)
{
	return cpu_fetch_u8(cpu);
}

u8 zeropage(struct Cpu6510* cpu)
{
	u8 zeropage_addr = cpu_fetch_u8(cpu);
	return zeropage_addr;
}

u8 zeropage_x(struct Cpu6510* cpu)
{
	u8 zeropage_addr = cpu_fetch_u8(cpu) + cpu->x;
	return zeropage_addr;
}

u8 zeropage_y(struct Cpu6510* cpu)
{
	u8 zeropage_addr = cpu_fetch_u8(cpu) + cpu->y;
	return zeropage_addr;
}

u16 absolute(struct Cpu6510* cpu)
{
	return cpu_fetch_u16(cpu);
}

u16 absolute_x(struct Cpu6510* cpu)
{
	u16 address = cpu_fetch_u16(cpu) + cpu->x;
	return address;
}

u16 absolute_y(struct Cpu6510* cpu)
{
	u16 address = cpu_fetch_u16(cpu) + cpu->y;
	return address;
}

u8 indirect_x(struct Cpu6510* cpu)
{
	u8 zeropage_addr = cpu_fetch_u8(cpu);
	u8 vector = cpu->x + zeropage_addr;

	u16 effective_address = cpu_read_u16(cpu, vector);
	u8 value = cpu_read_u8(cpu, effective_address);

	return value;
}

u8 indirect_y(struct Cpu6510* cpu)
{
	u8 zeropage_vector = cpu_fetch_u8(cpu);
	u16 effective_address = cpu_read_u16(cpu, zeropage_vector) + cpu->y;

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
