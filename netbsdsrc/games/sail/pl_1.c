/*	$NetBSD: pl_1.c,v 1.5 1997/10/13 21:04:02 christos Exp $	*/

/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)pl_1.c	8.1 (Berkeley) 5/31/93";
#else
__RCSID("$NetBSD: pl_1.c,v 1.5 1997/10/13 21:04:02 christos Exp $");
#endif
#endif /* not lint */

#include "player.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * If we get here before a ship is chosen, then ms == 0 and
 * we don't want to update the score file, or do any Write's either.
 * We can assume the sync file is already created and may need
 * to be removed.
 * Of course, we don't do any more Sync()'s if we got here
 * because of a Sync() failure.
 */
void
leave(conditions)
int conditions;
{
	(void) signal(SIGHUP, SIG_IGN);
	(void) signal(SIGINT, SIG_IGN);
	(void) signal(SIGQUIT, SIG_IGN);
	(void) signal(SIGALRM, SIG_IGN);
	(void) signal(SIGCHLD, SIG_IGN);

	if (done_curses) {
		Msg("It looks like you've had it!");
		switch (conditions) {
		case LEAVE_QUIT:
			break;
		case LEAVE_CAPTURED:
			Msg("Your ship was captured.");
			break;
		case LEAVE_HURRICAN:
			Msg("Hurricane!  All ships destroyed.");
			break;
		case LEAVE_DRIVER:
			Msg("The driver died.");
			break;
		case LEAVE_SYNC:
			Msg("Synchronization error.");
			break;
		default:
			Msg("A funny thing happened (%d).", conditions);
		}
	} else {
		switch (conditions) {
		case LEAVE_QUIT:
			break;
		case LEAVE_DRIVER:
			printf("The driver died.\n");
			break;
		case LEAVE_FORK:
			perror("fork");
			break;
		case LEAVE_SYNC:
			printf("Synchronization error\n.");
			break;
		default:
			printf("A funny thing happened (%d).\n",
				conditions);
		}
	}

	if (ms != 0) {
		logger(ms);
		if (conditions != LEAVE_SYNC) {
			makemsg(ms, "Captain %s relinquishing.",
				mf->captain);
			Write(W_END, ms, 0, 0, 0, 0, 0);
			(void) Sync();
		}
	}
	sync_close(!hasdriver);
	sleep(5);
	cleanupscreen();
	exit(0);
}

/*ARGSUSED*/
void
choke(n)
	int n;
{
	leave(LEAVE_QUIT);
}

/*ARGSUSED*/
void
child(n)
	int n;
{
	union wait status;
	int pid;

	(void) signal(SIGCHLD, SIG_IGN);
	do {
		pid = wait3((int *)&status, WNOHANG, (struct rusage *)0);
		if (pid < 0 || (pid > 0 && !WIFSTOPPED(status)))
			hasdriver = 0;
	} while (pid > 0);
	(void) signal(SIGCHLD, child);
}
