/* esp32ulp-defs.h    
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

#ifndef ESP32ULP_PARSE_H
#define ESP32ULP_PARSE_H

#include "opcode/esp32ulp.h"

#define PCREL	1
#define CODE_FRAG_SIZE 4096  /* 1 page.  */



/* High-Nibble: group code, low nibble: register code.  */


#define T_REG_R       0x00
#define T_REG_P       0x10
#define T_REG_I       0x20
#define T_REG_B       0x30
#define T_REG_L       0x34
#define T_REG_M       0x24
#define T_REG_A       0x40

/* All registers above this value don't
   belong to a usuable register group.  */
#define T_NOGROUP     0xa0

/* Flags.  */
#define F_REG_NONE 0
#define F_REG_HIGH 1
#define F_REG_LOW 2

enum machine_registers
{
	REG_R0 = T_REG_R, REG_R1, REG_R2, REG_R3, REG_R4, REG_R5, REG_R6, REG_R7, REG_R8,
};

/* Register type checking macros.  */

#define CLASS_MASK 0xf0

/* register type checking.  */
#define _TYPECHECK(r, x) (((r).regno & CLASS_MASK) == T_REG_##x)

#define IS_DREG(r)       _TYPECHECK(r, R)

typedef enum
{
	ones_compl,
	twos_compl,
	mult,
	divide,
	mod,
	add,
	sub,
	lsh,
	rsh,
	logand,
	logior,
	logxor
} expr_opcodes_t;

struct expressionS;

#define SYMBOL_T       symbolS*

struct expression_cell
{
	int value;
	SYMBOL_T symbol;
};

/* User Type Definitions.  */
struct esp32ulp_insn
{
	unsigned long value;
	struct esp32ulp_insn *next;
	struct expression_cell *exp;
	int pcrel;
	int reloc;
};

#define INSTR_T struct esp32ulp_insn*
#define EXPR_T  struct expression_cell*

typedef struct expr_node_struct Expr_Node;

extern INSTR_T gencode(unsigned long x);
extern INSTR_T conscode(INSTR_T head, INSTR_T tail);
extern INSTR_T conctcode(INSTR_T head, INSTR_T tail);
extern INSTR_T note_reloc
(INSTR_T code, Expr_Node *, int reloc, int pcrel);
extern INSTR_T note_reloc1
(INSTR_T code, const char * sym, int reloc, int pcrel);
extern INSTR_T note_reloc2
(INSTR_T code, const char *symbol, int reloc, int value, int pcrel);

/* Types of expressions.  */
typedef enum
{
	Expr_Node_Binop,		/* Binary operator.  */
	Expr_Node_Unop,		/* Unary operator.  */
	Expr_Node_Reloc,		/* Symbol to be relocated.  */
	Expr_Node_GOT_Reloc,		/* Symbol to be relocated using the GOT.  */
	Expr_Node_Constant 		/* Constant.  */
} Expr_Node_Type;

/* Types of operators.  */
typedef enum
{
	Expr_Op_Type_Add,
	Expr_Op_Type_Sub,
	Expr_Op_Type_Mult,
	Expr_Op_Type_Div,
	Expr_Op_Type_Mod,
	Expr_Op_Type_Lshift,
	Expr_Op_Type_Rshift,
	Expr_Op_Type_BAND,		/* Bitwise AND.  */
	Expr_Op_Type_BOR,		/* Bitwise OR.  */
	Expr_Op_Type_BXOR,		/* Bitwise exclusive OR.  */
	Expr_Op_Type_LAND,		/* Logical AND.  */
	Expr_Op_Type_LOR,		/* Logical OR.  */
	Expr_Op_Type_NEG,
	Expr_Op_Type_COMP		/* Complement.  */
} Expr_Op_Type;

/* The value that can be stored ... depends on type.  */
typedef union
{
	const char *s_value;		/* if relocation symbol, the text.  */
	long long i_value;		/* if constant, the value.  */
	Expr_Op_Type op_value;	/* if operator, the value.  */
} Expr_Node_Value;

/* The expression node.  */
struct expr_node_struct
{
	Expr_Node_Type 	type;
	Expr_Node_Value	value;
	Expr_Node		*Left_Child;
	Expr_Node		*Right_Child;
};


/* Operations on the expression node.  */
Expr_Node *Expr_Node_Create(Expr_Node_Type type,
	Expr_Node_Value value,
	Expr_Node *Left_Child,
	Expr_Node *Right_Child);

/* Generate the reloc structure as a series of instructions.  */
INSTR_T Expr_Node_Gen_Reloc(Expr_Node *head, int parent_reloc);

#define MKREF(x)	mkexpr (0,x)

#define NULL_CODE ((INSTR_T) 0)

#ifndef EXPR_VALUE
#define EXPR_VALUE(x)  (((x)->type == Expr_Node_Constant) ? ((x)->value.i_value) : 0)
#endif
#ifndef EXPR_SYMBOL
#define EXPR_SYMBOL(x) ((x)->symbol)
#endif


typedef long reg_t;


typedef struct _register
{
	reg_t regno;       /* Register ID as defined in machine_registers.  */
	int   flags;
} Register;


typedef struct _macfunc
{
	char n;
	char op;
	char w;
	char P;
	Register dst;
	Register s0;
	Register s1;
} Macfunc;

typedef enum
{
	SEMANTIC_ERROR,
	NO_INSN_GENERATED,
	INSN_GENERATED
} parse_state;


#ifdef __cplusplus
extern "C" {
#endif

	extern int debug_codeselection;

	void error(const char *format, ...);
	void warn(char *format, ...);
	int  semantic_error(char *syntax);
	void semantic_error_2(char *syntax);

	EXPR_T mkexpr(int, SYMBOL_T);

	/* Defined in esp32ulp-lex.l.  */
	void set_start_state(void);

	extern int insn_regmask(int, int);
#ifdef __cplusplus
}
#endif

#endif  /* ESP32ULP_PARSE_H */

