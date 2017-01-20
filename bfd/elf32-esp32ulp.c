/* ESP32ULP ELF-32 support for BFD.
Copyright (c) 2016-2017 Espressif Systems (Shanghai) PTE LTD.

based on Copyright (C) 2005-2017 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/esp32ulp.h"
#include "dwarf2.h"
#include "hashtab.h"

static bfd_reloc_status_type esp32ulp_pltpc_reloc( 
	bfd *abfd ATTRIBUTE_UNUSED, arelent *reloc_entry ATTRIBUTE_UNUSED, asymbol *symbol ATTRIBUTE_UNUSED, 
	void * data ATTRIBUTE_UNUSED, asection *input_section ATTRIBUTE_UNUSED, bfd *output_bfd ATTRIBUTE_UNUSED, char **error_message ATTRIBUTE_UNUSED
	)
{
	bfd_reloc_status_type flag = bfd_reloc_ok;
	return flag;
}

//static bfd_reloc_status_type esp32ulp_imm16_reloc(bfd *abfd, arelent *reloc_entry, asymbol *symbol, void * data, asection *input_section, bfd *output_bfd, char **error_message ATTRIBUTE_UNUSED)
//{
//	//DEBUG_TRACE("dya_pass esp32ulp_imm16_reloc\n");
//	bfd_vma relocation, x;
//	bfd_size_type reloc_addr = reloc_entry->address;
//	bfd_vma output_base = 0;
//	reloc_howto_type *howto = reloc_entry->howto;
//	asection *output_section;
//	bfd_boolean relocatable = (output_bfd != NULL);
//
//	/* Is the address of the relocation really within the section?  */
//	if (reloc_entry->address > bfd_get_section_limit(abfd, input_section))
//		return bfd_reloc_outofrange;
//
//	if (bfd_is_und_section(symbol->section)
//		&& (symbol->flags & BSF_WEAK) == 0
//		&& !relocatable)
//		return bfd_reloc_undefined;
//
//	output_section = symbol->section->output_section;
//	relocation = symbol->value;
//
//	/* Convert input-section-relative symbol value to absolute.  */
//	if (relocatable)
//		output_base = 0;
//	else
//		output_base = output_section->vma;
//
//	if (!relocatable || !strcmp(symbol->name, symbol->section->name))
//		relocation += output_base + symbol->section->output_offset;
//
//	/* Add in supplied addend.  */
//	relocation += reloc_entry->addend;
//
//	if (relocatable)
//	{
//		reloc_entry->address += input_section->output_offset;
//		reloc_entry->addend += symbol->section->output_offset;
//	}
//	else
//	{
//		reloc_entry->addend = 0;
//	}
//
//	if (howto->complain_on_overflow != complain_overflow_dont)
//	{
//		bfd_reloc_status_type flag;
//		flag = bfd_check_overflow(howto->complain_on_overflow,
//			howto->bitsize,
//			howto->rightshift,
//			bfd_arch_bits_per_address(abfd),
//			relocation);
//		if (flag != bfd_reloc_ok)
//			return flag;
//	}
//
//	/* Here the variable relocation holds the final address of the
//	symbol we are relocating against, plus any addend.  */
//
//	//DEBUG_TRACE("dya_pass - esp32ulp_imm16_reloc = %i, reloc_addr=%i, howto->rightshift=%i\n", (unsigned int)relocation, (unsigned int)reloc_addr, (int)howto->rightshift);
//	relocation >>= (bfd_vma)howto->rightshift;
//	x = relocation;
//	bfd_put_16(abfd, x, (unsigned char *)data + reloc_addr);
//	return bfd_reloc_ok;
//}


static bfd_reloc_status_type esp32ulp_jumprelr_reloc(bfd *abfd, arelent *reloc_entry, asymbol *symbol, void * data, asection *input_section, bfd *output_bfd, char **error_message ATTRIBUTE_UNUSED)
{
	bfd_vma relocation;
	bfd_size_type addr = reloc_entry->address;
	bfd_vma output_base = 0;
	asection *output_section;
	bfd_boolean relocatable = (output_bfd != NULL);

	//DEBUG_TRACE("dya_pass esp32ulp_jumprelr_reloc: relocatable=%i\n", (int)relocatable);

	/* Is the address of the relocation really within the section?  */
	if (reloc_entry->address > bfd_get_section_limit(abfd, input_section))
		return bfd_reloc_outofrange;

	if (bfd_is_und_section(symbol->section)
		&& (symbol->flags & BSF_WEAK) == 0
		&& !relocatable)
		return bfd_reloc_undefined;

	output_section = symbol->section->output_section;
	relocation = symbol->value;
	/* Convert input-section-relative symbol value to absolute.  */
	if (relocatable)
		output_base = 0;
	else
		output_base = output_section->vma;

	if ((symbol->name
		&& symbol->section->name
		&& !strcmp(symbol->name, symbol->section->name))
		|| !relocatable)
	{
		relocation += output_base + symbol->section->output_offset;
	}

	relocation += reloc_entry->addend;

	if (relocatable)
	{
		/* This output will be relocatable ... like ld -r. */
		reloc_entry->address += input_section->output_offset;
		reloc_entry->addend += symbol->section->output_offset;
		//DEBUG_TRACE("dya_pass esp32ulp_jumprelr_reloc: address=%08x, addend=%08x, symbol->value=%08x\n", (int)reloc_entry->address, (int)reloc_entry->addend, (int)symbol->value);
	}
	else
	{
		reloc_entry->addend = 0;
	}

	/* Here the variable relocation holds the final address of the
	symbol we are relocating against, plus any addend.  */
	unsigned int ddd = 0;
	memcpy(&ddd, (unsigned char *)data + addr, 4);
	relocation -= reloc_entry->address;
	//DEBUG_TRACE("dya_pass esp32ulp_jumprelr_reloc: relocation=%08x, ddd=%08x\n", (unsigned int)relocation, ddd);
	ddd &= ~(0x000000ff < 17);
	int reloc = (int)relocation;
	//printf("relock=%i \n",reloc);
	if ((reloc > 0x7f) || (reloc < -0x7f))
	{
		return bfd_reloc_outofrange;
	}
	if (reloc >= 0)
	{
		ddd |= ((reloc & 0x7f) << 17);
	}
	else
	{
		reloc = -reloc;
		ddd |= ((reloc & 0x7f) << 17) | (0x01 << 24);
	}
	memcpy((unsigned char *)data + addr, &ddd, 4);
	//DEBUG_TRACE("dya_pass esp32ulp_jumprelr_reloc: final = reloc=%08x, ddd=%08x\n", (unsigned int)reloc, ddd);
	return bfd_reloc_ok;
}

static bfd_reloc_status_type esp32ulp_bfd_reloc(bfd *abfd, arelent *reloc_entry, asymbol *symbol, void * data, asection *input_section, bfd *output_bfd, char **error_message ATTRIBUTE_UNUSED)
{
	bfd_vma relocation;
	bfd_size_type addr = reloc_entry->address;
	bfd_vma output_base = 0;
	reloc_howto_type *howto = reloc_entry->howto;
	asection *output_section;
	bfd_boolean relocatable = (output_bfd != NULL);

	/* Is the address of the relocation really within the section?  */
	if (reloc_entry->address > bfd_get_section_limit(abfd, input_section))
		return bfd_reloc_outofrange;

	if (bfd_is_und_section(symbol->section)
		&& (symbol->flags & BSF_WEAK) == 0
		&& !relocatable)
		return bfd_reloc_undefined;

	/* Get symbol value.  (Common symbols are special.)  */
	if (bfd_is_com_section(symbol->section))
		relocation = 0;
	else
		relocation = symbol->value;

	output_section = symbol->section->output_section;

	/* Convert input-section-relative symbol value to absolute.  */
	if (relocatable)
		output_base = 0;
	else
		output_base = output_section->vma;

	if (!relocatable || !strcmp(symbol->name, symbol->section->name))
		relocation += output_base + symbol->section->output_offset;

	if (!relocatable && !strcmp(symbol->name, symbol->section->name))
	{
		/* Add in supplied addend.  */
		relocation += reloc_entry->addend;
	}

	/* Here the variable relocation holds the final address of the
	symbol we are relocating against, plus any addend.  */

	if (howto->pc_relative == TRUE)
	{
		relocation -= input_section->output_section->vma + input_section->output_offset;

		if (howto->pcrel_offset == TRUE)
			relocation -= reloc_entry->address;
	}

	if (relocatable)
	{
		reloc_entry->address += input_section->output_offset;
		reloc_entry->addend += symbol->section->output_offset;
	}

	if (howto->complain_on_overflow != complain_overflow_dont)
	{
		bfd_reloc_status_type status;

		status = bfd_check_overflow(howto->complain_on_overflow,
			howto->bitsize,
			howto->rightshift,
			bfd_arch_bits_per_address(abfd),
			relocation);
		if (status != bfd_reloc_ok)
			return status;
	}

	/* If rightshift is 1 and the number odd, return error.  */
	if (howto->rightshift && (relocation & 0x01))
	{
		_bfd_error_handler(_("relocation should be even number"));
		return bfd_reloc_overflow;
	}

	relocation >>= (bfd_vma)howto->rightshift;

	/* Shift everything up to where it's going to be used.  */

	relocation <<= (bfd_vma)howto->bitpos;

#define DOIT(x)								\
  x = ( (x & ~howto->dst_mask) | (relocation & howto->dst_mask))

	/* handle 8 and 16 bit relocations here. */
	switch (howto->size)
	{
	case 0:
	{
		char x = bfd_get_8(abfd, (char *)data + addr);
		DOIT(x);
		bfd_put_8(abfd, x, (unsigned char *)data + addr);
	}
	break;

	case 1:
	{
		unsigned short x = bfd_get_16(abfd, (bfd_byte *)data + addr);
		DOIT(x);
		bfd_put_16(abfd, (bfd_vma)x, (unsigned char *)data + addr);
	}
	break;

	default:
		return bfd_reloc_other;
	}

	return bfd_reloc_ok;
}

/* HOWTO Table for esp ulp.
 */

#define ESP32ULP_RELOC_MIN 0
#define ESP32ULP_RELOC_MAX 0x21
#define ESP32ULP_GNUEXT_RELOC_MIN 0x40
#define ESP32ULP_GNUEXT_RELOC_MAX 0x43
#define ESP32ULP_ARELOC_MIN 0xE0
#define ESP32ULP_ARELOC_MAX 0xF3

static reloc_howto_type esp32ulp_howto_table[] =
{
	/* This reloc does nothing. .  */
	HOWTO(R_ESP32ULP_UNUSED0,					/* type.  */
	0,											/* rightshift.  */
	3,											/* size (0 = byte, 1 = short, 2 = long).  */
	0,											/* bitsize.  */
	FALSE,										/* pc_relative.  */
	0,											/* bitpos.  */
	complain_overflow_dont,						/* complain_on_overflow.  */
	bfd_elf_generic_reloc,						/* special_function.  */
	"R_ESP32ULP_UNUSED0",						/* name.  */
	FALSE,										/* partial_inplace.  */
	0,											/* src_mask.  */
	0,											/* dst_mask.  */
	FALSE),										/* pcrel_offset.  */


	HOWTO(R_ESP32ULP_RIMM16,		/* type.  */
	0,								/* rightshift.  */
	2,								/* size (0 = byte, 1 = short, 2 = long).  */
	11,								/* bitsize.  */
	FALSE,							/* pc_relative.  */
	2,								/* bitpos.  */
	complain_overflow_unsigned,		/* complain_on_overflow.  */
	NULL,			/* special_function.  */
	"R_ESP32ULP_RIMM16",			/* name.  */
	FALSE,							/* partial_inplace.  */
	0x00001FFC,								/* src_mask.  */ // dya-pass - 0
	0x00001FFC,						/* dst_mask.  */
	FALSE),							/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_JUMPR,			/* type.  */
	0,								/* rightshift.  */
	2,								/* size (0 = byte, 1 = short, 2 = long).  */
	8,								/* bitsize.  */
	TRUE,							/* pc_relative.  */
	17,								/* bitpos.  */
	complain_overflow_signed,		/* complain_on_overflow.  */
	esp32ulp_jumprelr_reloc,		/* special_function.  */
	"R_ESP32ULP_JUMPR",				/* name.  */
	FALSE,							/* partial_inplace.  */
	(0xff << 17),					/* src_mask.  */
	(0xff << 17),					/* dst_mask.  */
	TRUE),							/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_LOAD16,		/* type.  */
	0,								/* rightshift.  */
	2,								/* size (0 = byte, 1 = short, 2 = long).  */
	11,								/* bitsize.  */
	FALSE,							/* pc_relative.  */
	2,								/* bitpos.  */
	complain_overflow_unsigned,		/* complain_on_overflow.  */
	NULL,			/* special_function.  */
	"R_ESP32ULP_LOAD16",			/* name.  */
	FALSE,							/* partial_inplace.  */
	0x00001FFC,						/* src_mask.  */ // dya-pass - 0
	0x00001FFC,						/* dst_mask.  */
	FALSE),							/* pcrel_offset.  */
	
	HOWTO(R_ESP32ULP_WR_MEM,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	13,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	8,			/* bitpos.  */
	complain_overflow_unsigned, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_WR_MEM",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x7ff << 10),				/* src_mask.  */ // dya-pass - 0
	(0x7ff << 10),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_ALUI,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	16,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	4,			/* bitpos.  */
	complain_overflow_signed, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_ALUI",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x00ffff << 4),	/* src_mask.  */ // dya-pass - 0
	(0x00ffff << 4),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_WAIT,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	16,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	0,			/* bitpos.  */
	complain_overflow_unsigned, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_WAIT",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x00ffff << 0),				/* src_mask.  */ // dya-pass - 0
	(0x00ffff << 0),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_TSENS_CYCLE,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	12,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	16,			/* bitpos.  */
	complain_overflow_unsigned, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_TSENS_CYCLE",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x3fff << 16),				/* src_mask.  */ // dya-pass - 0
	(0x3fff << 16),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_TSENS_DELAY,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	14,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	2,			/* bitpos.  */
	complain_overflow_unsigned, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_TSENS_DELAY",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x3fff << 2),				/* src_mask.  */ // dya-pass - 0
	(0x3fff << 2),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */


	HOWTO(R_ESP32ULP_ADC_CYCLE,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	16,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	8,			/* bitpos.  */
	complain_overflow_unsigned, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_ADC_CYCLE",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x00ffff << 8),				/* src_mask.  */ // dya-pass - 0
	(0x00ffff << 8),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */


	HOWTO(R_ESP32ULP_ADC_SEL,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	1,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	6,			/* bitpos.  */
	complain_overflow_unsigned, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_ADC_SEL",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x1 << 6),				/* src_mask.  */ // dya-pass - 0
	(0x1 << 6),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */


	HOWTO(R_ESP32ULP_ADC_MUX,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	4,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	2,			/* bitpos.  */
	complain_overflow_unsigned, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_ADC_MUX",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x0f << 2),				/* src_mask.  */ // dya-pass - 0
	(0x0f << 2),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */


	HOWTO(R_ESP32ULP_WAKE,		/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	1,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	0,							/* bitpos.  */
	complain_overflow_unsigned,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_WAKE",			/* name.  */
	FALSE,						/* partial_inplace.  */
	(0x1 << 0),							/* src_mask.  */ // dya-pass - 0
	(0x1 << 0),					/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */


	HOWTO(R_ESP32ULP_SLEEP,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	4,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	0,			/* bitpos.  */
	complain_overflow_unsigned, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_SLEEP",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x0f << 0),				/* src_mask.  */ // dya-pass - 0
	(0x0f << 0),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_STAGE,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	16,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	4,			/* bitpos.  */
	complain_overflow_signed, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_STAGE",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x00ff << 4),				/* src_mask.  */ // dya-pass - 0
	(0x00ff << 4),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */


	HOWTO(R_ESP32ULP_JUMPR_STEP,	/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	8,			/* bitsize.  */
	TRUE,			/* pc_relative.  */
	17,			/* bitpos.  */
	complain_overflow_signed, /* complain_on_overflow.  */
	esp32ulp_jumprelr_reloc,	/* special_function.  */
	"R_ESP32ULP_JUMPR_STEP",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x00ff << 17),			  /* src_mask.  */
	(0x00ff << 17),		/* dst_mask.  */
	TRUE),			/* pcrel_offset.  */


	HOWTO(R_ESP32ULP_JUMPR_THRESH,		/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	16,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	0,			/* bitpos.  */
	complain_overflow_signed, /* complain_on_overflow.  */
	NULL,		/* special_function.  */
	"R_ESP32ULP_JUMPR_THRESH",	/* name.  */
	FALSE,			/* partial_inplace.  */
	(0x00ffff << 0),				/* src_mask.  */ // dya-pass - 0
	(0x00ffff << 0),		/* dst_mask.  */
	FALSE),			/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_JUMPS_THRESH,		/* type.  */
	0,									/* rightshift.  */
	2,									/* size (0 = byte, 1 = short, 2 = long).  */
	8,									/* bitsize.  */
	FALSE,								/* pc_relative.  */
	0,									/* bitpos.  */
	complain_overflow_signed,			/* complain_on_overflow.  */
	NULL,								/* special_function.  */
	"R_ESP32ULP_JUMPS_THRESH",			/* name.  */
	FALSE,								/* partial_inplace.  */
	(0x00ff << 0),									/* src_mask.  */ // dya-pass - 0
	(0x00ff << 0),						/* dst_mask.  */
	FALSE),								/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_REG_RW_HIGH,	/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	5,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	23,							/* bitpos.  */
	complain_overflow_unsigned,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_REG_RW_HIGH",		/* name.  */
	FALSE,						/* partial_inplace.  */
	(0x3f << 23),							/* src_mask.  */ // dya-pass - 0
	(0x3f << 23),				/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_REG_RW_LOW,	/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	5,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	18,							/* bitpos.  */
	complain_overflow_unsigned,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_REG_RW_LOW",		/* name.  */
	FALSE,						/* partial_inplace.  */
	(0x3f << 18),				/* src_mask.  */ // dya-pass - 0
	(0x3f << 18),				/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_REG_RW_ADDR,	/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	10,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	0,							/* bitpos.  */
	complain_overflow_unsigned,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_REG_RW_ADDR",		/* name.  */
	FALSE,						/* partial_inplace.  */
	(0x3ff << 0),				/* src_mask.  */ // dya-pass - 0
	(0x3ff << 0),				/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_REG_RW_DATA,	/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	8,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	10,							/* bitpos.  */
	complain_overflow_unsigned,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_REG_RW_DATA",		/* name.  */
	FALSE,						/* partial_inplace.  */
	(0x00ff << 10),				/* src_mask.  */ // dya-pass - 0
	(0x00ff << 10),				/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_I2C_RW_HIGH,	/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	16,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	19,							/* bitpos.  */
	complain_overflow_signed,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_I2C_RW_HIGH",		/* name.  */
	FALSE,						/* partial_inplace.  */
	(0x7 << 19),				/* src_mask.  */ // dya-pass - 0
	(0x7 << 19),				/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_I2C_RW_LOW,	/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	16,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	16,							/* bitpos.  */
	complain_overflow_signed,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_I2C_RW_LOW",		/* name.  */
	FALSE,						/* partial_inplace.  */
	(0x7 << 16),				/* src_mask.  */ // dya-pass - 0
	(0x7 << 16),				/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_I2C_RW_ADDR,	/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	16,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	0,							/* bitpos.  */
	complain_overflow_signed,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_I2C_RW_ADDR",		/* name.  */
	FALSE,						/* partial_inplace.  */
	(0xff << 0),				/* src_mask.  */ // dya-pass - 0
	(0xff << 0),				/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_I2C_RW_DATA,	/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	16,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	8,							/* bitpos.  */
	complain_overflow_signed,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_I2C_RW_DATA",		/* name.  */
	FALSE,						/* partial_inplace.  */
	(0x00ff << 8),				/* src_mask.  */ // dya-pass - 0
	(0x00ff << 8),				/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_I2C_RW_SEL,	/* type.  */
	0,							/* rightshift.  */
	2,							/* size (0 = byte, 1 = short, 2 = long).  */
	16,							/* bitsize.  */
	FALSE,						/* pc_relative.  */
	22,							/* bitpos.  */
	complain_overflow_signed,	/* complain_on_overflow.  */
	NULL,						/* special_function.  */
	"R_ESP32ULP_I2C_RW_SEL",		/* name.  */
	FALSE,						/* partial_inplace.  */
	(0xf << 22),				/* src_mask.  */ // dya-pass - 0
	(0xf << 22),				/* dst_mask.  */
	FALSE),						/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_RESERVE,	/* type.  */
	0,			/* rightshift.  */
	2,			/* size (0 = byte, 1 = short, 2 = long).  */
	8,			/* bitsize.  */
	TRUE,			/* pc_relative.  */
	17,			/* bitpos.  */
	complain_overflow_signed, /* complain_on_overflow.  */
	esp32ulp_bfd_reloc,	/* special_function.  */
	"R_ESP32ULP_RESERVE",	/* name.  */
	FALSE,			/* partial_inplace.  */
	0x01fe0000,		/* src_mask.  */
	0x01fe0000,		/* dst_mask.  */
	TRUE),			/* pcrel_offset.  */
};

static reloc_howto_type esp32ulp_gnuext_howto_table[] =
{
	HOWTO(R_ESP32ULP_PLTPC,		/* type.  */
	0,			/* rightshift.  */
	1,			/* size (0 = byte, 1 = short, 2 = long).  */
	16,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	0,			/* bitpos.  */
	complain_overflow_bitfield, /* complain_on_overflow.  */
	esp32ulp_pltpc_reloc,	/* special_function.  */
	"R_ESP32ULP_PLTPC",	/* name.  */
	FALSE,			/* partial_inplace.  */
	0xffff,		/* src_mask.  */
	0xffff,		/* dst_mask.  */
	FALSE),		/* pcrel_offset.  */

	HOWTO(R_ESP32ULP_GOT,		/* type.  */
	0,			/* rightshift.  */
	1,			/* size (0 = byte, 1 = short, 2 = long).  */
	16,			/* bitsize.  */
	FALSE,			/* pc_relative.  */
	0,			/* bitpos.  */
	complain_overflow_bitfield, /* complain_on_overflow.  */
	bfd_elf_generic_reloc,	/* special_function.  */
	"R_ESP32ULP_GOT",		/* name.  */
	FALSE,			/* partial_inplace.  */
	0x7fff,		/* src_mask.  */
	0x7fff,		/* dst_mask.  */
	FALSE),		/* pcrel_offset.  */

	/* GNU extension to record C++ vtable hierarchy.  */
	HOWTO(R_ESP32ULP_GNU_VTINHERIT, /* type.  */
	0,                     /* rightshift.  */
	2,                     /* size (0 = byte, 1 = short, 2 = long).  */
	0,                     /* bitsize.  */
	FALSE,                 /* pc_relative.  */
	0,                     /* bitpos.  */
	complain_overflow_dont, /* complain_on_overflow.  */
	NULL,                  /* special_function.  */
	"R_ESP32ULP_GNU_VTINHERIT", /* name.  */
	FALSE,                 /* partial_inplace.  */
	0,                     /* src_mask.  */
	0,                     /* dst_mask.  */
	FALSE),                /* pcrel_offset.  */

	/* GNU extension to record C++ vtable member usage.  */
	HOWTO(R_ESP32ULP_GNU_VTENTRY,	/* type.  */
	0,                     /* rightshift.  */
	2,                     /* size (0 = byte, 1 = short, 2 = long).  */
	0,                     /* bitsize.  */
	FALSE,                 /* pc_relative.  */
	0,			/* bitpos.  */
	complain_overflow_dont, /* complain_on_overflow.  */
	_bfd_elf_rel_vtable_reloc_fn, /* special_function.  */
	"R_ESP32ULP_GNU_VTENTRY",	/* name.  */
	FALSE,                 /* partial_inplace.  */
	0,                     /* src_mask.  */
	0,                     /* dst_mask.  */
	FALSE)                 /* pcrel_offset.  */
};

struct esp32ulp_reloc_map
{
	bfd_reloc_code_real_type 	bfd_reloc_val;
	unsigned int			esp32ulp_reloc_val;
};

static const struct esp32ulp_reloc_map esp32ulp_reloc_map[] =
{
	{ BFD_RELOC_NONE, R_ESP32ULP_UNUSED0 },
	{ BFD_RELOC_ESP32ULP_16_IMM, R_ESP32ULP_RIMM16 },
	{ BFD_RELOC_ESP32ULP_JUMPR, R_ESP32ULP_JUMPR },
	{ BFD_RELOC_ESP32ULP_16_LOAD, R_ESP32ULP_LOAD16 },
	{ BFD_RELOC_ESP32ULP_WR_MEM, R_ESP32ULP_WR_MEM },
	{ BFD_RELOC_ESP32ULP_ALUI, R_ESP32ULP_ALUI },
	{ BFD_RELOC_ESP32ULP_WAIT, R_ESP32ULP_WAIT },
	{ BFD_RELOC_ESP32ULP_TSENS_CYCLE, R_ESP32ULP_TSENS_CYCLE },
	{ BFD_RELOC_ESP32ULP_TSENS_DELAY, R_ESP32ULP_TSENS_DELAY },

	{ BFD_RELOC_ESP32ULP_ADC_CYCLE, R_ESP32ULP_ADC_CYCLE },
	{ BFD_RELOC_ESP32ULP_ADC_SEL, R_ESP32ULP_ADC_SEL },
	{ BFD_RELOC_ESP32ULP_ADC_MUX, R_ESP32ULP_ADC_MUX },
	{ BFD_RELOC_ESP32ULP_WAKE, R_ESP32ULP_WAKE },
	{ BFD_RELOC_ESP32ULP_SLEEP, R_ESP32ULP_SLEEP },

	{ BFD_RELOC_ESP32ULP_STAGE, R_ESP32ULP_STAGE },

	{ BFD_RELOC_ESP32ULP_JUMPR_STEP, R_ESP32ULP_JUMPR_STEP },
	{ BFD_RELOC_ESP32ULP_JUMPR_THRESH, R_ESP32ULP_JUMPR_THRESH },
	{ BFD_RELOC_ESP32ULP_JUMPS_THRESH, R_ESP32ULP_JUMPS_THRESH },

	{ BFD_RELOC_ESP32ULP_REG_RW_HIGH, R_ESP32ULP_REG_RW_HIGH },
	{ BFD_RELOC_ESP32ULP_REG_RW_LOW, R_ESP32ULP_REG_RW_LOW },
	{ BFD_RELOC_ESP32ULP_REG_RW_ADDR, R_ESP32ULP_REG_RW_ADDR },
	{ BFD_RELOC_ESP32ULP_REG_RW_DATA, R_ESP32ULP_REG_RW_DATA },

	{ BFD_RELOC_ESP32ULP_I2C_RW_HIGH, R_ESP32ULP_I2C_RW_HIGH },
	{ BFD_RELOC_ESP32ULP_I2C_RW_LOW, R_ESP32ULP_I2C_RW_LOW },
	{ BFD_RELOC_ESP32ULP_I2C_RW_ADDR, R_ESP32ULP_I2C_RW_ADDR },
	{ BFD_RELOC_ESP32ULP_I2C_RW_DATA, R_ESP32ULP_I2C_RW_DATA },
	{ BFD_RELOC_ESP32ULP_I2C_RW_SEL, R_ESP32ULP_I2C_RW_SEL },

	{ BFD_RELOC_ESP32ULP_RESERVE, R_ESP32ULP_RESERVE },

	{ BFD_RELOC_ESP32ULP_GOT, R_ESP32ULP_GOT },
	{ BFD_RELOC_ESP32ULP_PLTPC, R_ESP32ULP_PLTPC },

	{ BFD_RELOC_VTABLE_INHERIT, R_ESP32ULP_GNU_VTINHERIT },
	{ BFD_RELOC_VTABLE_ENTRY, R_ESP32ULP_GNU_VTENTRY },
};


static void
esp32ulp_info_to_howto(bfd *abfd ATTRIBUTE_UNUSED,
arelent *cache_ptr,
Elf_Internal_Rela *dst)
{
	unsigned int r_type;

	r_type = ELF32_R_TYPE(dst->r_info);

	if (r_type <= ESP32ULP_RELOC_MAX)
		cache_ptr->howto = &esp32ulp_howto_table[r_type];

	else if (r_type >= ESP32ULP_GNUEXT_RELOC_MIN && r_type <= ESP32ULP_GNUEXT_RELOC_MAX)
		cache_ptr->howto = &esp32ulp_gnuext_howto_table[r_type - ESP32ULP_GNUEXT_RELOC_MIN];

	else
		cache_ptr->howto = (reloc_howto_type *)NULL;
}

/* Given a BFD reloc type, return the howto.  */
static reloc_howto_type *
esp32ulp_bfd_reloc_type_lookup(bfd * abfd ATTRIBUTE_UNUSED,
bfd_reloc_code_real_type code)
{
	unsigned int i;
	unsigned int r_type = (unsigned int)-1;

	for (i = sizeof(esp32ulp_reloc_map) / sizeof(esp32ulp_reloc_map[0]); i--;)
		if (esp32ulp_reloc_map[i].bfd_reloc_val == code)
			r_type = esp32ulp_reloc_map[i].esp32ulp_reloc_val;

	if (r_type <= ESP32ULP_RELOC_MAX)
		return &esp32ulp_howto_table[r_type];

	else if (r_type >= ESP32ULP_GNUEXT_RELOC_MIN && r_type <= ESP32ULP_GNUEXT_RELOC_MAX)
		return &esp32ulp_gnuext_howto_table[r_type - ESP32ULP_GNUEXT_RELOC_MIN];

	return (reloc_howto_type *)NULL;
}

static reloc_howto_type *
esp32ulp_bfd_reloc_name_lookup(bfd *abfd ATTRIBUTE_UNUSED,
const char *r_name)
{
	unsigned int i;

	for (i = 0;
		i < (sizeof(esp32ulp_howto_table)
		/ sizeof(esp32ulp_howto_table[0]));
	i++)
		if (esp32ulp_howto_table[i].name != NULL
			&& strcasecmp(esp32ulp_howto_table[i].name, r_name) == 0)
			return &esp32ulp_howto_table[i];

	for (i = 0;
		i < (sizeof(esp32ulp_gnuext_howto_table)
		/ sizeof(esp32ulp_gnuext_howto_table[0]));
	i++)
		if (esp32ulp_gnuext_howto_table[i].name != NULL
			&& strcasecmp(esp32ulp_gnuext_howto_table[i].name, r_name) == 0)
			return &esp32ulp_gnuext_howto_table[i];

	return NULL;
}

/* Given a esp32ulp relocation type, return the howto.  */
static reloc_howto_type *
esp32ulp_reloc_type_lookup(bfd * abfd ATTRIBUTE_UNUSED,
unsigned int r_type)
{
	if (r_type <= ESP32ULP_RELOC_MAX)
		return &esp32ulp_howto_table[r_type];

	else if (r_type >= ESP32ULP_GNUEXT_RELOC_MIN && r_type <= ESP32ULP_GNUEXT_RELOC_MAX)
		return &esp32ulp_gnuext_howto_table[r_type - ESP32ULP_GNUEXT_RELOC_MIN];

	return (reloc_howto_type *)NULL;
}
//
///* Set by ld emulation if --code-in-l1.  */
//bfd_boolean elf32_esp32ulp_code_in_l1 = 0;
//
///* Set by ld emulation if --data-in-l1.  */
//bfd_boolean elf32_esp32ulp_data_in_l1 = 0;

static void
elf32_esp32ulp_final_write_processing(bfd *abfd,
bfd_boolean linker ATTRIBUTE_UNUSED)
{
	(void)abfd;
	(void)linker;
}

/* Return TRUE if the name is a local label.
esp32ulp local labels begin with L$.  */
static bfd_boolean
esp32ulp_is_local_label_name(bfd *abfd, const char *label)
{
	if (label[0] == 'L' && label[1] == '$')
		return TRUE;

	return _bfd_elf_is_local_label_name(abfd, label);
}

/* Look through the relocs for a section during the first phase, and
allocate space in the global offset table or procedure linkage
table.  */

static bfd_boolean
esp32ulp_check_relocs(bfd * abfd,
struct bfd_link_info *info,
	asection *sec,
	const Elf_Internal_Rela *relocs)
{
	bfd *dynobj;
	Elf_Internal_Shdr *symtab_hdr;
	struct elf_link_hash_entry **sym_hashes;
	bfd_signed_vma *local_got_refcounts;
	const Elf_Internal_Rela *rel;
	const Elf_Internal_Rela *rel_end;
	asection *sgot;
	asection *srelgot;

	if (bfd_link_relocatable(info))
		return TRUE;

	dynobj = elf_hash_table(info)->dynobj;
	symtab_hdr = &elf_tdata(abfd)->symtab_hdr;
	sym_hashes = elf_sym_hashes(abfd);
	local_got_refcounts = elf_local_got_refcounts(abfd);

	sgot = NULL;
	srelgot = NULL;

	rel_end = relocs + sec->reloc_count;
	for (rel = relocs; rel < rel_end; rel++)
	{
		unsigned long r_symndx;
		struct elf_link_hash_entry *h;

		r_symndx = ELF32_R_SYM(rel->r_info);
		if (r_symndx < symtab_hdr->sh_info)
			h = NULL;
		else
		{
			h = sym_hashes[r_symndx - symtab_hdr->sh_info];

			/* PR15323, ref flags aren't set for references in the same
			object.  */
			h->root.non_ir_ref = 1;
		}

		switch (ELF32_R_TYPE(rel->r_info))
		{
			/* This relocation describes the C++ object vtable hierarchy.
			Reconstruct it for later use during GC.  */
		case R_ESP32ULP_GNU_VTINHERIT:
			if (!bfd_elf_gc_record_vtinherit(abfd, sec, h, rel->r_offset))
				return FALSE;
			break;

			/* This relocation describes which C++ vtable entries
			are actually used.  Record for later use during GC.  */
		case R_ESP32ULP_GNU_VTENTRY:
			BFD_ASSERT(h != NULL);
			if (h != NULL
				&& !bfd_elf_gc_record_vtentry(abfd, sec, h, rel->r_addend))
				return FALSE;
			break;

		case R_ESP32ULP_GOT:
			if (h != NULL
				&& strcmp(h->root.root.string, "__GLOBAL_OFFSET_TABLE_") == 0)
				break;
			/* Fall through.  */

			if (dynobj == NULL)
			{
				/* Create the .got section.  */
				elf_hash_table(info)->dynobj = dynobj = abfd;
				if (!_bfd_elf_create_got_section(dynobj, info))
					return FALSE;
			}

			sgot = elf_hash_table(info)->sgot;
			srelgot = elf_hash_table(info)->srelgot;
			BFD_ASSERT(sgot != NULL);

			if (h != NULL)
			{
				if (h->got.refcount == 0)
				{
					/* Make sure this symbol is output as a dynamic symbol.  */
					if (h->dynindx == -1 && !h->forced_local)
					{
						if (!bfd_elf_link_record_dynamic_symbol(info, h))
							return FALSE;
					}

					/* Allocate space in the .got section.  */
					sgot->size += 4;
					/* Allocate relocation space.  */
					srelgot->size += sizeof(Elf32_External_Rela);
				}
				h->got.refcount++;
			}
			else
			{
				/* This is a global offset table entry for a local symbol.  */
				if (local_got_refcounts == NULL)
				{
					bfd_size_type size;

					size = symtab_hdr->sh_info;
					size *= sizeof(bfd_signed_vma);
					local_got_refcounts = ((bfd_signed_vma *)
						bfd_zalloc(abfd, size));
					if (local_got_refcounts == NULL)
						return FALSE;
					elf_local_got_refcounts(abfd) = local_got_refcounts;
				}
				if (local_got_refcounts[r_symndx] == 0)
				{
					sgot->size += 4;
					if (bfd_link_pic(info))
					{
						/* If we are generating a shared object, we need to
						output a R_68K_RELATIVE reloc so that the dynamic
						linker can adjust this GOT entry.  */
						srelgot->size += sizeof(Elf32_External_Rela);
					}
				}
				local_got_refcounts[r_symndx]++;
			}
			break;

		default:
			break;
		}
	}

	return TRUE;
}

static enum elf_reloc_type_class
elf32_esp32ulp_reloc_type_class(const struct bfd_link_info *info ATTRIBUTE_UNUSED,
const asection *rel_sec ATTRIBUTE_UNUSED,
const Elf_Internal_Rela * rela)
{
	switch ((int)ELF32_R_TYPE(rela->r_info))
	{
	default:
		return reloc_class_normal;
	}
}

static bfd_reloc_status_type
esp32ulp_final_link_relocate(Elf_Internal_Rela *rel, reloc_howto_type *howto,
bfd *input_bfd, asection *input_section,
bfd_byte *contents, bfd_vma address,
bfd_vma value, bfd_vma addend)
{
	int r_type = ELF32_R_TYPE(rel->r_info);
	//DEBUG_TRACE("dya_pass - esp32ulp_final_link_relocate r_type=%i, addend=%i\n", r_type, (unsigned int)addend);
	if ((r_type == R_ESP32ULP_JUMPR) || (r_type == R_ESP32ULP_JUMPR_STEP))
	{
		bfd_reloc_status_type r = bfd_reloc_ok;

		if (address > bfd_get_section_limit(input_bfd, input_section))
			return bfd_reloc_outofrange;


		value += addend;

		/* Perform usual pc-relative correction.  */
		value -= input_section->output_section->vma + input_section->output_offset;
		value -= address;

		unsigned int ddd = 0;
		memcpy(&ddd, contents + address, 4);
		ddd &= ~(0xff << 17);
		int compare = (int)value;
		//printf("final reloc, compare=%i\n", compare);
		if ((compare > 127) || (compare < -128))
		{
			return bfd_reloc_outofrange;
		}

		if ((compare) >= 0)
		{
			ddd |= ((compare & 0x0000007f) << 17);
		}
		else
		{
			compare = -compare;
			ddd |= ((compare & 0x0000007f) << 17) | (1 << 24);
		}
		memcpy(contents + address, &ddd, 4);
		//DEBUG_TRACE("dya_pass - esp32ulp_final_link_relocate: ddd=%08x, value=%08x, addr=%08x\n", ddd, (int)value, (int)address);
		return r;
	}

	return _bfd_final_link_relocate(howto, input_bfd, input_section, contents,
		rel->r_offset, value, addend);

}


#define ESP32ULP_RELOC_FOR_GLOBAL_SYMBOL(info, input_bfd, input_section, rel,	\
				r_symndx, symtab_hdr, sym_hashes,	\
				h, sec, relocation,			\
				unresolved_reloc, warned, ignored, sectioin_flags)	\
  do									\
          {									\
      /* It seems this can happen with erroneous or unsupported		\
   	 input (mixing a.out and elf in an archive, for example.)  */	\
      if (sym_hashes == NULL)						\
	return FALSE;							\
									\
      h = sym_hashes[r_symndx - symtab_hdr->sh_info];			\
									\
      if (info->wrap_hash != NULL					\
	  && (input_section->flags & SEC_DEBUGGING) != 0)		\
	h = ((struct elf_link_hash_entry *)				\
	     unwrap_hash_lookup (info, input_bfd, &h->root));		\
									\
																											      while (h->root.type == bfd_link_hash_indirect			\
	     || h->root.type == bfd_link_hash_warning)			\
	h = (struct elf_link_hash_entry *) h->root.u.i.link;		\
									\
      warned = FALSE;							\
      ignored = FALSE;							\
      unresolved_reloc = FALSE;						\
      relocation = 0;							\
      if (h->root.type == bfd_link_hash_defined				\
	  || h->root.type == bfd_link_hash_defweak)			\
	  	  	  	{								\
	  sec = h->root.u.def.section;					\
	  if (sec == NULL						\
	      || sec->output_section == NULL)				\
	    /* Set a flag that will be cleared later if we find a	\
   	       relocation value for this symbol.  output_section	\
   	       is typically NULL for symbols satisfied by a shared	\
   	       library.  */						\
	    unresolved_reloc = TRUE;					\
				  		  	  	  else								\
			/*DEBUG_TRACE("ESP32ULP_RELOC_FOR_GLOBAL_SYMBOL - value = %08x, output_section->vma = %08x, sec->output_offset = %08x, sec->size = %08x, flags=%08x\n", (unsigned int)h->root.u.def.value, (unsigned int)sec->output_section->vma,  (unsigned int)sec->output_offset, (unsigned int)sec->output_section->rawsize, (unsigned int)sec->flags);*/ \
	    relocation = (h->root.u.def.value				\
			  + sec->output_section->vma    			\
			  + sec->output_offset);											\
			  sectioin_flags = sec->flags;										\
	  	}																		\
	        else if (h->root.type == bfd_link_hash_undefweak)					\
	;																			\
	        else if (info->unresolved_syms_in_objects == RM_IGNORE		\
	       && ELF_ST_VISIBILITY (h->other) == STV_DEFAULT)		\
	ignored = TRUE;							\
	        else if (!bfd_link_relocatable (info))				\
	{								\
	  bfd_boolean err;						\
	  err = (info->unresolved_syms_in_objects == RM_GENERATE_ERROR	\
		 || ELF_ST_VISIBILITY (h->other) != STV_DEFAULT);	\
	  (*info->callbacks->undefined_symbol) (info,			\
						h->root.root.string,	\
						input_bfd,		\
						input_section,		\
						rel->r_offset, err);	\
	  warned = TRUE;						\
	}								\
      (void) unresolved_reloc;						\
      (void) warned;							\
      (void) ignored;							\
      }									\
	    while (0)








static bfd_boolean
esp32ulp_relocate_section(bfd * output_bfd,
struct bfd_link_info *info,
	bfd * input_bfd,
	asection * input_section,
	bfd_byte * contents,
	Elf_Internal_Rela * relocs,
	Elf_Internal_Sym * local_syms,
	asection ** local_sections)
{
	bfd *dynobj;
	Elf_Internal_Shdr *symtab_hdr;
	struct elf_link_hash_entry **sym_hashes;
	bfd_vma *local_got_offsets;
	asection *sgot;
	Elf_Internal_Rela *rel;
	Elf_Internal_Rela *relend;
	int i = 0;

	dynobj = elf_hash_table(info)->dynobj;
	symtab_hdr = &elf_tdata(input_bfd)->symtab_hdr;
	sym_hashes = elf_sym_hashes(input_bfd);
	local_got_offsets = elf_local_got_offsets(input_bfd);

	sgot = NULL;

	rel = relocs;
	relend = relocs + input_section->reloc_count;
	//DEBUG_TRACE("dya_pass - esp32ulp_relocate_section\n");
	for (; rel < relend; rel++, i++)
	{
		int r_type;
		reloc_howto_type *howto;
		unsigned long r_symndx;
		struct elf_link_hash_entry *h;
		Elf_Internal_Sym *sym;
		asection *sec;
		bfd_vma relocation = 0;
		//bfd_vma old_reloc = relocation;
		bfd_boolean unresolved_reloc;
		bfd_reloc_status_type r;
		bfd_vma address;

		r_type = ELF32_R_TYPE(rel->r_info);
		if (r_type < 0 || r_type >= 243)
		{
			bfd_set_error(bfd_error_bad_value);
			return FALSE;
		}

		if (r_type == R_ESP32ULP_GNU_VTENTRY
			|| r_type == R_ESP32ULP_GNU_VTINHERIT)
			continue;

		howto = esp32ulp_reloc_type_lookup(input_bfd, r_type);
		if (howto == NULL)
		{
			bfd_set_error(bfd_error_bad_value);
			return FALSE;
		}
		r_symndx = ELF32_R_SYM(rel->r_info);

		h = NULL;
		sym = NULL;
		sec = NULL;
		unresolved_reloc = FALSE;
		
		unsigned int section_flags = 0;

		if (r_symndx < symtab_hdr->sh_info)
		{
			sym = local_syms + r_symndx;
			sec = local_sections[r_symndx];
			relocation = _bfd_elf_rela_local_sym(output_bfd, sym, &sec, rel);
			section_flags = sec->flags;
		}
		else
		{
			bfd_boolean warned, ignored;
			ESP32ULP_RELOC_FOR_GLOBAL_SYMBOL(info, input_bfd, input_section, rel,
				r_symndx, symtab_hdr, sym_hashes,
				h, sec, relocation,
				unresolved_reloc, warned, ignored, section_flags);
		}

		if (sec != NULL && discarded_section(sec))
			RELOC_AGAINST_DISCARDED_SECTION(info, input_bfd, input_section,
			rel, 1, relend, howto, 0, contents);

		if (bfd_link_relocatable(info))
			continue;

		address = rel->r_offset;

		/* Then, process normally.  */
		//DEBUG_TRACE("dya_pass - esp32ulp_relocate_section r_type=%i\n", (int)r_type);
		switch (r_type)
		{
		case R_ESP32ULP_GNU_VTINHERIT:
		case R_ESP32ULP_GNU_VTENTRY:
			return bfd_reloc_ok;

		case R_ESP32ULP_GOT:
			/* Relocation is to the address of the entry for this symbol
			in the global offset table.  */
			if (h != NULL
				&& strcmp(h->root.root.string, "__GLOBAL_OFFSET_TABLE_") == 0)
				goto do_default;
			/* Fall through.  */
			/* Relocation is the offset of the entry for this symbol in
			the global offset table.  */

			{
				bfd_vma off;

				if (dynobj == NULL)
				{
					/* Create the .got section.  */
					elf_hash_table(info)->dynobj = dynobj = output_bfd;
					if (!_bfd_elf_create_got_section(dynobj, info))
						return FALSE;
				}

				sgot = elf_hash_table(info)->sgot;
				BFD_ASSERT(sgot != NULL);

				if (h != NULL)
				{
					bfd_boolean dyn;

					off = h->got.offset;
					BFD_ASSERT(off != (bfd_vma)-1);
					dyn = elf_hash_table(info)->dynamic_sections_created;

					if (!WILL_CALL_FINISH_DYNAMIC_SYMBOL(dyn,
						bfd_link_pic(info),
						h)
						|| (bfd_link_pic(info)
						&& (info->symbolic
						|| h->dynindx == -1
						|| h->forced_local)
						&& h->def_regular))
					{
						/* This is actually a static link, or it is a
						-Bsymbolic link and the symbol is defined
						locally, or the symbol was forced to be local
						because of a version file..  We must initialize
						this entry in the global offset table.  Since
						the offset must always be a multiple of 4, we
						use the least significant bit to record whether
						we have initialized it already.

						When doing a dynamic link, we create a .rela.got
						relocation entry to initialize the value.  This
						is done in the finish_dynamic_symbol routine.  */
						if ((off & 1) != 0)
							off &= ~1;
						else
						{
							bfd_put_32(output_bfd, relocation,
								sgot->contents + off);
							h->got.offset |= 1;
						}
					}
					else
						unresolved_reloc = FALSE;
				}
				else
				{
					BFD_ASSERT(local_got_offsets != NULL);
					off = local_got_offsets[r_symndx];
					BFD_ASSERT(off != (bfd_vma)-1);

					/* The offset must always be a multiple of 4.  We use
					the least significant bit to record whether we have
					already generated the necessary reloc.  */
					if ((off & 1) != 0)
						off &= ~1;
					else
					{
						bfd_put_32(output_bfd, relocation, sgot->contents + off);

						if (bfd_link_pic(info))
						{
							asection *s;
							Elf_Internal_Rela outrel;
							bfd_byte *loc;

							s = elf_hash_table(info)->srelgot;
							BFD_ASSERT(s != NULL);

							outrel.r_offset = (sgot->output_section->vma
								+ sgot->output_offset + off);
							outrel.r_info =
								ELF32_R_INFO(0, R_ESP32ULP_RIMM16);
							outrel.r_addend = relocation;
							loc = s->contents;
							loc +=
								s->reloc_count++ * sizeof(Elf32_External_Rela);
							bfd_elf32_swap_reloca_out(output_bfd, &outrel, loc);
						}

						local_got_offsets[r_symndx] |= 1;
					}
				}

				relocation = sgot->output_offset + off;
				rel->r_addend = 0;
				/* esp32ulp : preg = [preg + 17bitdiv4offset] relocation is div by 4.  */
				relocation /= 4;
			}
			goto do_default;

		default:
		do_default :
				//DEBUG_TRACE("dya_pass esp32ulp_final_link_relocate rel->r_addend=%i\n", (int)rel->r_addend);
//				old_reloc = (unsigned int)relocation;
				if (section_flags != 0) relocation = relocation >> 2;
//				printf("relocation - 999 = %08x, old_reloc = %08x, input_section->flags = %08x, section name = %s\n", (unsigned int)relocation, (unsigned int)old_reloc, section_flags, input_section->name);
				   rel->r_addend = 0; // dya_pass
				   r = esp32ulp_final_link_relocate(rel, howto, input_bfd, input_section,
					   contents, address,
					   relocation, rel->r_addend);

				   break;
		}

		/* Dynamic relocs are not propagated for SEC_DEBUGGING sections
		because such sections are not SEC_ALLOC and thus ld.so will
		not process them.  */
		if (unresolved_reloc
			&& !((input_section->flags & SEC_DEBUGGING) != 0 && h->def_dynamic)
			&& _bfd_elf_section_offset(output_bfd, info, input_section,
			rel->r_offset) != (bfd_vma)-1)
		{
			_bfd_error_handler
				/* xgettext:c-format */
				(_("%B(%A+0x%lx): unresolvable relocation against symbol `%s'"),
				input_bfd,
				input_section, (long)rel->r_offset, h->root.root.string);
			return FALSE;
		}

		if (r != bfd_reloc_ok)
		{
			const char *name;

			if (h != NULL)
				name = h->root.root.string;
			else
			{
				name = bfd_elf_string_from_elf_section(input_bfd,
					symtab_hdr->sh_link,
					sym->st_name);
				if (name == NULL)
					return FALSE;
				if (*name == '\0')
					name = bfd_section_name(input_bfd, sec);
			}

			if (r == bfd_reloc_overflow)
				(*info->callbacks->reloc_overflow)
				(info, (h ? &h->root : NULL), name, howto->name,
				(bfd_vma)0, input_bfd, input_section, rel->r_offset);
			else
			{
				_bfd_error_handler
					/* xgettext:c-format */
					(_("%B(%A+0x%lx): reloc against `%s': error %d"),
					input_bfd, input_section,
					(long)rel->r_offset, name, (int)r);
				return FALSE;
			}
		}
	}

	return TRUE;
}

static asection *
esp32ulp_gc_mark_hook(asection * sec,
struct bfd_link_info *info,
	Elf_Internal_Rela * rel,
struct elf_link_hash_entry *h,
	Elf_Internal_Sym * sym)
{
	if (h != NULL)
		switch (ELF32_R_TYPE(rel->r_info))
	{
		case R_ESP32ULP_GNU_VTINHERIT:
		case R_ESP32ULP_GNU_VTENTRY:
			return NULL;
	}

	return _bfd_elf_gc_mark_hook(sec, info, rel, h, sym);
}

/* Update the got entry reference counts for the section being removed.  */

static bfd_boolean
esp32ulp_gc_sweep_hook(bfd * abfd,
struct bfd_link_info *info,
	asection * sec,
	const Elf_Internal_Rela * relocs)
{
	Elf_Internal_Shdr *symtab_hdr;
	struct elf_link_hash_entry **sym_hashes;
	bfd_signed_vma *local_got_refcounts;
	const Elf_Internal_Rela *rel, *relend;
	bfd *dynobj;
	asection *sgot;
	asection *srelgot;

	dynobj = elf_hash_table(info)->dynobj;
	if (dynobj == NULL)
		return TRUE;

	symtab_hdr = &elf_tdata(abfd)->symtab_hdr;
	sym_hashes = elf_sym_hashes(abfd);
	local_got_refcounts = elf_local_got_refcounts(abfd);

	sgot = elf_hash_table(info)->sgot;
	srelgot = elf_hash_table(info)->srelgot;

	relend = relocs + sec->reloc_count;
	for (rel = relocs; rel < relend; rel++)
	{
		unsigned long r_symndx;
		struct elf_link_hash_entry *h;

		switch (ELF32_R_TYPE(rel->r_info))
		{
		case R_ESP32ULP_GOT:
			r_symndx = ELF32_R_SYM(rel->r_info);
			if (r_symndx >= symtab_hdr->sh_info)
			{
				h = sym_hashes[r_symndx - symtab_hdr->sh_info];
				if (h->got.refcount > 0)
				{
					--h->got.refcount;
					if (h->got.refcount == 0)
					{
						/* We don't need the .got entry any more.  */
						sgot->size -= 4;
						srelgot->size -= sizeof(Elf32_External_Rela);
					}
				}
			}
			else if (local_got_refcounts != NULL)
			{
				if (local_got_refcounts[r_symndx] > 0)
				{
					--local_got_refcounts[r_symndx];
					if (local_got_refcounts[r_symndx] == 0)
					{
						/* We don't need the .got entry any more.  */
						sgot->size -= 4;
						if (bfd_link_pic(info))
							srelgot->size -= sizeof(Elf32_External_Rela);
					}
				}
			}
			break;
		default:
			break;
		}
	}
	return TRUE;
}

extern const bfd_target esp32ulp_elf32_fdpic_vec;
#define IS_FDPIC(bfd) ((bfd)->xvec == &esp32ulp_elf32_fdpic_vec)

/* An extension of the elf hash table data structure,
containing some additional Blackfin-specific data.  */
struct esp32ulpfdpic_elf_link_hash_table
{
	struct elf_link_hash_table elf;

	/* A pointer to the .rofixup section.  */
	asection *sgotfixup;
	/* GOT base offset.  */
	bfd_vma got0;
	/* Location of the first non-lazy PLT entry, i.e., the number of
	bytes taken by lazy PLT entries.  */
	bfd_vma plt0;
	/* A hash table holding information about which symbols were
	referenced with which PIC-related relocations.  */
	struct htab *relocs_info;
	/* Summary reloc information collected by
	_esp32ulpfdpic_count_got_plt_entries.  */
	struct _esp32ulpfdpic_dynamic_got_info *g;
};

/* Get the Blackfin ELF linker hash table from a link_info structure.  */

#define esp32ulpfdpic_hash_table(info) \
  (elf_hash_table_id ((struct elf_link_hash_table *) ((info)->hash)) \
  == ESP32ULP_ELF_DATA ? ((struct esp32ulpfdpic_elf_link_hash_table *) ((info)->hash)) : NULL)

#define esp32ulpfdpic_got_section(info) \
  (esp32ulpfdpic_hash_table (info)->elf.sgot)
#define esp32ulpfdpic_gotrel_section(info) \
  (esp32ulpfdpic_hash_table (info)->elf.srelgot)
#define esp32ulpfdpic_gotfixup_section(info) \
  (esp32ulpfdpic_hash_table (info)->sgotfixup)
#define esp32ulpfdpic_plt_section(info) \
  (esp32ulpfdpic_hash_table (info)->elf.splt)
#define esp32ulpfdpic_pltrel_section(info) \
  (esp32ulpfdpic_hash_table (info)->elf.srelplt)
#define esp32ulpfdpic_relocs_info(info) \
  (esp32ulpfdpic_hash_table (info)->relocs_info)
#define esp32ulpfdpic_got_initial_offset(info) \
  (esp32ulpfdpic_hash_table (info)->got0)
#define esp32ulpfdpic_plt_initial_offset(info) \
  (esp32ulpfdpic_hash_table (info)->plt0)
#define esp32ulpfdpic_dynamic_got_plt_info(info) \
  (esp32ulpfdpic_hash_table (info)->g)

/* The name of the dynamic interpreter.  This is put in the .interp
section.  */

#define ELF_DYNAMIC_INTERPRETER "/lib/ld.so.1"

#define DEFAULT_STACK_SIZE 0x20000

/* This structure is used to collect the number of entries present in
each addressable range of the got.  */
struct _esp32ulpfdpic_dynamic_got_info
{
	/* Several bits of information about the current link.  */
	struct bfd_link_info *info;
	/* Total size needed for GOT entries within the 18- or 32-bit
	ranges.  */
	bfd_vma got17m4, gothilo;
	/* Total size needed for function descriptor entries within the 18-
	or 32-bit ranges.  */
	bfd_vma fd17m4, fdhilo;
	/* Total size needed function descriptor entries referenced in PLT
	entries, that would be profitable to place in offsets close to
	the PIC register.  */
	bfd_vma fdplt;
	/* Total size needed by lazy PLT entries.  */
	bfd_vma lzplt;
	/* Number of relocations carried over from input object files.  */
	unsigned long relocs;
	/* Number of fixups introduced by relocations in input object files.  */
	unsigned long fixups;
};

/* Create a Blackfin ELF linker hash table.  */

static struct bfd_link_hash_table *
esp32ulpfdpic_elf_link_hash_table_create(bfd *abfd)
{
	struct esp32ulpfdpic_elf_link_hash_table *ret;
	bfd_size_type amt = sizeof(struct esp32ulpfdpic_elf_link_hash_table);

	ret = bfd_zmalloc(amt);
	if (ret == NULL)
		return NULL;

	if (!_bfd_elf_link_hash_table_init(&ret->elf, abfd,
		_bfd_elf_link_hash_newfunc,
		sizeof(struct elf_link_hash_entry),
		ESP32ULP_ELF_DATA))
	{
		free(ret);
		return NULL;
	}

	return &ret->elf.root;
}

/* Decide whether a reference to a symbol can be resolved locally or
not.  If the symbol is protected, we want the local address, but
its function descriptor must be assigned by the dynamic linker.  */
#define ESP32ULPFDPIC_SYM_LOCAL(INFO, H) \
  (_bfd_elf_symbol_refs_local_p ((H), (INFO), 1) \
   || ! elf_hash_table (INFO)->dynamic_sections_created)
#define ESP32ULPFDPIC_FUNCDESC_LOCAL(INFO, H) \
  ((H)->dynindx == -1 || ! elf_hash_table (INFO)->dynamic_sections_created)

/* This structure collects information on what kind of GOT, PLT or
function descriptors are required by relocations that reference a
certain symbol.  */
struct esp32ulpfdpic_relocs_info
{
	/* The index of the symbol, as stored in the relocation r_info, if
	we have a local symbol; -1 otherwise.  */
	long symndx;
	union
	{
		/* The input bfd in which the symbol is defined, if it's a local
		symbol.  */
		bfd *abfd;
		/* If symndx == -1, the hash table entry corresponding to a global
		symbol (even if it turns out to bind locally, in which case it
		should ideally be replaced with section's symndx + addend).  */
		struct elf_link_hash_entry *h;
	} d;
	/* The addend of the relocation that references the symbol.  */
	bfd_vma addend;

	/* The fields above are used to identify an entry.  The fields below
	contain information on how an entry is used and, later on, which
	locations it was assigned.  */
	/* The following 2 fields record whether the symbol+addend above was
	ever referenced with a GOT relocation.  The 17M4 suffix indicates a
	GOT17M4 relocation; hilo is used for GOTLO/GOTHI pairs.  */
	unsigned got17m4;
	unsigned gothilo;
	/* Whether a FUNCDESC relocation references symbol+addend.  */
	unsigned fd;
	/* Whether a FUNCDESC_GOT relocation references symbol+addend.  */
	unsigned fdgot17m4;
	unsigned fdgothilo;
	/* Whether a FUNCDESC_GOTOFF relocation references symbol+addend.  */
	unsigned fdgoff17m4;
	unsigned fdgoffhilo;
	/* Whether symbol+addend is referenced with GOTOFF17M4, GOTOFFLO or
	GOTOFFHI relocations.  The addend doesn't really matter, since we
	envision that this will only be used to check whether the symbol
	is mapped to the same segment as the got.  */
	unsigned gotoff;
	/* Whether symbol+addend is referenced by a LABEL24 relocation.  */
	unsigned call;
	/* Whether symbol+addend is referenced by a 32 or FUNCDESC_VALUE
	relocation.  */
	unsigned sym;
	/* Whether we need a PLT entry for a symbol.  Should be implied by
	something like:
	(call && symndx == -1 && ! ESP32ULPFDPIC_SYM_LOCAL (info, d.h))  */
	unsigned plt : 1;
	/* Whether a function descriptor should be created in this link unit
	for symbol+addend.  Should be implied by something like:
	(plt || fdgotoff17m4 || fdgotofflohi
	|| ((fd || fdgot17m4 || fdgothilo)
	&& (symndx != -1 || ESP32ULPFDPIC_FUNCDESC_LOCAL (info, d.h))))  */
	unsigned privfd : 1;
	/* Whether a lazy PLT entry is needed for this symbol+addend.
	Should be implied by something like:
	(privfd && symndx == -1 && ! ESP32ULPFDPIC_SYM_LOCAL (info, d.h)
	&& ! (info->flags & DF_BIND_NOW))  */
	unsigned lazyplt : 1;
	/* Whether we've already emitted GOT relocations and PLT entries as
	needed for this symbol.  */
	unsigned done : 1;

	/* The number of R_ESP32ULP_BYTE4_DATA, R_ESP32ULP_FUNCDESC and R_ESP32ULP_FUNCDESC_VALUE
	relocations referencing the symbol.  */
	unsigned relocs32, relocsfd, relocsfdv;

	/* The number of .rofixups entries and dynamic relocations allocated
	for this symbol, minus any that might have already been used.  */
	unsigned fixups, dynrelocs;

	/* The offsets of the GOT entries assigned to symbol+addend, to the
	function descriptor's address, and to a function descriptor,
	respectively.  Should be zero if unassigned.  The offsets are
	counted from the value that will be assigned to the PIC register,
	not from the beginning of the .got section.  */
	bfd_signed_vma got_entry, fdgot_entry, fd_entry;
	/* The offsets of the PLT entries assigned to symbol+addend,
	non-lazy and lazy, respectively.  If unassigned, should be
	(bfd_vma)-1.  */
	bfd_vma plt_entry, lzplt_entry;
};

/* Compute a hash with the key fields of an esp32ulpfdpic_relocs_info entry.  */
static hashval_t
esp32ulpfdpic_relocs_info_hash(const void *entry_)
{
	const struct esp32ulpfdpic_relocs_info *entry = entry_;

	return (entry->symndx == -1
		? (long)entry->d.h->root.root.hash
		: entry->symndx + (long)entry->d.abfd->id * 257) + entry->addend;
}

/* Test whether the key fields of two esp32ulpfdpic_relocs_info entries are
identical.  */
static int
esp32ulpfdpic_relocs_info_eq(const void *entry1, const void *entry2)
{
	const struct esp32ulpfdpic_relocs_info *e1 = entry1;
	const struct esp32ulpfdpic_relocs_info *e2 = entry2;

	return e1->symndx == e2->symndx && e1->addend == e2->addend
		&& (e1->symndx == -1 ? e1->d.h == e2->d.h : e1->d.abfd == e2->d.abfd);
}

/* Find or create an entry in a hash table HT that matches the key
fields of the given ENTRY.  If it's not found, memory for a new
entry is allocated in ABFD's obstack.  */
static struct esp32ulpfdpic_relocs_info *
esp32ulpfdpic_relocs_info_find(struct htab *ht,
bfd *abfd,
const struct esp32ulpfdpic_relocs_info *entry,
enum insert_option insert)
{
	struct esp32ulpfdpic_relocs_info **loc;

	if (!ht)
		return NULL;

	loc = (struct esp32ulpfdpic_relocs_info **) htab_find_slot(ht, entry, insert);

	if (!loc)
		return NULL;

	if (*loc)
		return *loc;

	*loc = bfd_zalloc(abfd, sizeof(**loc));

	if (!*loc)
		return *loc;

	(*loc)->symndx = entry->symndx;
	(*loc)->d = entry->d;
	(*loc)->addend = entry->addend;
	(*loc)->plt_entry = (bfd_vma)-1;
	(*loc)->lzplt_entry = (bfd_vma)-1;

	return *loc;
}

/* Obtain the address of the entry in HT associated with H's symbol +
addend, creating a new entry if none existed.  ABFD is only used
for memory allocation purposes.  */
inline static struct esp32ulpfdpic_relocs_info *
esp32ulpfdpic_relocs_info_for_global(struct htab *ht,
bfd *abfd,
struct elf_link_hash_entry *h,
	bfd_vma addend,
enum insert_option insert)
{
	struct esp32ulpfdpic_relocs_info entry;

	entry.symndx = -1;
	entry.d.h = h;
	entry.addend = addend;

	return esp32ulpfdpic_relocs_info_find(ht, abfd, &entry, insert);
}

/* Obtain the address of the entry in HT associated with the SYMNDXth
local symbol of the input bfd ABFD, plus the addend, creating a new
entry if none existed.  */
inline static struct esp32ulpfdpic_relocs_info *
esp32ulpfdpic_relocs_info_for_local(struct htab *ht,
bfd *abfd,
long symndx,
bfd_vma addend,
enum insert_option insert)
{
	struct esp32ulpfdpic_relocs_info entry;

	entry.symndx = symndx;
	entry.d.abfd = abfd;
	entry.addend = addend;

	return esp32ulpfdpic_relocs_info_find(ht, abfd, &entry, insert);
}

/* Merge fields set by check_relocs() of two entries that end up being
mapped to the same (presumably global) symbol.  */

inline static void
esp32ulpfdpic_pic_merge_early_relocs_info(struct esp32ulpfdpic_relocs_info *e2,
struct esp32ulpfdpic_relocs_info const *e1)
{
	e2->got17m4 |= e1->got17m4;
	e2->gothilo |= e1->gothilo;
	e2->fd |= e1->fd;
	e2->fdgot17m4 |= e1->fdgot17m4;
	e2->fdgothilo |= e1->fdgothilo;
	e2->fdgoff17m4 |= e1->fdgoff17m4;
	e2->fdgoffhilo |= e1->fdgoffhilo;
	e2->gotoff |= e1->gotoff;
	e2->call |= e1->call;
	e2->sym |= e1->sym;
}

/* Every block of 65535 lazy PLT entries shares a single call to the
resolver, inserted in the 32768th lazy PLT entry (i.e., entry #
32767, counting from 0).  All other lazy PLT entries branch to it
in a single instruction.  */

#define LZPLT_RESOLVER_EXTRA 10
#define LZPLT_NORMAL_SIZE 6
#define LZPLT_ENTRIES 1362

#define ESP32ULPFDPIC_LZPLT_BLOCK_SIZE ((bfd_vma) LZPLT_NORMAL_SIZE * LZPLT_ENTRIES + LZPLT_RESOLVER_EXTRA)
#define ESP32ULPFDPIC_LZPLT_RESOLV_LOC (LZPLT_NORMAL_SIZE * LZPLT_ENTRIES / 2)

/* Add a dynamic relocation to the SRELOC section.  */

inline static bfd_vma
_esp32ulpfdpic_add_dyn_reloc(bfd *output_bfd, asection *sreloc, bfd_vma offset,
int reloc_type, long dynindx, bfd_vma addend,
struct esp32ulpfdpic_relocs_info *entry)
{
	Elf_Internal_Rela outrel;
	bfd_vma reloc_offset;

	outrel.r_offset = offset;
	outrel.r_info = ELF32_R_INFO(dynindx, reloc_type);
	outrel.r_addend = addend;

	reloc_offset = sreloc->reloc_count * sizeof(Elf32_External_Rel);
	BFD_ASSERT(reloc_offset < sreloc->size);
	bfd_elf32_swap_reloc_out(output_bfd, &outrel,
		sreloc->contents + reloc_offset);
	sreloc->reloc_count++;

	/* If the entry's index is zero, this relocation was probably to a
	linkonce section that got discarded.  We reserved a dynamic
	relocation, but it was for another entry than the one we got at
	the time of emitting the relocation.  Unfortunately there's no
	simple way for us to catch this situation, since the relocation
	is cleared right before calling relocate_section, at which point
	we no longer know what the relocation used to point to.  */
	if (entry->symndx)
	{
		BFD_ASSERT(entry->dynrelocs > 0);
		entry->dynrelocs--;
	}

	return reloc_offset;
}

/* Add a fixup to the ROFIXUP section.  */

static bfd_vma
_esp32ulpfdpic_add_rofixup(bfd *output_bfd, asection *rofixup, bfd_vma offset,
struct esp32ulpfdpic_relocs_info *entry)
{
	bfd_vma fixup_offset;

	if (rofixup->flags & SEC_EXCLUDE)
		return -1;

	fixup_offset = rofixup->reloc_count * 4;
	if (rofixup->contents)
	{
		BFD_ASSERT(fixup_offset < rofixup->size);
		bfd_put_32(output_bfd, offset, rofixup->contents + fixup_offset);
	}
	rofixup->reloc_count++;

	if (entry && entry->symndx)
	{
		/* See discussion about symndx == 0 in _esp32ulpfdpic_add_dyn_reloc
		above.  */
		BFD_ASSERT(entry->fixups > 0);
		entry->fixups--;
	}

	return fixup_offset;
}

/* Find the segment number in which OSEC, and output section, is
located.  */

static unsigned
_esp32ulpfdpic_osec_to_segment(bfd *output_bfd, asection *osec)
{
	Elf_Internal_Phdr *p = _bfd_elf_find_segment_containing_section(output_bfd, osec);

	return (p != NULL) ? p - elf_tdata(output_bfd)->phdr : -1;
}

/* Relocate an Blackfin ELF section.

The RELOCATE_SECTION function is called by the new ELF backend linker
to handle the relocations for a section.

The relocs are always passed as Rela structures; if the section
actually uses Rel structures, the r_addend field will always be
zero.

This function is responsible for adjusting the section contents as
necessary, and (if using Rela relocs and generating a relocatable
output file) adjusting the reloc addend as necessary.

This function does not have to worry about setting the reloc
address or the reloc symbol index.

LOCAL_SYMS is a pointer to the swapped in local symbols.

LOCAL_SECTIONS is an array giving the section in the input file
corresponding to the st_shndx field of each local symbol.

The global hash table entry for the global symbols can be found
via elf_sym_hashes (input_bfd).

When generating relocatable output, this function must handle
STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
going to be the section symbol corresponding to the output
section, which means that the addend must be adjusted
accordingly.  */

static bfd_boolean
esp32ulpfdpic_relocate_section(bfd * output_bfd,
struct bfd_link_info *info,
	bfd * input_bfd,
	asection * input_section,
	bfd_byte * contents,
	Elf_Internal_Rela * relocs,
	Elf_Internal_Sym * local_syms,
	asection ** local_sections)
{
	(void)local_sections; // dya - remove warning
	Elf_Internal_Shdr *symtab_hdr;
	struct elf_link_hash_entry **sym_hashes;
	Elf_Internal_Rela *rel;
	Elf_Internal_Rela *relend;
	unsigned isec_segment,
		check_segment[2];
	int silence_segment_error = !bfd_link_pic(info);

	symtab_hdr = &elf_tdata(input_bfd)->symtab_hdr;
	sym_hashes = elf_sym_hashes(input_bfd);
	relend = relocs + input_section->reloc_count;

	isec_segment = _esp32ulpfdpic_osec_to_segment(output_bfd,
		input_section->output_section);

	for (rel = relocs; rel < relend; rel++)
	{
		reloc_howto_type *howto;
		unsigned long r_symndx;
		Elf_Internal_Sym *sym;
		asection *sec;
		struct elf_link_hash_entry *h;
		bfd_vma relocation;
		bfd_reloc_status_type r;
		const char * name = NULL;
		int r_type;
		struct esp32ulpfdpic_relocs_info *picrel;

		r_type = ELF32_R_TYPE(rel->r_info);

		if (r_type == R_ESP32ULP_GNU_VTINHERIT
			|| r_type == R_ESP32ULP_GNU_VTENTRY)
			continue;

		r_symndx = ELF32_R_SYM(rel->r_info);
		howto = esp32ulp_reloc_type_lookup(input_bfd, r_type);
		if (howto == NULL)
		{
			bfd_set_error(bfd_error_bad_value);
			return FALSE;
		}

		h = NULL;
		sym = NULL;
		sec = NULL;
		unsigned int section_flags = 0;
		if (r_symndx < symtab_hdr->sh_info)
		{
			sym = local_syms + r_symndx;
			relocation = _bfd_elf_rela_local_sym(output_bfd, sym, &sec, rel);
			section_flags = 0;
			name = bfd_elf_string_from_elf_section
				(input_bfd, symtab_hdr->sh_link, sym->st_name);
			name = (name == NULL) ? bfd_section_name(input_bfd, sec) : name;
		}
		else
		{
			bfd_boolean warned, ignored;
			bfd_boolean unresolved_reloc;

			ESP32ULP_RELOC_FOR_GLOBAL_SYMBOL(info, input_bfd, input_section, rel,
				r_symndx, symtab_hdr, sym_hashes,
				h, sec, relocation,
				unresolved_reloc, warned, ignored, section_flags);
		}

		if (sec != NULL && discarded_section(sec))
			RELOC_AGAINST_DISCARDED_SECTION(info, input_bfd, input_section,
			rel, 1, relend, howto, 0, contents);

		if (bfd_link_relocatable(info))
			continue;

		if (h != NULL
			&& (h->root.type == bfd_link_hash_defined
			|| h->root.type == bfd_link_hash_defweak)
			&& !ESP32ULPFDPIC_SYM_LOCAL(info, h))
		{
			relocation = 0;
		}
		(void)section_flags;
		switch (r_type)
		{

		default:
			picrel = NULL;
			if (h && !ESP32ULPFDPIC_SYM_LOCAL(info, h)
				&& _bfd_elf_section_offset(output_bfd, info, input_section,
				rel->r_offset) != (bfd_vma)-1)
			{
				info->callbacks->warning
					(info, _("relocation references symbol not defined in the module"),
					name, input_bfd, input_section, rel->r_offset);
				return FALSE;
			}
			break;
		}

		switch (r_type)
		{
		default:
			check_segment[0] = isec_segment;
			check_segment[1] = sec
				? _esp32ulpfdpic_osec_to_segment(output_bfd, sec->output_section)
				: (unsigned)-1;
			break;
		}

		if (check_segment[0] != check_segment[1] && IS_FDPIC(output_bfd))
		{
#if 1 /* If you take this out, remove the #error from fdpic-static-6.d
		in the ld testsuite.  */
			/* This helps catch problems in GCC while we can't do more
			than static linking.  The idea is to test whether the
			input file basename is crt0.o only once.  */
			if (silence_segment_error == 1)
				silence_segment_error =
				(strlen(input_bfd->filename) == 6
				&& filename_cmp(input_bfd->filename, "crt0.o") == 0)
				|| (strlen(input_bfd->filename) > 6
				&& filename_cmp(input_bfd->filename
				+ strlen(input_bfd->filename) - 7,
				"/crt0.o") == 0)
				? -1 : 0;
#endif
			if (!silence_segment_error
				/* We don't want duplicate errors for undefined
				symbols.  */
				&& !(picrel && picrel->symndx == -1
				&& picrel->d.h->root.type == bfd_link_hash_undefined))
				info->callbacks->warning
				(info,
				bfd_link_pic(info)
				? _("relocations between different segments are not supported")
				: _("warning: relocation references a different segment"),
				name, input_bfd, input_section, rel->r_offset);
			if (!silence_segment_error && bfd_link_pic(info))
				return FALSE;
			elf_elfheader(output_bfd)->e_flags |= EF_ESP32ULP_PIC;
		}

		r = esp32ulp_final_link_relocate(rel, howto, input_bfd, input_section,
			contents, rel->r_offset,
			relocation, rel->r_addend);

		if (r != bfd_reloc_ok)
		{
			const char * msg = (const char *)NULL;

			switch (r)
			{
			case bfd_reloc_overflow:
				(*info->callbacks->reloc_overflow)
					(info, (h ? &h->root : NULL), name, howto->name,
					(bfd_vma)0, input_bfd, input_section, rel->r_offset);
				break;

			case bfd_reloc_undefined:
				(*info->callbacks->undefined_symbol)
					(info, name, input_bfd, input_section, rel->r_offset, TRUE);
				break;

			case bfd_reloc_outofrange:
				msg = _("internal error: out of range error");
				break;

			case bfd_reloc_notsupported:
				msg = _("internal error: unsupported relocation error");
				break;

			case bfd_reloc_dangerous:
				msg = _("internal error: dangerous relocation");
				break;

			default:
				msg = _("internal error: unknown error");
				break;
			}

			if (msg)
				(*info->callbacks->warning) (info, msg, name, input_bfd,
				input_section, rel->r_offset);
		}
	}

	return TRUE;
}

/* Update the relocation information for the relocations of the section
being removed.  */

static bfd_boolean
esp32ulpfdpic_gc_sweep_hook(bfd *abfd,
struct bfd_link_info *info,
	asection *sec,
	const Elf_Internal_Rela *relocs)
{
	Elf_Internal_Shdr *symtab_hdr;
	struct elf_link_hash_entry **sym_hashes, **sym_hashes_end;
	const Elf_Internal_Rela *rel;
	const Elf_Internal_Rela *rel_end;
	struct esp32ulpfdpic_relocs_info *picrel;

	BFD_ASSERT(IS_FDPIC(abfd));

	symtab_hdr = &elf_tdata(abfd)->symtab_hdr;
	sym_hashes = elf_sym_hashes(abfd);
	sym_hashes_end = sym_hashes + symtab_hdr->sh_size / sizeof(Elf32_External_Sym);
	if (!elf_bad_symtab(abfd))
		sym_hashes_end -= symtab_hdr->sh_info;

	rel_end = relocs + sec->reloc_count;
	for (rel = relocs; rel < rel_end; rel++)
	{
		struct elf_link_hash_entry *h;
		unsigned long r_symndx;

		r_symndx = ELF32_R_SYM(rel->r_info);
		if (r_symndx < symtab_hdr->sh_info)
			h = NULL;
		else
			h = sym_hashes[r_symndx - symtab_hdr->sh_info];

		if (h != NULL)
			picrel = esp32ulpfdpic_relocs_info_for_global(esp32ulpfdpic_relocs_info(info),
			abfd, h,
			rel->r_addend, NO_INSERT);
		else
			picrel = esp32ulpfdpic_relocs_info_for_local(esp32ulpfdpic_relocs_info
			(info), abfd, r_symndx,
			rel->r_addend, NO_INSERT);

		if (!picrel)
			return TRUE;
	}

	return TRUE;
}

/* We need dynamic symbols for every section, since segments can
relocate independently.  */
static bfd_boolean
_esp32ulpfdpic_link_omit_section_dynsym(bfd *output_bfd ATTRIBUTE_UNUSED,
struct bfd_link_info *info ATTRIBUTE_UNUSED,
	asection *p)
{
	switch (elf_section_data(p)->this_hdr.sh_type)
	{
	case SHT_PROGBITS:
	case SHT_NOBITS:
		/* If sh_type is yet undecided, assume it could be
		SHT_PROGBITS/SHT_NOBITS.  */
	case SHT_NULL:
		return FALSE;

		/* There shouldn't be section relative relocations
		against any other section.  */
	default:
		return TRUE;
	}
}

/* Create  a .got section, as well as its additional info field.  This
is almost entirely copied from
elflink.c:_bfd_elf_create_got_section().  */

static bfd_boolean
_esp32ulp_create_got_section(bfd *abfd, struct bfd_link_info *info)
{
	flagword flags, pltflags;
	asection *s;
	struct elf_link_hash_entry *h;
	const struct elf_backend_data *bed = get_elf_backend_data(abfd);
	int ptralign;

	/* This function may be called more than once.  */
	s = elf_hash_table(info)->sgot;
	if (s != NULL)
		return TRUE;

	/* Machine specific: although pointers are 32-bits wide, we want the
	GOT to be aligned to a 64-bit boundary, such that function
	descriptors in it can be accessed with 64-bit loads and
	stores.  */
	ptralign = 3;

	flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY
		| SEC_LINKER_CREATED);
	pltflags = flags;

	s = bfd_make_section_anyway_with_flags(abfd, ".got", flags);
	elf_hash_table(info)->sgot = s;
	if (s == NULL
		|| !bfd_set_section_alignment(abfd, s, ptralign))
		return FALSE;

	if (bed->want_got_sym)
	{
		/* Define the symbol _GLOBAL_OFFSET_TABLE_ at the start of the .got
		(or .got.plt) section.  We don't do this in the linker script
		because we don't want to define the symbol if we are not creating
		a global offset table.  */
		h = _bfd_elf_define_linkage_sym(abfd, info, s, "__GLOBAL_OFFSET_TABLE_");
		elf_hash_table(info)->hgot = h;
		if (h == NULL)
			return FALSE;

		/* Machine-specific: we want the symbol for executables as
		well.  */
		if (!bfd_elf_link_record_dynamic_symbol(info, h))
			return FALSE;
	}

	/* The first bit of the global offset table is the header.  */
	s->size += bed->got_header_size;

	/* This is the machine-specific part.  Create and initialize section
	data for the got.  */
	if (IS_FDPIC(abfd))
	{
		esp32ulpfdpic_relocs_info(info) = htab_try_create(1,
			esp32ulpfdpic_relocs_info_hash,
			esp32ulpfdpic_relocs_info_eq,
			(htab_del)NULL);
		if (!esp32ulpfdpic_relocs_info(info))
			return FALSE;

		s = bfd_make_section_anyway_with_flags(abfd, ".rel.got",
			(flags | SEC_READONLY));
		if (s == NULL
			|| !bfd_set_section_alignment(abfd, s, 2))
			return FALSE;

		esp32ulpfdpic_gotrel_section(info) = s;

		/* Machine-specific.  */
		s = bfd_make_section_anyway_with_flags(abfd, ".rofixup",
			(flags | SEC_READONLY));
		if (s == NULL
			|| !bfd_set_section_alignment(abfd, s, 2))
			return FALSE;

		esp32ulpfdpic_gotfixup_section(info) = s;
	}

	pltflags |= SEC_CODE;
	if (bed->plt_not_loaded)
		pltflags &= ~(SEC_CODE | SEC_LOAD | SEC_HAS_CONTENTS);
	if (bed->plt_readonly)
		pltflags |= SEC_READONLY;

	s = bfd_make_section_anyway_with_flags(abfd, ".plt", pltflags);
	if (s == NULL
		|| !bfd_set_section_alignment(abfd, s, bed->plt_alignment))
		return FALSE;
	/* Blackfin-specific: remember it.  */
	esp32ulpfdpic_plt_section(info) = s;

	if (bed->want_plt_sym)
	{
		/* Define the symbol _PROCEDURE_LINKAGE_TABLE_ at the start of the
		.plt section.  */
		struct bfd_link_hash_entry *bh = NULL;

		if (!(_bfd_generic_link_add_one_symbol
			(info, abfd, "__PROCEDURE_LINKAGE_TABLE_", BSF_GLOBAL, s, 0, NULL,
			FALSE, get_elf_backend_data(abfd)->collect, &bh)))
			return FALSE;
		h = (struct elf_link_hash_entry *) bh;
		h->def_regular = 1;
		h->type = STT_OBJECT;

		if (!bfd_link_executable(info)
			&& !bfd_elf_link_record_dynamic_symbol(info, h))
			return FALSE;
	}

	/* Blackfin-specific: we want rel relocations for the plt.  */
	s = bfd_make_section_anyway_with_flags(abfd, ".rel.plt",
		flags | SEC_READONLY);
	if (s == NULL
		|| !bfd_set_section_alignment(abfd, s, bed->s->log_file_align))
		return FALSE;
	/* Blackfin-specific: remember it.  */
	esp32ulpfdpic_pltrel_section(info) = s;

	return TRUE;
}

/* Make sure the got and plt sections exist, and that our pointers in
the link hash table point to them.  */

static bfd_boolean
elf32_esp32ulpfdpic_create_dynamic_sections(bfd *abfd, struct bfd_link_info *info)
{
	/* This is mostly copied from
	elflink.c:_bfd_elf_create_dynamic_sections().  */
	flagword flags;
	asection *s;
	const struct elf_backend_data *bed = get_elf_backend_data(abfd);

	flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY
		| SEC_LINKER_CREATED);

	/* We need to create .plt, .rel[a].plt, .got, .got.plt, .dynbss, and
	.rel[a].bss sections.  */

	/* Blackfin-specific: we want to create the GOT in the Blackfin way.  */
	if (!_esp32ulp_create_got_section(abfd, info))
		return FALSE;

	/* Blackfin-specific: make sure we created everything we wanted.  */
	BFD_ASSERT(esp32ulpfdpic_got_section(info) && esp32ulpfdpic_gotrel_section(info)
		/* && esp32ulpfdpic_gotfixup_section (info) */
		&& esp32ulpfdpic_plt_section(info)
		&& esp32ulpfdpic_pltrel_section(info));

	if (bed->want_dynbss)
	{
		/* The .dynbss section is a place to put symbols which are defined
		by dynamic objects, are referenced by regular objects, and are
		not functions.  We must allocate space for them in the process
		image and use a R_*_COPY reloc to tell the dynamic linker to
		initialize them at run time.  The linker script puts the .dynbss
		section into the .bss section of the final image.  */
		s = bfd_make_section_anyway_with_flags(abfd, ".dynbss",
			SEC_ALLOC | SEC_LINKER_CREATED);
		if (s == NULL)
			return FALSE;

		/* The .rel[a].bss section holds copy relocs.  This section is not
		normally needed.  We need to create it here, though, so that the
		linker will map it to an output section.  We can't just create it
		only if we need it, because we will not know whether we need it
		until we have seen all the input files, and the first time the
		main linker code calls BFD after examining all the input files
		(size_dynamic_sections) the input sections have already been
		mapped to the output sections.  If the section turns out not to
		be needed, we can discard it later.  We will never need this
		section when generating a shared object, since they do not use
		copy relocs.  */
		if (!bfd_link_pic(info))
		{
			s = bfd_make_section_anyway_with_flags(abfd,
				".rela.bss",
				flags | SEC_READONLY);
			if (s == NULL
				|| !bfd_set_section_alignment(abfd, s, bed->s->log_file_align))
				return FALSE;
		}
	}

	return TRUE;
}

/* Compute the total GOT size required by each symbol in each range.
Symbols may require up to 4 words in the GOT: an entry pointing to
the symbol, an entry pointing to its function descriptor, and a
private function descriptors taking two words.  */

static void
_esp32ulpfdpic_count_nontls_entries(struct esp32ulpfdpic_relocs_info *entry,
struct _esp32ulpfdpic_dynamic_got_info *dinfo)
{
	/* Allocate space for a GOT entry pointing to the symbol.  */
	if (entry->got17m4)
		dinfo->got17m4 += 4;
	else if (entry->gothilo)
		dinfo->gothilo += 4;
	else
		entry->relocs32--;
	entry->relocs32++;

	/* Allocate space for a GOT entry pointing to the function
	descriptor.  */
	if (entry->fdgot17m4)
		dinfo->got17m4 += 4;
	else if (entry->fdgothilo)
		dinfo->gothilo += 4;
	else
		entry->relocsfd--;
	entry->relocsfd++;

	/* Decide whether we need a PLT entry, a function descriptor in the
	GOT, and a lazy PLT entry for this symbol.  */
	entry->plt = entry->call
		&& entry->symndx == -1 && !ESP32ULPFDPIC_SYM_LOCAL(dinfo->info, entry->d.h)
		&& elf_hash_table(dinfo->info)->dynamic_sections_created;
	entry->privfd = entry->plt
		|| entry->fdgoff17m4 || entry->fdgoffhilo
		|| ((entry->fd || entry->fdgot17m4 || entry->fdgothilo)
		&& (entry->symndx != -1
		|| ESP32ULPFDPIC_FUNCDESC_LOCAL(dinfo->info, entry->d.h)));
	entry->lazyplt = entry->privfd
		&& entry->symndx == -1 && !ESP32ULPFDPIC_SYM_LOCAL(dinfo->info, entry->d.h)
		&& !(dinfo->info->flags & DF_BIND_NOW)
		&& elf_hash_table(dinfo->info)->dynamic_sections_created;

	/* Allocate space for a function descriptor.  */
	if (entry->fdgoff17m4)
		dinfo->fd17m4 += 8;
	else if (entry->privfd && entry->plt)
		dinfo->fdplt += 8;
	else if (entry->privfd)
		dinfo->fdhilo += 8;
	else
		entry->relocsfdv--;
	entry->relocsfdv++;

	if (entry->lazyplt)
		dinfo->lzplt += LZPLT_NORMAL_SIZE;
}

/* Compute the number of dynamic relocations and fixups that a symbol
requires, and add (or subtract) from the grand and per-symbol
totals.  */

static void
_esp32ulpfdpic_count_relocs_fixups(struct esp32ulpfdpic_relocs_info *entry,
struct _esp32ulpfdpic_dynamic_got_info *dinfo,
	bfd_boolean subtract)
{
	bfd_vma relocs = 0, fixups = 0;

	if (!bfd_link_pde(dinfo->info))
		relocs = entry->relocs32 + entry->relocsfd + entry->relocsfdv;
	else
	{
		if (entry->symndx != -1 || ESP32ULPFDPIC_SYM_LOCAL(dinfo->info, entry->d.h))
		{
			if (entry->symndx != -1
				|| entry->d.h->root.type != bfd_link_hash_undefweak)
				fixups += entry->relocs32 + 2 * entry->relocsfdv;
		}
		else
			relocs += entry->relocs32 + entry->relocsfdv;

		if (entry->symndx != -1
			|| ESP32ULPFDPIC_FUNCDESC_LOCAL(dinfo->info, entry->d.h))
		{
			if (entry->symndx != -1
				|| entry->d.h->root.type != bfd_link_hash_undefweak)
				fixups += entry->relocsfd;
		}
		else
			relocs += entry->relocsfd;
	}

	if (subtract)
	{
		relocs = -relocs;
		fixups = -fixups;
	}

	entry->dynrelocs += relocs;
	entry->fixups += fixups;
	dinfo->relocs += relocs;
	dinfo->fixups += fixups;
}

/* Compute the total GOT and PLT size required by each symbol in each range. *
Symbols may require up to 4 words in the GOT: an entry pointing to
the symbol, an entry pointing to its function descriptor, and a
private function descriptors taking two words.  */

static int
_esp32ulpfdpic_count_got_plt_entries(void **entryp, void *dinfo_)
{
	struct esp32ulpfdpic_relocs_info *entry = *entryp;
	struct _esp32ulpfdpic_dynamic_got_info *dinfo = dinfo_;

	_esp32ulpfdpic_count_nontls_entries(entry, dinfo);

	_esp32ulpfdpic_count_relocs_fixups(entry, dinfo, FALSE);

	return 1;
}

/* This structure is used to assign offsets to got entries, function
descriptors, plt entries and lazy plt entries.  */

struct _esp32ulpfdpic_dynamic_got_plt_info
{
	/* Summary information collected with _esp32ulpfdpic_count_got_plt_entries.  */
	struct _esp32ulpfdpic_dynamic_got_info g;

	/* For each addressable range, we record a MAX (positive) and MIN
	(negative) value.  CUR is used to assign got entries, and it's
	incremented from an initial positive value to MAX, then from MIN
	to FDCUR (unless FDCUR wraps around first).  FDCUR is used to
	assign function descriptors, and it's decreased from an initial
	non-positive value to MIN, then from MAX down to CUR (unless CUR
	wraps around first).  All of MIN, MAX, CUR and FDCUR always point
	to even words.  ODD, if non-zero, indicates an odd word to be
	used for the next got entry, otherwise CUR is used and
	incremented by a pair of words, wrapping around when it reaches
	MAX.  FDCUR is decremented (and wrapped) before the next function
	descriptor is chosen.  FDPLT indicates the number of remaining
	slots that can be used for function descriptors used only by PLT
	entries.  */
	struct _esp32ulpfdpic_dynamic_got_alloc_data
	{
		bfd_signed_vma max, cur, odd, fdcur, min;
		bfd_vma fdplt;
	} got17m4, gothilo;
};

/* Determine the positive and negative ranges to be used by each
offset range in the GOT.  FDCUR and CUR, that must be aligned to a
double-word boundary, are the minimum (negative) and maximum
(positive) GOT offsets already used by previous ranges, except for
an ODD entry that may have been left behind.  GOT and FD indicate
the size of GOT entries and function descriptors that must be
placed within the range from -WRAP to WRAP.  If there's room left,
up to FDPLT bytes should be reserved for additional function
descriptors.  */

inline static bfd_signed_vma
_esp32ulpfdpic_compute_got_alloc_data(struct _esp32ulpfdpic_dynamic_got_alloc_data *gad,
bfd_signed_vma fdcur,
bfd_signed_vma odd,
bfd_signed_vma cur,
bfd_vma got,
bfd_vma fd,
bfd_vma fdplt,
bfd_vma wrap)
{
	bfd_signed_vma wrapmin = -wrap;

	/* Start at the given initial points.  */
	gad->fdcur = fdcur;
	gad->cur = cur;

	/* If we had an incoming odd word and we have any got entries that
	are going to use it, consume it, otherwise leave gad->odd at
	zero.  We might force gad->odd to zero and return the incoming
	odd such that it is used by the next range, but then GOT entries
	might appear to be out of order and we wouldn't be able to
	shorten the GOT by one word if it turns out to end with an
	unpaired GOT entry.  */
	if (odd && got)
	{
		gad->odd = odd;
		got -= 4;
		odd = 0;
	}
	else
		gad->odd = 0;

	/* If we're left with an unpaired GOT entry, compute its location
	such that we can return it.  Otherwise, if got doesn't require an
	odd number of words here, either odd was already zero in the
	block above, or it was set to zero because got was non-zero, or
	got was already zero.  In the latter case, we want the value of
	odd to carry over to the return statement, so we don't want to
	reset odd unless the condition below is true.  */
	if (got & 4)
	{
		odd = cur + got;
		got += 4;
	}

	/* Compute the tentative boundaries of this range.  */
	gad->max = cur + got;
	gad->min = fdcur - fd;
	gad->fdplt = 0;

	/* If function descriptors took too much space, wrap some of them
	around.  */
	if (gad->min < wrapmin)
	{
		gad->max += wrapmin - gad->min;
		gad->min = wrapmin;
	}
	/* If there is space left and we have function descriptors
	referenced in PLT entries that could take advantage of shorter
	offsets, place them here.  */
	else if (fdplt && gad->min > wrapmin)
	{
		bfd_vma fds;
		if ((bfd_vma)(gad->min - wrapmin) < fdplt)
			fds = gad->min - wrapmin;
		else
			fds = fdplt;

		fdplt -= fds;
		gad->min -= fds;
		gad->fdplt += fds;
	}

	/* If GOT entries took too much space, wrap some of them around.
	This may well cause gad->min to become lower than wrapmin.  This
	will cause a relocation overflow later on, so we don't have to
	report it here . */
	if ((bfd_vma)gad->max > wrap)
	{
		gad->min -= gad->max - wrap;
		gad->max = wrap;
	}
	/* If there is more space left, try to place some more function
	descriptors for PLT entries.  */
	else if (fdplt && (bfd_vma)gad->max < wrap)
	{
		bfd_vma fds;
		if ((bfd_vma)(wrap - gad->max) < fdplt)
			fds = wrap - gad->max;
		else
			fds = fdplt;

		fdplt -= fds;
		gad->max += fds;
		gad->fdplt += fds;
	}

	/* If odd was initially computed as an offset past the wrap point,
	wrap it around.  */
	if (odd > gad->max)
		odd = gad->min + odd - gad->max;

	/* _esp32ulpfdpic_get_got_entry() below will always wrap gad->cur if needed
	before returning, so do it here too.  This guarantees that,
	should cur and fdcur meet at the wrap point, they'll both be
	equal to min.  */
	if (gad->cur == gad->max)
		gad->cur = gad->min;

	return odd;
}

/* Compute the location of the next GOT entry, given the allocation
data for a range.  */

inline static bfd_signed_vma
_esp32ulpfdpic_get_got_entry(struct _esp32ulpfdpic_dynamic_got_alloc_data *gad)
{
	bfd_signed_vma ret;

	if (gad->odd)
	{
		/* If there was an odd word left behind, use it.  */
		ret = gad->odd;
		gad->odd = 0;
	}
	else
	{
		/* Otherwise, use the word pointed to by cur, reserve the next
		as an odd word, and skip to the next pair of words, possibly
		wrapping around.  */
		ret = gad->cur;
		gad->odd = gad->cur + 4;
		gad->cur += 8;
		if (gad->cur == gad->max)
			gad->cur = gad->min;
	}

	return ret;
}

/* Compute the location of the next function descriptor entry in the
GOT, given the allocation data for a range.  */

inline static bfd_signed_vma
_esp32ulpfdpic_get_fd_entry(struct _esp32ulpfdpic_dynamic_got_alloc_data *gad)
{
	/* If we're at the bottom, wrap around, and only then allocate the
	next pair of words.  */
	if (gad->fdcur == gad->min)
		gad->fdcur = gad->max;
	return gad->fdcur -= 8;
}

/* Assign GOT offsets for every GOT entry and function descriptor.
Doing everything in a single pass is tricky.  */

static int
_esp32ulpfdpic_assign_got_entries(void **entryp, void *info_)
{
	struct esp32ulpfdpic_relocs_info *entry = *entryp;
	struct _esp32ulpfdpic_dynamic_got_plt_info *dinfo = info_;

	if (entry->got17m4)
		entry->got_entry = _esp32ulpfdpic_get_got_entry(&dinfo->got17m4);
	else if (entry->gothilo)
		entry->got_entry = _esp32ulpfdpic_get_got_entry(&dinfo->gothilo);

	if (entry->fdgot17m4)
		entry->fdgot_entry = _esp32ulpfdpic_get_got_entry(&dinfo->got17m4);
	else if (entry->fdgothilo)
		entry->fdgot_entry = _esp32ulpfdpic_get_got_entry(&dinfo->gothilo);

	if (entry->fdgoff17m4)
		entry->fd_entry = _esp32ulpfdpic_get_fd_entry(&dinfo->got17m4);
	else if (entry->plt && dinfo->got17m4.fdplt)
	{
		dinfo->got17m4.fdplt -= 8;
		entry->fd_entry = _esp32ulpfdpic_get_fd_entry(&dinfo->got17m4);
	}
	else if (entry->plt)
	{
		dinfo->gothilo.fdplt -= 8;
		entry->fd_entry = _esp32ulpfdpic_get_fd_entry(&dinfo->gothilo);
	}
	else if (entry->privfd)
		entry->fd_entry = _esp32ulpfdpic_get_fd_entry(&dinfo->gothilo);

	return 1;
}

/* Assign GOT offsets to private function descriptors used by PLT
entries (or referenced by 32-bit offsets), as well as PLT entries
and lazy PLT entries.  */

static int
_esp32ulpfdpic_assign_plt_entries(void **entryp, void *info_)
{
	struct esp32ulpfdpic_relocs_info *entry = *entryp;
	struct _esp32ulpfdpic_dynamic_got_plt_info *dinfo = info_;

	/* If this symbol requires a local function descriptor, allocate
	one.  */
	if (entry->privfd && entry->fd_entry == 0)
	{
		if (dinfo->got17m4.fdplt)
		{
			entry->fd_entry = _esp32ulpfdpic_get_fd_entry(&dinfo->got17m4);
			dinfo->got17m4.fdplt -= 8;
		}
		else
		{
			BFD_ASSERT(dinfo->gothilo.fdplt);
			entry->fd_entry = _esp32ulpfdpic_get_fd_entry(&dinfo->gothilo);
			dinfo->gothilo.fdplt -= 8;
		}
	}

	if (entry->plt)
	{
		int size;

		/* We use the section's raw size to mark the location of the
		next PLT entry.  */
		entry->plt_entry = esp32ulpfdpic_plt_section(dinfo->g.info)->size;

		/* Figure out the length of this PLT entry based on the
		addressing mode we need to reach the function descriptor.  */
		BFD_ASSERT(entry->fd_entry);
		if (entry->fd_entry >= -(1 << (18 - 1))
			&& entry->fd_entry + 4 < (1 << (18 - 1)))
			size = 10;
		else
			size = 16;

		esp32ulpfdpic_plt_section(dinfo->g.info)->size += size;
	}

	if (entry->lazyplt)
	{
		entry->lzplt_entry = dinfo->g.lzplt;
		dinfo->g.lzplt += LZPLT_NORMAL_SIZE;
		/* If this entry is the one that gets the resolver stub, account
		for the additional instruction.  */
		if (entry->lzplt_entry % ESP32ULPFDPIC_LZPLT_BLOCK_SIZE
			== ESP32ULPFDPIC_LZPLT_RESOLV_LOC)
			dinfo->g.lzplt += LZPLT_RESOLVER_EXTRA;
	}

	return 1;
}

/* Cancel out any effects of calling _esp32ulpfdpic_assign_got_entries and
_esp32ulpfdpic_assign_plt_entries.  */

static int
_esp32ulpfdpic_reset_got_plt_entries(void **entryp, void *ignore ATTRIBUTE_UNUSED)
{
	struct esp32ulpfdpic_relocs_info *entry = *entryp;

	entry->got_entry = 0;
	entry->fdgot_entry = 0;
	entry->fd_entry = 0;
	entry->plt_entry = (bfd_vma)-1;
	entry->lzplt_entry = (bfd_vma)-1;

	return 1;
}

/* Follow indirect and warning hash entries so that each got entry
points to the final symbol definition.  P must point to a pointer
to the hash table we're traversing.  Since this traversal may
modify the hash table, we set this pointer to NULL to indicate
we've made a potentially-destructive change to the hash table, so
the traversal must be restarted.  */
static int
_esp32ulpfdpic_resolve_final_relocs_info(void **entryp, void *p)
{
	struct esp32ulpfdpic_relocs_info *entry = *entryp;
	htab_t *htab = p;

	if (entry->symndx == -1)
	{
		struct elf_link_hash_entry *h = entry->d.h;
		struct esp32ulpfdpic_relocs_info *oentry;

		while (h->root.type == bfd_link_hash_indirect
			|| h->root.type == bfd_link_hash_warning)
			h = (struct elf_link_hash_entry *)h->root.u.i.link;

		if (entry->d.h == h)
			return 1;

		oentry = esp32ulpfdpic_relocs_info_for_global(*htab, 0, h, entry->addend,
			NO_INSERT);

		if (oentry)
		{
			/* Merge the two entries.  */
			esp32ulpfdpic_pic_merge_early_relocs_info(oentry, entry);
			htab_clear_slot(*htab, entryp);
			return 1;
		}

		entry->d.h = h;

		/* If we can't find this entry with the new bfd hash, re-insert
		it, and get the traversal restarted.  */
		if (!htab_find(*htab, entry))
		{
			htab_clear_slot(*htab, entryp);
			entryp = htab_find_slot(*htab, entry, INSERT);
			if (!*entryp)
				*entryp = entry;
			/* Abort the traversal, since the whole table may have
			moved, and leave it up to the parent to restart the
			process.  */
			*(htab_t *)p = NULL;
			return 0;
		}
	}

	return 1;
}

/* Compute the total size of the GOT, the PLT, the dynamic relocations
section and the rofixup section.  Assign locations for GOT and PLT
entries.  */

static bfd_boolean
_esp32ulpfdpic_size_got_plt(bfd *output_bfd,
struct _esp32ulpfdpic_dynamic_got_plt_info *gpinfop)
{
	bfd_signed_vma odd;
	bfd_vma limit;
	struct bfd_link_info *info = gpinfop->g.info;
	bfd *dynobj = elf_hash_table(info)->dynobj;

	memcpy(esp32ulpfdpic_dynamic_got_plt_info(info), &gpinfop->g,
		sizeof(gpinfop->g));

	odd = 12;
	/* Compute the total size taken by entries in the 18-bit range,
	to tell how many PLT function descriptors we can bring into it
	without causing it to overflow.  */
	limit = odd + gpinfop->g.got17m4 + gpinfop->g.fd17m4;
	if (limit < (bfd_vma)1 << 18)
		limit = ((bfd_vma)1 << 18) - limit;
	else
		limit = 0;
	if (gpinfop->g.fdplt < limit)
		limit = gpinfop->g.fdplt;

	/* Determine the ranges of GOT offsets that we can use for each
	range of addressing modes.  */
	odd = _esp32ulpfdpic_compute_got_alloc_data(&gpinfop->got17m4,
		0,
		odd,
		16,
		gpinfop->g.got17m4,
		gpinfop->g.fd17m4,
		limit,
		(bfd_vma)1 << (18 - 1));
	odd = _esp32ulpfdpic_compute_got_alloc_data(&gpinfop->gothilo,
		gpinfop->got17m4.min,
		odd,
		gpinfop->got17m4.max,
		gpinfop->g.gothilo,
		gpinfop->g.fdhilo,
		gpinfop->g.fdplt - gpinfop->got17m4.fdplt,
		(bfd_vma)1 << (32 - 1));

	/* Now assign (most) GOT offsets.  */
	htab_traverse(esp32ulpfdpic_relocs_info(info), _esp32ulpfdpic_assign_got_entries,
		gpinfop);

	esp32ulpfdpic_got_section(info)->size = gpinfop->gothilo.max
		- gpinfop->gothilo.min
		/* If an odd word is the last word of the GOT, we don't need this
		word to be part of the GOT.  */
		- (odd + 4 == gpinfop->gothilo.max ? 4 : 0);
	if (esp32ulpfdpic_got_section(info)->size == 0)
		esp32ulpfdpic_got_section(info)->flags |= SEC_EXCLUDE;
	else if (esp32ulpfdpic_got_section(info)->size == 12
		&& !elf_hash_table(info)->dynamic_sections_created)
	{
		esp32ulpfdpic_got_section(info)->flags |= SEC_EXCLUDE;
		esp32ulpfdpic_got_section(info)->size = 0;
	}
	else
	{
		esp32ulpfdpic_got_section(info)->contents =
			(bfd_byte *)bfd_zalloc(dynobj,
			esp32ulpfdpic_got_section(info)->size);
		if (esp32ulpfdpic_got_section(info)->contents == NULL)
			return FALSE;
	}

	if (elf_hash_table(info)->dynamic_sections_created)
		/* Subtract the number of lzplt entries, since those will generate
		relocations in the pltrel section.  */
		esp32ulpfdpic_gotrel_section(info)->size =
		(gpinfop->g.relocs - gpinfop->g.lzplt / LZPLT_NORMAL_SIZE)
		* get_elf_backend_data(output_bfd)->s->sizeof_rel;
	else
		BFD_ASSERT(gpinfop->g.relocs == 0);
	if (esp32ulpfdpic_gotrel_section(info)->size == 0)
		esp32ulpfdpic_gotrel_section(info)->flags |= SEC_EXCLUDE;
	else
	{
		esp32ulpfdpic_gotrel_section(info)->contents =
			(bfd_byte *)bfd_zalloc(dynobj,
			esp32ulpfdpic_gotrel_section(info)->size);
		if (esp32ulpfdpic_gotrel_section(info)->contents == NULL)
			return FALSE;
	}

	esp32ulpfdpic_gotfixup_section(info)->size = (gpinfop->g.fixups + 1) * 4;
	if (esp32ulpfdpic_gotfixup_section(info)->size == 0)
		esp32ulpfdpic_gotfixup_section(info)->flags |= SEC_EXCLUDE;
	else
	{
		esp32ulpfdpic_gotfixup_section(info)->contents =
			(bfd_byte *)bfd_zalloc(dynobj,
			esp32ulpfdpic_gotfixup_section(info)->size);
		if (esp32ulpfdpic_gotfixup_section(info)->contents == NULL)
			return FALSE;
	}

	if (elf_hash_table(info)->dynamic_sections_created)
		esp32ulpfdpic_pltrel_section(info)->size =
		gpinfop->g.lzplt / LZPLT_NORMAL_SIZE * get_elf_backend_data(output_bfd)->s->sizeof_rel;
	if (esp32ulpfdpic_pltrel_section(info)->size == 0)
		esp32ulpfdpic_pltrel_section(info)->flags |= SEC_EXCLUDE;
	else
	{
		esp32ulpfdpic_pltrel_section(info)->contents =
			(bfd_byte *)bfd_zalloc(dynobj,
			esp32ulpfdpic_pltrel_section(info)->size);
		if (esp32ulpfdpic_pltrel_section(info)->contents == NULL)
			return FALSE;
	}

	/* Add 4 bytes for every block of at most 65535 lazy PLT entries,
	such that there's room for the additional instruction needed to
	call the resolver.  Since _esp32ulpfdpic_assign_got_entries didn't
	account for them, our block size is 4 bytes smaller than the real
	block size.  */
	if (elf_hash_table(info)->dynamic_sections_created)
	{
		esp32ulpfdpic_plt_section(info)->size = gpinfop->g.lzplt
			+ ((gpinfop->g.lzplt + (ESP32ULPFDPIC_LZPLT_BLOCK_SIZE - 4) - LZPLT_NORMAL_SIZE)
			/ (ESP32ULPFDPIC_LZPLT_BLOCK_SIZE - 4) * LZPLT_RESOLVER_EXTRA);
	}

	/* Reset it, such that _esp32ulpfdpic_assign_plt_entries() can use it to
	actually assign lazy PLT entries addresses.  */
	gpinfop->g.lzplt = 0;

	/* Save information that we're going to need to generate GOT and PLT
	entries.  */
	esp32ulpfdpic_got_initial_offset(info) = -gpinfop->gothilo.min;

	if (get_elf_backend_data(output_bfd)->want_got_sym)
		elf_hash_table(info)->hgot->root.u.def.value
		= esp32ulpfdpic_got_initial_offset(info);

	if (elf_hash_table(info)->dynamic_sections_created)
		esp32ulpfdpic_plt_initial_offset(info) =
		esp32ulpfdpic_plt_section(info)->size;

	htab_traverse(esp32ulpfdpic_relocs_info(info), _esp32ulpfdpic_assign_plt_entries,
		gpinfop);

	/* Allocate the PLT section contents only after
	_esp32ulpfdpic_assign_plt_entries has a chance to add the size of the
	non-lazy PLT entries.  */
	if (esp32ulpfdpic_plt_section(info)->size == 0)
		esp32ulpfdpic_plt_section(info)->flags |= SEC_EXCLUDE;
	else
	{
		esp32ulpfdpic_plt_section(info)->contents =
			(bfd_byte *)bfd_zalloc(dynobj,
			esp32ulpfdpic_plt_section(info)->size);
		if (esp32ulpfdpic_plt_section(info)->contents == NULL)
			return FALSE;
	}

	return TRUE;
}

/* Set the sizes of the dynamic sections.  */

static bfd_boolean
elf32_esp32ulpfdpic_size_dynamic_sections(bfd *output_bfd,
struct bfd_link_info *info)
{
	struct elf_link_hash_table *htab;
	bfd *dynobj;
	asection *s;
	struct _esp32ulpfdpic_dynamic_got_plt_info gpinfo;

	htab = elf_hash_table(info);
	dynobj = htab->dynobj;
	BFD_ASSERT(dynobj != NULL);

	if (htab->dynamic_sections_created)
	{
		/* Set the contents of the .interp section to the interpreter.  */
		if (bfd_link_executable(info) && !info->nointerp)
		{
			s = bfd_get_linker_section(dynobj, ".interp");
			BFD_ASSERT(s != NULL);
			s->size = sizeof ELF_DYNAMIC_INTERPRETER;
			s->contents = (bfd_byte *)ELF_DYNAMIC_INTERPRETER;
		}
	}

	memset(&gpinfo, 0, sizeof(gpinfo));
	gpinfo.g.info = info;

	for (;;)
	{
		htab_t relocs = esp32ulpfdpic_relocs_info(info);

		htab_traverse(relocs, _esp32ulpfdpic_resolve_final_relocs_info, &relocs);

		if (relocs == esp32ulpfdpic_relocs_info(info))
			break;
	}

	htab_traverse(esp32ulpfdpic_relocs_info(info), _esp32ulpfdpic_count_got_plt_entries,
		&gpinfo.g);

	/* Allocate space to save the summary information, we're going to
	use it if we're doing relaxations.  */
	esp32ulpfdpic_dynamic_got_plt_info(info) = bfd_alloc(dynobj, sizeof(gpinfo.g));

	if (!_esp32ulpfdpic_size_got_plt(output_bfd, &gpinfo))
		return FALSE;

	if (elf_hash_table(info)->dynamic_sections_created)
	{
		if (esp32ulpfdpic_got_section(info)->size)
			if (!_bfd_elf_add_dynamic_entry(info, DT_PLTGOT, 0))
				return FALSE;

		if (esp32ulpfdpic_pltrel_section(info)->size)
			if (!_bfd_elf_add_dynamic_entry(info, DT_PLTRELSZ, 0)
				|| !_bfd_elf_add_dynamic_entry(info, DT_PLTREL, DT_REL)
				|| !_bfd_elf_add_dynamic_entry(info, DT_JMPREL, 0))
				return FALSE;

		if (esp32ulpfdpic_gotrel_section(info)->size)
			if (!_bfd_elf_add_dynamic_entry(info, DT_REL, 0)
				|| !_bfd_elf_add_dynamic_entry(info, DT_RELSZ, 0)
				|| !_bfd_elf_add_dynamic_entry(info, DT_RELENT,
				sizeof(Elf32_External_Rel)))
				return FALSE;
	}

	s = bfd_get_linker_section(dynobj, ".dynbss");
	if (s && s->size == 0)
		s->flags |= SEC_EXCLUDE;

	s = bfd_get_linker_section(dynobj, ".rela.bss");
	if (s && s->size == 0)
		s->flags |= SEC_EXCLUDE;

	return TRUE;
}

static bfd_boolean
elf32_esp32ulpfdpic_always_size_sections(bfd *output_bfd,
struct bfd_link_info *info)
{
	if (!bfd_link_relocatable(info)
		&& !bfd_elf_stack_segment_size(output_bfd, info,
		"__stacksize", DEFAULT_STACK_SIZE))
		return FALSE;

	return TRUE;
}

/* Check whether any of the relocations was optimized away, and
subtract it from the relocation or fixup count.  */
static bfd_boolean
_esp32ulpfdpic_check_discarded_relocs(bfd *abfd, asection *sec,
struct bfd_link_info *info,
	bfd_boolean *changed)
{
	(void)info;// dya - remove warning
	(void)changed;// dya - remove warning
	Elf_Internal_Shdr *symtab_hdr;
	struct elf_link_hash_entry **sym_hashes, **sym_hashes_end;

	if ((sec->flags & SEC_RELOC) == 0
		|| sec->reloc_count == 0)
		return TRUE;

	symtab_hdr = &elf_tdata(abfd)->symtab_hdr;
	sym_hashes = elf_sym_hashes(abfd);
	sym_hashes_end = sym_hashes + symtab_hdr->sh_size / sizeof(Elf32_External_Sym);
	if (!elf_bad_symtab(abfd))
		sym_hashes_end -= symtab_hdr->sh_info;

	return TRUE;
}

static bfd_boolean
esp32ulpfdpic_elf_discard_info(bfd *ibfd,
struct elf_reloc_cookie *cookie ATTRIBUTE_UNUSED,
struct bfd_link_info *info)
{
	bfd_boolean changed = FALSE;
	asection *s;
	bfd *obfd = NULL;

	/* Account for relaxation of .eh_frame section.  */
	for (s = ibfd->sections; s; s = s->next)
		if (s->sec_info_type == SEC_INFO_TYPE_EH_FRAME)
		{
			if (!_esp32ulpfdpic_check_discarded_relocs(ibfd, s, info, &changed))
				return FALSE;
			obfd = s->output_section->owner;
		}

	if (changed)
	{
		struct _esp32ulpfdpic_dynamic_got_plt_info gpinfo;

		memset(&gpinfo, 0, sizeof(gpinfo));
		memcpy(&gpinfo.g, esp32ulpfdpic_dynamic_got_plt_info(info),
			sizeof(gpinfo.g));

		/* Clear GOT and PLT assignments.  */
		htab_traverse(esp32ulpfdpic_relocs_info(info),
			_esp32ulpfdpic_reset_got_plt_entries,
			NULL);

		if (!_esp32ulpfdpic_size_got_plt(obfd, &gpinfo))
			return FALSE;
	}

	return TRUE;
}

static bfd_boolean
elf32_esp32ulpfdpic_finish_dynamic_sections(bfd *output_bfd,
struct bfd_link_info *info)
{
	bfd *dynobj;
	asection *sdyn;

	dynobj = elf_hash_table(info)->dynobj;

	if (esp32ulpfdpic_got_section(info))
	{
		BFD_ASSERT(esp32ulpfdpic_gotrel_section(info)->size
			/* PR 17334: It appears that the GOT section can end up
			being bigger than the number of relocs.  Presumably
			because some relocs have been deleted.  A test case has
			yet to be generated for verify this, but in the meantime
			the test below has been changed from == to >= so that
			applications can continue to be built.  */
			>= (esp32ulpfdpic_gotrel_section(info)->reloc_count
			* sizeof(Elf32_External_Rel)));

		if (esp32ulpfdpic_gotfixup_section(info))
		{
			struct elf_link_hash_entry *hgot = elf_hash_table(info)->hgot;
			bfd_vma got_value = hgot->root.u.def.value
				+ hgot->root.u.def.section->output_section->vma
				+ hgot->root.u.def.section->output_offset;

			_esp32ulpfdpic_add_rofixup(output_bfd, esp32ulpfdpic_gotfixup_section(info),
				got_value, 0);

			if (esp32ulpfdpic_gotfixup_section(info)->size
				!= (esp32ulpfdpic_gotfixup_section(info)->reloc_count * 4))
			{
				_bfd_error_handler
					("LINKER BUG: .rofixup section size mismatch");
				return FALSE;
			}
		}
	}
	if (elf_hash_table(info)->dynamic_sections_created)
	{
		BFD_ASSERT(esp32ulpfdpic_pltrel_section(info)->size
			== (esp32ulpfdpic_pltrel_section(info)->reloc_count
			* sizeof(Elf32_External_Rel)));
	}

	sdyn = bfd_get_linker_section(dynobj, ".dynamic");

	if (elf_hash_table(info)->dynamic_sections_created)
	{
		Elf32_External_Dyn * dyncon;
		Elf32_External_Dyn * dynconend;

		BFD_ASSERT(sdyn != NULL);

		dyncon = (Elf32_External_Dyn *)sdyn->contents;
		dynconend = (Elf32_External_Dyn *)(sdyn->contents + sdyn->size);

		for (; dyncon < dynconend; dyncon++)
		{
			Elf_Internal_Dyn dyn;

			bfd_elf32_swap_dyn_in(dynobj, dyncon, &dyn);

			switch (dyn.d_tag)
			{
			default:
				break;

			case DT_PLTGOT:
				dyn.d_un.d_ptr = esp32ulpfdpic_got_section(info)->output_section->vma
					+ esp32ulpfdpic_got_section(info)->output_offset
					+ esp32ulpfdpic_got_initial_offset(info);
				bfd_elf32_swap_dyn_out(output_bfd, &dyn, dyncon);
				break;

			case DT_JMPREL:
				dyn.d_un.d_ptr = esp32ulpfdpic_pltrel_section(info)
					->output_section->vma
					+ esp32ulpfdpic_pltrel_section(info)->output_offset;
				bfd_elf32_swap_dyn_out(output_bfd, &dyn, dyncon);
				break;

			case DT_PLTRELSZ:
				dyn.d_un.d_val = esp32ulpfdpic_pltrel_section(info)->size;
				bfd_elf32_swap_dyn_out(output_bfd, &dyn, dyncon);
				break;
			}
		}
	}

	return TRUE;
}

/* Adjust a symbol defined by a dynamic object and referenced by a
regular object.  */

static bfd_boolean
elf32_esp32ulpfdpic_adjust_dynamic_symbol(struct bfd_link_info *info,
struct elf_link_hash_entry *h)
{
	bfd * dynobj;

	dynobj = elf_hash_table(info)->dynobj;

	/* Make sure we know what is going on here.  */
	BFD_ASSERT(dynobj != NULL
		&& (h->u.weakdef != NULL
		|| (h->def_dynamic
		&& h->ref_regular
		&& !h->def_regular)));

	/* If this is a weak symbol, and there is a real definition, the
	processor independent code will have arranged for us to see the
	real definition first, and we can just use the same value.  */
	if (h->u.weakdef != NULL)
	{
		BFD_ASSERT(h->u.weakdef->root.type == bfd_link_hash_defined
			|| h->u.weakdef->root.type == bfd_link_hash_defweak);
		h->root.u.def.section = h->u.weakdef->root.u.def.section;
		h->root.u.def.value = h->u.weakdef->root.u.def.value;
	}

	return TRUE;
}

/* Perform any actions needed for dynamic symbols.  */

static bfd_boolean
elf32_esp32ulpfdpic_finish_dynamic_symbol
(bfd *output_bfd ATTRIBUTE_UNUSED,
struct bfd_link_info *info ATTRIBUTE_UNUSED,
struct elf_link_hash_entry *h ATTRIBUTE_UNUSED,
	Elf_Internal_Sym *sym ATTRIBUTE_UNUSED)
{
	return TRUE;
}

/* Decide whether to attempt to turn absptr or lsda encodings in
shared libraries into pcrel within the given input section.  */

static bfd_boolean
esp32ulpfdpic_elf_use_relative_eh_frame
(bfd *input_bfd ATTRIBUTE_UNUSED,
struct bfd_link_info *info ATTRIBUTE_UNUSED,
	asection *eh_frame_section ATTRIBUTE_UNUSED)
{
	/* We can't use PC-relative encodings in FDPIC binaries, in general.  */
	return FALSE;
}

/* Adjust the contents of an eh_frame_hdr section before they're output.  */

static bfd_byte
esp32ulpfdpic_elf_encode_eh_address(bfd *abfd,
struct bfd_link_info *info,
	asection *osec, bfd_vma offset,
	asection *loc_sec, bfd_vma loc_offset,
	bfd_vma *encoded)
{
	struct elf_link_hash_entry *h;

	h = elf_hash_table(info)->hgot;
	BFD_ASSERT(h && h->root.type == bfd_link_hash_defined);

	if (!h || (_esp32ulpfdpic_osec_to_segment(abfd, osec)
		== _esp32ulpfdpic_osec_to_segment(abfd, loc_sec->output_section)))
		return _bfd_elf_encode_eh_address(abfd, info, osec, offset,
		loc_sec, loc_offset, encoded);

	BFD_ASSERT(_esp32ulpfdpic_osec_to_segment(abfd, osec)
		== (_esp32ulpfdpic_osec_to_segment
		(abfd, h->root.u.def.section->output_section)));

	*encoded = osec->vma + offset
		- (h->root.u.def.value
		+ h->root.u.def.section->output_section->vma
		+ h->root.u.def.section->output_offset);

	return DW_EH_PE_datarel | DW_EH_PE_sdata4;
}



/* Look through the relocs for a section during the first phase.

Besides handling virtual table relocs for gc, we have to deal with
all sorts of PIC-related relocations.  We describe below the
general plan on how to handle such relocations, even though we only
collect information at this point, storing them in hash tables for
perusal of later passes.

32 relocations are propagated to the linker output when creating
position-independent output.  LO16 and HI16 relocations are not
supposed to be encountered in this case.

LABEL16 should always be resolvable by the linker, since it's only
used by branches.

LABEL24, on the other hand, is used by calls.  If it turns out that
the target of a call is a dynamic symbol, a PLT entry must be
created for it, which triggers the creation of a private function
descriptor and, unless lazy binding is disabled, a lazy PLT entry.

GPREL relocations require the referenced symbol to be in the same
segment as _gp, but this can only be checked later.

All GOT, GOTOFF and FUNCDESC relocations require a .got section to
exist.  LABEL24 might as well, since it may require a PLT entry,
that will require a got.

Non-FUNCDESC GOT relocations require a GOT entry to be created
regardless of whether the symbol is dynamic.  However, since a
global symbol that turns out to not be exported may have the same
address of a non-dynamic symbol, we don't assign GOT entries at
this point, such that we can share them in this case.  A relocation
for the GOT entry always has to be created, be it to offset a
private symbol by the section load address, be it to get the symbol
resolved dynamically.

FUNCDESC GOT relocations require a GOT entry to be created, and
handled as if a FUNCDESC relocation was applied to the GOT entry in
an object file.

FUNCDESC relocations referencing a symbol that turns out to NOT be
dynamic cause a private function descriptor to be created.  The
FUNCDESC relocation then decays to a 32 relocation that points at
the private descriptor.  If the symbol is dynamic, the FUNCDESC
relocation is propagated to the linker output, such that the
dynamic linker creates the canonical descriptor, pointing to the
dynamically-resolved definition of the function.

Non-FUNCDESC GOTOFF relocations must always refer to non-dynamic
symbols that are assigned to the same segment as the GOT, but we
can only check this later, after we know the complete set of
symbols defined and/or exported.

FUNCDESC GOTOFF relocations require a function descriptor to be
created and, unless lazy binding is disabled or the symbol is not
dynamic, a lazy PLT entry.  Since we can't tell at this point
whether a symbol is going to be dynamic, we have to decide later
whether to create a lazy PLT entry or bind the descriptor directly
to the private function.

FUNCDESC_VALUE relocations are not supposed to be present in object
files, but they may very well be simply propagated to the linker
output, since they have no side effect.


A function descriptor always requires a FUNCDESC_VALUE relocation.
Whether it's in .plt.rel or not depends on whether lazy binding is
enabled and on whether the referenced symbol is dynamic.

The existence of a lazy PLT requires the resolverStub lazy PLT
entry to be present.


As for assignment of GOT, PLT and lazy PLT entries, and private
descriptors, we might do them all sequentially, but we can do
better than that.  For example, we can place GOT entries and
private function descriptors referenced using 12-bit operands
closer to the PIC register value, such that these relocations don't
overflow.  Those that are only referenced with LO16 relocations
could come next, but we may as well place PLT-required function
descriptors in the 12-bit range to make them shorter.  Symbols
referenced with LO16/HI16 may come next, but we may place
additional function descriptors in the 16-bit range if we can
reliably tell that we've already placed entries that are ever
referenced with only LO16.  PLT entries are therefore generated as
small as possible, while not introducing relocation overflows in
GOT or FUNCDESC_GOTOFF relocations.  Lazy PLT entries could be
generated before or after PLT entries, but not intermingled with
them, such that we can have more lazy PLT entries in range for a
branch to the resolverStub.  The resolverStub should be emitted at
the most distant location from the first lazy PLT entry such that
it's still in range for a branch, or closer, if there isn't a need
for so many lazy PLT entries.  Additional lazy PLT entries may be
emitted after the resolverStub, as long as branches are still in
range.  If the branch goes out of range, longer lazy PLT entries
are emitted.

We could further optimize PLT and lazy PLT entries by giving them
priority in assignment to closer-to-gr17 locations depending on the
number of occurrences of references to them (assuming a function
that's called more often is more important for performance, so its
PLT entry should be faster), or taking hints from the compiler.
Given infinite time and money... :-)  */

static bfd_boolean
esp32ulpfdpic_check_relocs(bfd *abfd, struct bfd_link_info *info,
asection *sec, const Elf_Internal_Rela *relocs)
{
	Elf_Internal_Shdr *symtab_hdr;
	struct elf_link_hash_entry **sym_hashes;
	const Elf_Internal_Rela *rel;
	const Elf_Internal_Rela *rel_end;

	if (bfd_link_relocatable(info))
		return TRUE;

	symtab_hdr = &elf_tdata(abfd)->symtab_hdr;
	sym_hashes = elf_sym_hashes(abfd);

	rel_end = relocs + sec->reloc_count;
	for (rel = relocs; rel < rel_end; rel++)
	{
		struct elf_link_hash_entry *h;
		unsigned long r_symndx;

		r_symndx = ELF32_R_SYM(rel->r_info);
		if (r_symndx < symtab_hdr->sh_info)
			h = NULL;
		else
			h = sym_hashes[r_symndx - symtab_hdr->sh_info];

		switch (ELF32_R_TYPE(rel->r_info))
		{
			/* This relocation describes the C++ object vtable hierarchy.
			Reconstruct it for later use during GC.  */
		case R_ESP32ULP_GNU_VTINHERIT:
			if (!bfd_elf_gc_record_vtinherit(abfd, sec, h, rel->r_offset))
				return FALSE;
			break;

			/* This relocation describes which C++ vtable entries are actually
			used.  Record for later use during GC.  */
		case R_ESP32ULP_GNU_VTENTRY:
			BFD_ASSERT(h != NULL);
			if (h != NULL
				&& !bfd_elf_gc_record_vtentry(abfd, sec, h, rel->r_addend))
				return FALSE;
			break;

		case R_ESP32ULP_JUMPR:
			break;

		default:
			_bfd_error_handler
				/* xgettext:c-format */
				(_("%B: unsupported relocation type %i"),
				abfd, ELF32_R_TYPE(rel->r_info));
			return FALSE;
		}
	}

	return TRUE;
}

/* Set the right machine number for a Blackfin ELF file.  */

static bfd_boolean
elf32_esp32ulp_object_p(bfd *abfd)
{
	bfd_default_set_arch_mach(abfd, bfd_arch_esp32ulp, 0);
	return (((elf_elfheader(abfd)->e_flags & EF_ESP32ULP_FDPIC) != 0)
		== (IS_FDPIC(abfd)));
}

static bfd_boolean
elf32_esp32ulp_set_private_flags(bfd * abfd, flagword flags)
{
	elf_elfheader(abfd)->e_flags = flags;
	elf_flags_init(abfd) = TRUE;
	return TRUE;
}

/* Display the flags field.  */
static bfd_boolean
elf32_esp32ulp_print_private_bfd_data(bfd * abfd, void * ptr)
{
	FILE *file = (FILE *)ptr;
	flagword flags;

	BFD_ASSERT(abfd != NULL && ptr != NULL);

	/* Print normal ELF private data.  */
	_bfd_elf_print_private_bfd_data(abfd, ptr);

	flags = elf_elfheader(abfd)->e_flags;

	/* xgettext:c-format */
	fprintf(file, _("private flags = %lx:"), elf_elfheader(abfd)->e_flags);

	if (flags & EF_ESP32ULP_PIC)
		fprintf(file, " -fpic");

	if (flags & EF_ESP32ULP_FDPIC)
		fprintf(file, " -mfdpic");

	fputc('\n', file);

	return TRUE;
}

/* Merge backend specific data from an object file to the output
object file when linking.  */

static bfd_boolean
elf32_esp32ulp_merge_private_bfd_data(bfd *ibfd, struct bfd_link_info *info)
{
	bfd *obfd = info->output_bfd;
	flagword old_flags, new_flags;
	bfd_boolean error = FALSE;

	new_flags = elf_elfheader(ibfd)->e_flags;
	old_flags = elf_elfheader(obfd)->e_flags;

	if (new_flags & EF_ESP32ULP_FDPIC)
		new_flags &= ~EF_ESP32ULP_PIC;

#ifndef DEBUG
	if (0)
#endif
		_bfd_error_handler
		("old_flags = 0x%.8lx, new_flags = 0x%.8lx, init = %s, filename = %s",
		old_flags, new_flags, elf_flags_init(obfd) ? "yes" : "no",
		bfd_get_filename(ibfd));

	if (!elf_flags_init(obfd))			/* First call, no flags set.  */
	{
		elf_flags_init(obfd) = TRUE;
		elf_elfheader(obfd)->e_flags = new_flags;
	}

	if (((new_flags & EF_ESP32ULP_FDPIC) == 0) != (!IS_FDPIC(obfd)))
	{
		error = TRUE;
		if (IS_FDPIC(obfd))
			_bfd_error_handler
			(_("%s: cannot link non-fdpic object file into fdpic executable"),
			bfd_get_filename(ibfd));
		else
			_bfd_error_handler
			(_("%s: cannot link fdpic object file into non-fdpic executable"),
			bfd_get_filename(ibfd));
	}

	if (error)
		bfd_set_error(bfd_error_bad_value);

	return !error;
}

/* esp32ulp ELF linker hash entry.  */

struct esp32ulp_link_hash_entry
{
	struct elf_link_hash_entry root;

	/* Number of PC relative relocs copied for this symbol.  */
	struct esp32ulp_pcrel_relocs_copied *pcrel_relocs_copied;
};

/* esp32ulp ELF linker hash table.  */

struct esp32ulp_link_hash_table
{
	struct elf_link_hash_table root;

	/* Small local sym cache.  */
	struct sym_cache sym_cache;
};

#define esp32ulp_hash_entry(ent) ((struct esp32ulp_link_hash_entry *) (ent))

static struct bfd_hash_entry *
esp32ulp_link_hash_newfunc(struct bfd_hash_entry *entry,
struct bfd_hash_table *table, const char *string)
{
	struct bfd_hash_entry *ret = entry;

	/* Allocate the structure if it has not already been allocated by a
	subclass.  */
	if (ret == NULL)
		ret = bfd_hash_allocate(table, sizeof(struct esp32ulp_link_hash_entry));
	if (ret == NULL)
		return ret;

	/* Call the allocation method of the superclass.  */
	ret = _bfd_elf_link_hash_newfunc(ret, table, string);
	if (ret != NULL)
		esp32ulp_hash_entry(ret)->pcrel_relocs_copied = NULL;

	return ret;
}

/* Create an esp32ulp ELF linker hash table.  */

static struct bfd_link_hash_table *
esp32ulp_link_hash_table_create(bfd * abfd)
{
	struct esp32ulp_link_hash_table *ret;
	bfd_size_type amt = sizeof(struct esp32ulp_link_hash_table);

	ret = bfd_zmalloc(amt);
	if (ret == NULL)
		return NULL;

	if (!_bfd_elf_link_hash_table_init(&ret->root, abfd,
		esp32ulp_link_hash_newfunc,
		sizeof(struct elf_link_hash_entry),
		ESP32ULP_ELF_DATA))
	{
		free(ret);
		return NULL;
	}

	ret->sym_cache.abfd = NULL;

	return &ret->root.root;
}

/* The size in bytes of an entry in the procedure linkage table.  */

/* Finish up the dynamic sections.  */

static bfd_boolean
esp32ulp_finish_dynamic_sections(bfd * output_bfd ATTRIBUTE_UNUSED,
struct bfd_link_info *info)
{
	bfd *dynobj;
	asection *sdyn;

	dynobj = elf_hash_table(info)->dynobj;

	sdyn = bfd_get_linker_section(dynobj, ".dynamic");

	if (elf_hash_table(info)->dynamic_sections_created)
	{
		Elf32_External_Dyn *dyncon, *dynconend;

		BFD_ASSERT(sdyn != NULL);

		dyncon = (Elf32_External_Dyn *)sdyn->contents;
		dynconend = (Elf32_External_Dyn *)(sdyn->contents + sdyn->size);
		for (; dyncon < dynconend; dyncon++)
		{
			Elf_Internal_Dyn dyn;

			bfd_elf32_swap_dyn_in(dynobj, dyncon, &dyn);

		}

	}
	return TRUE;
}

/* Finish up dynamic symbol handling.  We set the contents of various
dynamic sections here.  */

static bfd_boolean
esp32ulp_finish_dynamic_symbol(bfd * output_bfd,
struct bfd_link_info *info,
struct elf_link_hash_entry *h,
	Elf_Internal_Sym * sym)
{
	if (h->got.offset != (bfd_vma)-1)
	{
		asection *sgot;
		asection *srela;
		Elf_Internal_Rela rela;
		bfd_byte *loc;

		/* This symbol has an entry in the global offset table.
		Set it up.  */

		sgot = elf_hash_table(info)->sgot;
		srela = elf_hash_table(info)->srelgot;
		BFD_ASSERT(sgot != NULL && srela != NULL);

		rela.r_offset = (sgot->output_section->vma
			+ sgot->output_offset
			+ (h->got.offset & ~(bfd_vma)1));

		/* If this is a -Bsymbolic link, and the symbol is defined
		locally, we just want to emit a RELATIVE reloc.  Likewise if
		the symbol was forced to be local because of a version file.
		The entry in the global offset table will already have been
		initialized in the relocate_section function.  */
		if (bfd_link_pic(info)
			&& (info->symbolic
			|| h->dynindx == -1 || h->forced_local) && h->def_regular)
		{
			_bfd_error_handler(_("*** check this relocation %s"),
				__FUNCTION__);
			rela.r_info = ELF32_R_INFO(0, R_ESP32ULP_RIMM16);
			rela.r_addend = bfd_get_signed_32(output_bfd,
				(sgot->contents
				+
				(h->got.
				offset & ~(bfd_vma)1)));
		}
		else
		{
			bfd_put_32(output_bfd, (bfd_vma)0,
				sgot->contents + (h->got.offset & ~(bfd_vma)1));
			rela.r_info = ELF32_R_INFO(h->dynindx, R_ESP32ULP_GOT);
			rela.r_addend = 0;
		}

		loc = srela->contents;
		loc += srela->reloc_count++ * sizeof(Elf32_External_Rela);
		bfd_elf32_swap_reloca_out(output_bfd, &rela, loc);
	}

	if (h->needs_copy)
	{
		BFD_ASSERT(0);
	}
	/* Mark _DYNAMIC and _GLOBAL_OFFSET_TABLE_ as absolute.  */
	if (strcmp(h->root.root.string, "__DYNAMIC") == 0
		|| h == elf_hash_table(info)->hgot)
		sym->st_shndx = SHN_ABS;

	return TRUE;
}

/* Adjust a symbol defined by a dynamic object and referenced by a
regular object.  The current definition is in some section of the
dynamic object, but we're not including those sections.  We have to
change the definition to something the rest of the link can
understand.  */

static bfd_boolean
esp32ulp_adjust_dynamic_symbol(struct bfd_link_info *info,
struct elf_link_hash_entry *h)
{
	bfd *dynobj;
	asection *s;
	unsigned int power_of_two;

	dynobj = elf_hash_table(info)->dynobj;

	/* Make sure we know what is going on here.  */
	BFD_ASSERT(dynobj != NULL
		&& (h->needs_plt
		|| h->u.weakdef != NULL
		|| (h->def_dynamic && h->ref_regular && !h->def_regular)));

	/* If this is a function, put it in the procedure linkage table.  We
	will fill in the contents of the procedure linkage table later,
	when we know the address of the .got section.  */
	if (h->type == STT_FUNC || h->needs_plt)
	{
		BFD_ASSERT(0);
	}

	/* If this is a weak symbol, and there is a real definition, the
	processor independent code will have arranged for us to see the
	real definition first, and we can just use the same value.  */
	if (h->u.weakdef != NULL)
	{
		BFD_ASSERT(h->u.weakdef->root.type == bfd_link_hash_defined
			|| h->u.weakdef->root.type == bfd_link_hash_defweak);
		h->root.u.def.section = h->u.weakdef->root.u.def.section;
		h->root.u.def.value = h->u.weakdef->root.u.def.value;
		return TRUE;
	}

	/* This is a reference to a symbol defined by a dynamic object which
	is not a function.  */

	/* If we are creating a shared library, we must presume that the
	only references to the symbol are via the global offset table.
	For such cases we need not do anything here; the relocations will
	be handled correctly by relocate_section.  */
	if (bfd_link_pic(info))
		return TRUE;

	/* We must allocate the symbol in our .dynbss section, which will
	become part of the .bss section of the executable.  There will be
	an entry for this symbol in the .dynsym section.  The dynamic
	object will contain position independent code, so all references
	from the dynamic object to this symbol will go through the global
	offset table.  The dynamic linker will use the .dynsym entry to
	determine the address it must put in the global offset table, so
	both the dynamic object and the regular object will refer to the
	same memory location for the variable.  */

	s = bfd_get_linker_section(dynobj, ".dynbss");
	BFD_ASSERT(s != NULL);

#if 0 /* Bfin does not currently have a COPY reloc.  */
	/* We must generate a R_ESP32ULP_COPY reloc to tell the dynamic linker to
	copy the initial value out of the dynamic object and into the
	runtime process image.  We need to remember the offset into the
	.rela.bss section we are going to use.  */
	if ((h->root.u.def.section->flags & SEC_ALLOC) != 0)
	{
		asection *srel;

		srel = bfd_get_linker_section(dynobj, ".rela.bss");
		BFD_ASSERT(srel != NULL);
		srel->size += sizeof(Elf32_External_Rela);
		h->needs_copy = 1;
	}
#else
	if ((h->root.u.def.section->flags & SEC_ALLOC) != 0)
	{
		_bfd_error_handler(_("the esp32ulp target does not currently support the generation of copy relocations"));
		return FALSE;
	}
#endif
	/* We need to figure out the alignment required for this symbol.  I
	have no idea how ELF linkers handle this.  */
	power_of_two = bfd_log2(h->size);
	if (power_of_two > 3)
		power_of_two = 3;

	/* Apply the required alignment.  */
	s->size = BFD_ALIGN(s->size, (bfd_size_type)(1 << power_of_two));
	if (power_of_two > bfd_get_section_alignment(dynobj, s))
	{
		if (!bfd_set_section_alignment(dynobj, s, power_of_two))
			return FALSE;
	}

	/* Define the symbol as being at this point in the section.  */
	h->root.u.def.section = s;
	h->root.u.def.value = s->size;

	/* Increment the section size to make room for the symbol.  */
	s->size += h->size;

	return TRUE;
}

/* The esp32ulp linker needs to keep track of the number of relocs that it
decides to copy in check_relocs for each symbol.  This is so that it
can discard PC relative relocs if it doesn't need them when linking
with -Bsymbolic.  We store the information in a field extending the
regular ELF linker hash table.  */

/* This structure keeps track of the number of PC relative relocs we have
copied for a given symbol.  */

struct esp32ulp_pcrel_relocs_copied
{
	/* Next section.  */
	struct esp32ulp_pcrel_relocs_copied *next;
	/* A section in dynobj.  */
	asection *section;
	/* Number of relocs copied in this section.  */
	bfd_size_type count;
};

/* This function is called via elf_link_hash_traverse if we are
creating a shared object.  In the -Bsymbolic case it discards the
space allocated to copy PC relative relocs against symbols which
are defined in regular objects.  For the normal shared case, it
discards space for pc-relative relocs that have become local due to
symbol visibility changes.  We allocated space for them in the
check_relocs routine, but we won't fill them in in the
relocate_section routine.

We also check whether any of the remaining relocations apply
against a readonly section, and set the DF_TEXTREL flag in this
case.  */

static bfd_boolean
esp32ulp_discard_copies(struct elf_link_hash_entry *h, void * inf)
{
	struct bfd_link_info *info = (struct bfd_link_info *) inf;
	struct esp32ulp_pcrel_relocs_copied *s;

	if (!h->def_regular || (!info->symbolic && !h->forced_local))
	{
		if ((info->flags & DF_TEXTREL) == 0)
		{
			/* Look for relocations against read-only sections.  */
			for (s = esp32ulp_hash_entry(h)->pcrel_relocs_copied;
				s != NULL; s = s->next)
				if ((s->section->flags & SEC_READONLY) != 0)
				{
					info->flags |= DF_TEXTREL;
					break;
				}
		}

		return TRUE;
	}

	for (s = esp32ulp_hash_entry(h)->pcrel_relocs_copied;
		s != NULL; s = s->next)
		s->section->size -= s->count * sizeof(Elf32_External_Rela);

	return TRUE;
}

static bfd_boolean
esp32ulp_size_dynamic_sections(bfd * output_bfd ATTRIBUTE_UNUSED,
struct bfd_link_info *info)
{
	bfd *dynobj;
	asection *s;
	bfd_boolean relocs;

	dynobj = elf_hash_table(info)->dynobj;
	BFD_ASSERT(dynobj != NULL);

	if (elf_hash_table(info)->dynamic_sections_created)
	{
		/* Set the contents of the .interp section to the interpreter.  */
		if (bfd_link_executable(info))
		{
			s = bfd_get_linker_section(dynobj, ".interp");
			BFD_ASSERT(s != NULL);
			s->size = sizeof ELF_DYNAMIC_INTERPRETER;
			s->contents = (unsigned char *)ELF_DYNAMIC_INTERPRETER;
		}
	}
	else
	{
		/* We may have created entries in the .rela.got section.
		However, if we are not creating the dynamic sections, we will
		not actually use these entries.  Reset the size of .rela.got,
		which will cause it to get stripped from the output file
		below.  */
		s = elf_hash_table(info)->srelgot;
		if (s != NULL)
			s->size = 0;
	}

	/* If this is a -Bsymbolic shared link, then we need to discard all
	PC relative relocs against symbols defined in a regular object.
	For the normal shared case we discard the PC relative relocs
	against symbols that have become local due to visibility changes.
	We allocated space for them in the check_relocs routine, but we
	will not fill them in in the relocate_section routine.  */
	if (bfd_link_pic(info))
		elf_link_hash_traverse(elf_hash_table(info),
		esp32ulp_discard_copies, info);

	/* The check_relocs and adjust_dynamic_symbol entry points have
	determined the sizes of the various dynamic sections.  Allocate
	memory for them.  */
	relocs = FALSE;
	for (s = dynobj->sections; s != NULL; s = s->next)
	{
		const char *name;
		bfd_boolean strip;

		if ((s->flags & SEC_LINKER_CREATED) == 0)
			continue;

		/* It's OK to base decisions on the section name, because none
		of the dynobj section names depend upon the input files.  */
		name = bfd_get_section_name(dynobj, s);

		strip = FALSE;

		if (CONST_STRNEQ(name, ".rela"))
		{
			if (s->size == 0)
			{
				/* If we don't need this section, strip it from the
				output file.  This is mostly to handle .rela.bss and
				.rela.plt.  We must create both sections in
				create_dynamic_sections, because they must be created
				before the linker maps input sections to output
				sections.  The linker does that before
				adjust_dynamic_symbol is called, and it is that
				function which decides whether anything needs to go
				into these sections.  */
				strip = TRUE;
			}
			else
			{
				relocs = TRUE;

				/* We use the reloc_count field as a counter if we need
				to copy relocs into the output file.  */
				s->reloc_count = 0;
			}
		}
		else if (!CONST_STRNEQ(name, ".got"))
		{
			/* It's not one of our sections, so don't allocate space.  */
			continue;
		}

		if (strip)
		{
			s->flags |= SEC_EXCLUDE;
			continue;
		}

		/* Allocate memory for the section contents.  */
		/* FIXME: This should be a call to bfd_alloc not bfd_zalloc.
		Unused entries should be reclaimed before the section's contents
		are written out, but at the moment this does not happen.  Thus in
		order to prevent writing out garbage, we initialise the section's
		contents to zero.  */
		s->contents = (bfd_byte *)bfd_zalloc(dynobj, s->size);
		if (s->contents == NULL && s->size != 0)
			return FALSE;
	}

	if (elf_hash_table(info)->dynamic_sections_created)
	{
		/* Add some entries to the .dynamic section.  We fill in the
		values later, in esp32ulp_finish_dynamic_sections, but we
		must add the entries now so that we get the correct size for
		the .dynamic section.  The DT_DEBUG entry is filled in by the
		dynamic linker and used by the debugger.  */
#define add_dynamic_entry(TAG, VAL) \
  _bfd_elf_add_dynamic_entry (info, TAG, VAL)

		if (!bfd_link_pic(info))
		{
			if (!add_dynamic_entry(DT_DEBUG, 0))
				return FALSE;
		}


		if (relocs)
		{
			if (!add_dynamic_entry(DT_RELA, 0)
				|| !add_dynamic_entry(DT_RELASZ, 0)
				|| !add_dynamic_entry(DT_RELAENT,
				sizeof(Elf32_External_Rela)))
				return FALSE;
		}

		if ((info->flags & DF_TEXTREL) != 0)
		{
			if (!add_dynamic_entry(DT_TEXTREL, 0))
				return FALSE;
		}
	}
#undef add_dynamic_entry

	return TRUE;
}

/* Given a .data section and a .emreloc in-memory section, store
relocation information into the .emreloc section which can be
used at runtime to relocate the section.  This is called by the
linker when the --embedded-relocs switch is used.  This is called
after the add_symbols entry point has been called for all the
objects, and before the final_link entry point is called.  */

bfd_boolean
bfd_esp32ulp_elf32_create_embedded_relocs(bfd *abfd,
struct bfd_link_info *info,
	asection *datasec,
	asection *relsec,
	char **errmsg)
{
	Elf_Internal_Shdr *symtab_hdr;
	Elf_Internal_Sym *isymbuf = NULL;
	Elf_Internal_Rela *internal_relocs = NULL;
	Elf_Internal_Rela *irel, *irelend;
	bfd_byte *p;
	bfd_size_type amt;

	BFD_ASSERT(!bfd_link_relocatable(info));

	*errmsg = NULL;

	if (datasec->reloc_count == 0)
		return TRUE;

	symtab_hdr = &elf_tdata(abfd)->symtab_hdr;

	/* Get a copy of the native relocations.  */
	internal_relocs = (_bfd_elf_link_read_relocs
		(abfd, datasec, NULL, (Elf_Internal_Rela *)NULL,
		info->keep_memory));
	if (internal_relocs == NULL)
		goto error_return;

	amt = (bfd_size_type)datasec->reloc_count * 12;
	relsec->contents = (bfd_byte *)bfd_alloc(abfd, amt);
	if (relsec->contents == NULL)
		goto error_return;

	p = relsec->contents;

	irelend = internal_relocs + datasec->reloc_count;
	for (irel = internal_relocs; irel < irelend; irel++, p += 12)
	{
		/* We are going to write a four byte longword into the runtime
		reloc section.  The longword will be the address in the data
		section which must be relocated.  It is followed by the name
		of the target section NUL-padded or truncated to 8
		characters.  */

		/* We can only relocate absolute longword relocs at run time.  */
		*errmsg = _("unsupported reloc type");
		bfd_set_error(bfd_error_bad_value);
		goto error_return;
	}

	if (isymbuf != NULL && symtab_hdr->contents != (unsigned char *)isymbuf)
		free(isymbuf);
	if (internal_relocs != NULL
		&& elf_section_data(datasec)->relocs != internal_relocs)
		free(internal_relocs);
	return TRUE;

error_return:
	if (isymbuf != NULL && symtab_hdr->contents != (unsigned char *)isymbuf)
		free(isymbuf);
	if (internal_relocs != NULL
		&& elf_section_data(datasec)->relocs != internal_relocs)
		free(internal_relocs);
	return FALSE;
}

struct bfd_elf_special_section const elf32_esp32ulp_special_sections[] =
{
	{ NULL, 0, 0, 0, 0 }
};


#define TARGET_LITTLE_SYM		esp32ulp_elf32_vec
#define TARGET_LITTLE_NAME		"elf32-esp32ulp"
#define ELF_ARCH				bfd_arch_esp32ulp
#define ELF_TARGET_ID			ESP32ULP_ELF_DATA
#define ELF_MACHINE_CODE		EM_ESP32ULP
#define ELF_MAXPAGESIZE			0x1000
#define elf_symbol_leading_char		'_'

#define bfd_elf32_bfd_reloc_type_lookup	esp32ulp_bfd_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup \
					esp32ulp_bfd_reloc_name_lookup
#define elf_info_to_howto		esp32ulp_info_to_howto
#define elf_info_to_howto_rel		0
#define elf_backend_object_p		elf32_esp32ulp_object_p

#define bfd_elf32_bfd_is_local_label_name \
                                        esp32ulp_is_local_label_name
#define esp32ulp_hash_table(p) \
  ((struct esp32ulp_link_hash_table *) (p)->hash)



#define elf_backend_create_dynamic_sections \
                                        _bfd_elf_create_dynamic_sections
#define bfd_elf32_bfd_link_hash_table_create \
                                        esp32ulp_link_hash_table_create
#define bfd_elf32_bfd_final_link        bfd_elf_gc_common_final_link

#define elf_backend_check_relocs        esp32ulp_check_relocs
#define elf_backend_adjust_dynamic_symbol \
                                        esp32ulp_adjust_dynamic_symbol
#define elf_backend_size_dynamic_sections \
                                        esp32ulp_size_dynamic_sections
#define elf_backend_relocate_section    esp32ulp_relocate_section
#define elf_backend_finish_dynamic_symbol \
                                        esp32ulp_finish_dynamic_symbol
#define elf_backend_finish_dynamic_sections \
                                        esp32ulp_finish_dynamic_sections
#define elf_backend_gc_mark_hook        esp32ulp_gc_mark_hook
#define elf_backend_gc_sweep_hook       esp32ulp_gc_sweep_hook
#define bfd_elf32_bfd_merge_private_bfd_data \
                                        elf32_esp32ulp_merge_private_bfd_data
#define bfd_elf32_bfd_set_private_flags \
                                        elf32_esp32ulp_set_private_flags
#define bfd_elf32_bfd_print_private_bfd_data \
                                        elf32_esp32ulp_print_private_bfd_data
#define elf_backend_final_write_processing \
                                        elf32_esp32ulp_final_write_processing
#define elf_backend_reloc_type_class    elf32_esp32ulp_reloc_type_class
#define elf_backend_stack_align		8
#define elf_backend_can_gc_sections 1
#define elf_backend_special_sections	elf32_esp32ulp_special_sections
#define elf_backend_can_refcount 1
#define elf_backend_want_got_plt 0
#define elf_backend_plt_readonly 1
#define elf_backend_want_plt_sym 0
#define elf_backend_got_header_size     12
#define elf_backend_rela_normal         1

#include "elf32-target.h"

#undef TARGET_LITTLE_SYM
#define TARGET_LITTLE_SYM          esp32ulp_elf32_fdpic_vec
#undef TARGET_LITTLE_NAME
#define TARGET_LITTLE_NAME		"elf32-esp32ulpfdpic"
#undef	elf32_bed
#define	elf32_bed		elf32_esp32ulpfdpic_bed

#undef elf_backend_gc_sweep_hook
#define elf_backend_gc_sweep_hook       esp32ulpfdpic_gc_sweep_hook

#undef elf_backend_got_header_size
#define elf_backend_got_header_size     0

#undef elf_backend_relocate_section
#define elf_backend_relocate_section    esp32ulpfdpic_relocate_section
#undef elf_backend_check_relocs
#define elf_backend_check_relocs        esp32ulpfdpic_check_relocs

#undef bfd_elf32_bfd_link_hash_table_create
#define bfd_elf32_bfd_link_hash_table_create \
		esp32ulpfdpic_elf_link_hash_table_create
#undef elf_backend_always_size_sections
#define elf_backend_always_size_sections \
		elf32_esp32ulpfdpic_always_size_sections

#undef elf_backend_create_dynamic_sections
#define elf_backend_create_dynamic_sections \
		elf32_esp32ulpfdpic_create_dynamic_sections
#undef elf_backend_adjust_dynamic_symbol
#define elf_backend_adjust_dynamic_symbol \
		elf32_esp32ulpfdpic_adjust_dynamic_symbol
#undef elf_backend_size_dynamic_sections
#define elf_backend_size_dynamic_sections \
		elf32_esp32ulpfdpic_size_dynamic_sections
#undef elf_backend_finish_dynamic_symbol
#define elf_backend_finish_dynamic_symbol \
		elf32_esp32ulpfdpic_finish_dynamic_symbol
#undef elf_backend_finish_dynamic_sections
#define elf_backend_finish_dynamic_sections \
		elf32_esp32ulpfdpic_finish_dynamic_sections

#undef elf_backend_discard_info
#define elf_backend_discard_info \
		esp32ulpfdpic_elf_discard_info
#undef elf_backend_can_make_relative_eh_frame
#define elf_backend_can_make_relative_eh_frame \
		esp32ulpfdpic_elf_use_relative_eh_frame
#undef elf_backend_can_make_lsda_relative_eh_frame
#define elf_backend_can_make_lsda_relative_eh_frame \
		esp32ulpfdpic_elf_use_relative_eh_frame
#undef elf_backend_encode_eh_address
#define elf_backend_encode_eh_address \
		esp32ulpfdpic_elf_encode_eh_address

#undef elf_backend_may_use_rel_p
#define elf_backend_may_use_rel_p       1
#undef elf_backend_may_use_rela_p
#define elf_backend_may_use_rela_p      1
/* We use REL for dynamic relocations only.  */
#undef elf_backend_default_use_rela_p
#define elf_backend_default_use_rela_p  1

#undef elf_backend_omit_section_dynsym
#define elf_backend_omit_section_dynsym _esp32ulpfdpic_link_omit_section_dynsym

#include "elf32-target.h"
