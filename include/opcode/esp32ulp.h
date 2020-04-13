/* esp32ulp.h -- Header file for ESP32ULP opcode table
   
   Copyright (c) 2016-2017 Espressif Systems (Shanghai) PTE LTD.

   This file is part of GDB, GAS, and the GNU binutils.

   GDB, GAS, and the GNU binutils are free software; you can redistribute
   them and/or modify them under the terms of the GNU General Public
   License as published by the Free Software Foundation; either version 3,
   or (at your option) any later version.

   GDB, GAS, and the GNU binutils are distributed in the hope that they
   will be useful, but WITHOUT ANY WARRANTY; without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING3.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#ifndef OPCODE_ESP32ULP_H
#define OPCODE_ESP32ULP_H
// ====================   I_ALUR  ============================

#define OPCODE_ALU 7            /*!< Arithmetic instructions */
#define SUB_OPCODE_ALU_REG 0    /*!< Arithmetic instruction, both source values are in register */
#define SUB_OPCODE_ALU_IMM 1    /*!< Arithmetic instruction, one source value is an immediate */
#define SUB_OPCODE_ALU_CNT 2    /*!< Arithmetic instruction between counter register and an immediate (not implemented yet)*/
#define ALU_SEL_ADD 0           /*!< Addition */
#define ALU_SEL_SUB 1           /*!< Subtraction */
#define ALU_SEL_AND 2           /*!< Logical AND */
#define ALU_SEL_OR  3           /*!< Logical OR */
#define ALU_SEL_MOV 4           /*!< Copy value (immediate to destination register or source register to destination register */
#define ALU_SEL_LSH 5           /*!< Shift left by given number of bits */
#define ALU_SEL_RSH 6           /*!< Shift right by given number of bits */
#define ALU_SEL_SINC  0
#define ALU_SEL_SDEC  1
#define ALU_SEL_SRST  2


//   ------------------   halt  ------------------------
#define OPCODE_HALT 11             /*!< Instruction: store indirect to RTC memory */

typedef struct {
	unsigned int unused : 28;
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_BRANCH) */
} cmd_halt;                      /*!< Format of ALU instruction  */


#define OP_CMD_HALT() { *(unsigned int*)&(cmd_halt ){ \
    .unused = 0, \
    .opcode = OPCODE_HALT } }

//   ------------------   WAKEUP  ------------------------
#define OPCODE_EXIT 9            /*!< Stop executing the program (not implemented yet) */
#define SUB_OPCODE_WAKEUP 0        /*!< Stop executing the program and optionally wake up the chip */

typedef struct {
	unsigned int wakeup : 1;        /*!< Set to 1 to wake up chip */
	unsigned int unused : 24;       /*!< Unused */
	unsigned int sub_opcode : 3;    /*!< Sub opcode (SUB_OPCODE_WAKEUP) */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_END) */
} cmd_wakeup;                          /*!< Format of END instruction with wakeup */


#define OP_CMD_WAKEUP(wake) { *(unsigned int*)&(cmd_wakeup ){ \
    .wakeup = wake, \
	.unused = 0,\
	.sub_opcode = SUB_OPCODE_WAKEUP,\
    .opcode = OPCODE_EXIT } }

//   ------------------   WAIT  ------------------------
#define OPCODE_WAIT 4            /*!< Instruction: delay (nop) for a given number of cycles */

typedef struct {
	unsigned int wait : 16;        /*!< Set to 1 to wake up chip */
	unsigned int unused : 12;       /*!< Unused */
	unsigned int opcode : 4;        /*!< Opcode (OPCODE_WAIT) */
} cmd_wait;                          /*!< Format of END instruction with wakeup */


#define OP_CMD_WAIT(cyc) { *(unsigned int*)&(cmd_wait ){ \
    .wait = cyc, \
	.unused = 0,\
    .opcode = OPCODE_WAIT } }

//   ------------------   TSENS  ------------------------
#define OPCODE_TSENS 10         /*!< Instruction: temperature sensor measurement (not implemented yet) */

typedef struct {
	unsigned int dreg : 2;          /*!< Register where to store temperature measurement result */
	unsigned int wait_delay : 14;    /*!< Cycles to wait after measurement is done */
	unsigned int cycles : 12;        /*!< Cycles used to perform measurement */
	unsigned int opcode : 4;         /*!< Opcode (OPCODE_TSENS) */
} cmd_tsens;                        /*!< Format of TSENS instruction */


#define OP_CMD_TSENS(dreg, delay) { *(unsigned int*)&(cmd_tsens ){ \
    .dreg = dreg, \
	.wait_delay = delay,\
	.cycles = 0,\
    .opcode = OPCODE_TSENS } }

//   ------------------   MEAS  ------------------------
#define OPCODE_ADC 5            /*!< Instruction: SAR ADC measurement (not implemented yet) */

typedef struct{
	unsigned int dreg : 2;          /*!< Register where to store ADC result */
	unsigned int sar_mux : 4;           /*!< Select SARADC pad (mux + 1) */
	unsigned int sar_sel : 1;       /*!< Select SARADC0 (0) or SARADC1 (1) */
	unsigned int unused1 : 1;       /*!< Unused */
	unsigned int cycles : 16;       /*!< TBD, cycles used for measurement */
	unsigned int unused2 : 4;       /*!< Unused */
	unsigned int opcode : 4;         /*!< Opcode (OPCODE_ADC) */
} cmd_adc;

#define OP_CMD_ADC(d_reg, mux, sel) { *(unsigned int*)&(cmd_adc ){ \
    .dreg = d_reg, \
    .sar_mux = mux, \
    .sar_sel = sel, \
    .unused1 = 0, \
    .cycles = 0, \
    .unused2 = 0, \
    .opcode = OPCODE_ADC } }


#endif
