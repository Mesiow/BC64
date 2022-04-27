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

	u8 cycles;
	struct Memory* mem;
};

void cpu_init(struct Cpu6510* cpu);
void cpu_set_flag(struct Cpu6510 *cpu, u8 flags);
void cpu_clear_flag(struct Cpu6510* cpu, u8 flags);
void cpu_affect_flag(struct Cpu6510* cpu, u8 condition, u8 flags);
void cpu_write_mem_u8(struct Cpu6510* cpu, u8 value, u16 address);
u8 cpu_get_flag(struct Cpu6510* cpu, u8 flag);

u8 cpu_is_signed(u8 value);
u8 cpu_overflow_from_add_u8(u8 op1, u8 op2, u8 carry);
u8 cpu_overflow_from_sub_u8(u8 op1, u8 op2, u8 carry);

u8 cpu_carry_occured_u8(u8 op1, u8 op2, u8 carry);
u8 cpu_borrow_occured_u8(u8 op1, u8 op2, u8 carry);


u16 cpu_fetch_u16(struct Cpu6510* cpu);
u16 cpu_read_u16(struct Cpu6510* cpu, u16 address);
u8 cpu_fetch_u8(struct Cpu6510* cpu);
u8 cpu_read_u8(struct Cpu6510* cpu, u16 address);

u8 cpu_clock(struct Cpu6510* cpu);
void cpu_execute_instruction(struct Cpu6510* cpu, u8 opcode);
void cpu_handle_interrupts(struct Cpu6510* cpu);

//instructions
void brk(struct Cpu6510* cpu);
void jsr(struct Cpu6510* cpu);
void rts(struct Cpu6510* cpu);
void rti(struct Cpu6510* cpu);
void php(struct Cpu6510* cpu);
void plp(struct Cpu6510* cpu);
void pha(struct Cpu6510* cpu);
void jmp_abs(struct Cpu6510* cpu);
void jmp_ind(struct Cpu6510* cpu);

void ora(struct Cpu6510* cpu, u8 value);
void ora_imm(struct Cpu6510* cpu);
void ora_zpg(struct Cpu6510* cpu, u8 zpg_address);
void ora_abs(struct Cpu6510* cpu, u16 abs_address);
void ora_indir_x(struct Cpu6510* cpu);
void ora_indir_y(struct Cpu6510* cpu);

void asl(struct Cpu6510* cpu, u8 *value);
void asl_abs(struct Cpu6510* cpu, u16 abs_address);
void asl_zpg(struct Cpu6510* cpu, u8 zpg_address);
void asla(struct Cpu6510* cpu);

void and(struct Cpu6510* cpu, u8 value);
void and_imm(struct Cpu6510* cpu);
void and_zpg(struct Cpu6510* cpu, u8 zpg_address);
void and_abs(struct Cpu6510* cpu, u16 abs_address);
void and_indir_x(struct Cpu6510* cpu);
void and_indir_y(struct Cpu6510* cpu);

void bit(struct Cpu6510* cpu, u8 value);
void bit_zpg(struct Cpu6510* cpu, u8 zpg_address);
void bit_abs(struct Cpu6510* cpu, u16 abs_address);

void rol(struct Cpu6510* cpu, u8* value);
void rol_abs(struct Cpu6510* cpu, u16 abs_address);
void rol_zpg(struct Cpu6510* cpu, u8 zpg_address);
void rola(struct Cpu6510* cpu);

void eor(struct Cpu6510* cpu, u8 value);
void eor_imm(struct Cpu6510* cpu);
void eor_zpg(struct Cpu6510* cpu, u8 zpg_address);
void eor_abs(struct Cpu6510* cpu, u16 abs_address);
void eor_indir_x(struct Cpu6510* cpu);
void eor_indir_y(struct Cpu6510* cpu);

void lsr(struct Cpu6510* cpu, u8* value);
void lsr_abs(struct Cpu6510* cpu, u16 abs_address);
void lsr_zpg(struct Cpu6510* cpu, u8 zpg_address);
void lsra(struct Cpu6510* cpu);

void adc(struct Cpu6510* cpu, u8 value);
void adc_imm(struct Cpu6510* cpu);
void adc_abs(struct Cpu6510* cpu, u16 abs_address);
void adc_zpg(struct Cpu6510* cpu, u8 zpg_address);
void adc_indir_x(struct Cpu6510* cpu);
void adc_indir_y(struct Cpu6510* cpu);

void ror(struct Cpu6510* cpu, u8* value);
void ror_abs(struct Cpu6510* cpu, u16 abs_address);
void ror_zpg(struct Cpu6510* cpu, u8 zpg_address);
void rora(struct Cpu6510* cpu);

void store(struct Cpu6510* cpu, u8 reg, u16 address);
void transfer(struct Cpu6510* cpu, u8 source_register, u8* dest_register);

void branch(struct Cpu6510* cpu, u8 condition);
void clc(struct Cpu6510* cpu);
void sec(struct Cpu6510* cpu);
void cli(struct Cpu6510* cpu);
void sei(struct Cpu6510* cpu);
void dey(struct Cpu6510* cpu);

//addressing modes
u8 immediate(struct Cpu6510* cpu);
u8 zeropage(struct Cpu6510* cpu);
u8 zeropage_x(struct Cpu6510* cpu);
u8 zeropage_y(struct Cpu6510* cpu);
u16 absolute(struct Cpu6510* cpu);
u16 absolute_x(struct Cpu6510* cpu);
u16 absolute_y(struct Cpu6510* cpu);
u16 indirect_x(struct Cpu6510* cpu);
u16 indirect_y(struct Cpu6510* cpu);


void push_u16(struct Cpu6510* cpu, u16 value);
void push_u8(struct Cpu6510* cpu, u8 value);
void pop_u16(struct Cpu6510* cpu, u16 *reg);
void pop_u8(struct Cpu6510* cpu, u8* reg);