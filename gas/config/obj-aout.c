/* a.out object file format
   Copyright (C) 1989, 1990, 1991 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 1,
or (at your option) any later version.

GAS is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with GAS; see the file COPYING.  If not, write
to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

/* $Id$ */

#include "as.h"

#include "obstack.h"

 /* in: segT   out: N_TYPE bits */
const short seg_N_TYPE[] = {
  N_ABS,
  N_TEXT,
  N_DATA,
  N_BSS,
  N_UNDF, /* unknown */
  N_UNDF, /* absent */
  N_UNDF, /* pass1 */
  N_UNDF, /* error */
  N_UNDF, /* bignum/flonum */
  N_UNDF, /* difference */
  N_UNDF, /* debug */
  N_UNDF, /* ntv */
  N_UNDF, /* ptv */
  N_REGISTER, /* register */
};

const segT N_TYPE_seg [N_TYPE+2] = {	/* N_TYPE == 0x1E = 32-2 */
	SEG_UNKNOWN,			/* N_UNDF == 0 */
	SEG_GOOF,
	SEG_ABSOLUTE,			/* N_ABS == 2 */
	SEG_GOOF,
	SEG_TEXT,			/* N_TEXT == 4 */
	SEG_GOOF,
	SEG_DATA,			/* N_DATA == 6 */
	SEG_GOOF,
	SEG_BSS,			/* N_BSS == 8 */
	SEG_GOOF,
	SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF,
	SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF,
	SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF,
	SEG_REGISTER,			/* dummy N_REGISTER for regs = 30 */
	SEG_GOOF,
};

#ifdef __STDC__
static void obj_aout_stab(int what);
static void obj_aout_line(void);
static void obj_aout_desc(void);
#else /* __STDC__ */
static void obj_aout_desc();
static void obj_aout_stab();
static void obj_aout_line();
#endif /* __STDC__ */

const pseudo_typeS obj_pseudo_table[] = {
	{ "line",	obj_aout_line,		0	}, /* source code line number */
	{ "ln",		obj_aout_line,		0	}, /* source code line number */
	{ "desc",	obj_aout_desc,		0	}, /* def */
	{ "stabd",	obj_aout_stab,		'd'	}, /* stabs */
	{ "stabn",	obj_aout_stab,		'n'	}, /* stabs */
	{ "stabs",	obj_aout_stab,		's'	}, /* stabs */

	{ NULL}	/* end sentinel */
}; /* obj_pseudo_table */


/* Relocation. */

/*
 * In: length of relocation (or of address) in chars: 1, 2 or 4.
 * Out: GNU LD relocation length code: 0, 1, or 2.
 */

static unsigned char
nbytes_r_length [] = {
  42, 0, 1, 42, 2
  };

/*
 *		emit_relocations()
 *
 * Crawl along a fixS chain. Emit the segment's relocations.
 */
void obj_emit_relocations(where, fixP, segment_address_in_file)
char **where;
fixS *fixP; /* Fixup chain for this segment. */
relax_addressT segment_address_in_file;
{
	struct reloc_info_generic ri;
	register symbolS *symbolP;

	/* If a machine dependent emitter is needed, call it instead. */
	if (md_emit_relocations) {
		(*md_emit_relocations) (fixP, segment_address_in_file);
		return;
	}

	/* JF this is for paranoia */
	bzero((char *)&ri,sizeof(ri));
	for (;  fixP;  fixP = fixP->fx_next) {
		if ((symbolP = fixP->fx_addsy) != 0) {
			ri.r_bsr		= fixP->fx_bsr;
			ri.r_disp		= fixP->fx_im_disp;
			ri.r_callj		= fixP->fx_callj;
			ri.r_length		= nbytes_r_length [fixP->fx_size];
			ri.r_pcrel		= fixP->fx_pcrel;
			ri.r_address	= fixP->fx_frag->fr_address + fixP->fx_where - segment_address_in_file;

			if (S_GET_TYPE(symbolP) == N_UNDF) {
				ri.r_extern	= 1;
				ri.r_symbolnum	= symbolP->sy_number;
			} else {
				ri.r_extern	= 0;
				ri.r_symbolnum	= S_GET_TYPE(symbolP);
			}

			/* Output the relocation information in machine-dependent form. */
			md_ri_to_chars(*where, &ri);
			*where += md_reloc_size;
		} /* if there is an add symbol */
	} /* for each fix */

	return;
} /* emit_relocations() */

/* Aout file generation & utilities */
void obj_header_append(where, headers)
char **where;
object_headers *headers;
{
	tc_headers_hook(headers);

#ifdef CROSS_ASSEMBLE
	md_number_to_chars(*where, headers->header.a_info, sizeof(headers->header.a_info));
	*where += sizeof(headers->header.a_info);
	md_number_to_chars(*where, headers->header.a_text, sizeof(headers->header.a_text));
	*where += sizeof(headers->header.a_text);
	md_number_to_chars(*where, headers->header.a_data, sizeof(headers->header.a_data));
	*where += sizeof(headers->header.a_data);
	md_number_to_chars(*where, headers->header.a_bss, sizeof(headers->header.a_bss));
	*where += sizeof(headers->header.a_bss);
	md_number_to_chars(*where, headers->header.a_syms, sizeof(headers->header.a_syms));
	*where += sizeof(headers->header.a_syms);
	md_number_to_chars(*where, headers->header.a_entry, sizeof(headers->header.a_entry));
	*where += sizeof(headers->header.a_entry);
	md_number_to_chars(*where, headers->header.a_trsize, sizeof(headers->header.a_trsize));
	*where += sizeof(headers->header.a_trsize);
	md_number_to_chars(*where, headers->header.a_drsize, sizeof(headers->header.a_drsize));
	*where += sizeof(headers->header.a_drsize);
#ifdef EXEC_MACHINE_TYPE
	md_number_to_chars(*where, headers->header.a_machtype, sizeof(headers->header.a_machtype));
	*where += sizeof(headers->header.a_machtype);
#endif /* EXEC_MACHINE_TYPE */
#ifdef EXEC_VERSION
	md_number_to_chars(*where, headers->header.a_version, sizeof(headers->header.a_version));
	*where += sizeof(headers->header.a_version);
#endif /* EXEC_VERSION */

#else /* CROSS_ASSEMBLE */

	append(where, (char *) &headers->header, sizeof(headers->header));
#endif /* CROSS_ASSEMBLE */

	return;
} /* obj_append_header() */

void obj_symbol_to_chars(where, symbolP)
char **where;
symbolS *symbolP;
{
	md_number_to_chars((char *)&(S_GET_OFFSET(symbolP)), S_GET_OFFSET(symbolP), sizeof(S_GET_OFFSET(symbolP)));
	md_number_to_chars((char *)&(S_GET_DESC(symbolP)), S_GET_DESC(symbolP), sizeof(S_GET_DESC(symbolP)));
	md_number_to_chars((char *)&(S_GET_VALUE(symbolP)), S_GET_VALUE(symbolP), sizeof(S_GET_VALUE(symbolP)));

	append(where, (char *)&symbolP->sy_symbol, sizeof(obj_symbol_type));
} /* obj_symbol_to_chars() */

void obj_emit_symbols(where, symbol_rootP)
char **where;
symbolS *symbol_rootP;
{
    symbolS *	symbolP;

    /*
     * Emit all symbols left in the symbol chain.
 */
    for(symbolP = symbol_rootP; symbolP; symbolP = symbol_next(symbolP)) {
	/* Used to save the offset of the name. It is used to point
	   to the string in memory but must be a file offset. */
	register char *temp;

	temp = S_GET_NAME(symbolP);
	S_SET_OFFSET(symbolP, symbolP->sy_name_offset);

	/* Any symbol still undefined and is not a dbg symbol is made N_EXT. */
	if (!S_IS_DEBUG(symbolP) && !S_IS_DEFINED(symbolP)) S_SET_EXTERNAL(symbolP);

	obj_symbol_to_chars(where, symbolP);
	S_SET_NAME(symbolP,temp);
    }
} /* emit_symbols() */

void obj_symbol_new_hook(symbolP)
symbolS *symbolP;
{
	S_SET_OTHER(symbolP, 0);
	S_SET_DESC(symbolP, 0);
	return;
} /* obj_symbol_new_hook() */

static void obj_aout_line() {
	/* Assume delimiter is part of expression. */
	/* BSD4.2 as fails with delightful bug, so we */
	/* are not being incompatible here. */
	new_logical_line((char *)NULL, (int)(get_absolute_expression()));
	demand_empty_rest_of_line();
} /* obj_aout_line() */

/*
 *			stab()
 *
 * Handle .stabX directives, which used to be open-coded.
 * So much creeping featurism overloaded the semantics that we decided
 * to put all .stabX thinking in one place. Here.
 *
 * We try to make any .stabX directive legal. Other people's AS will often
 * do assembly-time consistency checks: eg assigning meaning to n_type bits
 * and "protecting" you from setting them to certain values. (They also zero
 * certain bits before emitting symbols. Tut tut.)
 *
 * If an expression is not absolute we either gripe or use the relocation
 * information. Other people's assemblers silently forget information they
 * don't need and invent information they need that you didn't supply.
 *
 * .stabX directives always make a symbol table entry. It may be junk if
 * the rest of your .stabX directive is malformed.
 */
static void obj_aout_stab(what)
int what;
{
  register symbolS *	symbolP = 0;
  register char *	string;
	   int saved_type = 0;
  	   int length;
  	   int goof;	/* TRUE if we have aborted. */
	   long longint;

/*
 * Enter with input_line_pointer pointing past .stabX and any following
 * whitespace.
 */
	goof = 0; /* JF who forgot this?? */
	if (what == 's') {
		string = demand_copy_C_string(& length);
		SKIP_WHITESPACE();
		if (* input_line_pointer == ',')
			input_line_pointer ++;
		else {
			as_bad("I need a comma after symbol's name");
			goof = 1;
		}
	} else
		string = "";

/*
 * Input_line_pointer->after ','.  String->symbol name.
 */
	if (! goof) {
		symbolP = symbol_new(string,
				     SEG_UNKNOWN,
				     0,
				     (struct frag *)0);
		switch (what) {
		case 'd':
			S_SET_NAME(symbolP, NULL); /* .stabd feature. */
			S_SET_VALUE(symbolP, obstack_next_free(&frags) - frag_now->fr_literal);
			symbolP->sy_frag = frag_now;
			break;

		case 'n':
			symbolP->sy_frag = &zero_address_frag;
			break;

		case 's':
			symbolP->sy_frag = & zero_address_frag;
			break;

		default:
			BAD_CASE(what);
			break;
		}

		if (get_absolute_expression_and_terminator(&longint) == ',')
			symbolP->sy_symbol.n_type = saved_type = longint;
		else {
			as_bad("I want a comma after the n_type expression");
			goof = 1;
			input_line_pointer --; /* Backup over a non-',' char. */
		}
	}

	if (!goof) {
		if (get_absolute_expression_and_terminator(&longint) == ',')
			S_SET_OTHER(symbolP, longint);
		else {
			as_bad("I want a comma after the n_other expression");
			goof = 1;
			input_line_pointer--; /* Backup over a non-',' char. */
		}
	}

	if (!goof) {
		S_SET_DESC(symbolP, get_absolute_expression());
		if (what == 's' || what == 'n') {
			if (*input_line_pointer != ',') {
				as_bad("I want a comma after the n_desc expression");
				goof = 1;
			} else {
				input_line_pointer++;
			}
		}
	}

	if ((!goof) && (what=='s' || what=='n')) {
		pseudo_set(symbolP);
		symbolP->sy_symbol.n_type = saved_type;
	}

	if (goof)
		ignore_rest_of_line();
	else
		demand_empty_rest_of_line ();
} /* obj_aout_stab() */

static void obj_aout_desc() {
	register char *name;
	register char c;
	register char *p;
	register symbolS *symbolP;
	register int temp;

	/*
	 * Frob invented at RMS' request. Set the n_desc of a symbol.
 */
	name = input_line_pointer;
	c = get_symbol_end();
	p = input_line_pointer;
	* p = c;
	SKIP_WHITESPACE();
	if (*input_line_pointer != ',') {
		*p = 0;
		as_bad("Expected comma after name \"%s\"", name);
		*p = c;
		ignore_rest_of_line();
	} else {
		input_line_pointer ++;
		temp = get_absolute_expression();
		*p = 0;
		symbolP = symbol_find_or_make(name);
		*p = c;
		S_SET_DESC(symbolP,temp);
	}
	demand_empty_rest_of_line();
} /* obj_aout_desc() */

void obj_read_begin_hook() {
	return;
} /* obj_read_begin_hook() */

void obj_crawl_symbol_chain(headers)
object_headers *headers;
{
	symbolS *symbolP;
	symbolS **symbolPP;
	int symbol_number = 0;

	/* JF deal with forward references first... */
	for (symbolP = symbol_rootP; symbolP; symbolP = symbol_next(symbolP)) {
		if (symbolP->sy_forward) {
			S_SET_VALUE(symbolP, S_GET_VALUE(symbolP)
				    + S_GET_VALUE(symbolP->sy_forward)
				    + symbolP->sy_forward->sy_frag->fr_address);

			symbolP->sy_forward=0;
		} /* if it has a forward reference */
	} /* walk the symbol chain */

	tc_crawl_symbol_chain(headers);

	symbolPP = &symbol_rootP;	/*->last symbol chain link. */
	while ((symbolP  = *symbolPP) != NULL) {
		if (flagseen['R'] && (S_GET_SEGMENT(symbolP) == SEG_DATA)) {
			S_SET_SEGMENT(symbolP, SEG_TEXT);
		} /* if pusing data into text */

		S_SET_VALUE(symbolP, S_GET_VALUE(symbolP) + symbolP->sy_frag->fr_address);

		/* OK, here is how we decide which symbols go out into the
		   brave new symtab.  Symbols that do are:

		   * symbols with no name (stabd's?)
		   * symbols with debug info in their N_TYPE

		   Symbols that don't are:
		   * symbols that are registers
		   * symbols with \1 as their 3rd character (numeric labels)
		   * "local labels" as defined by S_LOCAL_NAME(name)
		   if the -L switch was passed to gas.

		   All other symbols are output.  We complain if a deleted
		   symbol was marked external. */


		if (!S_IS_REGISTER(symbolP)
		    && (!S_GET_NAME(symbolP)
			|| S_IS_DEBUG(symbolP)
#ifdef TC_I960
			/* FIXME-SOON this ifdef seems highly dubious to me.  xoxorich. */
			|| !S_IS_DEFINED(symbolP)
			|| S_IS_EXTERNAL(symbolP)
#endif /* TC_I960 */
			|| (S_GET_NAME(symbolP)[0] != '\001' && (flagseen ['L'] || ! S_LOCAL_NAME(symbolP))))) {
			symbolP->sy_number = symbol_number++;

			/* The + 1 after strlen account for the \0 at the
			   end of each string */
			if (!S_IS_STABD(symbolP)) {
				/* Ordinary case. */
				symbolP->sy_name_offset = string_byte_count;
				string_byte_count += strlen(S_GET_NAME(symbolP)) + 1;
			}
			else	/* .Stabd case. */
			    symbolP->sy_name_offset = 0;
			symbolPP = &(symbol_next(symbolP));
		} else {
			if (S_IS_EXTERNAL(symbolP) || !S_IS_DEFINED(symbolP)) {
				as_bad("Local symbol %s never defined", S_GET_NAME(symbolP));
			} /* oops. */

			/* Unhook it from the chain */
			*symbolPP = symbol_next(symbolP);
		} /* if this symbol should be in the output */
	} /* for each symbol */

	H_SET_SYMBOL_TABLE_SIZE(headers, symbol_number);

	return;
} /* obj_crawl_symbol_chain() */

/*
 * Find strings by crawling along symbol table chain.
 */

void obj_emit_strings(where)
char **where;
{
	symbolS *symbolP;

#ifdef CROSS_ASSEMBLE
	/* Gotta do md_ byte-ordering stuff for string_byte_count first - KWK */
	md_number_to_chars(*where, string_byte_count, sizeof(string_byte_count));
	*where += sizeof(string_byte_count);
#else /* CROSS_ASSEMBLE */
	append (where, (char *)&string_byte_count, (unsigned long)sizeof(string_byte_count));
#endif /* CROSS_ASSEMBLE */

	for(symbolP = symbol_rootP; symbolP; symbolP = symbol_next(symbolP)) {
		if(S_GET_NAME(symbolP))
		    append(&next_object_file_charP, S_GET_NAME(symbolP),
			   (unsigned long)(strlen (S_GET_NAME(symbolP)) + 1));
	} /* walk symbol chain */

	return;
} /* obj_emit_strings() */

/*
 * Local Variables:
 * comment-column: 0
 * fill-column: 131
 * End:
 */

/* end of obj-aout.c */
