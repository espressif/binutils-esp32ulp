/* output-file.c -  Deal with the output file
   Copyright (C) 1987, 1990, 1991 Free Software Foundation, Inc.

This file is part of GAS, the GNU Assembler.

GAS is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GAS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GAS; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* static const char rcsid[] = "$Id$"; */

/*
 * Confines all details of emitting object bytes to this module.
 * All O/S specific crocks should live here.
 * What we lose in "efficiency" we gain in modularity.
 * Note we don't need to #include the "as.h" file. No common coupling!
 */

 /* note that we do need config info.  xoxorich. */

/* #include "style.h" */
#include <stdio.h>

#include "as.h"

#include "output-file.h"

static FILE *stdoutput;

void output_file_create(name)
char *name;
{
  if(name[0]=='-' && name[1]=='\0')
    stdoutput=stdout;
  else if ( ! (stdoutput = fopen( name, "w" )) )
    {
      as_perror ("FATAL: Can't create %s", name);
      exit(42);
    }
} /* output_file_create() */



void output_file_close(filename)
char *filename;
{
  if ( EOF == fclose( stdoutput ) )
    {
      as_perror ("FATAL: Can't close %s", filename);
      exit(42);
    }
  stdoutput = NULL;		/* Trust nobody! */
} /* output_file_close() */

void output_file_append(where, length, filename)
char *where;
long length;
char *filename;
{

  for (; length; length--,where++)
    {
    	(void)putc(*where,stdoutput);
	if(ferror(stdoutput))
      /* if ( EOF == (putc( *where, stdoutput )) ) */
	{
	  as_perror("Failed to emit an object byte", filename);
	  as_fatal("Can't continue");
	}
    }
} /* output_file_append() */

/* end: output-file.c */
