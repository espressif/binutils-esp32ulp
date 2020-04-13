/* tc-esp32ulp.c    
ESP32ULP GNU Assembler

Copyright (c) 2016-2017 Espressif Systems (Shanghai) PTE LTD.
   
based on Copyright (C) 2005-2017 Free Software Foundation, Inc.

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


extern int yyparse(void);
struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *yy_str);
extern void yy_delete_buffer(YY_BUFFER_STATE b);
static parse_state parse(char *line);
long check_reg_range(fixS* fixP, long value);

/* Global variables. */
struct esp32ulp_insn *insn;
int last_insn_size;

extern struct obstack mempool;
FILE *errorf;

/* Flags to set in the elf header */
#define DEFAULT_FLAGS 0

#ifdef OBJ_FDPIC_ELF
# define DEFAULT_FDPIC EF_ESP32ULP_FDPIC
#else
# define DEFAULT_FDPIC 0
#endif

static flagword esp32ulp_flags = DEFAULT_FLAGS | DEFAULT_FDPIC;

static void esp32ulp_s_bss(int ignore ATTRIBUTE_UNUSED)
{
	int temp;

	temp = get_absolute_expression();
	subseg_set(bss_section, (subsegT)temp);
	demand_empty_rest_of_line();
}

const pseudo_typeS md_pseudo_table[] = {
	{ "align", s_align_bytes, 0 },
	{ "byte2", cons, 2 },
	{ "byte4", cons, 4 },
	{ "db", cons, 1 },
	{ "dd", cons, 4 },
	{ "dw", cons, 2 },
	{ "p", s_ignore, 0 },
	{ "pdata", s_ignore, 0 },
	{ "var", s_ignore, 0 },
	{ "bss", esp32ulp_s_bss, 0 },
	{ 0, 0, 0 }
};

/* Characters that are used to denote comments and line separators. */
const char comment_chars[] = "#";
const char line_comment_chars[] = "#";
const char line_separator_chars[] = ";";

/* Characters that can be used to separate the mantissa from the
exponent in floating point numbers. */
const char EXP_CHARS[] = "eE";

/* Characters that mean this number is a floating point constant.
As in 0f12.456 or  0d1.2345e12.  */
const char FLT_CHARS[] = "fFdDxX";
unsigned int esp32ulp_anomaly_checks = 0;

// This variable will be used to define command set for different CPU types for the ULP 
// The default is CPU_TYPE_ESP32ULP
int ulp_cpu_type = CPU_TYPE_ESP32ULP;


/* Define esp32ulp-specific command-line options (there are none). */
const char *md_shortopts = "";

#define OPTION_MCPU		(OPTION_MD_BASE)

struct option md_longopts[] =
{
	{ "mcpu",	required_argument,	NULL, 	OPTION_MCPU	},
	{ NULL, no_argument, NULL, 0 },
};

size_t md_longopts_size = sizeof(md_longopts);

int md_parse_option(int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
//	printf("md_parse_option: arg - %s\n", arg);
	switch (c)
	{
		case OPTION_MCPU:
		{
			if (strcasecmp(arg , "esp32") == 0)
			{
				ulp_cpu_type = CPU_TYPE_ESP32ULP;
			} else if (strcasecmp(arg , "esp32s2") == 0)
			{
				ulp_cpu_type = CPU_TYPE_ESP32ULP_S2;
			} else
			{
				// MCU not supported!
				ulp_cpu_type = CPU_TYPE_ESP32ULP;
				return 0;
			}
			break;
		}
		default:
			return 0;
	}
	return 1;
}

const char wrong_cpu_message[] = "This command not supported by this version of ULP CPU. Please use different --mcpu=... option to compile.";

void md_show_usage(FILE * stream)
{
	fprintf(stream, _(" ESP32 ULP processor has no specific assembler options:\n"));
}

/* Perform machine-specific initializations.  */
void md_begin(void)
{
	/* Set the ELF flags if desired. */
	if (esp32ulp_flags)
		bfd_set_private_flags(stdoutput, esp32ulp_flags);

	/* Set the default machine type. */
	if (!bfd_set_arch_mach(stdoutput, bfd_arch_esp32ulp, 0))
		as_warn(_("Could not set architecture and machine."));

	/* Ensure that lines can begin with '(', for multiple
	register stack pops. */
	lex_type['('] = LEX_BEGIN_NAME;

#ifdef OBJ_ELF
	record_alignment(text_section, 2);
	record_alignment(data_section, 2);
	record_alignment(bss_section, 2);
#endif

	errorf = stderr;
	obstack_init(&mempool);

#ifdef DEBUG
	extern int debug_codeselection;
	debug_codeselection = 1;
#endif

	last_insn_size = 0;
}

/* Perform the main parsing, and assembly of the input here.  Also,
call the required routines for alignment and fixups here.
This is called for every line that contains real assembly code.  */
//#include "../../gdb/symtab.h"
void md_assemble(char *line)
{
	char *toP ;
	int  insn_size;
	size_t len;
	static size_t buffer_len = 0;
	static char *current_inputline;
	parse_state state;

	len = strlen(line);
	if (len + 2 > buffer_len)
	{
		buffer_len = len + 40;
		current_inputline = XRESIZEVEC(char, current_inputline, buffer_len);
	}
	memcpy(current_inputline, line, len);
	current_inputline[len] = ';';
	current_inputline[len + 1] = '\0';

	// run the parser on the instruction
	//   it will return a list of chained "insn",
	// 	 the first contains the opcode of the instruction
	//   and may be followed by other "insn" like an address
	state = parse(current_inputline);
	if (state == NO_INSN_GENERATED || !insn)
		return;

	// add 4 bytes to the fragment code buffer to put the new instruction
	// and get buffer pointer (toP) on where to write the instruction
	insn_size = 4;
	toP = frag_more(insn_size);			

#ifdef DEBUG
	printf("INS: %s\n", line);
#endif
	md_number_to_chars(toP, insn->value, insn_size);	// put the 4-byte instruction into the current fragment code buffer 
	
	while (insn)
	{
		if (insn->reloc && insn->exp->symbol)
		{
			//DEBUG_TRACE("insn->reloc && insn->exp->symbol BFD_ARELOC_ESP32ULP_PUSH size =%i, insn->exp->value=%i, insn->pcrel=%i, insn->reloc=%i\n", size, (unsigned int)insn->exp->value, insn->pcrel, insn->reloc);

			// generate a relocation request for this instruction so that linker will put the right address
			//   toP is the pointer on this instruction in the buffer of the current code fragment 
			//   frag_now->fr_literal is the pointer on the begining of the buffer of the current code fragment
			fix_new(frag_now, toP - frag_now->fr_literal, insn_size, insn->exp->symbol, insn->exp->value, insn->pcrel, insn->reloc);
		}
#ifdef DEBUG
		//DEBUG_TRACE(" reloc : value = %08x, pc=%08x, reloc=%08x BFD_RELOC_ESP32ULP_PLTPC = %08x\n", (unsigned int)insn->value, (unsigned int)insn->pcrel, (unsigned int)insn->reloc, BFD_RELOC_ESP32ULP_PLTPC);
		if (insn->exp != ((void*)(0)))
		{
			//DEBUG_TRACE(" exp: %08x, sy_obj=%i\n", insn->exp->value, insn->exp->symbol->sy_obj.local);
		}
#endif
		insn = insn->next;
		if (insn)
		{
			int ml = insn->mult;
			if (ml != 0)
			{
				toP = frag_more(insn_size);
				md_number_to_chars(toP, insn->value, insn_size);	// put the 4-byte instruction into the current fragment code buffer 
			}
		}
	}
#ifdef OBJ_ELF
	dwarf2_emit_insn(insn_size);
	//DEBUG_TRACE("dya_pass ============== >insn_size=%i\n", (unsigned int)insn_size);
#endif

	while (*line++ != '\0')
		if (*line == '\n')
			bump_line_counters();
}

/* Parse one line of instructions, and generate opcode for it.
To parse the line, YACC and LEX are used, because the instruction set
syntax doesn't confirm to the AT&T assembly syntax.
To call a YACC & LEX generated parser, we must provide the input via
a FILE stream, otherwise stdin is used by default.  Below the input
to the function will be put into a temporary file, then the generated
parser uses the temporary file for parsing.  */

static parse_state parse(char *line)
{
	parse_state state;
	YY_BUFFER_STATE buffstate;

	buffstate = yy_scan_string(line);

	/* our lex requires setting the start state to keyword
	every line as the first word may be a keyword.
	Fixes a bug where we could not have keywords as labels.  */
	set_start_state();

	/* Call yyparse here.  */
	//DEBUG_TRACE("yyparse: %s\n", line);
	state = yyparse();
	//DEBUG_TRACE("yyparse result!: %i\n", (int)state);
	if (state == SEMANTIC_ERROR)
	{
		as_bad(_("Parse failed."));
		//DEBUG_TRACE("yyparse ERROR!: %s\n", line);
		insn = 0;
	}

	yy_delete_buffer(buffstate);
	return state;
}

/* We need to handle various expressions properly.
Such as, [SP--] = 34, concerned by md_assemble().  */

void md_operand(expressionS * expressionP)
{
	if (*input_line_pointer == '[')
	{
		as_tsktsk("We found a '['!");
		input_line_pointer++;
		expression(expressionP);
	}
}

/* Handle undefined symbols. */
symbolS * md_undefined_symbol(char *name ATTRIBUTE_UNUSED)
{
	return (symbolS *)0;
}

int md_estimate_size_before_relax(fragS * fragP ATTRIBUTE_UNUSED,
segT segment ATTRIBUTE_UNUSED)
{
	return 0;
}

long check_reg_range(fixS* fixP, long value)
{
	long result = value;
	if ((value > DR_REG_MAX_DIRECT) || (value < 0)){
		if ((value >= DR_REG_RTCCNTL_BASE) && (value < DR_REG_IO_MUX_BASE)){
			result = (value - DR_REG_RTCCNTL_BASE)/4;
		} else {
			as_bad_where(fixP->fx_file, fixP->fx_line, _("Register address out of range. Must be in range of 0..0x3ff or 0x3ff48000..0x3ff49000."));
		}
	}
	return result;
}

static const char *con = ".__constant";
static const char *op = ".__operator";

// Step to reach next instruction

#include "tc-esp32ulp_esp32.h"
#include "tc-esp32ulp_esp32s2.h"

void md_apply_fix(fixS *fixP, valueT *valueP, segT seg ATTRIBUTE_UNUSED)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		md_apply_fix_esp32(fixP, valueP, seg);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		md_apply_fix_esp32s2(fixP, valueP, seg);
	} else 
	{
		printf("Error!!!");
	}
}

/* Round up a section size to the appropriate boundary.  */
valueT md_section_align(segT segment, valueT size)
{
	int boundary = bfd_get_section_alignment(stdoutput, segment);
	return ((size + (1 << boundary) - 1) & -(1 << boundary));
}


const char* md_atof(int type, char * litP, int * sizeP)
{
	return ieee_md_atof(type, litP, sizeP, FALSE);
}


/* If while processing a fixup, a reloc really needs to be created
then it is done here.  */

arelent* tc_gen_reloc(asection *seg ATTRIBUTE_UNUSED, fixS *fixp)
{
	arelent *reloc;

	reloc = XNEW(arelent);
	reloc->sym_ptr_ptr = XNEW(asymbol *);
	*reloc->sym_ptr_ptr = symbol_get_bfdsym(fixp->fx_addsy);
	reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
	
	//reloc->address = reloc->address / 4;

	reloc->addend = fixp->fx_offset;
	reloc->howto = bfd_reloc_type_lookup(stdoutput, fixp->fx_r_type);
	
	//DEBUG_TRACE("tc_gen_reloc - reloc->address =%08x, symvalue=%08x, seg->flags=%08x, name=%s\n", (unsigned int)reloc->address, (unsigned int)(*reloc->sym_ptr_ptr)->value, (unsigned int)seg->flags, (*reloc->sym_ptr_ptr)->name);

	if (reloc->howto == (reloc_howto_type *)NULL)
	{
		as_bad_where(fixp->fx_file, fixp->fx_line,
			/* xgettext:c-format.  */
			_("reloc %d not supported by object file format"),
			(int)fixp->fx_r_type);

		xfree(reloc);

		return NULL;
	}

	return reloc;
}

/*  The location from which a PC relative jump should be calculated,
given a PC relative reloc.  */

long md_pcrel_from_section(fixS *fixP, segT sec)
{
	if (fixP->fx_addsy != (symbolS *)NULL
		&& (!S_IS_DEFINED(fixP->fx_addsy)
		|| S_GET_SEGMENT(fixP->fx_addsy) != sec))
	{
		/* The symbol is undefined (or is defined but not in this section).
		Let the linker figure it out.  */
		return 0;
	}
	return fixP->fx_frag->fr_address + fixP->fx_where;
}

/* Return true if the fix can be handled by GAS, false if it must
be passed through to the linker.  */

bfd_boolean esp32ulp_fix_adjustable(fixS *fixP)
{
	switch (fixP->fx_r_type)
	{
		/* Adjust_reloc_syms doesn't know about the GOT.  */
	case BFD_RELOC_ESP32ULP_GOT:
	case BFD_RELOC_ESP32ULP_PLTPC:
		/* We need the symbol name for the VTABLE entries.  */
	case BFD_RELOC_VTABLE_INHERIT:
	case BFD_RELOC_VTABLE_ENTRY:
		return 0;

	default:
		return 1;
	}
}

/* Special extra functions that help esp32ulp-parse.y perform its job.  */

struct obstack mempool;

INSTR_T conscode(INSTR_T head, INSTR_T tail)
{
	if (!head)
		return tail;
	head->next = tail;
	return head;
}

INSTR_T
conctcode(INSTR_T head, INSTR_T tail)
{
	INSTR_T temp = (head);
	if (!head)
		return tail;
	while (temp->next)
		temp = temp->next;
	temp->next = tail;

	return head;
}

INSTR_T note_reloc(INSTR_T code, Expr_Node * symbol, int reloc, int pcrel)
{
	/* Assert that the symbol is not an operator.  */
	gas_assert(symbol->type == Expr_Node_Reloc);

	return note_reloc1(code, symbol->value.s_value, reloc, pcrel);

}

INSTR_T note_reloc1(INSTR_T code, const char *symbol, int reloc, int pcrel)
{
	code->reloc = reloc;
	code->exp = mkexpr(0, symbol_find_or_make(symbol));
	code->pcrel = pcrel;
	return code;
}

INSTR_T note_reloc2(INSTR_T code, const char *symbol, int reloc, int value, int pcrel)
{
	code->reloc = reloc;
	code->exp = mkexpr(value, symbol_find_or_make(symbol));
	code->pcrel = pcrel;
	return code;
}

INSTR_T gencode(unsigned long x)
{
	INSTR_T cell = XOBNEW(&mempool, struct esp32ulp_insn);
	memset(cell, 0, sizeof(struct esp32ulp_insn));
	cell->value = (x);
	return cell;
}

int reloc;
int ninsns;
int count_insns;

static void *allocate(size_t n)
{
	return obstack_alloc(&mempool, n);
}

Expr_Node * Expr_Node_Create(Expr_Node_Type type, Expr_Node_Value value, Expr_Node *Left_Child, Expr_Node *Right_Child)
{


	Expr_Node *node = (Expr_Node *)allocate(sizeof(Expr_Node));
	node->type = type;
	node->value = value;
	node->Left_Child = Left_Child;
	node->Right_Child = Right_Child;
	return node;
}

bfd_boolean esp32ulp_eol_in_insn(char *line)
{
	/* Allow a new-line to appear in the middle of a multi-issue instruction.  */

	char *temp = line;

	if (*line != '\n')
		return FALSE;

	/* A semi-colon followed by a newline is always the end of a line.  */
	if (line[-1] == ';')
		return FALSE;

	if (line[-1] == '|')
		return TRUE;

	/* If the || is on the next line, there might be leading whitespace.  */
	temp++;
	while (*temp == ' ' || *temp == '\t') temp++;

	if (*temp == '|')
		return TRUE;

	return FALSE;
}

bfd_boolean esp32ulp_start_label(char *s)
{
	while (*s != 0)
	{
		if (*s == '(' || *s == '[')
			return FALSE;
		s++;
	}

	return TRUE;
}


INSTR_T Expr_Node_Gen_Reloc(Expr_Node *head, int parent_reloc);

INSTR_T Expr_Node_Gen_Reloc(Expr_Node * head, int parent_reloc)
{
	// INSTR_T result = Expr_Node_Gen_Reloc_esp32(head, parent_reloc);
	// if (NULL_CODE == result) result = Expr_Node_Gen_Reloc_esp32(head, parent_reloc);
	// return result;
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return Expr_Node_Gen_Reloc_esp32(head, parent_reloc);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return Expr_Node_Gen_Reloc_esp32s2(head, parent_reloc);
	}
	return 0;
}

INSTR_T Expr_Node_Gen_Reloc_R(Expr_Node * head)
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
		note1 = conscode(Expr_Node_Gen_Reloc_R(head->Left_Child), NULL_CODE);
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

/* Esp32ULP opcode generation.  */

/* These functions are called by the generated parser
(from esp32ulp-parse.y), the register type classification
happens in esp32ulp-lex.l.  */

#include "esp32ulp-aux.h"
#include "opcode/esp32ulp.h"

#define INIT(t)  t c_code = init_##t
#define ASSIGN(x) c_code.opcode |= ((x & c_code.mask_##x)<<c_code.bits_##x)
#define ASSIGNF(x,f) c_code.opcode |= ((x & c_code.mask_##f)<<c_code.bits_##f)

#define GEN_OPCODE32()  \
	conscode (gencode (HI (c_code.opcode)), \
	conscode (gencode (LO (c_code.opcode)), NULL_CODE))

#define GEN_OPCODE16()  \
	conscode (gencode (c_code.opcode), NULL_CODE)


/*  32 BIT INSTRUCTIONS.  */
#define GEN_OPCODE32_DYA(opcode)  \
	conscode (gencode (opcode), NULL_CODE)


// =================================   ESP32 commands ===========================================================

int esp32ulp_force_relocation(struct fix *fixp)
{
	if (fixp->fx_r_type == BFD_RELOC_ESP32ULP_JUMPR)
		return TRUE;

	return generic_force_reloc(fixp);
}

INSTR_T esp32ulp_gen_wait()
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_gen_wait_esp32();
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_gen_wait_esp32s2();
	}
	return 0;
}


//==================================    ESP     ==========================

// ===============  Esp32 ULP Instructions implementation ================================

INSTR_T esp32ulp_gen_alur(int dst, int src1, int src2, int operation)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_gen_alur_esp32(dst, src1, src2, operation);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_gen_alur_esp32s2(dst, src1, src2, operation);
	}
	return 0;
}

INSTR_T esp32ulp_gen_alui(int dst, int src1, Expr_Node* addr, int operation)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_gen_alui_esp32(dst, src1, addr, operation);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_gen_alui_esp32s2(dst, src1, addr, operation);
	}
	return 0;
}

INSTR_T esp32ulp_gen_jump_r(int dst_reg, int cond)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_gen_jump_r_esp32(dst_reg, cond);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_gen_jump_r_esp32s2(dst_reg, cond);
	}
	return 0;
}

INSTR_T esp32ulp_gen_jump_i(Expr_Node* addr, int cond)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_gen_jump_i_esp32(addr, cond);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_gen_jump_i_esp32s2(addr, cond);
	}
	return 0;
}


// INSTR_T esp32ulp_gen_jump_relr(Expr_Node* addr, int judge, int thresh)
// {
// 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
// 	{
// 		return esp32ulp_gen_jump_relr_esp32(addr, judge, thresh);
// 	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
// 	{
// 		return 0;
// 	}
// 	return 0;
// }

INSTR_T esp32ulp_cmd_jump_relr(Expr_Node* step, Expr_Node* thresh, int cond)
{
	//DEBUG_TRACE("esp32ulp_cmd_jump_relr \n");
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_cmd_jump_relr_esp32(step, thresh, cond);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_cmd_jump_relr_esp32s2(step, thresh, cond);
	}
	return 0;
}

INSTR_T esp32ulp_cmd_jump_rels(Expr_Node* step, Expr_Node* thresh, int cond)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_cmd_jump_rels_esp32(step, thresh, cond);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_cmd_jump_rels_esp32s2(step, thresh, cond);
	}
	return 0;
}

INSTR_T esp32ulp_cmd_reg_rd(Expr_Node* addr, Expr_Node* high, Expr_Node* low)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_cmd_reg_rd_esp32(addr, high, low);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_cmd_reg_rd_esp32s2(addr, high, low);
	}
	return 0;
}

INSTR_T esp32ulp_cmd_reg_wr(Expr_Node* addr, Expr_Node* high, Expr_Node* low, Expr_Node* data)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_cmd_reg_wr_esp32(addr, high, low, data);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_cmd_reg_wr_esp32s2(addr, high, low, data);
	}
	return 0;
}

INSTR_T esp32ulp_cmd_i2c_rd(Expr_Node* i2c_addr, Expr_Node* high, Expr_Node* low, Expr_Node* i2c_sel)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_cmd_i2c_rd_esp32(i2c_addr, high, low, i2c_sel);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_cmd_i2c_rd_esp32s2(i2c_addr, high, low, i2c_sel);
	}
	return 0;
}

INSTR_T esp32ulp_cmd_i2c_wr(Expr_Node* i2c_addr, Expr_Node* high, Expr_Node* low, Expr_Node* i2c_sel, Expr_Node* data)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_cmd_i2c_wr_esp32(i2c_addr, high, low, i2c_sel, data);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_cmd_i2c_wr_esp32s2(i2c_addr, high, low, i2c_sel, data);
	}
	return 0;
}

INSTR_T esp32ulp_rd_mem_addr(int lh, int dst_reg, int src_reg, Expr_Node* addr)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_rd_mem_addr_esp32(lh, dst_reg, src_reg, addr);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_rd_mem_addr_esp32s2(lh, dst_reg, src_reg, addr);
	}
	return 0;
}

INSTR_T esp32ulp_cmd_halt()
{
	unsigned int local_op = OP_CMD_HALT();

	return GEN_OPCODE32_DYA(local_op);
}

INSTR_T esp32ulp_cmd_sleep(Expr_Node*  cycles)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_cmd_sleep_esp32(cycles);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32s2ulp_cmd_sleep_esp32s2(cycles);
	}
	return 0;
}

INSTR_T esp32ulp_cmd_wakeup(Expr_Node*  wake)
{
	(void)wake;
	//DEBUG_TRACE("dya_pass - OP_CMD_WAKEUP \n");
	//int wake_val = EXPR_VALUE(wake);
	//unsigned int local_op = OP_CMD_WAKEUP(wake_val);
	//return conscode(gencode(local_op), Expr_Node_Gen_Reloc(wake, BFD_RELOC_ESP32ULP_WAKE));
	unsigned int local_op = OP_CMD_WAKEUP(1);
	return GEN_OPCODE32_DYA(local_op);
}

INSTR_T esp32ulp_cmd_stage(int dir, Expr_Node* imm)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_cmd_stage_esp32(dir, imm);
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_cmd_stage_esp32s2(dir, imm);
	}
	return 0;
}

INSTR_T esp32ulp_cmd_stage_rst()
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return esp32ulp_cmd_stage_rst_esp32();
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_cmd_stage_rst_esp32s2();
	}
	return 0;
}

INSTR_T esp32ulp_cmd_wait(Expr_Node* cycles)
{
	int cycles_val = EXPR_VALUE(cycles);
	unsigned int local_op = OP_CMD_WAIT(cycles_val);
	int rel = BFD_RELOC_ESP32ULP_WAIT;
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		rel = BFD_RELOC_ESP32ULP_WAIT;
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		rel = BFD_RELOC_ESP32S2ULP_WAIT;
	}

	//DEBUG_TRACE("esp32ulp_cmd_wait - cycles=%i, local_op=%i, rel=%i\n", cycles_val, local_op, rel);
	return conscode(gencode(local_op), Expr_Node_Gen_Reloc(cycles, rel));
}

INSTR_T esp32ulp_cmd_tsens(int dreg, Expr_Node* delay)
{
	//DEBUG_TRACE("dya_pass - OP_CMD_TSENS \n");
	int delay_val = EXPR_VALUE(delay);
	unsigned int local_op = OP_CMD_TSENS(dreg, delay_val);
	int rel = BFD_RELOC_ESP32ULP_TSENS_DELAY;
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		rel = BFD_RELOC_ESP32ULP_TSENS_DELAY;
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		rel = BFD_RELOC_ESP32S2ULP_TSENS_DELAY;
	}
	return conscode(gencode(local_op), Expr_Node_Gen_Reloc(delay, rel));
}

INSTR_T esp32ulp_cmd_adc(int dreg, Expr_Node* sar_sel, Expr_Node* mux)
{
	//DEBUG_TRACE("dya_pass - OP_CMD_ADC \n");
	unsigned int sar_val = EXPR_VALUE(sar_sel);
	unsigned int mux_val = EXPR_VALUE(mux);
	unsigned int local_op = OP_CMD_ADC(dreg, mux_val, sar_val);

	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		return conscode(gencode(local_op),
						conctcode(Expr_Node_Gen_Reloc(mux, BFD_RELOC_ESP32ULP_ADC_MUX), Expr_Node_Gen_Reloc(sar_sel, BFD_RELOC_ESP32ULP_ADC_SEL)));
	}
	else if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return conscode(gencode(local_op),
						conctcode(Expr_Node_Gen_Reloc(mux, BFD_RELOC_ESP32S2ULP_ADC_MUX), Expr_Node_Gen_Reloc(sar_sel, BFD_RELOC_ESP32S2ULP_ADC_SEL)));
	}
	return NULL_CODE;
}

INSTR_T esp32ulp_wr_mem_stlh(int low_hi, int dst_reg, int src_reg, Expr_Node *offset)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		as_fatal(wrong_cpu_message);
		return 0;
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_wr_mem_stlh_esp32s2(low_hi, dst_reg, src_reg, offset);
	}
	return 0;
}

INSTR_T esp32ulp_wr_mem_st32(int dst_reg, int src_reg, Expr_Node *offset, Expr_Node *label)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		as_fatal(wrong_cpu_message);
		return 0;
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_wr_mem_st32_esp32s2(dst_reg, src_reg, offset, label);
	}
	return 0;
}

INSTR_T esp32ulp_wr_mem_sti32(int dst_reg, int src_reg, Expr_Node* label)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		as_fatal(wrong_cpu_message);
		return 0;
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_wr_mem_sti32_esp32s2(dst_reg, src_reg, label);
	}
	return 0;
}

INSTR_T esp32ulp_wr_mem_st_l(int low_hi, int dst_reg, int src_reg, Expr_Node* offset, Expr_Node* label)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		as_fatal(wrong_cpu_message);
		return 0;
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_wr_mem_st_l_esp32s2(low_hi, dst_reg, src_reg, offset, label);
	}
	return 0;
}

INSTR_T esp32ulp_wr_mem_sti_l(int dst_reg, int src_reg, Expr_Node* label)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		as_fatal(wrong_cpu_message);
		return 0;
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_wr_mem_sti_l_esp32s2(dst_reg, src_reg, label);
	}
	return 0;
}
INSTR_T esp32ulp_wr_mem_sti(int dst_reg, int src_reg)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		as_fatal(wrong_cpu_message);
		return 0;
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_wr_mem_sti_esp32s2(dst_reg, src_reg);
	}
	return 0;
}

INSTR_T esp32ulp_wr_mem_sto(Expr_Node* offset)
{
	if (ulp_cpu_type == CPU_TYPE_ESP32ULP)
	{
		as_fatal(wrong_cpu_message);
		return 0;
	} else 	if (ulp_cpu_type == CPU_TYPE_ESP32ULP_S2)
	{
		return esp32ulp_wr_mem_sto_esp32s2(offset);
	}
	return 0;
}

//#include "tc-esp32ulp_esp32.c"
//#include "tc-esp32ulp_esp32s2.c"
