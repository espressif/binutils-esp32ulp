/* BFD Support for the ESP32ULP processor.
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

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

const bfd_arch_info_type bfd_esp32ulp_arch =
{
	16,     			/* Bits in a word.  */
	32,  				/* Bits in an address.  */
	8,     				/* Bits in a byte.  */
	bfd_arch_esp32ulp,
	0,                	/* Only one machine.  */
	"esp32ulp",        	/* Arch name.  */
	"esp32ulp",        	/* Arch printable name.  */
	4,                	/* Section align power.  */
	TRUE,             	/* The one and only.  */
	bfd_default_compatible,
	bfd_default_scan,
	bfd_arch_default_fill,
	0,
};
