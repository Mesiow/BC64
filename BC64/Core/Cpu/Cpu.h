#include "../Util.h"

#define FLAG_C (1 << 0)
#define FLAG_Z (1 << 1)
#define FLAG_I (1 << 2) //irq disable
#define FLAG_D (1 << 3) //bcd flag
#define FLAG_V (1 << 6)
#define FLAG_N (1 << 7)

#define RESET_VECTOR 0xFFFC
#define NMI_VECTOR 0xFFFA
#define IRQ_VECTOR 0xFFFE

struct Cpu6510TestMemory {
	u8 *memory;
};

void load_cpu_test_rom(struct Cpu6510* cpu, const char* path);
void cpu_free_test(struct Cpu6510* cpu);
void cpu_test_write_u8(struct Cpu6510* cpu, u8 value, u16 address);
u8 cpu_test_read_u8(struct Cpu6510* cpu, u16 address);

struct Cpu6510 {
	u8 acc;
	u8 x;
	u8 y;
	u8 sr;
	u8 sp;
	u16 pc;
	u8 brk;

	u8 halt;
	u8 cycles;
	struct Memory* mem;

	u8 all_suite_test_enabled;
	struct Cpu6510TestMemory test;
};

void cpu_init(struct Cpu6510* cpu, struct Memory *mem);
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
void cpu_handle_irq(struct Cpu6510* cpu);
void cpu_handle_nmi(struct Cpu6510* cpu);
void cpu_handle_reset(struct Cpu6510* cpu);
void cpu_interrupt(struct Cpu6510* cpu, u16 vector);

//instructions
void brk(struct Cpu6510* cpu);
void jsr(struct Cpu6510* cpu);
void rts(struct Cpu6510* cpu);
void rti(struct Cpu6510* cpu);
void php(struct Cpu6510* cpu);
void plp(struct Cpu6510* cpu);
void pha(struct Cpu6510* cpu);
void pla(struct Cpu6510* cpu);
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

void sbc(struct Cpu6510* cpu, u8 value);
void sbc_imm(struct Cpu6510* cpu);
void sbc_abs(struct Cpu6510* cpu, u16 abs_address);
void sbc_zpg(struct Cpu6510* cpu, u8 zpg_address);
void sbc_indir_x(struct Cpu6510* cpu);
void sbc_indir_y(struct Cpu6510* cpu);

void ror(struct Cpu6510* cpu, u8* value);
void ror_abs(struct Cpu6510* cpu, u16 abs_address);
void ror_zpg(struct Cpu6510* cpu, u8 zpg_address);
void rora(struct Cpu6510* cpu);

void store(struct Cpu6510* cpu, u8 reg, u16 address);
void transfer(struct Cpu6510* cpu, u8 source_register, u8* dest_register);
void load_mem(struct Cpu6510* cpu, u8* dest_register, u16 address);
void load_imm(struct Cpu6510* cpu, u8* dest_register);

void compare_mem(struct Cpu6510* cpu, u8 reg, u16 address);
void compare_imm(struct Cpu6510* cpu, u8 reg);

void dec(struct Cpu6510* cpu, u16 address);
void inc(struct Cpu6510* cpu, u16 address);

void branch(struct Cpu6510* cpu, u8 condition);
void clc(struct Cpu6510* cpu);
void sec(struct Cpu6510* cpu);
void cli(struct Cpu6510* cpu);
void sei(struct Cpu6510* cpu);
void dey(struct Cpu6510* cpu);
void dex(struct Cpu6510* cpu);
void iny(struct Cpu6510* cpu);
void inx(struct Cpu6510* cpu);
void clv(struct Cpu6510* cpu);
void cld(struct Cpu6510* cpu);
void sed(struct Cpu6510* cpu);

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