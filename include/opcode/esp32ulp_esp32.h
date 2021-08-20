#ifndef OPCODE__esp32ulp_esp32_H_
#define OPCODE__esp32ulp_esp32_H_


// ====================   I_ALUR  ============================

typedef struct {
	unsigned int dreg : 2;          /*!< Destination register */
	unsigned int sreg : 2;          /*!< Register with operand A */
	unsigned int treg : 2;          /*!< Register with operand B */
	unsigned int unused : 15;       /*!< Unused */
	unsigned int sel : 4;           /*!< Operation to perform, one of ALU_SEL_xxx */
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_ALU_REG) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_ALU) */
} alu_reg;                      /*!< Format of ALU instruction (both sources are registers) */


#define I_ALUR(reg_dest, reg_src1, reg_src2, op_sel) { *(unsigned int*)&(alu_reg ){ \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .treg = reg_src2, \
    .unused = 0, \
    .sel = op_sel, \
    .sub_opcode = SUB_OPCODE_ALU_REG, \
    .opcode = OPCODE_ALU } }


// ====================   I_ALUI  ============================

typedef struct {
	unsigned int dreg : 2;          /*!< Destination register */
	unsigned int sreg : 2;          /*!< Register with operand A */
	unsigned int imm  : 16;          /*!< imm value */
	unsigned int unused : 1;       /*!< Unused */
	unsigned int sel : 4;           /*!< Operation to perform, one of ALU_SEL_xxx */
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_ALU_REG) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_ALU) */
} alu_reg_i;                      /*!< Format of ALU instruction (both sources are registers) */



#define I_ALUI(reg_dest, reg_src1, imme, op_sel) { *(unsigned int*)&(alu_reg_i ){ \
    .dreg = reg_dest, \
    .sreg = reg_src1, \
    .imm = imme, \
    .unused = 0, \
    .sel = op_sel, \
    .sub_opcode = SUB_OPCODE_ALU_IMM, \
    .opcode = OPCODE_ALU } }

// ====================   I_ALUS  ============================

typedef struct {
	unsigned int unused1 : 4;          /*!< Destination register */
	unsigned int imm : 8;          /*!< imm value */
	unsigned int unused2 : 9;       /*!< Unused */
	unsigned int sel : 4;           /*!< Operation to perform, one of ALU_SEL_xxx */
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_ALU_REG) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_ALU) */
} alu_reg_s;                      /*!< Format of ALU instruction (both sources are registers) */



#define I_ALUS(op_sel, imme) { *(unsigned int*)&(alu_reg_s ){ \
    .unused1 = 0, \
    .imm = imme, \
    .unused2 = 0, \
    .sel = op_sel, \
    .sub_opcode = SUB_OPCODE_ALU_CNT, \
    .opcode = OPCODE_ALU } }

//   ------------------   Jump   ------------------------

typedef struct {
	unsigned int dreg : 2;          /*!< Register which contains target PC, expressed in words (used if .reg == 1) */
	unsigned int addr : 11;         /*!< Target PC, expressed in words (used if .reg == 0) */
	unsigned int unused : 8;        /*!< Unused */
	unsigned int reg : 1;           /*!< Target PC in register (1) or immediate (0) */
	unsigned int type : 3;          /*!< Jump condition (BX_JUMP_TYPE_xxx) */
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_BX) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
} jump_alu_ri;                      /*!< Format of ALU instruction (both sources are registers) */

#define OPCODE_BRANCH 8         /*!< Branch instructions */
#define SUB_OPCODE_BX  0        /*!< Branch to absolute PC (immediate or in register) */
#define SUB_OPCODE_BR  1        /*!< Branch to relative PC */
#define SUB_OPCODE_BS  2        /*!< Branch to relative PC */

#define I_JUMP_RI(reg_pc, imm_pc, cond, mode) { *(unsigned int*)&(jump_alu_ri ){ \
    .dreg = reg_pc, \
    .addr = imm_pc, \
    .unused = 0, \
	.reg = mode,\
	.type = cond,\
    .sub_opcode = SUB_OPCODE_BX, \
    .opcode = OPCODE_BRANCH } }


//   ------------------   Jump  relr ------------------------

typedef struct {
	unsigned int threshold : 16;
	unsigned int judge : 1;         
	unsigned int step: 8;        
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_BX) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
} jump_alu_relr;                      /*!< Format of ALU instruction  */


#define I_JUMP_RELR(thresh, jud, stp) { *(unsigned int*)&(jump_alu_relr ){ \
    .threshold = thresh, \
    .judge = jud, \
    .step = ((stp < 0) ? ((-stp) | (1 << 7)) : stp) , \
    .sub_opcode = SUB_OPCODE_BR, \
    .opcode = OPCODE_BRANCH } }

//   ------------------   Jump  rels ------------------------

typedef struct {
	unsigned int threshold : 8;
	unsigned int unused : 7;
	unsigned int judge : 2;
	unsigned int step : 8;
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_BX) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
} jump_alu_rels;                      /*!< Format of ALU instruction  */


#define I_JUMP_RELS(thresh, jud, stp) { *(unsigned int*)&(jump_alu_rels ){ \
    .threshold = thresh, \
    .judge = jud, \
    .step = ((stp < 0) ? ((-stp) | (1 << 7)) : stp) , \
    .sub_opcode = SUB_OPCODE_BS, \
    .opcode = OPCODE_BRANCH } }

//   ------------------   wr_mem  ------------------------
#define OPCODE_ST 6             /*!< Instruction: store indirect to RTC memory */
#define SUB_OPCODE_ST 4         /*!< Store 32 bits, 16 MSBs contain PC, 16 LSBs contain value from source register */

typedef struct {
	unsigned int dreg : 2;
	unsigned int sreg : 2;
	unsigned int unused1 : 6;
	unsigned int offset : 11;
	unsigned int unused2 : 4;
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_BX) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
} wr_mem;                      /*!< Format of ALU instruction  */


#define WR_MEM(dest, src, offs) { *(unsigned int*)&(wr_mem ){ \
    .dreg = dest, \
    .sreg = src, \
    .unused1 = 0, \
    .offset = offs, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_ST, \
    .opcode = OPCODE_ST } }

//   ------------------   rd_mem  ------------------------
#define OPCODE_LD 13             /*!< Instruction: store indirect to RTC memory */
#define SUB_OPCODE_LD 0         /*!< Store 32 bits, 16 MSBs contain PC, 16 LSBs contain value from source register */

typedef struct {
	unsigned int dreg : 2;
	unsigned int sreg : 2;
	unsigned int unused1 : 6;
	unsigned int offset : 11;
	unsigned int unused2 : 4;
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_BX) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
} rd_mem;                      /*!< Format of ALU instruction  */


#define RD_MEM(dest, src, offs) { *(unsigned int*)&(rd_mem ){ \
    .dreg = dest, \
    .sreg = src, \
    .unused1 = 0, \
    .offset = offs, \
    .unused2 = 0, \
    .sub_opcode = SUB_OPCODE_LD, \
    .opcode = OPCODE_LD } }


//   ------------------   SLEEP  ------------------------
#define SUB_OPCODE_SLEEP  1 /*!< Stop executing the program and run it again after selected interval */

typedef struct {
	unsigned int cycle_sel : 4;     /*!< Select which one of SARADC_ULP_CP_SLEEP_CYCx_REG to get the sleep duration from */
	unsigned int unused : 21;       /*!< Unused */
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_SLEEP) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_END) */
} cmd_sleep;                        /*!< Format of END instruction with wakeup */


#define OP_CMD_SLEEP(sleep) { *(unsigned int*)&(cmd_sleep ){ \
    .cycle_sel = sleep, \
	.unused = 0,\
	.sub_opcode = SUB_OPCODE_SLEEP,\
    .opcode = OPCODE_EXIT } }


//   ------------------   RD/WR reg  ------------------------
#define OPCODE_WR_REG 1         /*!< Instruction: write peripheral register (RTC_CNTL/RTC_IO/SARADC) (not implemented yet) */

#define OPCODE_RD_REG 2         /*!< Instruction: read peripheral register (RTC_CNTL/RTC_IO/SARADC) (not implemented yet) */

typedef struct{
	unsigned int addr : 10;          /*!< Address within either RTC_CNTL, RTC_IO, or SARADC */
	unsigned int data : 8;          /*!< 8 bits of data to write */
	unsigned int low : 5;           /*!< Low bit */
	unsigned int high : 5;          /*!< High bit */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_WR_REG) */
} cmd_wr_reg;                       /*!< Format of WR_REG instruction */

typedef struct {
	unsigned int addr : 10;          /*!< Address within either RTC_CNTL, RTC_IO, or SARADC */
	unsigned int unused : 8;        /*!< Unused */
	unsigned int low : 5;           /*!< Low bit */
	unsigned int high : 5;          /*!< High bit */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_WR_REG) */
} cmd_rd_reg;                       /*!< Format of RD_REG instruction */

/**
* Write literal value to a peripheral register
*
* reg[high_bit : low_bit] = val
* This instruction can access RTC_CNTL_, RTC_IO_, and SENS_ peripheral registers.
*/
#define I_WR_REG(reg_addr, low_bit, high_bit, val) { *(unsigned int*)&(cmd_wr_reg ){ \
    .addr = reg_addr, \
    .data = val, \
    .low = low_bit, \
    .high = high_bit, \
    .opcode = OPCODE_WR_REG } }

/**
* Read from peripheral register into R0
*
* R0 = reg[high_bit : low_bit]
* This instruction can access RTC_CNTL_, RTC_IO_, and SENS_ peripheral registers.
*/
#define I_RD_REG(reg_addr, low_bit, high_bit) { *(unsigned int*)&(cmd_rd_reg ){ \
    .addr = reg_addr, \
    .unused = 0, \
    .low = low_bit, \
    .high = high_bit, \
    .opcode = OPCODE_RD_REG } }

//   ------------------   RD/WR reg  ------------------------
#define OPCODE_I2C 3            /*!< Instruction: read/write I2C (not implemented yet) */
#define OPCODE_I2C_RD 0
#define OPCODE_I2C_WR 1

typedef struct {
	unsigned int i2c_addr : 8;      /*!< I2C slave address */
	unsigned int data : 8;          /*!< Data to read or write */
	unsigned int low_bits : 3;      /*!< TBD */
	unsigned int high_bits : 3;     /*!< TBD */
	unsigned int i2c_sel : 4;       /*!< TBD, select reg_i2c_slave_address[7:0] */
	unsigned int unused : 1;        /*!< Unused */
	unsigned int rw_bit : 1;            /*!< Write (1) or read (0) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_I2C) */
} cmd_i2c;                          /*!< Format of I2C instruction */

#define I_I2C_RW(addr, val, low, high, sel, rw) { *(unsigned int*)&(cmd_i2c ){ \
    .i2c_addr = addr, \
    .data = val, \
    .low_bits = low, \
    .high_bits = high, \
    .i2c_sel = sel, \
    .unused = 0, \
    .rw_bit = rw, \
	.opcode = OPCODE_I2C } }

// --------------------------------------------------------------------------

#endif // OPCODE__esp32ulp_esp32_H_