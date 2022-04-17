#include "Util.h"

#define FLAG_C (1 << 0)
#define FLAG_Z (1 << 1)
#define FLAG_I (1 << 2) //irq disable
#define FLAG_D (1 << 3) //bcd flag
#define FLAG_B (1 << 4) //break
#define FLAG_V (1 << 6)
#define FLAG_N (1 << 7)

#define RESET_VECTOR 0xFFFC
#define NMI_VECTOR 0xFFFA
#define IRQ_VECTOR 0xFFFE

//The c64 uses a MOS 6510 cpu (similar to the 6502)

struct Cpu6510 {
	u8 acc;
	u8 x;
	u8 y;
	u8 sr;
	u16 sp;
	u16 pc;
};

void cpu_init(struct Cpu6510* cpu);
void cpu_set_flag(struct Cpu6510 *cpu, u8 flags);
void cpu_clear_flag(struct Cpu6510* cpu, u8 flags);
void cpu_affect_flag(struct Cpu6510* cpu, u8 condition, u8 flags);