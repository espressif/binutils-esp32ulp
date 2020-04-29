#include "tc-esp32ulp_esp32s2.h"
#ifdef TC_ESP32ULP
#include "tc-esp32ulp.h"
#include "opcode/esp32ulp.h"
#include "opcode/esp32ulp_esp32s2.h"

static const char *op = ".__operator";

INSTR_T esp32ulp_gen_wait_esp32s2(void)
{
	unsigned int local_op = OP_CMD_WAIT(0);
	return GEN_OPCODE32_DYA(local_op);
}

INSTR_T esp32ulp_gen_alur_esp32s2(int dst, int src1, int src2, int operation)
{
	//DEBUG_TRACE("dya_pass - esp32ulp_gen_alu_r - dst=%i, src1=%i, src2=%i, op = %i\n", dst, src1, src2, operation);
	unsigned int local_op = I_ALUR(dst, src1, src2, operation);
	return GEN_OPCODE32_DYA(local_op);
}

INSTR_T esp32ulp_gen_alui_esp32s2(int dst, int src1, Expr_Node* addr, int operation)
{
	//DEBUG_TRACE("dya_pass - esp32ulp_gen_alu_i - dst=%i, src1=%i, operation = %i, type=%i, op_value = %i\n", dst, src1, operation, (int)addr->type, (int)addr->value.op_value);
	int imm_val = EXPR_VALUE(addr);
	unsigned int local_op = I_ALUI(dst, src1, imm_val, operation);
	int rel = BFD_RELOC_ESP32S2ULP_ALUI;
	return conscode(gencode(local_op), Expr_Node_Gen_Reloc(addr, rel));
}

INSTR_T esp32ulp_cmd_stage_esp32s2(int dir, Expr_Node* imm)
{
	//DEBUG_TRACE("dya_pass - OP_CMD_STAGE_INC \n");
	int imm_val = EXPR_VALUE(imm);
	unsigned int local_op = I_ALUS(dir, imm_val);

	return conscode(gencode(local_op), Expr_Node_Gen_Reloc(imm, BFD_RELOC_ESP32S2ULP_STAGE));
}

INSTR_T esp32ulp_cmd_stage_rst_esp32s2()
{
	//DEBUG_TRACE("dya_pass - OP_CMD_STAGE_RST \n");
	unsigned int local_op = I_ALUS(ALU_SEL_SRST, 0);

	return GEN_OPCODE32_DYA(local_op);
}

INSTR_T esp32ulp_gen_jump_r_esp32s2(int dst_reg, int cond)
{
	//DEBUG_TRACE("dya_pass - esp32s2ulp_gen_jump_r - dst_reg=%i, cond=%i\n", dst_reg, cond);
	unsigned int local_op = I_JUMP_RI(dst_reg, 0, cond, 1);
	return GEN_OPCODE32_DYA(local_op);
}

INSTR_T esp32ulp_gen_jump_i_esp32s2(Expr_Node* addr, int cond)
{
	int val;
	int rel = 0;

	rel = BFD_RELOC_ESP32S2ULP_16_IMM;

	val = EXPR_VALUE(addr);
	if ((val % 4) != 0) error("%s","Esp32S2Ulp is 32-bit addressing processor. Relative jump value must be divided by 4.");
	unsigned int local_op = I_JUMP_RI(0, val>>2, cond, 0);
	return conscode(gencode(local_op), Expr_Node_Gen_Reloc(addr, rel));
}

INSTR_T esp32ulp_cmd_reg_rd_esp32s2(Expr_Node* addr, Expr_Node* high, Expr_Node* low)
{
	unsigned int addr_val = EXPR_VALUE(addr);
	unsigned int high_val = EXPR_VALUE(high);
	unsigned int low_val = EXPR_VALUE(low);

	if (addr_val > DR_REG_MAX_DIRECT)
	{
		if ((addr_val >= DR_REG_RTCCNTL_BASE) && (addr_val < DR_REG_IO_MUX_BASE))
		{
			addr_val = (addr_val - DR_REG_RTCCNTL_BASE)/4;
		} else {
			error("%s","Register address out of range. Must be 0..0x3ff, or in range of 0x3ff48000 .. 0x3ff49000.");
		}
	}
	unsigned int local_op = I_RD_REG(addr_val, low_val, high_val);

	return conscode(gencode(local_op),
		conctcode(Expr_Node_Gen_Reloc(addr, BFD_RELOC_ESP32S2ULP_REG_RW_ADDR),
		conctcode(Expr_Node_Gen_Reloc(high, BFD_RELOC_ESP32S2ULP_REG_RW_HIGH), Expr_Node_Gen_Reloc(low, BFD_RELOC_ESP32S2ULP_REG_RW_LOW))));
}

INSTR_T esp32ulp_cmd_reg_wr_esp32s2(Expr_Node* addr, Expr_Node* high, Expr_Node* low, Expr_Node* data)
{
	//DEBUG_TRACE("dya_pass - OP_CMD_REG_WR \n");
	unsigned int addr_val = EXPR_VALUE(addr);
	unsigned int high_val = EXPR_VALUE(high);
	unsigned int low_val = EXPR_VALUE(low);
	unsigned int data_val = EXPR_VALUE(data);

	if (addr_val > DR_REG_MAX_DIRECT)
	{
		if ((addr_val >= DR_REG_RTCCNTL_BASE) && (addr_val < DR_REG_IO_MUX_BASE))
		{
			addr_val = (addr_val - DR_REG_RTCCNTL_BASE)/4;
		} else {
			error("%s","Register address out of range. Must be 0..0x3ff, or in range of 0x3ff48000 .. 0x3ff49000.");
		}
	}
	unsigned int local_op = I_WR_REG(addr_val, low_val, high_val, data_val);

	return conscode(gencode(local_op),
		conctcode(Expr_Node_Gen_Reloc(addr, BFD_RELOC_ESP32S2ULP_REG_RW_ADDR),
		conctcode(Expr_Node_Gen_Reloc(data, BFD_RELOC_ESP32S2ULP_REG_RW_DATA),
		conctcode(Expr_Node_Gen_Reloc(high, BFD_RELOC_ESP32S2ULP_REG_RW_HIGH), Expr_Node_Gen_Reloc(low, BFD_RELOC_ESP32S2ULP_REG_RW_LOW)))));
}

INSTR_T esp32ulp_cmd_i2c_rd_esp32s2(Expr_Node* i2c_addr, Expr_Node* high, Expr_Node* low, Expr_Node* i2c_sel)
{
	//DEBUG_TRACE("dya_pass - OP_CMD_I2C_RD \n");
	unsigned int addr_val = EXPR_VALUE(i2c_addr);
	unsigned int high_val = EXPR_VALUE(high);
	unsigned int low_val = EXPR_VALUE(low);
	unsigned int i2c_sel_val = EXPR_VALUE(i2c_sel);

	unsigned int local_op = I_I2C_RW(addr_val, 0, low_val, high_val, i2c_sel_val, OPCODE_I2C_RD);

	return conscode(gencode(local_op),
		conctcode(Expr_Node_Gen_Reloc(i2c_addr, BFD_RELOC_ESP32S2ULP_I2C_RW_ADDR),
		conctcode(Expr_Node_Gen_Reloc(i2c_sel, BFD_RELOC_ESP32S2ULP_I2C_RW_SEL),
		conctcode(Expr_Node_Gen_Reloc(high, BFD_RELOC_ESP32S2ULP_I2C_RW_HIGH), Expr_Node_Gen_Reloc(low, BFD_RELOC_ESP32S2ULP_I2C_RW_LOW)))));
}

INSTR_T esp32ulp_cmd_i2c_wr_esp32s2(Expr_Node* i2c_addr, Expr_Node* high, Expr_Node* low, Expr_Node* i2c_sel, Expr_Node* data)
{
	//DEBUG_TRACE("dya_pass - OP_CMD_I2C_RD \n");
	unsigned int addr_val = EXPR_VALUE(i2c_addr);
	unsigned int high_val = EXPR_VALUE(high);
	unsigned int low_val = EXPR_VALUE(low);
	unsigned int i2c_sel_val = EXPR_VALUE(i2c_sel);
	unsigned int data_val = EXPR_VALUE(data);

	unsigned int local_op = I_I2C_RW(addr_val, data_val, low_val, high_val, i2c_sel_val, OPCODE_I2C_WR);

	return conscode(gencode(local_op),
		conctcode(Expr_Node_Gen_Reloc(i2c_addr, BFD_RELOC_ESP32S2ULP_I2C_RW_ADDR),
		conctcode(Expr_Node_Gen_Reloc(i2c_sel, BFD_RELOC_ESP32S2ULP_I2C_RW_SEL),
		conctcode(Expr_Node_Gen_Reloc(data, BFD_RELOC_ESP32S2ULP_I2C_RW_DATA),
		conctcode(Expr_Node_Gen_Reloc(high, BFD_RELOC_ESP32S2ULP_I2C_RW_HIGH), Expr_Node_Gen_Reloc(low, BFD_RELOC_ESP32S2ULP_I2C_RW_LOW))))));
}




// Conditions for JUMPS instructions
#define JUMPR_EQ 2 
#define JUMPR_GT 1
#define JUMPR_LT 0
#define JUMPR_LE 3
#define JUMPR_GE 4
static int ConvertBrcond2JumpRs2(int cond);
static int ConvertBrcond2JumpRs2(int cond)
{
	int result = JUMPR_EQ;
	switch(cond)
	{
		case BRCOND_EQ:
			result = JUMPR_EQ;
			break;
		case BRCOND_GT:
			result = JUMPR_GT;
			break;
		case BRCOND_LT:
			result = JUMPR_LT;
			break;
		case BRCOND_LE:
			result = JUMPR_LE;
			break;
		case BRCOND_GE:
			result = JUMPR_GE;
			break;
		default:
			printf("ConvertBrcond2JumpRS2 conversion error!!!");
	}
	return result;
}

// Conditions for JUMPS instructions
#define JUMPS_EQ 4 
#define JUMPS_GT 3
#define JUMPS_LT 1
#define JUMPS_LE 5
#define JUMPS_GE 7

static int ConvertBrcond2JumpSs2(int cond);
static int ConvertBrcond2JumpSs2(int cond)
{
	int result = JUMPS_EQ;
	switch(cond)
	{
		case BRCOND_EQ:
			result = JUMPS_EQ;
			break;
		case BRCOND_GT:
			result = JUMPS_GT;
			break;
		case BRCOND_LT:
			result = JUMPS_LT;
			break;
		case BRCOND_LE:
			result = JUMPS_LE;
			break;
		case BRCOND_GE:
			result = JUMPS_GE;
			break;
		default:
			printf("ConvertBrcond2JumpSS2 conversion error!!!");
	}
	return result;
}

INSTR_T esp32ulp_cmd_jump_relr_esp32s2(Expr_Node* step, Expr_Node* thresh, int cond)
{
	cond = ConvertBrcond2JumpRs2(cond);
	int step_val = EXPR_VALUE(step);
	int thresh_val = EXPR_VALUE(thresh);
	if ((cond == JUMPR_LE) || (cond == JUMPR_GE)) // EQ == 3, GT = 4
	{
		unsigned int walk_cond = 0;
		unsigned int result_cond = 0;
		if (cond == JUMPR_LE) // EQ
		{
			//	Jumpr   step, threshold, LT
			//	Jumpr   step, threshold, EQ
			walk_cond = JUMPR_LT;
			result_cond = JUMPR_EQ;
		}
		if (cond == JUMPR_GE) // GT
		{
			//	Jumpr   step,  threshold, GT
			//	Jumpr   step, threshold,  EQ

			walk_cond = JUMPR_GT;
			result_cond = JUMPR_EQ;
		}
		unsigned int local_walk = I_JUMP_RELR(thresh_val, walk_cond, step_val>>2);
		if (step_val < 0) step_val = step_val - 4;
		unsigned int local_result = I_JUMP_RELR(thresh_val, result_cond, step_val>>2);

		INSTR_T result = conscode(gencode(local_result), 
					conctcode(Expr_Node_Gen_Reloc(step, BFD_RELOC_ESP32S2ULP_JUMPR_STEP), 
					Expr_Node_Gen_Reloc(thresh, BFD_RELOC_ESP32S2ULP_JUMPR_THRESH)));
					
		INSTR_T walk1 = conscode(gencode(local_walk), 
					conctcode(Expr_Node_Gen_Reloc(step, BFD_RELOC_ESP32S2ULP_JUMPR_STEP), 
					Expr_Node_Gen_Reloc(thresh, BFD_RELOC_ESP32S2ULP_JUMPR_THRESH)));
					
		INSTR_T last_inst = walk1->next;
		while (last_inst->next)
		{
			last_inst = last_inst->next;
		}
		walk1->mult = 0;
		result->mult = 1;
		last_inst->next = result;
		return walk1;

	} else 
	{
		//DEBUG_TRACE("dya_pass - esp32s2ulp_cmd_jump_relr\n");
		unsigned int local_op = I_JUMP_RELR(thresh_val, cond, step_val>>2);
		return conscode(gencode(local_op), 
					conctcode(Expr_Node_Gen_Reloc(step, BFD_RELOC_ESP32S2ULP_JUMPR_STEP), 
					Expr_Node_Gen_Reloc(thresh, BFD_RELOC_ESP32S2ULP_JUMPR_THRESH)));
	}
}


INSTR_T esp32ulp_cmd_jump_rels_esp32s2(Expr_Node* step, Expr_Node* thresh, int cond)
{
	cond = ConvertBrcond2JumpSs2(cond);
	int step_val = EXPR_VALUE(step);
	int thresh_val = EXPR_VALUE(thresh);
	{
		unsigned int local_op = I_JUMP_RELS(thresh_val, cond, step_val);

		INSTR_T result = conscode(gencode(local_op),
			conctcode(Expr_Node_Gen_Reloc(step, BFD_RELOC_ESP32S2ULP_JUMPR_STEP),
			Expr_Node_Gen_Reloc(thresh, BFD_RELOC_ESP32S2ULP_JUMPS_THRESH)
			)
			);
		
		return result;
	}
		   
}



INSTR_T esp32s2ulp_cmd_sleep_esp32s2(Expr_Node*  cycles)
{
	//DEBUG_TRACE("dya_pass - OP_CMD_SLEEP \n");
	int cycles_val = EXPR_VALUE(cycles);
	unsigned int local_op = OP_CMD_SLEEP(cycles_val);
	return conscode(gencode(local_op), Expr_Node_Gen_Reloc(cycles, BFD_RELOC_ESP32S2ULP_SLEEP));
}

INSTR_T esp32ulp_rd_mem_addr_esp32s2(int lh, int dst_reg, int src_reg, Expr_Node* addr)
{
	//DEBUG_TRACE("dya_pass - esp32s2ulp_rd_mem_addr - dst_reg=%i, src_reg=%i, addr=%i\n", dst_reg, src_reg, (int)0);
	int addr_val = EXPR_VALUE(addr);
	unsigned int local_op = RD_MEM(lh, dst_reg, src_reg, addr_val>>2);// I_JUMP_RELR(0, val, cond, 1);

	int rel = 0;
	rel = BFD_RELOC_ESP32S2ULP_WR_MEM;
	return conscode(gencode(local_op), Expr_Node_Gen_Reloc(addr, rel));
}











INSTR_T esp32ulp_wr_mem_stlh_esp32s2(int low_hi, int dst_reg, int src_reg, Expr_Node *offset)
{
    int addr_val = EXPR_VALUE(offset);
    unsigned int local_op = WR_MEM(dst_reg, src_reg, addr_val >> 2, 0, low_hi, 3, 0x4);

    return conscode(gencode(local_op), Expr_Node_Gen_Reloc(offset, BFD_RELOC_ESP32S2ULP_WR_MEM));
}

INSTR_T esp32ulp_wr_mem_st32_esp32s2(int dst_reg, int src_reg, Expr_Node *offset, Expr_Node *label)
{
	unsigned int label_val = EXPR_VALUE(label);
	unsigned int offset_val = EXPR_VALUE(offset);
	unsigned int local_op = WR_MEM(dst_reg, src_reg, offset_val >> 2, label_val, 0, 0, 0x4);

	return conscode(gencode(local_op),
		conctcode(Expr_Node_Gen_Reloc(offset, BFD_RELOC_ESP32S2ULP_WR_MEM), Expr_Node_Gen_Reloc(label, BFD_RELOC_ESP32S2ULP_WR_MEM_LABEL)));
}

INSTR_T esp32ulp_wr_mem_sti32_esp32s2(int dst_reg, int src_reg, Expr_Node* label)
{
	unsigned int label_val = EXPR_VALUE(label);
	unsigned int local_op = WR_MEM(dst_reg, src_reg, 0, label_val, 0, 0, 0x1);

	return conscode(gencode(local_op),Expr_Node_Gen_Reloc(label, BFD_RELOC_ESP32S2ULP_WR_MEM_LABEL));
}

INSTR_T esp32ulp_wr_mem_st_l_esp32s2(int low_hi, int dst_reg, int src_reg, Expr_Node* offset, Expr_Node* label)
{
	unsigned int label_val = EXPR_VALUE(label);
	unsigned int offset_val = EXPR_VALUE(offset);
	unsigned int local_op = WR_MEM(dst_reg, src_reg, offset_val >> 2, label_val, low_hi, 1, 0x4);

	return conscode(gencode(local_op),
		conctcode(Expr_Node_Gen_Reloc(label, BFD_RELOC_ESP32S2ULP_WR_MEM_LABEL), Expr_Node_Gen_Reloc(offset, BFD_RELOC_ESP32S2ULP_WR_MEM)));
}

INSTR_T esp32ulp_wr_mem_sti_l_esp32s2(int dst_reg, int src_reg, Expr_Node* label)
{
	unsigned int label_val = EXPR_VALUE(label);
	unsigned int local_op = WR_MEM(dst_reg, src_reg, 0, label_val, 0, 1, 0x1);

	return conscode(gencode(local_op),Expr_Node_Gen_Reloc(label, BFD_RELOC_ESP32S2ULP_WR_MEM_LABEL));
}
INSTR_T esp32ulp_wr_mem_sti_esp32s2(int dst_reg, int src_reg)
{
    unsigned int local_op = WR_MEM(dst_reg, src_reg, 0, 0, 0, 3, 0x1);
    return GEN_OPCODE32_DYA(local_op);
}

INSTR_T esp32ulp_wr_mem_sto_esp32s2(Expr_Node* offset)
{
    int addr_val = EXPR_VALUE(offset);
    unsigned int local_op = WR_MEM(0, 0, addr_val >> 2, 0, 0, 0, 0x2);

    return conscode(gencode(local_op), Expr_Node_Gen_Reloc(offset, BFD_RELOC_ESP32S2ULP_WR_MEM));
}


void md_apply_fix_esp32s2(fixS *fixP, valueT *valueP, segT seg ATTRIBUTE_UNUSED)
{
	char *where = fixP->fx_frag->fr_literal + fixP->fx_where;

	long value = *valueP;
	//DEBUG_TRACE("dya_pass - md_apply_fix: fixP->fx_r_type=%i, value=%i, fixP->fx_where=%i\n", fixP->fx_r_type, (unsigned int)value, (int)fixP->fx_where);
	switch (fixP->fx_r_type)
	{
	case BFD_RELOC_ESP32S2ULP_16_IMM:
		if (fixP->fx_addsy != NULL)// relocation will be done not in linker
		{
			asymbol *sym = symbol_get_bfdsym(fixP->fx_addsy);
			int force_reloc = S_FORCE_RELOC(fixP->fx_addsy, 1);
			//printf("force_reloc = %i \n", force_reloc);
			if (force_reloc != 0) if (sym->section->flags != 0) value = value >> 2;
		}
		if ((value < 0) || (value > 2047))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_16"));
		if ((value%4) != 0) 
			as_bad_where(fixP->fx_file, fixP->fx_line, _("Esp32S2Ulp is 32-bit addressing processor. Relative jump value must be divided by 4."));

		//value = value << 2;
		md_number_to_chars(where, value, 2);
		break;

	case BFD_RELOC_ESP32S2ULP_JUMPR:
		if (value < -0x8000 || value > 0x7fff)
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_ESP32S2ULP_JUMPR"));
		//value = value << 2;
		if (fixP->fx_addsy != NULL)// relocation will be done not in linker
		{
			asymbol *sym = symbol_get_bfdsym(fixP->fx_addsy);
			int force_reloc = S_FORCE_RELOC(fixP->fx_addsy, 1);
			if (force_reloc != 0) if (sym->section->flags != 0) value = value >> 2;
		}
		unsigned int temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~(0xff << 18);
		temp_val |= value;
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_JUMPR temp_val=%08x where[0]=%02x, where[1]=%02x, where[2]=%02x, where[3]=%02x, \n", (unsigned int)temp_val, (unsigned int)where[0], (int)where[1], (int)where[2], (int)where[3]);
		//md_number_to_chars(where, value, 0);
		break;
	case BFD_RELOC_ESP32S2ULP_16_LOAD:
		if (value > 0xffff)
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_LOAD_16"));
		//value = value << 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~(0xffff);
		temp_val |= value;
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_16_LOAD temp_val=%08x where[0]=%02x, where[1]=%02x, where[2]=%02x, where[3]=%02x, \n", (unsigned int)temp_val, (unsigned int)where[0], (int)where[1], (int)where[2], (int)where[3]);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_WR_MEM:
		value = value >> 2;
		if ((value > 1023) || (value < -1024))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_WR_MEM"));
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x7ff << 10));
		temp_val |= (value << 10);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_WR_MEM temp_val=%08x where[0]=%02x, where[1]=%02x, where[2]=%02x, where[3]=%02x, \n", (unsigned int)temp_val, (unsigned int)where[0], (int)where[1], (int)where[2], (int)where[3]);
		//md_number_to_chars(where, value, 0);
		break;
	case BFD_RELOC_ESP32S2ULP_WR_MEM_LABEL:
		if ((value > 3) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_WR_MEM_LABEL"));
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x3 << 4));
		temp_val |= (value << 4);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_WR_MEM_LABEL temp_val=%08x where[0]=%02x, where[1]=%02x, where[2]=%02x, where[3]=%02x, \n", (unsigned int)temp_val, (unsigned int)where[0], (int)where[1], (int)where[2], (int)where[3]);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_ALUI:
		if ((value > 0xffff) || (value < -0xffff))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_ALUI"));
		//value = 0x00ffff; // for tests only
		if (fixP->fx_addsy != NULL)// relocation will be done not in linker
		{
			asymbol *sym = symbol_get_bfdsym(fixP->fx_addsy);
			if (sym->section->flags != 0) value = value >> 2;
		}
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xffff << 4));
		temp_val |= (value << 4);
		memcpy(where, &temp_val, 4);
		
		//if (fixP->fx_addsy != NULL)
		//{
		//	asymbol *sym = symbol_get_bfdsym(fixP->fx_addsy);
		//	DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_ALUI temp_val=%08x value=%08x, flags=%08x, fixP=%08x, done=%08x, fixP->fx_addsy=%08x, flags=%08x, name=%s\n",
		//		(unsigned int)temp_val, 
		//		(unsigned int)value, (unsigned int)seg->flags, *(unsigned int*)fixP, (unsigned int)fixP->fx_done, *(unsigned int*)fixP->fx_addsy, sym->section->flags, sym->section->name);
		//}
		//else
		//{
		//	DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_ALUI temp_val=%08x value=%08x, flags=%08x, fixP=%08x, done=%08x, fixP->fx_addsy=%08x\n", (unsigned int)temp_val, (unsigned int)value, (unsigned int)seg->flags, *(unsigned int*)fixP, (unsigned int)fixP->fx_done, 0xdead);
		//}
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_WAIT:
		if ((value > 0xffff) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_WAIT"));
		//value = value << 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xffff << 0));
		temp_val |= (value << 0);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_WAIT temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_TSENS_CYCLE:
		if ((value > 0xfff) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_TSENS_CYLE"));
		//value = value << 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xfff << 16));
		temp_val |= (value << 16);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_TSENS_CYCLE temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_TSENS_DELAY:
		if ((value > 0x3fff) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_TSENS_DELAY"));
		//value = value << 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x3fff << 2));
		temp_val |= (value << 2);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_TSENS_DELAY temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_WAKE:
		if ((value > 1) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_WAKE"));
		//value = value << 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x1 << 0));
		temp_val |= (value << 0);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_WAKE temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_SLEEP:
		if ((value > 0xffff) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_SLEEP"));
		//value = value << 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xffff << 0));
		temp_val |= (value << 0);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_SLEEP temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_ADC_CYCLE:
		if ((value > 0xffff) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_ADC_CYCLE"));
		//value = value << 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xffff << 8));
		temp_val |= (value << 8);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_ADC_CYCLE temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_ADC_SEL:
		if ((value > 0x01) || (value < 0x00))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_ADC_SEL"));
		//value = value << 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x1 << 6));
		temp_val |= (value << 6);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_ADC_SEL temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_ADC_MUX:
		if ((value > 0x0f) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_ADC_MUX"));
		//value = value << 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x0f << 2));
		temp_val |= (value << 2);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_ADC_MUX temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_STAGE:
		if (value > 0xff)
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_STAGE"));

		temp_val = 0;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xff << 4));
		temp_val |= (value << 4);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_STAGE temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_JUMPR_STEP:
		if (value < -0x8000 || value > 0x7fff)
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_ESP32S2ULP_JUMPR_STEP"));
		
		value = value >> 2;
		temp_val = 0;
		memcpy(&temp_val, where, 4);
        if (value >= 0)
        {
            temp_val |= (value & 0x7f) << 18;
        }
        else
        {
            value = -value;
            temp_val |= (value & 0x7f) << 18;
            temp_val |= 0x80 << 18;
        }
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_JUMPR temp_val=%08x where[0]=%02x, where[1]=%02x, where[2]=%02x, where[3]=%02x, \n", (unsigned int)temp_val, (unsigned int)where[0], (int)where[1], (int)where[2], (int)where[3]);
		//md_number_to_chars(where, value, 0);
		break;
	case BFD_RELOC_ESP32S2ULP_JUMPR_THRESH:
		if ((value > 0x7fff) || (value < -0x7fff))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_JUMPR_THRESH"));
		//value = value << 2;
		temp_val = 0;
		value &= 0xffff;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xffff << 0));
		temp_val |= (value << 0);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_JUMPR_THRESH temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_JUMPS_THRESH:
		if ((value > 0x7f) || (value < -0x7f))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_JUMPS_THRESH"));
		//value = value << 2;
		temp_val = 0;
		value &= 0xff;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xff << 0));
		temp_val |= (value << 0);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_JUMPS_THRESH temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		//md_number_to_chars(where, value, 0);
		break;

	case BFD_RELOC_ESP32S2ULP_REG_RW_HIGH:
		if ((value > 0x1f) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_ESP32S2ULP_REG_RW_HIGH"));
		temp_val = 0;
		value &= 0x1f;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x1f << 23));
		temp_val |= (value << 23);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_REG_RW_HIGH temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		break;
	case BFD_RELOC_ESP32S2ULP_REG_RW_LOW:
		if ((value > 0x1f) || (value < 0))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_ESP32S2ULP_REG_RW_LOW"));
		temp_val = 0;
		value &= 0x1f;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x1f << 18));
		temp_val |= (value << 18);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_REG_RW_LOW temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		break;

	case BFD_RELOC_ESP32S2ULP_REG_RW_ADDR:
		value = check_reg_range(fixP, value);

		if (fixP->fx_addsy != NULL)// relocation will be done not in linker
		{
			asymbol *sym = symbol_get_bfdsym(fixP->fx_addsy);
			if (sym->section->flags != 0) value = value >> 2;
		}
		temp_val = 0;
		value &= 0x3ff;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x3ff << 0));
		temp_val |= (value << 0);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_REG_RW_ADDR temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		break;
	case BFD_RELOC_ESP32S2ULP_REG_RW_DATA:
		value = check_reg_range(fixP, value);

		temp_val = 0;
		value &= 0xff;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xff << 10));
		temp_val |= (value << 10);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_REG_RW_DATA temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		break;

	case BFD_RELOC_ESP32S2ULP_I2C_RW_HIGH:
		if ((value > 0x7) || (value < -0x7))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_ESP32S2ULP_I2C_RW_HIGH"));
		temp_val = 0;
		value &= 0x7;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x7 << 19));
		temp_val |= (value << 19);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_I2C_RW_HIGH temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		break;
	case BFD_RELOC_ESP32S2ULP_I2C_RW_LOW:
		if ((value > 0x7) || (value < -0x7))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_ESP32S2ULP_I2C_RW_LOW"));
		temp_val = 0;
		value &= 0x7;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0x7 << 16));
		temp_val |= (value << 16);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_I2C_RW_LOW temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		break;
	case BFD_RELOC_ESP32S2ULP_I2C_RW_SEL:
		if ((value > 0xf) || (value < -0xf))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_ESP32S2ULP_I2C_RW_SEL"));
		temp_val = 0;
		value &= 0xf;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xf << 22));
		temp_val |= (value << 22);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_I2C_RW_SEL temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		break;
	case BFD_RELOC_ESP32S2ULP_I2C_RW_ADDR:
		if ((value > 0xff) || (value < -0xff))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_ESP32S2ULP_I2C_RW_ADDR"));
		temp_val = 0;
		value &= 0xff;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xff << 0));
		temp_val |= (value << 0);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_I2C_RW_ADDR temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		break;
	case BFD_RELOC_ESP32S2ULP_I2C_RW_DATA:
		if ((value > 0xff) || (value < -0xff))
			as_bad_where(fixP->fx_file, fixP->fx_line, _("rel too far BFD_RELOC_ESP32S2ULP_I2C_RW_DATA"));
		temp_val = 0;
		value &= 0xff;
		memcpy(&temp_val, where, 4);
		temp_val &= ~((0xff << 8));
		temp_val |= (value << 8);
		memcpy(where, &temp_val, 4);
		//DEBUG_TRACE("dya_pass - md_apply_fix:BFD_RELOC_ESP32S2ULP_I2C_RW_DATA temp_val=%08x value=%08x\n", (unsigned int)temp_val, (unsigned int)value);
		break;

	default:
		fprintf(stderr, "Relocation %d not handled in Esp32s2 gas." " Contact support.\n", fixP->fx_r_type);
		return;
		//if ((BFD_ARELOC_ESP32S2ULP_PUSH > fixP->fx_r_type) || (BFD_ARELOC_ESP32S2ULP_COMP < fixP->fx_r_type))
		//{
		//	fprintf(stderr, "Relocation %d not handled in gas." " Contact support.\n", fixP->fx_r_type);
		//	return;
		//}
	}

	if (!fixP->fx_addsy)
		fixP->fx_done = TRUE;

}

static INSTR_T Expr_Node_Gen_Reloc_R_s2(Expr_Node * head);
static const char *con = ".__constant";

static INSTR_T Expr_Node_Gen_Reloc_R_s2(Expr_Node * head)
{

	INSTR_T note = 0;
	INSTR_T note1 = 0;

	switch (head->type)
	{
	case Expr_Node_Constant:
		note = conscode(note_reloc2(gencode(0), con, BFD_ARELOC_ESP32ULP_CONST, head->value.i_value, 0), NULL_CODE);
		break;
	case Expr_Node_Reloc:
		note = conscode(note_reloc(gencode(0), head, BFD_ARELOC_ESP32ULP_PUSH, 0), NULL_CODE);
		break;
	case Expr_Node_Unop:
		note1 = conscode(Expr_Node_Gen_Reloc_R_s2(head->Left_Child), NULL_CODE);
		switch (head->value.op_value)
		{
		case Expr_Op_Type_NEG:
			note = conctcode(note1, conscode(note_reloc1(gencode(0), op, BFD_ARELOC_ESP32ULP_NEG, 0), NULL_CODE));
			break;
		case Expr_Op_Type_COMP:
			note = conctcode(note1, conscode(note_reloc1(gencode(0), op, BFD_ARELOC_ESP32ULP_COMP, 0), NULL_CODE));
			break;
		default:
			fprintf(stderr, "%s:%d:Unknown operator found for arithmetic" " relocation", __FILE__, __LINE__);
		}
		break;
	default:
		fprintf(stderr, "%s:%d:Unknown node expression found during " "arithmetic relocation generation", __FILE__, __LINE__);
	}
	return note;
}


INSTR_T Expr_Node_Gen_Reloc_esp32s2(Expr_Node *head, int parent_reloc)
{
	/* Top level reloction expression generator VDSP style.
	If the relocation is just by itself, generate one item
	else generate this convoluted expression.  */

	INSTR_T note = NULL_CODE;
	INSTR_T note1 = NULL_CODE;
	int pcrel = 1; /* Is the parent reloc pcrelative?
					This calculation here and HOWTO should match.  */

	if (parent_reloc)
	{
		/*  If it's 32 bit quantity then 16bit code needs to be added.  */
		int value = 0;

		if (head->type == Expr_Node_Constant)
		{
			/* If note1 is not null code, we have to generate a right
			aligned value for the constant. Otherwise the reloc is
			a part of the basic command and the yacc file
			generates this.  */
			value = head->value.i_value;
		}
		switch (parent_reloc)
		{
			/*  Some relocations will need to allocate extra words.  */
			case BFD_RELOC_ESP32S2ULP_16_IMM:
			case BFD_RELOC_ESP32S2ULP_JUMPR:
			case BFD_RELOC_ESP32S2ULP_16_LOAD:
			case BFD_RELOC_ESP32S2ULP_WR_MEM:
			case BFD_RELOC_ESP32S2ULP_WR_MEM_LABEL:
			case BFD_RELOC_ESP32S2ULP_ALUI:
			case BFD_RELOC_ESP32S2ULP_WAIT:
			case BFD_RELOC_ESP32S2ULP_TSENS_DELAY:
			case BFD_RELOC_ESP32S2ULP_WAKE:
			case BFD_RELOC_ESP32S2ULP_SLEEP:
			case BFD_RELOC_ESP32S2ULP_ADC_CYCLE:
			case BFD_RELOC_ESP32S2ULP_ADC_SEL:
			case BFD_RELOC_ESP32S2ULP_ADC_MUX:
			case BFD_RELOC_ESP32S2ULP_STAGE:
			//case BFD_RELOC_ESP32S2ULP_JUMPR_STEP:
			case BFD_RELOC_ESP32S2ULP_JUMPR_THRESH:
			case BFD_RELOC_ESP32S2ULP_JUMPS_THRESH:
			case BFD_RELOC_ESP32S2ULP_REG_RW_HIGH:
			case BFD_RELOC_ESP32S2ULP_REG_RW_LOW:
			case BFD_RELOC_ESP32S2ULP_REG_RW_ADDR:
			case BFD_RELOC_ESP32S2ULP_REG_RW_DATA:
			case BFD_RELOC_ESP32S2ULP_I2C_RW_HIGH:
			case BFD_RELOC_ESP32S2ULP_I2C_RW_LOW:
			case BFD_RELOC_ESP32S2ULP_I2C_RW_ADDR:
			case BFD_RELOC_ESP32S2ULP_I2C_RW_DATA:
			case BFD_RELOC_ESP32S2ULP_I2C_RW_SEL:
				note1 = conscode(gencode(value), NULL_CODE);
				//printf("Expr_Node_Gen_Reloc_esp32s2 parent_reloc= %i\n", parent_reloc);
				pcrel = 0;
				break;
			case BFD_RELOC_16:
			case BFD_RELOC_24:
			case BFD_RELOC_32:
				note1 = conscode(gencode(value), NULL_CODE);
				pcrel = 0;
				break;
			default:
				note1 = NULL_CODE;
		}
	}
	if (head->type == Expr_Node_Constant)
		note = note1;
	else if (head->type == Expr_Node_Reloc)
	{
		note = note_reloc1(gencode(0), head->value.s_value, parent_reloc, pcrel);
		if (note1 != NULL_CODE)
			note = conscode(note1, note);
	}
	else if (head->type == Expr_Node_Binop 
		&& (head->value.op_value == Expr_Op_Type_Add 
		|| head->value.op_value == Expr_Op_Type_Sub) 
		&& head->Left_Child->type == Expr_Node_Reloc 
		&& head->Right_Child->type == Expr_Node_Constant)
	{
		int val = head->Right_Child->value.i_value;
		if (head->value.op_value == Expr_Op_Type_Sub)
			val = -val;
		note = conscode(note_reloc2(gencode(0), head->Left_Child->value.s_value,
									parent_reloc, val, 0),
						NULL_CODE);
		if (note1 != NULL_CODE)
			note = conscode(note1, note);
	}
	else
	{
		/* Call the recursive function.  */
		note = note_reloc1(gencode(0), op, parent_reloc, pcrel);
		if (note1 != NULL_CODE)
			note = conscode(note1, note);
		note = conctcode(Expr_Node_Gen_Reloc_R_s2(head), note);
	}
	return note;
}

#endif // TC_ESP32ULP