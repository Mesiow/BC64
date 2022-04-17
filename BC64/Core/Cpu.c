#include "Cpu.h"

void cpu_init(struct Cpu6510* cpu)
{
	cpu->acc = 0x0;
	cpu->x = 0x0;
	cpu->y = 0x0;
	cpu->sr = 0x0;
	cpu->sp = 0x0;
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
