/* ESPULP ELF support for BFD.
Copyright (c) 2016-2017 Espressif Systems (Shanghai) PTE LTD.

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

/* Disassembler for ESP32 ULP processor
   file to be put in folder opcodes/disassemble
   
   don't forget to add:
   1)opcodes/disassemble.c:
       #define ARCH_esp32ulp
	   ..
	   #ifdef ARCH_esp32ulp
       case bfd_arch_esp32ulp:
          disassemble = print_insn_esp32ulp;
          break;
       #endif
	   
	2) include/dis-asm.h:
	   extern int print_insn_esp32ulp     (bfd_vma, disassemble_info *);
		  
*/

#include "sysdep.h"
#include "dis-asm.h"
#include "floatformat.h"
#include "libiberty.h"
#include "opintl.h"


typedef struct
{
	unsigned int operand: 28;
	unsigned int opcode :  4;
} esp32ulp_inst;

static void disasm_esp32ulp_instruction(esp32ulp_inst inst, unsigned int addr);
static void unknown(void);
static void regwr(unsigned int operand);
static void regrd(unsigned int operand);
static void i2c(unsigned int operand);
static void wait(unsigned int operand);
static void adc(unsigned int operand);
static void store(unsigned int operand);
static void operation(unsigned int operand);
static void operation_reg(unsigned int operand);
static void operation_imm(unsigned int operand);
static void operation_stage(unsigned int operand);
static void jmp(unsigned int operand, unsigned int address);
static void jump(unsigned int operand, unsigned int address);
static void jumpr(unsigned int operand, unsigned int address);
static void jumps(unsigned int operand, unsigned int address);
static void wakesleep(unsigned int operand);
static void tsens(unsigned int operand);
static void halt(unsigned int operand);
static void load(unsigned int operand);


#define ESP32ULP_INSTR_SIZE		4
#define INST_TO_ADDR(inst) ((inst)*ESP32ULP_INSTR_SIZE)
#define ADDR_TO_INST(addr) ((addr)/ESP32ULP_INSTR_SIZE)

#define COPY(a,b)  memcpy(&a, &b, sizeof(a));

/* Maximum length of an instruction.  */
#define MAXLEN ESP32ULP_INSTR_SIZE

struct private
{
  /* Points to first byte not fetched.  */
  bfd_byte *max_fetched;
  bfd_byte the_buffer[MAXLEN];
  bfd_vma insn_start;
};

/* Make sure that bytes from INFO->PRIVATE_DATA->BUFFER (inclusive)
   to ADDR (exclusive) are valid.  Returns 1 for success, 0 on error.  */
#define FETCH_DATA(info, addr) \
  ((addr) <= ((struct private *) (info->private_data))->max_fetched \
   ? 1 : fetch_data ((info), (addr)))


static int
fetch_data (struct disassemble_info *info, bfd_byte *addr)
{
  int status;
  

  struct private *priv = (struct private *)info->private_data;
  bfd_vma start = priv->insn_start + (priv->max_fetched - priv->the_buffer);

  status = (*info->read_memory_func) (start,
				      priv->max_fetched,
				      addr - priv->max_fetched,
				      info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, start, info);
      return 0;
    }
  else
    priv->max_fetched = addr;
  return 1;
}

struct disassemble_info* dinfo ;

int
print_insn_esp32ulp		(bfd_vma memaddr, disassemble_info *info)
{
	dinfo = info;
	
	struct private priv;
	bfd_byte *buffer = priv.the_buffer;

	dinfo->private_data = & priv;
	priv.max_fetched = priv.the_buffer;
	priv.insn_start = memaddr;
  
    FETCH_DATA (dinfo, buffer + ESP32ULP_INSTR_SIZE);

	esp32ulp_inst inst;
	COPY(inst, *buffer);
	disasm_esp32ulp_instruction(inst, (unsigned int)memaddr);
	
	return ESP32ULP_INSTR_SIZE;
}


	




static void disasm_esp32ulp_instruction(esp32ulp_inst inst, unsigned int addr)
{
//	dinfo->fprintf_func(dinfo->stream, "%04x %04x  %01x%07x  ", ADDR_TO_INST(addr), addr, inst.opcode, inst.operand); 
		
	unsigned int operand = inst.operand;
	switch(inst.opcode)
	{
		case  1: regwr(operand); break;
		case  2: regrd(operand); break;
		case  3: i2c(operand); break;
		case  4: wait(operand); break;
		case  5: adc(operand); break;
		case  6: store(operand); break;
		case  7: operation(operand); break;
		case  8: jmp(operand, addr); break;
		case  9: wakesleep(operand); break;
		case 10: tsens(operand); break;
		case 11: halt(operand); break;
		case 13: load(operand); break;
		default: unknown(); break;
	}
}

static void unknown(void)
{
	dinfo->fprintf_func(dinfo->stream, "????");
}

// from soc.h
#define DR_REG_RTCCNTL_BASE                     0x3ff48000
#define DR_REG_RTCIO_BASE                       0x3ff48400
#define DR_REG_SENS_BASE                        0x3ff48800
#define DR_REG_RTC_I2C_BASE                     0x3ff48C00

typedef struct {
unsigned int addr:10;
unsigned int data:8;
unsigned int low:  5;
unsigned int high: 5;
} fregwr;


static void regwr(unsigned int operand)
{
	fregwr op;
	COPY(op, operand);

	unsigned int addr = INST_TO_ADDR(op.addr)+DR_REG_RTCCNTL_BASE;
	unsigned int base;
	unsigned int offset;
	char* pstr;

	if (addr>=DR_REG_RTC_I2C_BASE)    { base = DR_REG_RTC_I2C_BASE; pstr = "RTC_I2C_BASE"; }
	else if (addr>=DR_REG_SENS_BASE)  { base = DR_REG_SENS_BASE;    pstr = "SENS_BASE"; }
	else if (addr>=DR_REG_RTCIO_BASE) { base = DR_REG_RTCIO_BASE;   pstr = "RTCIO_BASE"; }
	else                              { base = DR_REG_RTCCNTL_BASE ;pstr = "RTCCNTL_BASE"; }
	
	offset = addr - base;
	
	dinfo->fprintf_func(dinfo->stream, "REG_WR    "); 
	dinfo->fprintf_func(dinfo->stream, "0x%x+0x%x, %d, %d, 0x%x", base, offset, op.high, op.low, op.data );
	dinfo->fprintf_func(dinfo->stream, "\t // REG(%s+0x%x)[%d:%d]=0x%x", pstr, offset,  op.high, op.low, op.data);
}

typedef struct {
unsigned int addr:10;
unsigned int dummy:8;
unsigned int low:  5;
unsigned int high: 5;
} fregrd;

static void regrd(unsigned int operand)
{
	fregrd op;
	COPY(op, operand);
	
	unsigned int addr = INST_TO_ADDR(op.addr)+DR_REG_RTCCNTL_BASE;
	unsigned int base;
	unsigned int offset;
	char* pstr;

	if (addr>=DR_REG_RTC_I2C_BASE)    { base = DR_REG_RTC_I2C_BASE; pstr = "RTC_I2C_BASE"; }
	else if (addr>=DR_REG_SENS_BASE)  { base = DR_REG_SENS_BASE;    pstr = "SENS_BASE"; }
	else if (addr>=DR_REG_RTCIO_BASE) { base = DR_REG_RTCIO_BASE;   pstr = "RTCIO_BASE"; }
	else                              { base = DR_REG_RTCCNTL_BASE ;pstr = "RTCCNTL_BASE"; }
	
	offset = addr - base;
	
	dinfo->fprintf_func(dinfo->stream, "REG_RD    "); 
	dinfo->fprintf_func(dinfo->stream, "0x%x+0x%x, %d, %d", base, offset, op.high, op.low);
	dinfo->fprintf_func(dinfo->stream, "\t\t // REG(%s+0x%x)[%d:%d]=0x%x", pstr, offset,  op.high, op.low);
}

typedef struct {
unsigned int subaddr:8;
unsigned int data:8;
unsigned int low:  3;
unsigned int high: 3;
unsigned int sel:  4;
unsigned int dummy:1;
unsigned int rw:   1;
} fi2c;

static void i2c(unsigned int operand)
{
	fi2c op;
	COPY(op, operand);
	
	if (op.rw == 0)
	dinfo->fprintf_func(dinfo->stream, "I2C_RD    0x%x, %d, %d, %d \t\t // I2C_READ SLAVE_ADDR 0x%x, REG%d [%d:%d]", op.subaddr, op.high, op.low, op.sel, op.subaddr, op.sel, op.low, op.high);
	else
	dinfo->fprintf_func(dinfo->stream, "I2C_WR    0x%x, 0x%x, %d, %d, %d \t // I2C_WRITE SLAVE_ADDR 0x%x, REG%d [%d:%d]=%x", op.subaddr, op.data, op.high, op.low, op.sel, op.subaddr, op.sel, op.low, op.high, op.data);
}

typedef struct {
unsigned int cycles:16;
unsigned int dummy:8;
} fwait;

static void wait(unsigned int operand)
{
	fwait op;
	COPY(op, operand);
		
	if (op.cycles == 0)
	dinfo->fprintf_func(dinfo->stream, "NOP", op.cycles);
	else
	dinfo->fprintf_func(dinfo->stream, "WAIT      %d", op.cycles);
}

typedef struct {
unsigned int Rdst:2;
unsigned int Sarmux:4;
unsigned int sel:1;
unsigned int dummy:21;
} fadc;

static void adc(unsigned int operand)
{
	fadc op;
	COPY(op, operand);
		
	dinfo->fprintf_func(dinfo->stream, "ADC      R%d, %d, %d\t\t\t\t\t // R%d=ADC%d pad%d", op.Rdst, op.Sarmux, op.sel, op.Rdst, op.Sarmux+1, op.sel+1);
}

typedef struct
{
  unsigned int operand:25;
  unsigned int choice: 3;
} fop;


typedef struct
{
  unsigned int Rdst:  2;
  unsigned int Rsrc1: 2;
  unsigned int Rsrc2: 2;
  unsigned int dummy:15;
  unsigned int sel:   4;
  unsigned int choice:3;		// 0
} fop_reg;

typedef struct
{
  unsigned int Rdst:  2;
  unsigned int Rsrc1: 2;
  unsigned int imm:   16;
  unsigned int dummy1:1;
  unsigned int sel:   4;
  unsigned int choice:3;		// 1
} fop_imm;

typedef struct
{
  unsigned int dummy: 4;
  unsigned int imm:   8;
  unsigned int dummy2:9;
  unsigned int sel:   4;
  unsigned int choice:3;		// 2
} fop_stage;

static void operation(unsigned int operand)
{
	fop op;	
	COPY(op, operand);
	
	switch(op.choice)
	{
		case 0: operation_reg(operand); break; 
		case 1: operation_imm(operand); break;
		case 2: operation_stage(operand); break;
		default: dinfo->fprintf_func(dinfo->stream, "ALU ???"); break;
	}
}

static void operation_reg(unsigned int operand)
{
	fop_reg opr;
	COPY(opr, operand);
	
	switch(opr.sel)
	{
		case 0: dinfo->fprintf_func(dinfo->stream, "ADD       R%d, R%d, R%d \t\t\t\t\t // R%d = R%d + R%d", opr.Rdst, opr.Rsrc1, opr.Rsrc2, opr.Rdst, opr.Rsrc1, opr.Rsrc2); break;
		case 1: dinfo->fprintf_func(dinfo->stream, "SUB       R%d, R%d, R%d \t\t\t\t\t // R%d = R%d - R%d", opr.Rdst, opr.Rsrc1, opr.Rsrc2, opr.Rdst, opr.Rsrc1, opr.Rsrc2); break;
		case 2: dinfo->fprintf_func(dinfo->stream, "AND       R%d, R%d, R%d \t\t\t\t\t // R%d = R%d & R%d", opr.Rdst, opr.Rsrc1, opr.Rsrc2, opr.Rdst, opr.Rsrc1, opr.Rsrc2); break;
		case 3: dinfo->fprintf_func(dinfo->stream, "OR        R%d, R%d, R%d \t\t\t\t\t // R%d = R%d | R%d", opr.Rdst, opr.Rsrc1, opr.Rsrc2, opr.Rdst, opr.Rsrc1, opr.Rsrc2); break;
		case 4: dinfo->fprintf_func(dinfo->stream, "MOVE      R%d, R%d      \t\t\t\t\t // R%d = R%d",       opr.Rdst, opr.Rsrc1,            opr.Rdst, opr.Rsrc1); break;
		case 5: dinfo->fprintf_func(dinfo->stream, "LSH       R%d, R%d, R%d \t\t\t\t\t // R%d = R%d<<R%d",  opr.Rdst, opr.Rsrc1, opr.Rsrc2, opr.Rdst, opr.Rsrc1, opr.Rsrc2); break;
		case 6: dinfo->fprintf_func(dinfo->stream, "RSH       R%d, R%d, R%d \t\t\t\t\t // R%d = R%d>>R%d",  opr.Rdst, opr.Rsrc1, opr.Rsrc2, opr.Rdst, opr.Rsrc1, opr.Rsrc2); break;
		default: dinfo->fprintf_func(dinfo->stream, "ALU ????"); break;
	}
}

static void operation_imm(unsigned int operand)
{
	fop_imm   opi;
	COPY(opi, operand);
	
	switch(opi.sel)
	{
		case 0: dinfo->fprintf_func(dinfo->stream, "ADD       R%d, R%d, %d \t\t\t\t\t // R%d = R%d + %d", opi.Rdst, opi.Rsrc1, opi.imm, opi.Rdst, opi.Rsrc1, opi.imm); break;
		case 1: dinfo->fprintf_func(dinfo->stream, "SUB       R%d, R%d, %d \t\t\t\t\t // R%d = R%d - %d", opi.Rdst, opi.Rsrc1, opi.imm, opi.Rdst, opi.Rsrc1, opi.imm); break;
		case 2: dinfo->fprintf_func(dinfo->stream, "AND       R%d, R%d, 0x%04x \t\t\t\t // R%d = R%d & 0x%x", opi.Rdst, opi.Rsrc1, opi.imm, opi.Rdst, opi.Rsrc1, opi.imm); break;
		case 3: dinfo->fprintf_func(dinfo->stream, "OR        R%d, R%d, 0x%04x \t\t\t\t // R%d = R%d | 0x%x", opi.Rdst, opi.Rsrc1, opi.imm, opi.Rdst, opi.Rsrc1, opi.imm); break;
		case 4: dinfo->fprintf_func(dinfo->stream, "MOVE      R%d, %d      \t\t\t\t\t // R%d = %d \t\t\t (can also be pointer to address 0x%04x)", opi.Rdst, opi.imm, opi.Rdst, opi.imm, INST_TO_ADDR(opi.imm)); break;
		case 5: dinfo->fprintf_func(dinfo->stream, "LSH       R%d, R%d, %d \t\t\t\t\t // R%d = R%d<<%d", opi.Rdst, opi.Rsrc1, opi.imm, opi.Rdst, opi.Rsrc1, opi.imm); break;
		case 6: dinfo->fprintf_func(dinfo->stream, "RSH       R%d, R%d, %d \t\t\t\t\t // R%d = R%d>>%d", opi.Rdst, opi.Rsrc1, opi.imm, opi.Rdst, opi.Rsrc1, opi.imm); break;
		default: dinfo->fprintf_func(dinfo->stream, "ALU ????"); break;
	}
}

static void operation_stage(unsigned int operand)
{
	fop_stage ops;	
	COPY(ops, operand);
		
	switch(ops.sel)
	{
		case 0: dinfo->fprintf_func(dinfo->stream, "STAGE_INC %d", ops.imm); break;
		case 1: dinfo->fprintf_func(dinfo->stream, "STAGE_DEC %d", ops.imm); break;
		case 2: dinfo->fprintf_func(dinfo->stream, "STAGE_RST"); break;

		default: dinfo->fprintf_func(dinfo->stream, "ALU ????"); break;
	}
}


typedef struct
{
  unsigned int operand:25;
  unsigned int choice:3;
} fjmp;
	
typedef struct
{
  unsigned int rdest:2;
  unsigned int addr:11;
  unsigned int dummy:8;
  unsigned int sel:  1;
  unsigned int type: 2;
  unsigned int choice:3;		// 0
} fjump;

typedef struct
{
  unsigned int thres:16;
  unsigned int cond:  1;
  unsigned int step:  8;
  unsigned int choice:3;		// 1
} fjumpr;

typedef struct
{
  unsigned int thres: 8;
  unsigned int dummy: 7;
  unsigned int cond:  2;
  unsigned int step:  8;
  unsigned int choice:3;		// 2
} fjumps;

static void jmp(unsigned int operand, unsigned int address)
{
	fjmp  op;	
	COPY(op, operand);
	
	switch(op.choice)
	{
		case 0: jump(operand, address); break;
		case 1: jumpr(operand, address); break;
		case 2: jumps(operand, address); break;
		default: dinfo->fprintf_func(dinfo->stream, "JUMP????"); break;
	}
}

static void jump(unsigned int operand, unsigned int address)
{
	fjump  op;
	COPY(op, operand);
	
	address = address;		// fake instruction to astatic void compiler warning (unused parameter)
	
	dinfo->fprintf_func(dinfo->stream, "JUMP      ");

	if (op.sel==0) 
		dinfo->fprintf_func(dinfo->stream, "0x%04x", INST_TO_ADDR(op.addr)); 
	else
		dinfo->fprintf_func(dinfo->stream, "R%d", op.rdest);

	switch(op.type)
	{
	case 1: dinfo->fprintf_func(dinfo->stream, ",EQ \t\t\t\t\t // if operation==0 THEN JUMP"); break;
	case 2: dinfo->fprintf_func(dinfo->stream, ",OV \t\t\t\t\t // if overflow THEN JUMP"); break;
	}
}

static void jumpr(unsigned int operand, unsigned int address)
{
	fjumpr op;	
	COPY(op, operand);
	
	unsigned int  relative = INST_TO_ADDR(op.step&0x7F);
	int           sign     = (op.step&0x80 ? -1 : +1);
	
	dinfo->fprintf_func(dinfo->stream, "JUMPR     ");
	dinfo->fprintf_func(dinfo->stream, "0x%04x, %d, %s", address+sign*relative, op.thres, (op.cond==0?"lt":"ge") );
	
	dinfo->fprintf_func(dinfo->stream, "\t\t\t\t\t // IF R0%s%d THEN JUMP TO [0x%04x%c0x%02x]", (op.cond==0?"<":">="), op.thres, address, (sign>0?'+':'-'), relative);
}

static void jumps(unsigned int operand, unsigned int address)
{
	fjumps op;
	COPY(op, operand);
	
	unsigned int  relative = INST_TO_ADDR(op.step&0x7F);
	int           sign     = (op.step&0x80 ? -1 : +1);
	
	dinfo->fprintf_func(dinfo->stream, "JUMPS     ");
	dinfo->fprintf_func(dinfo->stream, "0x%04x, ", address+sign*relative);
	
	switch(op.cond)
	{
		case 0:   dinfo->fprintf_func(dinfo->stream, "%d, lt", op.thres); break;
		case 1:   dinfo->fprintf_func(dinfo->stream, "%d, gt", op.thres); break;
		default:  dinfo->fprintf_func(dinfo->stream, "%d, eq", op.thres); break;
	}
	
	dinfo->fprintf_func(dinfo->stream, "\t\t\t\t\t // ");

	switch(op.cond)
	{
		case 0:   dinfo->fprintf_func(dinfo->stream, "IF COUNT<%d ", op.thres); break;
		case 1:   dinfo->fprintf_func(dinfo->stream, "IF COUNT>%d ", op.thres); break;
		default:  dinfo->fprintf_func(dinfo->stream, "IF COUNT==%d ", op.thres); break;
	}
	
	dinfo->fprintf_func(dinfo->stream,"THEN JUMP TO [0x%04x%c0x%02x]", address, (sign>0?'+':'-'), relative);
}



typedef struct {
unsigned int reg:4;
unsigned int dummy:19;
unsigned int wakeorsleep:1;
} fwakesleep;

static void wakesleep(unsigned int operand)
{
	fwakesleep op;
	COPY(op, operand);
	
	if (op.wakeorsleep==0)
		dinfo->fprintf_func(dinfo->stream, "WAKE");
	else
		dinfo->fprintf_func(dinfo->stream, "SLEEP      R%d", op.reg);
}

typedef struct {
unsigned int Rdst:2;
unsigned int delay:14;
unsigned int dummy:12;
} ftsens;

static void tsens(unsigned int operand)
{
	ftsens op;
	COPY(op, operand);	

	dinfo->fprintf_func(dinfo->stream, "TSENS      R%d, %d\t\t\t\t\t // delay=%d", op.Rdst, op.delay, op.delay);
}

static void halt(unsigned int operand)
{
	operand = operand;		// fake instruction to astatic void compiler warning (unused parameter)
		
	dinfo->fprintf_func(dinfo->stream, "HALT");
}

typedef struct {
unsigned int Rdst:2;
unsigned int Rsrc:2;
unsigned int dummy:6;
unsigned int offset:11;
unsigned int dummy2: 7;
} fload;

static void load(unsigned int operand)
{
	fload op;
	COPY(op, operand);
	
	dinfo->fprintf_func(dinfo->stream, "LD        R%d, R%d, %d \t\t\t\t\t // R%d = MEM[R%d+%d]", op.Rdst, op.Rsrc, INST_TO_ADDR(op.offset), op.Rdst, op.Rsrc, INST_TO_ADDR(op.offset));
}

typedef struct {
unsigned int Rdst:2;
unsigned int Rsrc:2;
unsigned int null:6;
unsigned int offset:11;
unsigned int null2: 4;
unsigned int cent:  3;
} fstore;

static void store(unsigned int operand)
{
	fstore op;
	COPY(op, operand);
	
	dinfo->fprintf_func(dinfo->stream, "ST        R%d, R%d, %d \t\t\t\t\t // MEM[R%d+%d] = R%d", op.Rdst, op.Rsrc, INST_TO_ADDR(op.offset), op.Rsrc, INST_TO_ADDR(op.offset), op.Rdst);
}