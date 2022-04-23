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

struct Cpu6510 {
	u8 acc;
	u8 x;
	u8 y;
	u8 sr;
	u16 sp;
	u16 pc;

	struct Memory* mem;
};

void cpu_init(struct Cpu6510* cpu);
void cpu_set_flag(struct Cpu6510 *cpu, u8 flags);
void cpu_clear_flag(struct Cpu6510* cpu, u8 flags);
void cpu_affect_flag(struct Cpu6510* cpu, u8 condition, u8 flags);
void cpu_write_mem_u8(struct Cpu6510* cpu, u8 value, u16 address);

u8 cpu_is_signed(u8 value);

u16 cpu_fetch_u16(struct Cpu6510* cpu);
u16 cpu_read_u16(struct Cpu6510* cpu, u16 address);
u8 cpu_fetch_u8(struct Cpu6510* cpu);
u8 cpu_read_u8(struct Cpu6510* cpu, u16 address);

u8 cpu_clock(struct Cpu6510* cpu);
void cpu_execute_instruction(struct Cpu6510* cpu, u8 opcode);

//instructions
void brk(struct Cpu6510* cpu);
void jsr(struct Cpu6510* cpu);
void rts(struct Cpu6510* cpu);
void php(struct Cpu6510* cpu);
void plp(struct Cpu6510* cpu);

void ora(struct Cpu6510* cpu, u8 value);
void ora_imm(struct Cpu6510* cpu);
void ora_zpg(struct Cpu6510* cpu, u8 zpg_value);
void ora_abs(struct Cpu6510* cpu, u16 abs_address);
void ora_indir_x(struct Cpu6510* cpu);
void ora_indir_y(struct Cpu6510* cpu);

//addressing modes
u8 immediate(struct Cpu6510* cpu);
u8 zeropage(struct Cpu6510* cpu);
u8 zeropage_x(struct Cpu6510* cpu);
u8 zeropage_y(struct Cpu6510* cpu);
u16 absolute(struct Cpu6510* cpu);
u16 absolute_x(struct Cpu6510* cpu);
u16 absolute_y(struct Cpu6510* cpu);
u8 indirect_x(struct Cpu6510* cpu);
u8 indirect_y(struct Cpu6510* cpu);


void push_u16(struct Cpu6510* cpu, u16 value);
void push_u8(struct Cpu6510* cpu, u8 value);
void pop_u16(struct Cpu6510* cpu, u16 *reg);
void pop_u8(struct Cpu6510* cpu, u8* reg);