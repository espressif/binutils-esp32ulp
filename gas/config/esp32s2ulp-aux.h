/* ESP32ULP ELF-32 support for GAS 
   Copyright (c) 2016-2017 Espressif Systems (Shanghai) PTE LTD.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "esp32s2ulp-defs.h"

#define REG_T Register *

INSTR_T esp32s2ulp_gen_progctrl (int, int);

INSTR_T esp32s2ulp_gen_alu_op(int, int, int);
INSTR_T esp32s2ulp_gen_alu_ADDR(int, int, int);
INSTR_T esp32s2ulp_gen_alu_ADDI(int, int, int);
INSTR_T esp32s2ulp_gen_jump_r(int, int);
INSTR_T esp32s2ulp_gen_jump_i(Expr_Node* , int);
INSTR_T esp32s2ulp_gen_jump_rels(Expr_Node*, int, int);
INSTR_T esp32s2ulp_move_const2reg(int, int);
INSTR_T esp32s2ulp_move_addr2reg(int, Expr_Node*);

INSTR_T esp32s2ulp_wr_mem_stlh(int low_hi, int dst_reg, int src_reg, Expr_Node* offset);
INSTR_T esp32s2ulp_wr_mem_st32(int dst_reg, int src_reg, Expr_Node* offset, Expr_Node* label);
INSTR_T esp32s2ulp_wr_mem_sti32(int dst_reg, int src_reg, Expr_Node* label);
INSTR_T esp32s2ulp_wr_mem_st_l(int low_hi, int dst_reg, int src_reg, Expr_Node* offset, Expr_Node* label);
INSTR_T esp32s2ulp_wr_mem_sti_l(int dst_reg, int src_reg, Expr_Node* label);
INSTR_T esp32s2ulp_wr_mem_sti(int dst_reg, int src_reg);
INSTR_T esp32s2ulp_wr_mem_offset(Expr_Node* offset);

INSTR_T esp32s2ulp_rd_mem_addr(int lh, int dst_reg, int src_reg, Expr_Node* addr);
INSTR_T esp32s2ulp_rd_mem_offset(int dst_reg, int src_reg, int addr);
INSTR_T esp32s2ulp_cmd_halt(void);
INSTR_T esp32s2ulp_cmd_sleep(Expr_Node*  cycles);
INSTR_T esp32s2ulp_cmd_wakeup(Expr_Node*  wake);
INSTR_T esp32s2ulp_cmd_wait(Expr_Node* cycles);
INSTR_T esp32s2ulp_cmd_tsens(int dreg, Expr_Node* delay);
INSTR_T esp32s2ulp_cmd_adc(int dreg,  Expr_Node* sar_sel, Expr_Node* mux);
INSTR_T esp32s2ulp_gen_alu_r(int dst, int src1, int src2, int operation);
INSTR_T esp32s2ulp_gen_alu_i(int dst, int src1, Expr_Node* imm, int operation);
INSTR_T esp32s2ulp_cmd_stage(int dir, Expr_Node* imm);
INSTR_T esp32s2ulp_cmd_stage_rst(void);
INSTR_T esp32s2ulp_cmd_jump_relr(Expr_Node* step, Expr_Node* thresh, int cond);
INSTR_T esp32s2ulp_cmd_jump_rels(Expr_Node* step, Expr_Node* thresh, int cond);
INSTR_T esp32s2ulp_cmd_reg_rd(Expr_Node* addr, Expr_Node* high, Expr_Node* low);
INSTR_T esp32s2ulp_cmd_reg_wr(Expr_Node* addr, Expr_Node* high, Expr_Node* low, Expr_Node* data);
// I2C operations 
INSTR_T esp32s2ulp_cmd_i2c_rd(Expr_Node* i2c_addr, Expr_Node* high, Expr_Node* low, Expr_Node* i2c_sel);
INSTR_T esp32s2ulp_cmd_i2c_wr(Expr_Node* i2c_addr, Expr_Node* high, Expr_Node* low, Expr_Node* i2c_sel, Expr_Node* data);



