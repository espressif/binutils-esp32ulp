/* ESP32-S2ULP ELF support for BFD.
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

#ifndef _ELF_ESP32S2ULP_H
#define _ELF_ESP32S2ULP_H

#include "elf/reloc-macros.h"

//#ifndef DEBUG_TRACE
//#define DEBUG_TRACE //printf
//#endif //DEBUG_TRACE

START_RELOC_NUMBERS (elf_esp32s2ulp_reloc_type)
  RELOC_NUMBER (R_ESP32S2ULP_UNUSED0, 0x00)          
  RELOC_NUMBER(R_ESP32S2ULP_RIMM16, 0x01)        
  RELOC_NUMBER(R_ESP32S2ULP_JUMPR, 0x02)
  RELOC_NUMBER(R_ESP32S2ULP_LOAD16, 0x03)        
  RELOC_NUMBER(R_ESP32S2ULP_WR_MEM, 0x04)        
  RELOC_NUMBER(R_ESP32S2ULP_ALUI, 0x05)
  RELOC_NUMBER(R_ESP32S2ULP_WAIT, 0x06)
  RELOC_NUMBER(R_ESP32S2ULP_TSENS_CYCLE, 0x07)
  RELOC_NUMBER(R_ESP32S2ULP_TSENS_DELAY, 0x08)

  RELOC_NUMBER(R_ESP32S2ULP_ADC_CYCLE, 0x09)
  RELOC_NUMBER(R_ESP32S2ULP_ADC_SEL, 0x0A)
  RELOC_NUMBER(R_ESP32S2ULP_ADC_MUX, 0x0B)
  RELOC_NUMBER(R_ESP32S2ULP_WAKE, 0x0C)
  RELOC_NUMBER(R_ESP32S2ULP_SLEEP, 0x0D)
  RELOC_NUMBER(R_ESP32S2ULP_STAGE, 0x0E)

  RELOC_NUMBER(R_ESP32S2ULP_JUMPR_STEP, 0x0F)
  RELOC_NUMBER(R_ESP32S2ULP_JUMPR_THRESH, 0x10)
  RELOC_NUMBER(R_ESP32S2ULP_JUMPS_THRESH, 0x11)

  RELOC_NUMBER(R_ESP32S2ULP_REG_RW_HIGH, 0x12)
  RELOC_NUMBER(R_ESP32S2ULP_REG_RW_LOW, 0x13)
  RELOC_NUMBER(R_ESP32S2ULP_REG_RW_ADDR, 0x14)
  RELOC_NUMBER(R_ESP32S2ULP_REG_RW_DATA, 0x15)

  RELOC_NUMBER(R_ESP32S2ULP_I2C_RW_HIGH, 0x16)
  RELOC_NUMBER(R_ESP32S2ULP_I2C_RW_LOW, 0x17)
  RELOC_NUMBER(R_ESP32S2ULP_I2C_RW_ADDR, 0x18)
  RELOC_NUMBER(R_ESP32S2ULP_I2C_RW_DATA, 0x19)
  RELOC_NUMBER(R_ESP32S2ULP_I2C_RW_SEL, 0x1A)

  RELOC_NUMBER(R_ESP32S2ULP_WR_MEM_LABEL, 0x1B)        

  RELOC_NUMBER(R_BFD_RELOC_32, 0x1C)

  RELOC_NUMBER(R_ESP32S2ULP_RESERVE, 0x1D)
  RELOC_NUMBER(R_ESP32S2ULP_PLTPC, 0x40)         /* PLT gnu only relocation */
  RELOC_NUMBER(R_ESP32S2ULP_GOT, 0x41)           /* GOT gnu only relocation */
  RELOC_NUMBER(R_ESP32S2ULP_GNU_VTINHERIT, 0x42) /* C++, gnu only */
  RELOC_NUMBER(R_ESP32S2ULP_GNU_VTENTRY, 0x43) /* C++, gnu only */
  END_RELOC_NUMBERS(R_ESP32S2ULP_max)

/* Processor specific flags for the ELF header e_flags field.  */
#define EF_ESP32S2ULP_PIC		0x00000001	/* -fpic */
#define EF_ESP32S2ULP_FDPIC		0x00000002      /* -mfdpic */

#define EF_ESP32S2ULP_CODE_IN_L1	0x00000010	/* --code-in-l1 */
#define EF_ESP32S2ULP_DATA_IN_L1	0x00000020	/* --data-in-l1 */

#define	EF_ESP32S2ULP_PIC_FLAGS	(EF_ESP32S2ULP_PIC | EF_ESP32S2ULP_FDPIC)

#define DR_REG_MAX_DIRECT                       0x3ff
#define DR_REG_RTCCNTL_BASE                     0x3ff48000
#define DR_REG_IO_MUX_BASE                      0x3ff49000

#endif /* _ELF_ESP32S2ULP_H */
