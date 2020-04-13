#ifndef _tc_esp32ulp_esp32s2_H_
#define _tc_esp32ulp_esp32s2_H_

#include "as.h"
#include "struc-symbol.h"
#include "esp32ulp-defs.h"
#include "obstack.h"
#include "safe-ctype.h"
#ifdef OBJ_ELF
#include "dwarf2dbg.h"
#endif
#include "elf/common.h"
#include "elf/esp32ulp.h"

INSTR_T esp32ulp_gen_wait_esp32s2(void);
INSTR_T esp32ulp_gen_alur_esp32s2(int dst, int src1, int src2, int operation);
INSTR_T esp32ulp_gen_alui_esp32s2(int dst, int src1, Expr_Node* addr, int operation);
INSTR_T esp32ulp_cmd_stage_esp32s2(int dir, Expr_Node* imm);
INSTR_T esp32ulp_cmd_stage_rst_esp32s2(void);
INSTR_T esp32ulp_gen_jump_r_esp32s2(int dst_reg, int cond);
INSTR_T esp32ulp_gen_jump_i_esp32s2(Expr_Node* addr, int cond);
INSTR_T esp32ulp_cmd_reg_rd_esp32s2(Expr_Node* addr, Expr_Node* high, Expr_Node* low);
INSTR_T esp32ulp_cmd_reg_wr_esp32s2(Expr_Node* addr, Expr_Node* high, Expr_Node* low, Expr_Node* data);
INSTR_T esp32ulp_cmd_i2c_rd_esp32s2(Expr_Node* i2c_addr, Expr_Node* high, Expr_Node* low, Expr_Node* i2c_sel);
INSTR_T esp32ulp_cmd_i2c_wr_esp32s2(Expr_Node* i2c_addr, Expr_Node* high, Expr_Node* low, Expr_Node* i2c_sel, Expr_Node* data);

INSTR_T esp32ulp_cmd_jump_relr_esp32s2(Expr_Node* step, Expr_Node* thresh, int cond);
INSTR_T esp32ulp_cmd_jump_rels_esp32s2(Expr_Node* step, Expr_Node* thresh, int cond);

INSTR_T esp32s2ulp_cmd_sleep_esp32s2(Expr_Node*  cycles);
INSTR_T esp32ulp_rd_mem_addr_esp32s2(int lh, int dst_reg, int src_reg, Expr_Node* addr);

INSTR_T esp32ulp_wr_mem_stlh_esp32s2(int low_hi, int dst_reg, int src_reg, Expr_Node *offset);
INSTR_T esp32ulp_wr_mem_st32_esp32s2(int dst_reg, int src_reg, Expr_Node *offset, Expr_Node *label);
INSTR_T esp32ulp_wr_mem_sti32_esp32s2(int dst_reg, int src_reg, Expr_Node* label);
INSTR_T esp32ulp_wr_mem_st_l_esp32s2(int low_hi, int dst_reg, int src_reg, Expr_Node* offset, Expr_Node* label);
INSTR_T esp32ulp_wr_mem_sti_l_esp32s2(int dst_reg, int src_reg, Expr_Node* label);
INSTR_T esp32ulp_wr_mem_sti_esp32s2(int dst_reg, int src_reg);
INSTR_T esp32ulp_wr_mem_sto_esp32s2(Expr_Node* offset);

void md_apply_fix_esp32s2(fixS *fixP, valueT *valueP, segT seg ATTRIBUTE_UNUSED);
INSTR_T Expr_Node_Gen_Reloc_esp32s2(Expr_Node * head, int parent_reloc);

#endif // _tc_esp32ulp_esp32s2_H_