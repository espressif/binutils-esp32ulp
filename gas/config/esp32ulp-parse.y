/* esp32ulp-parse.y  Espressif ESP32ULP Parser
   
   Copyright (c) 2016-2017 Espressif Systems (Shanghai) PTE LTD.

   based on Copyright (C) 2016-2017 Free Software Foundation, Inc.

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
%{

#include "as.h"

#include "esp32ulp-aux.h"  /* Opcode generating auxiliaries.  */
#include "elf/common.h"
#include "elf/esp32ulp.h"

#define IS_IMM(expr, bits)  value_match (expr, bits, 0, 1, 1)
#define IS_UIMM(expr, bits)  value_match (expr, bits, 0, 1, 0)
#define PROGCTRL(prgfunc, poprnd) \
	esp32ulp_gen_progctrl (prgfunc, poprnd)

#define ALU_ADDR(dst, src1, src2)  \
	esp32ulp_gen_alu_ADDR(dst.regno, src1.regno, src2.regno)

#define ALU_ADDI(dst, src1, imm)  \
	esp32ulp_gen_alu_ADDI(dst.regno, src1.regno, imm)

#define JUMP_R(dst, cond)  \
	esp32ulp_gen_jump_r(dst.regno, cond)
#define JUMP_I(dst, cond)  \
	esp32ulp_gen_jump_i(dst, cond)

#define JUMP_RELR(dst, judge, tresh)  \
	esp32ulp_gen_jump_relr(dst, judge, tresh)

#define CMD_JUMPR(step, tresh, cond)  \
	esp32ulp_cmd_jump_relr(step, tresh, cond)
#define CMD_JUMPS(step, tresh, cond)  \
	esp32ulp_cmd_jump_rels(step, tresh, cond)

#define MOVE_C2R(dst, data)  \
	esp32ulp_move_const2reg(dst.regno, data)
#define MOVE_E2R(dst, addr)  \
	esp32ulp_move_addr2reg(dst.regno, addr)

#define WR_MEM_ADDR(dst, src, addr)  \
	esp32ulp_wr_mem_addr(dst.regno,  src.regno, addr)
#define WR_MEM_OFFSET(dst, src, addr)  \
	esp32ulp_wr_mem_offset(dst.regno,  src.regno, addr)

#define RD_MEM_ADDR(dst, src, addr)  \
	esp32ulp_rd_mem_addr(dst.regno,  src.regno, addr)
#define RD_MEM_OFFSET(dst, src, addr)  \
	esp32ulp_rd_mem_offset(dst.regno,  src.regno, addr)

#define CMD_REG_RD(addr, high, low) \
	esp32ulp_cmd_reg_rd(addr, high, low)
#define CMD_REG_WR(addr, high, low, data) \
	esp32ulp_cmd_reg_wr(addr, high, low, data)

#define CMD_I2C_RD(i2c_addr, high, low, i2c_sel) \
	esp32ulp_cmd_i2c_rd(i2c_addr, high, low, i2c_sel)
#define CMD_I2C_WR(i2c_addr, high, low, i2c_sel, data) \
	esp32ulp_cmd_i2c_wr(i2c_addr, high, low, i2c_sel, data)



#define CMD_HALT()  \
	 esp32ulp_cmd_halt()
#define CMD_WAKEUP(wake)  \
	 esp32ulp_cmd_wakeup(wake)
#define CMD_SLEEP(cycles)  \
	 esp32ulp_cmd_sleep(cycles)
#define CMD_WAIT(cycles)  \
	 esp32ulp_cmd_wait(cycles)

#define CMD_TSENS(dreg, delay)  \
	esp32ulp_cmd_tsens(dreg.regno, delay)

#define CMD_ADC(dreg, sar_sel, mux)  \
	esp32ulp_cmd_adc(dreg.regno, sar_sel, mux)

#define CMD_ALUR(dst, src1, src2, operation)  \
	 esp32ulp_gen_alu_r(dst.regno,src1.regno,src2.regno, operation)
#define CMD_ALUI(dst, src1, imm, operation)  \
	 esp32ulp_gen_alu_i(dst.regno, src1.regno, imm, operation)
#define CMD_ALUI_DIR(dst, imm, operation)  \
	 esp32ulp_gen_alu_i(dst.regno, 0, imm, operation)

#define CMD_STAGE(dir, imm)  \
		esp32ulp_cmd_stage(dir, imm)
#define CMD_STAGE_RST()  \
		esp32ulp_cmd_stage_rst()

static int value_match (Expr_Node *, int, int, int, int);

extern FILE *errorf;
extern INSTR_T insn;

static Expr_Node *binary (Expr_Op_Type, Expr_Node *, Expr_Node *);
static Expr_Node *unary  (Expr_Op_Type, Expr_Node *);

static void notethat (const char *, ...);

extern char *yytext;
int yyerror (const char *);

/* Used to set SRCx fields to all 1s as described in the PRM.  */

void error (const char *format, ...)
{
    va_list ap;
    static char buffer[2000];

    va_start (ap, format);
    vsprintf (buffer, format, ap);
    va_end (ap);

    as_bad ("%s", buffer);
}

int
yyerror (const char *msg)
{
  if (msg[0] == '\0')
    error ("%s", msg);

  else if (yytext[0] != ';')
    error ("%s. Input text was %s.", msg, yytext);
  else
    error ("%s.", msg);

  return -1;
}

extern int yylex (void);
/* Return true if a value is inside a range.  */
#define IN_RANGE(x, low, high) \
  (((EXPR_VALUE(x)) >= (low)) && (EXPR_VALUE(x)) <= ((high)))


%}

%union {
  INSTR_T instr;
  Expr_Node *expr;
  SYMBOL_T symbol;
  long value;
  Register reg;
  Macfunc macfunc;
  struct { int r0; int s0; int x0; int aop; } modcodes;
  struct { int r0; } r0;
}


/* Registers.  */
%token REG

/* Progctrl.  */
%token NOP
%token JUMP JUMPR JUMPS

/* DYA tokens*/
%token ADD
%token SUB
%token LSH
%token RSH
%token AND
%token OR
%token MOVE

%token ST
%token LD
%token HALT
%token WAKE
%token SLEEP
%token TSENS
%token WAIT
%token ADC
%token STAGE_INC
%token STAGE_DEC
%token STAGE_RST
%token REG_RD
%token REG_WR
%token I2C_RD
%token I2C_WR

/* Emulator only.  */
%token ABORT

/* Operators.  */
%token NOT TILDA BANG
%token AMPERSAND BAR
%token PERCENT
%token CARET
%token BXOR

%token MINUS PLUS STAR SLASH
%token NEG

/* Shift/rotate ops.  */
%token SHIFT LSHIFT ASHIFT BXORSHIFT
%token ROT
%token LESS_LESS GREATER_GREATER
%token _GREATER_GREATER_GREATER
%token DIVS DIVQ

/* In place operators.  */
%token GE LT LE GT OV EQ
%token LESS_THAN

/* Cache.  */
%token FLUSHINV FLUSH
%token IFLUSH PREFETCH

/* Misc.  */
%token PRNT
%token OUTC
%token WHATREG
%token TESTSET

/* Modifiers.  */
%token ASL ASR
%token B W
%token NS S CO SCO
%token TH TL
%token BP
%token BREV
%token X Z
%token R RND RNDL RNDH RND12 RND20
%token V
%token LO HI

/* Bit ops.  */
%token BITTGL BITCLR BITSET BITTST BITMUX


/* Semantic auxiliaries.  */

%token COMMA
%token COLON SEMICOLON
%token RPAREN LPAREN LBRACK RBRACK
%token SYMBOL NUMBER
%token GOT GOT17M4 FUNCDESC_GOT17M4
%token AT

/* Types.  */
%type <instr> asm

%type <value> NUMBER
%type <expr> expr_1
%type <instr> asm_1

%type <r0> searchmod
%type <r0> branchmod
%type <r0> branchrel
%type <r0> branchrels

%type <expr> symbol
%type <symbol> SYMBOL
%type <expr> eterm
%type <reg> REG
%type <expr> expr
%type <expr> got
%type <expr> got_or_expr
%type <value> any_gotrel GOT GOT17M4 FUNCDESC_GOT17M4

/* Precedence rules.  */
%left BAR
%left CARET
%left AMPERSAND
%left LESS_LESS GREATER_GREATER
%left PLUS MINUS
%left STAR SLASH PERCENT


%right TILDA BANG
%start statement
%%
statement:
	| asm
	{
	  insn = $1;
	  if (insn == (INSTR_T) 0)
	    return NO_INSN_GENERATED;
	  else if (insn == (INSTR_T) - 1)
	    return SEMANTIC_ERROR;
	  else
	    return INSN_GENERATED;
	}
	;

asm: asm_1 SEMICOLON
	/* Parallel instructions.  */
	| error
	{
	$$ = 0;
	yyerror ("");
	yyerrok;
	}
	;

/* DSPMAC.  */

asm_1:

/* DYA operation  */
/*ALU operations*/

	| ADD REG COMMA REG COMMA REG
	{
		if (IS_DREG ($2) && IS_DREG ($4) && IS_DREG ($6))
		{
			notethat ("ADD : dest_reg = src_reg1 + src_reg2\n");
			$$ =  CMD_ALUR ($2, $4, $6, ALU_SEL_ADD);
		}
		else
			return yyerror ("Register mismatch");
	}
	| ADD REG COMMA REG COMMA expr
	{
		notethat ("ADD : dest_reg = src_reg1 + imm\n");
		if (IS_DREG ($2) && IS_DREG ($4))
		{
			notethat ("ADD : dest_reg = src_reg1 + imm\n");
			$$ =  CMD_ALUI ($2, $4, $6, ALU_SEL_ADD);
		}
		else
			return yyerror ("Register mismatch");
	}
	| SUB REG COMMA REG COMMA REG
	{
		if (IS_DREG ($2) && IS_DREG ($4) && IS_DREG ($6))
		{
			notethat ("SUB : dest_reg = src_reg1 + src_reg2\n");
			$$ =  CMD_ALUR ($2, $4, $6, ALU_SEL_SUB);
		}
		else
			return yyerror ("Register mismatch");
	}
	| SUB REG COMMA REG COMMA expr
	{
		if (IS_DREG ($2) && IS_DREG ($4))
		{
			notethat ("SUM : dest_reg = src_reg1 + imm\n");
			$$ =  CMD_ALUI ($2, $4, $6, ALU_SEL_SUB);
		}
		else
			return yyerror ("Register mismatch");
	}
	| AND REG COMMA REG COMMA REG
	{
		if (IS_DREG ($2) && IS_DREG ($4) && IS_DREG ($6))
		{
			notethat ("AND : dest_reg = src_reg1 + src_reg2\n");
			$$ =  CMD_ALUR ($2, $4, $6, ALU_SEL_AND);
		}
		else
			return yyerror ("Register mismatch");
	}
	| AND REG COMMA REG COMMA expr
	{
		if (IS_DREG ($2) && IS_DREG ($4))
		{
			notethat ("AND : dest_reg = src_reg1 + imm\n");
			$$ =  CMD_ALUI ($2, $4, $6, ALU_SEL_AND);
		}
		else
			return yyerror ("Register mismatch");
	}
	| OR REG COMMA REG COMMA REG
	{
		if (IS_DREG ($2) && IS_DREG ($4) && IS_DREG ($6))
		{
			notethat ("OR : dest_reg = src_reg1 | src_reg2\n");
			$$ =  CMD_ALUR ($2, $4, $6, ALU_SEL_OR);
		}
		else
			return yyerror ("Register mismatch");
	}
	| OR REG COMMA REG COMMA expr
	{
		if (IS_DREG ($2) && IS_DREG ($4))
		{
			notethat ("OR : dest_reg = src_reg1 | imm\n");
			$$ =  CMD_ALUI ($2, $4, $6, ALU_SEL_OR);
		}
		else
			return yyerror ("Register mismatch");
	}	
	| RSH REG COMMA REG COMMA REG
	{
		if (IS_DREG ($2) && IS_DREG ($4) && IS_DREG ($6))
		{
			notethat ("RSH : dest_reg = src_reg1 >> src_reg2\n");
			$$ =  CMD_ALUR ($2, $4, $6, ALU_SEL_RSH);
		}
		else
			return yyerror ("Register mismatch");
	}
	| RSH REG COMMA REG COMMA expr
	{
		if (IS_DREG ($2) && IS_DREG ($4))
		{
			notethat ("RSH : dest_reg = src_reg1 >> imm\n");
			$$ =  CMD_ALUI ($2, $4, $6, ALU_SEL_RSH);
		}
		else
			return yyerror ("Register mismatch");
	}	
	| LSH REG COMMA REG COMMA REG
	{
		if (IS_DREG ($2) && IS_DREG ($4) && IS_DREG ($6))
		{
			notethat ("LSH : dest_reg = src_reg1 << src_reg2\n");
			$$ =  CMD_ALUR ($2, $4, $6, ALU_SEL_LSH);
		}
		else
			return yyerror ("Register mismatch");
	}
	| LSH REG COMMA REG COMMA expr
	{
		if (IS_DREG ($2) && IS_DREG ($4))
		{
			notethat ("LSH : dest_reg = src_reg1 << imm\n");
			$$ =  CMD_ALUI ($2, $4, $6, ALU_SEL_LSH);
		}
		else
			return yyerror ("Register mismatch");
	}	
	| MOVE REG COMMA REG
	{
		if (IS_DREG ($2) && IS_DREG ($4))
		{
			notethat ("LSH : dest_reg = src_reg1\n");
			$$ =  CMD_ALUR ($2, $4, $4, ALU_SEL_MOV);
		}
		else
			return yyerror ("Register mismatch");
	}
	| MOVE REG COMMA expr
	{
		if (IS_DREG ($2))
		{
			notethat ("MOVE : dest_reg = imm\n");
			$$ =  CMD_ALUI_DIR ($2, $4, ALU_SEL_MOV);
		}
		else
			return yyerror ("Register mismatch");
	}	
	| STAGE_RST 
	{
		notethat ("STAGE_RST \n");
		$$ =  CMD_STAGE_RST();
	}	
	| STAGE_INC expr
	{
		notethat ("STAGE_INC \n");
		$$ =  CMD_STAGE(ALU_SEL_SINC, $2);
	}	
	| STAGE_DEC expr
	{
		notethat ("STAGE_DEC \n");
		$$ =  CMD_STAGE(ALU_SEL_SDEC, $2);
	}	
	
	
/*=================================================================================================*/
	| JUMP REG
	{
		if (IS_DREG ($2))
		{
			notethat ("JUMP_R : dest_addr, condition\n");
			$$ =  JUMP_R ($2, 0);
		}
		else
		{
			return yyerror ("Register mismatch");
		}
	}
	| JUMP REG COMMA branchmod
	{
		if (IS_DREG ($2))
		{
			notethat ("JUMP_R : dest_addr, condition\n");
			$$ =  JUMP_R ($2, $4.r0);
		}
		else
		{
			return yyerror ("Register mismatch");
		}
	}
	| JUMP expr
	{
		if (IS_IMM ($2, 11))
		{
			notethat ("JUMP_I : dest_addr, condition\n");
			$$ =  JUMP_I ($2, 0);
		}
		else
		{
			return yyerror ("Register mismatch");
		}
	}
	| JUMP expr COMMA branchmod
	{
		if (IS_IMM ($2, 11))
		{
			notethat ("JUMP   : dest_addr, condition\n");
			$$ =  JUMP_I ($2, $4.r0);
		}
		else
		{
			return yyerror ("Register mismatch");
		}
	}
	| JUMPR expr COMMA expr COMMA branchrel
	{
		if (IS_IMM ($2, 7) && IS_IMM ($4, 16))
		{
			notethat ("JUMPR : rel_addr, threshold, condition\n");
			$$ =  CMD_JUMPR($2, $4, $6.r0);
		}
		else
		{
			return yyerror ("Jump adress is to far");
		}
	}
	| JUMPS expr COMMA expr COMMA branchrels
	{
		if (IS_IMM ($2, 7) && IS_IMM ($4, 16))
		{
			notethat ("JUMPS : rel_addr, threshold, condition\n");
			$$ =  CMD_JUMPS($2, $4, $6.r0);
		}
		else
		{
			return yyerror ("Jump adress is to far");
		}
	}
	| ST REG COMMA REG COMMA expr
	{
		if (IS_UIMM ($6, 11) && IS_DREG ($2) && IS_DREG ($4))
		{
			notethat ("ST : Rdst, Rsrs, offset, MEM[Rsrc + offset] =  Rdst\n");
			$$ =  WR_MEM_ADDR($2, $4, $6);
		}
		else
		{
			return yyerror ("Register mismatch");
		}
	}
	| LD REG COMMA REG COMMA expr
	{
		if (IS_UIMM ($6, 11) && IS_DREG ($2) && IS_DREG ($4))
		{
			notethat ("LD : Rdst, Rsrs, offset, Rdst = MEM[Rsrc + offset]\n");
			$$ =  RD_MEM_ADDR($2, $4, $6);
		}
		else
		{
			return yyerror ("Register mismatch");
		}
	}
	| NOP
	{
	  notethat ("ProgCtrl: NOP\n");
	  $$ = PROGCTRL (0, 0);
	}	
	| HALT
	{
	  notethat ("ProgCtrl: END\n");
	  $$ = CMD_HALT ();
	}	
	| WAKE 
	{
	  notethat ("ProgCtrl: CMD_WAKEUP\n");
	  $$ = CMD_WAKEUP(NULL);
	}	
	| SLEEP expr
	{
	  notethat ("ProgCtrl: SLEEP\n");
	  $$ = CMD_SLEEP($2);
	}	
	| TSENS REG COMMA expr
	{
		if (IS_DREG ($2))
		{
	        notethat ("ProgCtrl: TSENS\n");
			$$ =  CMD_TSENS($2, $4);
		}
		else
		{
			return yyerror ("Register mismatch");
		}
	}	
	| ADC REG COMMA expr COMMA expr COMMA expr 
	{
		if (IS_DREG ($2))
		{
	        notethat ("ProgCtrl: ADC \n");
			$$ =  CMD_ADC($2, $4, $6);
		}
		else
		{
			return yyerror ("Register mismatch");
		}
	}	
	| WAIT expr
	{
	  notethat ("ProgCtrl: WAIT  count\n");
	  $$ = CMD_WAIT($2);
	}	
	| REG_RD expr COMMA expr COMMA expr
	{
		notethat ("ProgCtrl: REG_RD addr, high, low, \n");
	    $$ = CMD_REG_RD($2, $4, $6);
	}
	| REG_WR expr COMMA expr COMMA expr COMMA expr
	{
		notethat ("ProgCtrl: REG_WR  addr, high, low, value\n");
	    $$ = CMD_REG_WR($2, $4, $6, $8);
	}
	| I2C_RD expr COMMA expr COMMA expr COMMA expr
	{
		notethat ("ProgCtrl: I2C_RD - i2c_addr, high, low, i2c_sel\n");
	    $$ = CMD_I2C_RD($2, $4, $6, $8);
	}
	| I2C_WR expr COMMA expr COMMA expr COMMA expr COMMA expr
	{
		notethat ("ProgCtrl: I2C_RD - i2c_addr, value, high, low, i2c_sel\n");
	    $$ = CMD_I2C_WR($2, $6, $8, $10, $4);
	}
	/* --------------------------------------------------------------------------------------------------*/
;

/*  AUX RULES.  */
branchmod:
    EQ
	{
	$$.r0 = 1;
	}
	| OV
	{
	$$.r0 = 2;
	}
	;

branchrel:
    GE
	{
	$$.r0 = 1;
	}
	| LT
	{
	$$.r0 = 0;
	}
	;

branchrels:
    EQ
	{
	$$.r0 = 2;
	}
	|
    GT
	{
	$$.r0 = 1;
	}
	| LT
	{
	$$.r0 = 0;
	}
	;

searchmod:
    GE
	{
	$$.r0 = 1;
	}
	| GT
	{
	$$.r0 = 2;
	}
	| LE
	{
	$$.r0 = 3;
	}
	| LT
	{
	$$.r0 = 4;
	}
	| OV
	{
	$$.r0 = 5;
	}
	| EQ
	{
	$$.r0 = 6;
	}
	;


/* Expressions and Symbols.  */

symbol: SYMBOL
	{
	Expr_Node_Value val;
	val.s_value = S_GET_NAME($1);
	$$ = Expr_Node_Create (Expr_Node_Reloc, val, NULL, NULL);
	}
	;

any_gotrel:
	GOT
	{ $$ = BFD_RELOC_ESP32ULP_GOT; }
	| GOT17M4
	{ $$ = BFD_RELOC_ESP32ULP_GOT17M4; }
	| FUNCDESC_GOT17M4
	{ $$ = BFD_RELOC_ESP32ULP_FUNCDESC_GOT17M4; }
	;

got:	symbol AT any_gotrel
	{
	Expr_Node_Value val;
	val.i_value = $3;
	$$ = Expr_Node_Create (Expr_Node_GOT_Reloc, val, $1, NULL);
	}
	;

got_or_expr:	got
	{
	$$ = $1;
	}
	| expr
	{
	$$ = $1;
	}
	;

eterm: NUMBER
	{
	Expr_Node_Value val;
	val.i_value = $1;
	$$ = Expr_Node_Create (Expr_Node_Constant, val, NULL, NULL);
	}
	| symbol
	{
	$$ = $1;
	}
	| LPAREN expr_1 RPAREN
	{
	$$ = $2;
	}
	| TILDA expr_1
	{
	$$ = unary (Expr_Op_Type_COMP, $2);
	}
	| MINUS expr_1 %prec TILDA
	{
	$$ = unary (Expr_Op_Type_NEG, $2);
	}
	;

expr: expr_1
	{
	$$ = $1;
	}
	;

expr_1: expr_1 STAR expr_1
	{
	$$ = binary (Expr_Op_Type_Mult, $1, $3);
	}
	| expr_1 SLASH expr_1
	{
	$$ = binary (Expr_Op_Type_Div, $1, $3);
	}
	| expr_1 PERCENT expr_1
	{
	$$ = binary (Expr_Op_Type_Mod, $1, $3);
	}
	| expr_1 PLUS expr_1
	{
	$$ = binary (Expr_Op_Type_Add, $1, $3);
	}
	| expr_1 MINUS expr_1
	{
	$$ = binary (Expr_Op_Type_Sub, $1, $3);
	}
	| expr_1 LESS_LESS expr_1
	{
	$$ = binary (Expr_Op_Type_Lshift, $1, $3);
	}
	| expr_1 GREATER_GREATER expr_1
	{
	$$ = binary (Expr_Op_Type_Rshift, $1, $3);
	}
	| expr_1 AMPERSAND expr_1
	{
	$$ = binary (Expr_Op_Type_BAND, $1, $3);
	}
	| expr_1 CARET expr_1
	{
	$$ = binary (Expr_Op_Type_LOR, $1, $3);
	}
	| expr_1 BAR expr_1
	{
	$$ = binary (Expr_Op_Type_BOR, $1, $3);
	}
	| eterm
	{
	$$ = $1;
	}
	;


%%

EXPR_T mkexpr(int x, SYMBOL_T s)
{
	EXPR_T e = XNEW(struct expression_cell);
	e->value = x;
	EXPR_SYMBOL(e) = s;
	return e;
}

static int value_match(Expr_Node *exp, int sz, int sign, int mul, int issigned)
{
	int umax = (1 << sz) - 1;
	int min = -(1 << (sz - 1));
	int max = (1 << (sz - 1)) - 1;

	int v = (EXPR_VALUE(exp)) & 0xffffffff;

	if ((v % mul) != 0)
	{
		error("%s:%d: Value Error -- Must align to %d\n", __FILE__, __LINE__, mul);
		return 0;
	}

	v /= mul;

	if (sign)
		v = -v;

	if (issigned)
	{
		if (v >= min && v <= max) return 1;

#ifdef DEBUG
		fprintf(stderr, "signed value %lx out of range\n", v * mul);
#endif
		return 0;
	}
	if (v <= umax && v >= 0)
		return 1;
#ifdef DEBUG
	fprintf(stderr, "unsigned value %lx out of range\n", v * mul);
#endif
	return 0;
}

/* Return the expression structure that allows symbol operations.
If the left and right children are constants, do the operation.  */
static Expr_Node* binary(Expr_Op_Type op, Expr_Node *x, Expr_Node *y)
{
	Expr_Node_Value val;

	if (x->type == Expr_Node_Constant && y->type == Expr_Node_Constant)
	{
		switch (op)
		{
		case Expr_Op_Type_Add:
			x->value.i_value += y->value.i_value;
			break;
		case Expr_Op_Type_Sub:
			x->value.i_value -= y->value.i_value;
			break;
		case Expr_Op_Type_Mult:
			x->value.i_value *= y->value.i_value;
			break;
		case Expr_Op_Type_Div:
			if (y->value.i_value == 0)
				error("Illegal Expression:  Division by zero.");
			else
				x->value.i_value /= y->value.i_value;
			break;
		case Expr_Op_Type_Mod:
			x->value.i_value %= y->value.i_value;
			break;
		case Expr_Op_Type_Lshift:
			x->value.i_value <<= y->value.i_value;
			break;
		case Expr_Op_Type_Rshift:
			x->value.i_value >>= y->value.i_value;
			break;
		case Expr_Op_Type_BAND:
			x->value.i_value &= y->value.i_value;
			break;
		case Expr_Op_Type_BOR:
			x->value.i_value |= y->value.i_value;
			break;
		case Expr_Op_Type_BXOR:
			x->value.i_value ^= y->value.i_value;
			break;
		case Expr_Op_Type_LAND:
			x->value.i_value = x->value.i_value && y->value.i_value;
			break;
		case Expr_Op_Type_LOR:
			x->value.i_value = x->value.i_value || y->value.i_value;
			break;

		default:
			error("%s:%d: Internal assembler error\n", __FILE__, __LINE__);
		}
		return x;
	}
	/* Canonicalize order to EXPR OP CONSTANT.  */
	if (x->type == Expr_Node_Constant)
	{
		Expr_Node *t = x;
		x = y;
		y = t;
	}
	/* Canonicalize subtraction of const to addition of negated const.  */
	if (op == Expr_Op_Type_Sub && y->type == Expr_Node_Constant)
	{
		op = Expr_Op_Type_Add;
		y->value.i_value = -y->value.i_value;
	}
	if (y->type == Expr_Node_Constant && x->type == Expr_Node_Binop
		&& x->Right_Child->type == Expr_Node_Constant)
	{
		if (op == x->value.op_value && x->value.op_value == Expr_Op_Type_Add)
		{
			x->Right_Child->value.i_value += y->value.i_value;
			return x;
		}
	}

	/* Create a new expression structure.  */
	val.op_value = op;
	return Expr_Node_Create(Expr_Node_Binop, val, x, y);
}

static Expr_Node* unary(Expr_Op_Type op, Expr_Node *x)
{
	if (x->type == Expr_Node_Constant)
	{
		switch (op)
		{
		case Expr_Op_Type_NEG:
			x->value.i_value = -x->value.i_value;
			break;
		case Expr_Op_Type_COMP:
			x->value.i_value = ~x->value.i_value;
			break;
		default:
			error("%s:%d: Internal assembler error\n", __FILE__, __LINE__);
		}
		return x;
	}
	else
	{
		/* Create a new expression structure.  */
		Expr_Node_Value val;
		val.op_value = op;
		return Expr_Node_Create(Expr_Node_Unop, val, x, NULL);
	}
}

int debug_codeselection = 0;
static void
notethat(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	if (debug_codeselection)
	{
		vfprintf(errorf, format, ap);
	}
	va_end(ap);
}

#ifdef TEST
main(int argc, char **argv)
{
	yyparse();
}
#endif

