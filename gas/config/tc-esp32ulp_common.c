

extern int yyparse(void);
struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *yy_str);
extern void yy_delete_buffer(YY_BUFFER_STATE b);
static parse_state parse(char *line);
static long check_reg_range(fixS* fixP, long value);

int last_insn_size;

extern struct obstack mempool;
FILE *errorf;

/* Flags to set in the elf header */
#define DEFAULT_FLAGS 0

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



/* Define esp32ulp-specific command-line options (there are none). */
const char *md_shortopts = "";

struct option md_longopts[] =
{
	{ NULL, no_argument, NULL, 0 },
};

size_t md_longopts_size = sizeof(md_longopts);


int
md_parse_option(int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
	switch (c)
	{
	default:
		return 0;
	}

	return 1;
}

void
md_show_usage(FILE * stream)
{
	fprintf(stream, _(" ESP32 ULP processor has no specific assembler options:\n"));
}



/* Perform the main parsing, and assembly of the input here.  Also,
call the required routines for alignment and fixups here.
This is called for every line that contains real assembly code.  */
//#include "../../gdb/symtab.h"
void
md_assemble(char *line)
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
		//DEBUG_TRACE(" reloc : value = %08x, pc=%08x, reloc=%08x BFD_RELOC_ESP32ULP_PLTPC = %08x\n", (unsigned int)insn->value, (unsigned int)insn->pcrel, (unsigned int)insn->reloc, BFD_RELOC_ESP32S2ULP_PLTPC);
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

static parse_state
parse(char *line)
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

void
md_operand(expressionS * expressionP)
{
	if (*input_line_pointer == '[')
	{
		as_tsktsk("We found a '['!");
		input_line_pointer++;
		expression(expressionP);
	}
}

/* Handle undefined symbols. */
symbolS *
md_undefined_symbol(char *name ATTRIBUTE_UNUSED)
{
	return (symbolS *)0;
}

int
md_estimate_size_before_relax(fragS * fragP ATTRIBUTE_UNUSED,
segT segment ATTRIBUTE_UNUSED)
{
	return 0;
}

static long 
check_reg_range(fixS* fixP, long value)
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



/* Round up a section size to the appropriate boundary.  */
valueT
md_section_align(segT segment, valueT size)
{
	int boundary = bfd_get_section_alignment(stdoutput, segment);
	return ((size + (1 << boundary) - 1) & -(1 << boundary));
}


const char *
md_atof(int type, char * litP, int * sizeP)
{
	return ieee_md_atof(type, litP, sizeP, FALSE);
}


/* If while processing a fixup, a reloc really needs to be created
then it is done here.  */

arelent *
tc_gen_reloc(asection *seg ATTRIBUTE_UNUSED, fixS *fixp)
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

long
md_pcrel_from_section(fixS *fixP, segT sec)
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

INSTR_T
note_reloc(INSTR_T code, Expr_Node * symbol, int reloc, int pcrel)
{
	/* Assert that the symbol is not an operator.  */
	gas_assert(symbol->type == Expr_Node_Reloc);

	return note_reloc1(code, symbol->value.s_value, reloc, pcrel);

}

INSTR_T
note_reloc1(INSTR_T code, const char *symbol, int reloc, int pcrel)
{
	code->reloc = reloc;
	code->exp = mkexpr(0, symbol_find_or_make(symbol));
	code->pcrel = pcrel;
	return code;
}

INSTR_T
note_reloc2(INSTR_T code, const char *symbol, int reloc, int value, int pcrel)
{
	code->reloc = reloc;
	code->exp = mkexpr(value, symbol_find_or_make(symbol));
	code->pcrel = pcrel;
	return code;
}

/* Special extra functions that help esp32ulp-parse.y perform its job.  */

struct obstack mempool;

INSTR_T
conscode(INSTR_T head, INSTR_T tail)
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

int reloc;
int ninsns;
int count_insns;

static void *
allocate(size_t n)
{
	return obstack_alloc(&mempool, n);
}

Expr_Node *
Expr_Node_Create(Expr_Node_Type type,
Expr_Node_Value value,
Expr_Node *Left_Child,
Expr_Node *Right_Child)
{


	Expr_Node *node = (Expr_Node *)allocate(sizeof(Expr_Node));
	node->type = type;
	node->value = value;
	node->Left_Child = Left_Child;
	node->Right_Child = Right_Child;
	return node;
}

static const char *con = ".__constant";
static const char *op = ".__operator";
static INSTR_T Expr_Node_Gen_Reloc_R(Expr_Node * head);
INSTR_T Expr_Node_Gen_Reloc(Expr_Node *head, int parent_reloc);

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

// Conditions for JUMPS instructions
#define JUMPS_EQ 3 
#define JUMPS_GT 4
#define JUMPS_LE 2
#define JUMPS_LT 0
#define JUMPS_GE 1

// Step to reach next instruction
#define NEXT_INSTRUCTION_STEP  4
