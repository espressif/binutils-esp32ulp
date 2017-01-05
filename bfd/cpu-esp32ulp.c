/* BFD Support for the ESP32 ULP processor.

*/

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

const bfd_arch_info_type bfd_esp32ulp_arch =
{
	16,     		/* Bits in a word.  */
	32,  		/* Bits in an address.  */
	8,     		/* Bits in a byte.  */
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
