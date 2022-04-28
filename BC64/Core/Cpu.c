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
		case 0x10: branch(cpu, cpu_get_flag(cpu, FLAG_N) == 0); break;
		case 0x11: ora_indir_y(cpu); break;
		case 0x15: ora_zpg(cpu, zeropage_x(cpu)); break;
		case 0x16: asl_zpg(cpu, zeropage_x(cpu)); break;
		case 0x18: clc(cpu); break;
		case 0x19: ora_abs(cpu, absolute_y(cpu)); break;
		case 0x1D: ora_abs(cpu, absolute_x(cpu)); break;
		case 0x1E: asl_abs(cpu, absolute_x(cpu)); break;
		case 0x20: jsr(cpu); break;
		case 0x21: and_indir_x(cpu); break;
		case 0x24: bit_zpg(cpu, zeropage(cpu)); break;
		case 0x25: and_zpg(cpu, zeropage(cpu)); break;
		case 0x26: rol_zpg(cpu, zeropage(cpu)); break;
		case 0x28: plp(cpu); break;
		case 0x29: and_imm(cpu); break;
		case 0x2A: rola(cpu); break;
		case 0x2C: bit_abs(cpu, absolute(cpu)); break;
		case 0x2D: and_abs(cpu, absolute(cpu)); break;
		case 0x2E: rol_abs(cpu, absolute(cpu)); break;
		case 0x30: branch(cpu, cpu_get_flag(cpu, FLAG_N)); break;
		case 0x31: and_indir_y(cpu); break;
		case 0x35: and_zpg(cpu, zeropage_x(cpu)); break;
		case 0x36: rol_zpg(cpu, zeropage_x(cpu)); break;
		case 0x38: sec(cpu); break;
		case 0x39: and_abs(cpu, absolute_y(cpu)); break;
		case 0x3D: and_abs(cpu, absolute_x(cpu)); break;
		case 0x3E: rol_abs(cpu, absolute_x(cpu)); break;
		case 0x40: rti(cpu); break;
		case 0x41: eor_indir_x(cpu); break;
		case 0x45: eor_zpg(cpu, zeropage(cpu)); break;
		case 0x46: lsr_zpg(cpu, zeropage(cpu)); break;
		case 0x48: pha(cpu); break;
		case 0x49: eor_imm(cpu); break;
		case 0x4A: lsra(cpu); break;
		case 0x4C: jmp_abs(cpu); break;
		case 0x4D: eor_abs(cpu, absolute(cpu)); break;
		case 0x4E: lsr_abs(cpu, absolute(cpu)); break;
		case 0x51: eor_indir_y(cpu); break;
		case 0x55: eor_zpg(cpu, zeropage_x(cpu)); break;
		case 0x56: lsr_zpg(cpu, zeropage_x(cpu)); break;
		case 0x58: cli(cpu); break;
		case 0x59: eor_abs(cpu, absolute_y(cpu)); break;
		case 0x5D: eor_abs(cpu, absolute_x(cpu)); break;
		case 0x5E: lsr_abs(cpu, absolute_x(cpu)); break;
		case 0x60: rts(cpu); break;
		case 0x61: adc_indir_x(cpu); break;
		case 0x65: adc_zpg(cpu, zeropage(cpu)); break;
		case 0x66: ror_zpg(cpu, zeropage(cpu)); break;
		case 0x69: adc_imm(cpu); break;
		case 0x6A: rora(cpu); break;
		case 0x6C: jmp_ind(cpu); break;
		case 0x6D: adc_abs(cpu, absolute(cpu)); break;
		case 0x6E: ror_abs(cpu, absolute(cpu)); break;
		case 0x70: branch(cpu, cpu_get_flag(cpu, FLAG_V)); break;
		case 0x71: adc_indir_y(cpu); break;
		case 0x75: adc_zpg(cpu, zeropage_x(cpu)); break;
		case 0x76: ror_zpg(cpu, zeropage_x(cpu)); break;
		case 0x78: sei(cpu); break;
		case 0x79: adc_abs(cpu, absolute_y(cpu)); break;
		case 0x7D: adc_abs(cpu, absolute_x(cpu)); break;
		case 0x7E: ror_abs(cpu, absolute_x(cpu)); break;
		case 0x81: store(cpu, cpu->acc, indirect_x(cpu)); break;
		case 0x84: store(cpu, cpu->y, zeropage(cpu)); break;
		case 0x85: store(cpu, cpu->acc, zeropage(cpu)); break;
		case 0x86: store(cpu, cpu->x, zeropage(cpu)); break;
		case 0x8A: transfer(cpu, cpu->x, &cpu->acc); break;
		case 0x8C: store(cpu, cpu->y, absolute(cpu)); break;
		case 0x8D: store(cpu, cpu->acc, absolute(cpu)); break;
		case 0x8E: store(cpu, cpu->x, absolute(cpu)); break;
		case 0x90: branch(cpu, cpu_get_flag(cpu, FLAG_C) == 0); break;
		case 0x91: store(cpu, cpu->acc, indirect_y(cpu)); break;
		case 0x94: store(cpu, cpu->y, zeropage_x(cpu)); break;
		case 0x95: store(cpu, cpu->acc, zeropage_x(cpu)); break;
		case 0x96: store(cpu, cpu->x, zeropage_y(cpu)); break;
		case 0x98: transfer(cpu, cpu->y, &cpu->acc); break;
		case 0x99: store(cpu, cpu->acc, absolute_y(cpu)); break;
		case 0x9A: transfer(cpu, cpu->x, &cpu->sp); break;
		case 0x9D: store(cpu, cpu->acc, absolute_x(cpu)); break;
		case 0xA0: load_imm(cpu, &cpu->y); break;
		case 0xA1: load_mem(cpu, &cpu->acc, indirect_x(cpu)); break;
		case 0xA2: load_imm(cpu, &cpu->x); break;
		case 0xA4: load_mem(cpu, &cpu->y, zeropage(cpu)); break;
		case 0xA5: load_mem(cpu, &cpu->acc, zeropage(cpu)); break;
		case 0xA6: load_mem(cpu, &cpu->x, zeropage(cpu)); break;
		case 0xA8: transfer(cpu, cpu->acc, &cpu->y); break;
		case 0xA9: load_imm(cpu, &cpu->acc); break;
		case 0xAA: transfer(cpu, cpu->acc, &cpu->x); break;
		case 0xAC: load_mem(cpu, &cpu->y, absolute(cpu)); break;
		case 0xAD: load_mem(cpu, &cpu->acc, absolute(cpu)); break;
		case 0xAE: load_mem(cpu, &cpu->x, absolute(cpu)); break;
		case 0xB0: branch(cpu, cpu_get_flag(cpu, FLAG_C)); break;
		case 0xB1: load_mem(cpu, &cpu->acc, indirect_y(cpu)); break;
		case 0xB4: load_mem(cpu, &cpu->y, zeropage_x(cpu)); break;
		case 0xB5: load_mem(cpu, &cpu->acc, zeropage_x(cpu)); break;
		case 0xB6: load_mem(cpu, &cpu->x, zeropage_y(cpu)); break;
		case 0xB8: clv(cpu); break;
		case 0xB9: load_mem(cpu, &cpu->acc, absolute_y(cpu)); break;
		case 0xBC: load_mem(cpu, &cpu->y, absolute_x(cpu)); break;
		case 0xBD: load_mem(cpu, &cpu->acc, absolute_x(cpu)); break;
		case 0xBE: load_mem(cpu, &cpu->x, absolute_y(cpu)); break;
		case 0xC0: compare_imm(cpu, cpu->y); break;
		case 0xC1: compare_mem(cpu, cpu->acc, indirect_x(cpu)); break;
		case 0xC4: compare_mem(cpu, cpu->y, zeropage(cpu)); break;
		case 0xC5: compare_mem(cpu, cpu->acc, zeropage(cpu)); break;
		case 0xC6: dec(cpu, zeropage(cpu)); break;
		case 0xC8: iny(cpu); break;
		case 0xC9: compare_imm(cpu, cpu->acc); break;
		case 0xCA: dex(cpu); break;
		case 0xCC: compare_mem(cpu, cpu->y, absolute(cpu)); break;
		case 0xCD: compare_mem(cpu, cpu->acc, absolute(cpu)); break;
		case 0xCE: dec(cpu, absolute(cpu)); break;
		case 0xD0: branch(cpu, cpu_get_flag(cpu, FLAG_Z) == 0); break;
		case 0xD1: compare_mem(cpu, cpu->acc, indirect_y(cpu)); break;
		case 0xD5: compare_mem(cpu, cpu->acc, zeropage_x(cpu)); break;
		case 0xD6: dec(cpu, zeropage_x(cpu)); break;
		case 0xD8: cld(cpu); break;
		case 0xD9: compare_mem(cpu, cpu->acc, absolute_y(cpu)); break;
		case 0xDD: compare_mem(cpu, cpu->acc, absolute_x(cpu)); break;
		case 0xDE: dec(cpu, absolute_x(cpu)); break;
		case 0xE0: compare_imm(cpu, cpu->x); break;
		case 0xE4: compare_mem(cpu, cpu->x, zeropage(cpu)); break;
		case 0xE6: inc(cpu, zeropage(cpu)); break;
		case 0xE8: inx(cpu); break;
		case 0xEA: /*nop*/ break;
		case 0xEC: compare_mem(cpu, cpu->x, absolute(cpu)); break;
		case 0xEE: inc(cpu, absolute(cpu)); break;
		case 0xF0: branch(cpu, cpu_get_flag(cpu, FLAG_Z)); break;
		case 0xF6: inc(cpu, zeropage_x(cpu)); break;
		case 0xFE: inc(cpu, absolute_x(cpu)); break;

		default:
			printf("Unimplemented instruction: 0x%02X\n", opcode);
			break;
	}
}

void cpu_handle_interrupts(struct Cpu6510* cpu)
{
	u8 irq_enabled = cpu_get_flag(cpu, FLAG_I) == 0;
	if (irq_enabled) {
		push_u16(cpu, cpu->pc);

		cpu->sr |= (1 << 5);
		cpu_clear_flag(cpu, FLAG_B);
		cpu_set_flag(cpu, FLAG_I);

		push_u8(cpu, cpu->sr);
		
		u16 address = cpu_read_u16(cpu, IRQ_VECTOR);
		cpu->pc = address;
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

void rti(struct Cpu6510* cpu)
{
	//pull sr
	pop_u8(cpu, &cpu->sr);
	cpu->sr &= 0xCF; //break flag and bit 5 ignored (similar to plp)

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

void pha(struct Cpu6510* cpu)
{
	push_u8(cpu, cpu->acc);
}

void jmp_abs(struct Cpu6510* cpu)
{
	u16 abs_address = absolute(cpu);
	cpu->pc = abs_address;
}

void jmp_ind(struct Cpu6510* cpu)
{
	u16 address = cpu_fetch_u16(cpu);
	u16 effective_address = cpu_read_u16(cpu, address);

	cpu->pc = effective_address;
}

void ora(struct Cpu6510* cpu, u8 value)
{
	u8 result = cpu->acc | value;
	cpu->acc |= value;

	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
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
	u8 value = cpu_read_u8(cpu, indirect_x(cpu));
	ora(cpu, value);
}

void ora_indir_y(struct Cpu6510* cpu)
{
	u8 value = cpu_read_u8(cpu, indirect_y(cpu));
	ora(cpu, value);
}

void asl(struct Cpu6510* cpu, u8 *value)
{
	u8 val = *value;

	u8 msb = val >> 7;
	u8 result = val << 1;
	val <<= 1;

	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
	cpu_affect_flag(cpu, msb, FLAG_C);

	*value = val;
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

void and(struct Cpu6510* cpu, u8 value)
{
	u8 result = cpu->acc & value;
	cpu->acc &= value;

	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
}

void and_imm(struct Cpu6510* cpu)
{
	and(cpu, immediate(cpu));
}

void and_zpg(struct Cpu6510* cpu, u8 zpg_address)
{
	u8 value = cpu_read_u8(cpu, zpg_address);
	and(cpu, value);
}

void and_abs(struct Cpu6510* cpu, u16 abs_address)
{
	u8 value = cpu_read_u8(cpu, abs_address);
	and (cpu, value);
}

void and_indir_x(struct Cpu6510* cpu)
{
	u8 value = cpu_read_u8(cpu, indirect_x(cpu));
	and(cpu, value);
}

void and_indir_y(struct Cpu6510* cpu)
{
	u8 value = cpu_read_u8(cpu, indirect_y(cpu));
	and(cpu, value);
}

void bit(struct Cpu6510* cpu, u8 value)
{
	u8 result = cpu->acc & value;

	//bits 6 and 7 of operand(value) are put into SR (V,N)
	cpu_affect_flag(cpu, (value >> 7), FLAG_N);
	cpu_affect_flag(cpu, (value >> 6), FLAG_V);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
}

void bit_zpg(struct Cpu6510* cpu, u8 zpg_address)
{
	u8 value = cpu_read_u8(cpu, zpg_address);
	bit(cpu, value);
}

void bit_abs(struct Cpu6510* cpu, u16 abs_address)
{
	u8 value = cpu_read_u8(cpu, abs_address);
	bit(cpu, value);
}

void rol(struct Cpu6510* cpu, u8* value)
{
	u8 val = *value;

	u8 msb = val >> 7;
	u8 prev_carry = cpu_get_flag(cpu, FLAG_C);

	val <<= 1;
	val |= prev_carry;
	*value = val;

	cpu_affect_flag(cpu, cpu_is_signed(val), FLAG_N);
	cpu_affect_flag(cpu, val == 0, FLAG_Z);
	cpu_affect_flag(cpu, msb, FLAG_C);
}

void rol_abs(struct Cpu6510* cpu, u16 abs_address)
{
	u8 value = cpu_read_u8(cpu, abs_address);
	rol(cpu, &value);

	cpu_write_mem_u8(cpu, value, abs_address);
}

void rol_zpg(struct Cpu6510* cpu, u8 zpg_address)
{
	u8 value = cpu_read_u8(cpu, zpg_address);
	rol(cpu, &value);

	cpu_write_mem_u8(cpu, value, zpg_address);
}

void rola(struct Cpu6510* cpu)
{
	rol(cpu, &cpu->acc);
}

void eor(struct Cpu6510* cpu, u8 value)
{
	u8 result = cpu->acc ^ value;
	cpu->acc ^= value;

	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
}

void eor_imm(struct Cpu6510* cpu)
{
	eor(cpu, immediate(cpu));
}

void eor_zpg(struct Cpu6510* cpu, u8 zpg_address)
{
	u8 value = cpu_read_u8(cpu, zpg_address);
	eor(cpu, value);
}

void eor_abs(struct Cpu6510* cpu, u16 abs_address)
{
	u8 value = cpu_read_u8(cpu, abs_address);
	eor(cpu, value);
}

void eor_indir_x(struct Cpu6510* cpu)
{
	u8 value = cpu_read_u8(cpu, indirect_x(cpu));
	eor(cpu, value);
}

void eor_indir_y(struct Cpu6510* cpu)
{
	u8 value = cpu_read_u8(cpu, indirect_y(cpu));
	eor(cpu, value);
}

void lsr(struct Cpu6510* cpu, u8* value)
{
	u8 val = *value;

	u8 lsb = val & 0x1;
	u8 result = val >> 1;
	val >>= 1;

	cpu_clear_flag(cpu, FLAG_N);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
	cpu_affect_flag(cpu, lsb, FLAG_C);

	*value = val;
}

void lsr_abs(struct Cpu6510* cpu, u16 abs_address)
{
	u8 value = cpu_read_u8(cpu, abs_address);
	lsr(cpu, &value);

	cpu_write_mem_u8(cpu, value, abs_address);
}

void lsr_zpg(struct Cpu6510* cpu, u8 zpg_address)
{
	u8 value = cpu_read_u8(cpu, zpg_address);
	lsr(cpu, &value);

	cpu_write_mem_u8(cpu, value, zpg_address);
}

void lsra(struct Cpu6510* cpu)
{
	lsr(cpu, &cpu->acc);
}

void adc(struct Cpu6510* cpu, u8 value)
{
	u8 carry = cpu_get_flag(cpu, FLAG_C);
	u8 result = cpu->acc + value + carry;

	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
	cpu_affect_flag(cpu, cpu_carry_occured_u8(cpu->acc, value, carry), FLAG_C);
	cpu_affect_flag(cpu, cpu_overflow_from_add_u8(cpu->acc, value, carry), FLAG_V);

	cpu->acc = result;
}

void adc_imm(struct Cpu6510* cpu)
{
	adc(cpu, immediate(cpu));
}

void adc_abs(struct Cpu6510* cpu, u16 abs_address)
{
	u8 value = cpu_read_u8(cpu, abs_address);
	adc(cpu, value);
}

void adc_zpg(struct Cpu6510* cpu, u8 zpg_address)
{
	u8 value = cpu_read_u8(cpu, zpg_address);
	adc(cpu, value);
}

void adc_indir_x(struct Cpu6510* cpu)
{
	u8 value = cpu_read_u8(cpu, indirect_x(cpu)); 
	adc(cpu, value);
}

void adc_indir_y(struct Cpu6510* cpu)
{
	u8 value = cpu_read_u8(cpu, indirect_y(cpu));
	adc(cpu, value);
}

void ror(struct Cpu6510* cpu, u8* value)
{
	u8 val = *value;

	u8 lsb = val & 0x1;
	u8 carry = cpu_get_flag(cpu, FLAG_C);

	val >>= 1;
	
	if (carry)
		val |= (1 << 7);
	else
		val &= ~(1 << 7);

	cpu_affect_flag(cpu, cpu_is_signed(val), FLAG_N);
	cpu_affect_flag(cpu, val == 0, FLAG_Z);
	cpu_affect_flag(cpu, lsb, FLAG_C);

	*value = val;
}

void ror_abs(struct Cpu6510* cpu, u16 abs_address)
{
	u8 value = cpu_read_u8(cpu, abs_address);
	ror(cpu, &value);

	cpu_write_mem_u8(cpu, value, abs_address);
}

void ror_zpg(struct Cpu6510* cpu, u8 zpg_address)
{
	u8 value = cpu_read_u8(cpu, zpg_address);
	ror(cpu, &value);

	cpu_write_mem_u8(cpu, value, zpg_address);
}

void rora(struct Cpu6510* cpu)
{
	ror(cpu, &cpu->acc);
}

void store(struct Cpu6510* cpu, u8 reg, u16 address)
{
	cpu_write_mem_u8(cpu, reg, address);
}

void transfer(struct Cpu6510* cpu, u8 source_register, u8* dest_register)
{
	*dest_register = source_register;

	cpu_affect_flag(cpu, cpu_is_signed(source_register), FLAG_N);
	cpu_affect_flag(cpu, source_register == 0, FLAG_Z);
}

void load_mem(struct Cpu6510* cpu, u8* dest_register, u16 address)
{
	u8 value = cpu_read_u8(cpu, address);
	*dest_register = value;

	cpu_affect_flag(cpu, cpu_is_signed(value), FLAG_N);
	cpu_affect_flag(cpu, value == 0, FLAG_Z);
}

void load_imm(struct Cpu6510* cpu, u8* dest_register)
{
	u8 value = immediate(cpu);
	*dest_register = value;

	cpu_affect_flag(cpu, cpu_is_signed(value), FLAG_N);
	cpu_affect_flag(cpu, value == 0, FLAG_Z);
}

void compare_mem(struct Cpu6510* cpu, u8 reg, u16 address)
{
	u8 value = cpu_read_u8(cpu, address);
	u8 result = reg - value;

	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
	cpu_affect_flag(cpu, cpu_borrow_occured_u8(reg, value, 0), FLAG_C);
}

void compare_imm(struct Cpu6510* cpu, u8 reg)
{
	u8 value = immediate(cpu);
	u8 result = reg - value;

	cpu_affect_flag(cpu, cpu_is_signed(result), FLAG_N);
	cpu_affect_flag(cpu, result == 0, FLAG_Z);
	cpu_affect_flag(cpu, cpu_borrow_occured_u8(reg, value, 0), FLAG_C);
}

void dec(struct Cpu6510* cpu, u16 address)
{
	u8 value = cpu_read_u8(cpu, address);
	value--;

	cpu_affect_flag(cpu, cpu_is_signed(value), FLAG_N);
	cpu_affect_flag(cpu, value == 0, FLAG_Z);

	cpu_write_mem_u8(cpu, value, address);
}

void inc(struct Cpu6510* cpu, u16 address)
{
	u8 value = cpu_read_u8(cpu, cpu, address);
	value++;

	cpu_affect_flag(cpu, cpu_is_signed(value), FLAG_N);
	cpu_affect_flag(cpu, value == 0, FLAG_Z);

	cpu_write_mem_u8(cpu, value, address);
}

void branch(struct Cpu6510* cpu, u8 condition)
{
	if (condition) {
		s8 offset = (s8)cpu_fetch_u8(cpu);

		u16 prev_addr = cpu->pc;
		cpu->pc += offset;
		cpu->cycles += 1;

		//if we crossed a page boundary add a cycle
		if ((cpu->pc & 0xFF00) != (prev_addr & 0xFF00))
			cpu->cycles += 1;
	}
	else {
		cpu->pc++;
	}
}

void clc(struct Cpu6510* cpu)
{
	cpu_clear_flag(cpu, FLAG_C);
}

void sec(struct Cpu6510* cpu)
{
	cpu_set_flag(cpu, FLAG_C);
}

void cli(struct Cpu6510* cpu)
{
	cpu_clear_flag(cpu, FLAG_I);
}

void sei(struct Cpu6510* cpu)
{
	cpu_set_flag(cpu, FLAG_I);
}

void dey(struct Cpu6510* cpu)
{
	cpu->y--;
	cpu_affect_flag(cpu, cpu_is_signed(cpu->y), FLAG_N);
	cpu_affect_flag(cpu, cpu->y == 0, FLAG_Z);
}

void dex(struct Cpu6510* cpu)
{
	cpu->x--;
	cpu_affect_flag(cpu, cpu_is_signed(cpu->x), FLAG_N);
	cpu_affect_flag(cpu, cpu->x == 0, FLAG_Z);
}

void iny(struct Cpu6510* cpu)
{
	cpu->y++;
	cpu_affect_flag(cpu, cpu_is_signed(cpu->y), FLAG_N);
	cpu_affect_flag(cpu, cpu->y == 0, FLAG_Z);
}

void inx(struct Cpu6510* cpu)
{
	cpu->x++;
	cpu_affect_flag(cpu, cpu_is_signed(cpu->x), FLAG_N);
	cpu_affect_flag(cpu, cpu->x == 0, FLAG_Z);
}

void clv(struct Cpu6510* cpu)
{
	cpu_clear_flag(cpu, FLAG_V);
}

void cld(struct Cpu6510* cpu)
{
	cpu_clear_flag(cpu, FLAG_D);
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
	u8 lo = cpu_fetch_u8(cpu);
	u8 hi = cpu_fetch_u8(cpu);

	u16 address = ((hi << 8) | lo) + cpu->x;

	//if we crossed a page boundary add a cycle
	if ((address & 0xFF00) != (hi << 8))
		cpu->cycles += 1;

	return address;
}

u16 absolute_y(struct Cpu6510* cpu)
{
	u8 lo = cpu_fetch_u8(cpu);
	u8 hi = cpu_fetch_u8(cpu);

	u16 address = ((hi << 8) | lo) + cpu->y;

	//if we crossed a page boundary add a cycle
	if ((address & 0xFF00) != (hi << 8))
		cpu->cycles += 1;

	return address;
}

u16 indirect_x(struct Cpu6510* cpu)
{
	u8 zeropage_addr = cpu_fetch_u8(cpu);
	u8 vector = cpu->x + zeropage_addr;

	u16 effective_address = cpu_read_u16(cpu, vector);

	return effective_address;
}

u16 indirect_y(struct Cpu6510* cpu)
{
	u8 zeropage_vector = cpu_fetch_u8(cpu);
	u8 lo = cpu_read_u8(cpu, zeropage_vector);
	u8 hi = cpu_read_u8(cpu, zeropage_vector + 1);
	
	u16 effective_address = ((hi << 8) | lo) + cpu->y;

	//if we crossed a page boundary add a cycle
	if ((effective_address & 0xFF00) != (hi << 8))
		cpu->cycles += 1;

	return effective_address;
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
