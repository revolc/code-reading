/*	$NetBSD: limits.h,v 1.1.1.1 1996/05/05 12:17:13 oki Exp $	*/

/* Just use the common m68k definition */
#include <m68k/limits.h>

#ifdef KERNEL
#define CLOCKS_PER_SEC 100
#endif
