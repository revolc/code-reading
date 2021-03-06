/*	$NetBSD: ectype.h,v 1.3 1997/01/09 20:22:55 tls Exp $	*/

/*-
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)ectype.h	4.2 (Berkeley) 4/26/91
 *	$NetBSD: ectype.h,v 1.3 1997/01/09 20:22:55 tls Exp $
 */

#define	INCLUDED_ECTYPE

#define	E_UPPER	0x01
#define	E_LOWER	0x02
#define	E_DIGIT	0x04
#define	E_SPACE	0x08
#define	E_PUNCT	0x10
#define	E_PRINT 0x20

#define	Eisalpha(c)	(ectype[(c)&0xff]&(E_UPPER|E_LOWER))
#define	Eisupper(c)	(ectype[(c)&0xff]&E_UPPER)
#define	Eislower(c)	(ectype[(c)&0xff]&E_LOWER)
#define	Eisdigit(c)	(ectype[(c)&0xff]&E_DIGIT)
#define	Eisalnum(c)	(ectype[(c)&0xff]&(E_UPPER|E_LOWER|E_DIGIT))
#define	Eisspace(c)	(ectype[(c)&0xff]&E_SPACE)	/* blank or null */
#define	Eispunct(c)	(ectype[(c)&0xff]&E_PUNCT)
#define	Eisprint(c)	(ectype[(c)&0xff]&E_PRINT)
