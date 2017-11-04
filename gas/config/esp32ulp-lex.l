/* esp32ulp-lex.l    ESP32ULP GNU Assembler

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
%{

#include <ctype.h>

#include "as.h"
#include "esp32ulp-defs.h"
#include "esp32ulp-parse.h"

static long parse_int (char **end);
static int parse_reg (Register *r, int type, char *rt);
int yylex (void);

#define _REG yylval.reg


%}

/* Define Start States ... Actually we will use exclusion.
   If no start state is specified it should match any state
   and <INITIAL> would match some keyword rules only with
   initial.  */
%s KEYWORD
%s FLAGS

%%

[nN][oO][pP]                            return NOP;
[aA][dD][dD]                            return ADD;
[sS][uU][bB]                            return SUB;
[aA][nN][dD]                            return AND;
[mM][oO][vV][eE]                        return MOVE;
[oO][rR]                                return OR;
[lL][sS][hH]                            return LSH;
[rR][sS][hH]                            return RSH;

[jJ][uU][mM][pP]                        { BEGIN 0; return JUMP;}
[jJ][uU][mM][pP][rR]                    { BEGIN 0; return JUMPR;}
[jJ][uU][mM][pP][sS]                    { BEGIN 0; return JUMPS;}
[sS][tT]                                return ST;
[lL][dD]                                return LD;
[hH][aA][lL][tT]                        return HALT;
[wW][aA][kK][eE]                        return WAKE;
[sS][lL][eE][eE][pP]                    return SLEEP;
[tT][sS][eE][nN][sS]                    return TSENS;
[wW][aA][iI][tT]                        return WAIT;
[aA][dD][cC]                            return ADC;
[sS][tT][aA][gG][eE][_][iI][nN][cC]     return STAGE_INC;
[sS][tT][aA][gG][eE][_][dD][eE][cC]     return STAGE_DEC;
[sS][tT][aA][gG][eE][_][rR][sS][tT]     return STAGE_RST;
[rR][eE][gG][_][rR][dD]                 return REG_RD;
[rR][eE][gG][_][wW][rR]                 return REG_WR;
[iI][2][cC][_][rR][dD]                  return I2C_RD;
[iI][2][cC][_][wW][rR]                  return I2C_WR;


[eE][qQ]                                return EQ;
[oO][vV]                                return OV;
[gG][tT]                                return GT;
[gG][eE]                                return GE;
[lL][eE]                                return LE;
[lL][tT]                                return LT;


[rR][0-3]          return parse_reg (&yylval.reg, T_REG_R, yytext);

[Gg][Oo][Tt]	return GOT;


"~"                     return TILDA;
"|"                     return BAR;
"^"                     return CARET;
"]"                     return RBRACK;
"["                     return LBRACK;
">>"                    return GREATER_GREATER;
"<<"                    return LESS_LESS;
"<"                     return LESS_THAN;
"("                     BEGIN(FLAGS); return LPAREN;
")"                     BEGIN(INITIAL); return RPAREN;
":"                     return COLON;
"/"                     return SLASH;
"-"                     return MINUS;
","                     return COMMA;
"+"                     return PLUS;
"*"                     return STAR;
"&"                     return AMPERSAND;
"%"                     return PERCENT;
"!"                     return BANG;
";"                     return SEMICOLON;
"@"			return AT;

([0-9]+)|(0[xX][0-9a-fA-F]+)|([bhfodBHOFD]#[0-9a-fA-F]+)|(0"."[0-9]+) {
    yylval.value = parse_int (&yytext);
    return NUMBER;
  }
([0-9]+[bf])|([[:alpha:]\x80-\xff_$.][[:alnum:]\x80-\xff_$.]*) {
    if (isdigit(yytext[0])) {
        char lc= yytext[strlen(yytext)-1];
        if (lc == 'b') {
            char *name = fb_label_name (atoi(yytext), 0);
            yylval.symbol = symbol_find (name);
            if ((yylval.symbol!= NULL) && (S_IS_DEFINED (yylval.symbol))) {
                know (SEG_NORMAL (S_GET_SEGMENT (yylval.symbol)));
            } else {
                as_bad (_("backward ref to unknown label \"%d:\""),	(int) atoi(yytext));
            }
        } else if (lc == 'f') {
            char *name = fb_label_name (atoi(yytext), 1);
            yylval.symbol = symbol_find_or_make (name);
            #ifndef many_segments
              know (S_GET_SEGMENT (yylval.symbol) == undefined_section
                 || S_GET_SEGMENT (yylval.symbol) == text_section || S_GET_SEGMENT (yylval.symbol) == data_section);
            #endif
        }
    } else {
        yylval.symbol = symbol_find_or_make (yytext);
        symbol_mark_used (yylval.symbol);
    }
    return SYMBOL;
  }

[ \t\n]                                    ;
"/*".*"*/"                                 ;
.                                          return yytext[0];
%%
static long parse_int (char **end)
{
  char fmt = '\0';
  int not_done = 1;
  int shiftvalue = 0;
  char * char_bag;
  long value = 0;
  char *arg = *end;

  while (*arg && *arg == ' ')
    arg++;

  switch (*arg)
    {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        fmt = 'd';
        break;

      case '0':  /* Accept different formatted integers hex octal and binary. */
        {
	  char c = *++arg;
          arg++;
	  if (c == 'x' || c == 'X') /* Hex input.  */
	    fmt = 'h';
	  else if (c == 'b' || c == 'B')
	    fmt = 'b';
	  else if (c == '.')
	    fmt = 'f';
	  else
            {             /* Octal.  */
	      arg--;
	      fmt = 'o';
	    }
	  break;
        }

      case 'd':
      case 'D':
      case 'h':
      case 'H':
      case 'o':
      case 'O':
      case 'b':
      case 'B':
      case 'f':
      case 'F':
        {
	  fmt = *arg++;
	  if (*arg == '#')
	    arg++;
        }
    }

  switch (fmt)
    {
      case 'h':
      case 'H':
        shiftvalue = 4;
        char_bag = "0123456789ABCDEFabcdef";
        break;

      case 'o':
      case 'O':
        shiftvalue = 3;
        char_bag = "01234567";
        break;

      case 'b':
      case 'B':
        shiftvalue = 1;
        char_bag = "01";
        break;

/* The assembler allows for fractional constants to be created
   by either the 0.xxxx or the f#xxxx format

   i.e.   0.5 would result in 0x4000

   note .5 would result in the identifier .5.

   The assembler converts to fractional format 1.15 by the simple rule:

             value = (short) (finput * (1 << 15)).  */

      case 'f':
      case 'F':
        {
          float fval = 0.0;
          float pos = 10.0;
          while (1)
            {
              int c;
              c = *arg++;

              if (c >= '0' && c <= '9')
                {
                  float digit = (c - '0') / pos;
                  fval = fval + digit;
                  pos = pos * 10.0;
                }
              else
                {
	          *--arg = c;
                  value = (short) (fval * (1 << 15));
                  break;
                }
            }
          *end = arg+1;
          return value;
        }

      case 'd':
      case 'D':
      default:
        {
          while (1)
            {
              char c;
              c = *arg++;
              if (c >= '0' && c <= '9')
                value = (value * 10) + (c - '0');
              else
                {
                  /* Constants that are suffixed with k|K are multiplied by 1024
                     This suffix is only allowed on decimal constants. */
                  if (c == 'k' || c == 'K')
                    value *= 1024;
                  else
                    *--arg = c;
                  break;
                }
            }
          *end = arg+1;
          return value;
        }
    }

  while (not_done)
    {
      char c;
      c = *arg++;
      if (c == 0 || !strchr (char_bag, c))
	{
          not_done = 0;
          *--arg = c;
        }
      else
        {
          if (c >= 'a' && c <= 'z')
            c = c - ('a' - '9') + 1;
          else if (c >= 'A' && c <= 'Z')
            c = c - ('A' - '9') + 1;

          c -= '0';
          value = (value << shiftvalue) + c;
        }
    }
  *end = arg+1;
  return value;
}


static int parse_reg (Register *r, int cl, char *rt)
{
  r->regno = cl | (rt[1] - '0');
  r->flags = F_REG_NONE;
  return REG;
}

/* Our start state is KEYWORD as we have
   command keywords such as PREFETCH.  */

void
set_start_state (void)
{
  BEGIN KEYWORD;
}


#ifndef yywrap
int
yywrap ()
{
  return 1;
}
#endif
