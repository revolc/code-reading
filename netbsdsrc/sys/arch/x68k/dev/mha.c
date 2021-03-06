/*	$NetBSD: mha.c,v 1.2 1997/10/19 20:45:17 oki Exp $	*/

/*
 * Copyright (c) 1996 Masaru Oki, Takumi Nakamura and Masanobu Saitoh.  All rights reserved.
 * Copyright (c) 1994, 1995, 1996 Charles M. Hannum.  All rights reserved.
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
 *	This product includes software developed by Charles M. Hannum.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * Copyright (c) 1994 Jarle Greipsland
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Synchronous data transfers? */
#define SPC_USE_SYNCHRONOUS	0
#define SPC_SYNC_REQ_ACK_OFS 	8

/* Include debug functions?  At the end of this file there are a bunch of
 * functions that will print out various information regarding queued SCSI
 * commands, driver state and chip contents.  You can call them from the
 * kernel debugger.  If you set SPC_DEBUG to 0 they are not included (the
 * kernel uses less memory) but you lose the debugging facilities.
 */
#define SPC_DEBUG		0

/* End of customizable parameters */

/*
 * MB86601A SCSI Protocol Controller (SPC) routines for MANKAI Mach-2
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/device.h>
#include <sys/buf.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/queue.h>

#include <dev/scsipi/scsi_all.h>
#include <dev/scsipi/scsipi_all.h>
#include <dev/scsipi/scsi_message.h>
#include <dev/scsipi/scsiconf.h>

#include <x68k/x68k/iodevice.h>
#include <x68k/dev/mb86601reg.h>
#include <x68k/dev/mhavar.h>
#include <x68k/dev/dmavar.h>

#if 0
#define WAIT {if (sc->sc_pc[2]) {printf("[W_%d", __LINE__); while (sc->sc_pc[2] & 0x40);printf("]");}}
#else
#define WAIT {while (sc->sc_pc[2] & 0x40);}
#endif

#define SSR	(sc->sc_pc[2])
#define	SS_IREQUEST	0x80
#define	SS_BUSY		0x40
#define	SS_DREG_FULL	0x02

#define	NSR	(sc->sc_pc[3])

#define	SIR	(sc->sc_pc[4])

#define	CMR	(sc->sc_pc[5])
#define	CMD_SEL_AND_CMD	0x00
#define	CMD_SELECT	0x09
#define	CMD_SET_ATN	0x0a
#define	CMD_RESET_ATN	0x0b
#define	CMD_RESET_ACK	0x0d
#define	CMD_SEND_FROM_MPU	0x10
#define	CMD_SEND_FROM_DMA	0x11
#define	CMD_RECEIVE_TO_MPU	0x12
#define	CMD_RECEIVE_TO_DMA	0x13
#define	CMD_RECEIVE_MSG	0x1a
#define	CMD_RECEIVE_STS	0x1c
#define	CMD_SOFT_RESET	0x40
#define	CMD_SCSI_RESET	0x42
#define	CMD_SET_UP_REG	0x43

#define	SCR	(sc->sc_pc[11])

#define	TMR	(sc->sc_pc[12])
#define	TM_SYNC		0x80
#define	TM_ASYNC	0x00

#define	WAR	(sc->sc_pc[15])
#define	WA_MCSBUFWIN	0x00
#define	WA_UPMWIN	0x80
#define	WA_INITWIN	0xc0

#define	MBR	(sc->sc_pc[15])

#define ISCSR	(sc->sc_ps[2])

#define	CCR	(sc->sc_pcx[0])
#define	OIR	(sc->sc_pcx[1])
#define	AMR	(sc->sc_pcx[2])
#define	SMR	(sc->sc_pcx[3])
#define	SRR	(sc->sc_pcx[4])
#define	STR	(sc->sc_pcx[5])
#define	RTR	(sc->sc_pcx[6])
#define	ATR	(sc->sc_pcx[7])
#define	PER	(sc->sc_pcx[8])
#define	IER	(sc->sc_pcx[9])
#define	IE_ALL	0xBF

#define	GLR	(sc->sc_pcx[10])
#define	DMR	(sc->sc_pcx[11])
#define	IMR	(sc->sc_pcx[12])


#ifndef DDB
#define	Debugger() panic("should call debugger here (mha.c)")
#endif /* ! DDB */


#if SPC_DEBUG
#define SPC_SHOWACBS	0x01
#define SPC_SHOWINTS	0x02
#define SPC_SHOWCMDS	0x04
#define SPC_SHOWMISC	0x08
#define SPC_SHOWTRAC	0x10
#define SPC_SHOWSTART	0x20
#define SPC_SHOWPHASE	0x40
#define SPC_SHOWDMA	0x80
#define SPC_SHOWCCMDS	0x100
#define SPC_SHOWMSGS	0x200
#define SPC_DOBREAK	0x400

int mha_debug =
#if 0
0x7FF;
#else
SPC_SHOWSTART|SPC_SHOWTRAC;
#endif


#define SPC_ACBS(str)  do {if (mha_debug & SPC_SHOWACBS) printf str;} while (0)
#define SPC_MISC(str)  do {if (mha_debug & SPC_SHOWMISC) printf str;} while (0)
#define SPC_INTS(str)  do {if (mha_debug & SPC_SHOWINTS) printf str;} while (0)
#define SPC_TRACE(str) do {if (mha_debug & SPC_SHOWTRAC) printf str;} while (0)
#define SPC_CMDS(str)  do {if (mha_debug & SPC_SHOWCMDS) printf str;} while (0)
#define SPC_START(str) do {if (mha_debug & SPC_SHOWSTART) printf str;}while (0)
#define SPC_PHASE(str) do {if (mha_debug & SPC_SHOWPHASE) printf str;}while (0)
#define SPC_DMA(str)   do {if (mha_debug & SPC_SHOWDMA) printf str;}while (0)
#define SPC_MSGS(str)  do {if (mha_debug & SPC_SHOWMSGS) printf str;}while (0)
#define	SPC_BREAK()    do {if ((mha_debug & SPC_DOBREAK) != 0) Debugger();} while (0)
#define	SPC_ASSERT(x)  do {if (x) {} else {printf("%s at line %d: assertion failed\n", sc->sc_dev.dv_xname, __LINE__); Debugger();}} while (0)
#else
#define SPC_ACBS(str)
#define SPC_MISC(str)
#define SPC_INTS(str)
#define SPC_TRACE(str)
#define SPC_CMDS(str)
#define SPC_START(str)
#define SPC_PHASE(str)
#define SPC_DMA(str)
#define SPC_MSGS(str)
#define	SPC_BREAK()
#define	SPC_ASSERT(x)
#endif

int	mhamatch	__P((struct device *, void *, void *));
void	mhaattach	__P((struct device *, struct device *, void *));
void	mhaselect	__P((struct mha_softc *,
				     u_char, u_char, u_char *, u_char));
void	mha_scsi_reset	__P((struct mha_softc *));
void	mha_reset	__P((struct mha_softc *));
void	mha_free_acb	__P((struct mha_softc *, struct acb *, int));
void	mha_sense	__P((struct mha_softc *, struct acb *));
void	mha_msgin	__P((struct mha_softc *));
void	mha_msgout	__P((struct mha_softc *));
int	mha_dataout_pio	__P((struct mha_softc *, u_char *, int));
int	mha_datain_pio	__P((struct mha_softc *, u_char *, int));
int	mha_dataout	__P((struct mha_softc *, u_char *, int));
int	mha_datain	__P((struct mha_softc *, u_char *, int));
void	mha_abort	__P((struct mha_softc *, struct acb *));
void 	mha_init	__P((struct mha_softc *));
int	mha_scsi_cmd	__P((struct scsipi_xfer *));
int	mha_poll	__P((struct mha_softc *, struct acb *));
void	mha_sched	__P((struct mha_softc *));
void	mha_done	__P((struct mha_softc *, struct acb *));
int	mhaintr		__P((int));
void	mha_timeout	__P((void *));
void	mha_minphys	__P((struct buf *));
void	mha_dequeue	__P((struct mha_softc *, struct acb *));
inline void	mha_setsync	__P((struct mha_softc *, struct spc_tinfo *));
#ifdef SPC_DEBUG
void	mha_print_acb __P((struct acb *));
void	mha_show_scsi_cmd __P((struct acb *));
void	mha_print_active_acb __P((void));
void	mha_dump_driver __P((struct mha_softc *));
#endif
volatile void *	mha_find	__P((int));

static int mha_dataio_dma __P((int, int, struct mha_softc *, u_char *, int));

struct cfattach mha_ca = {
	sizeof(struct mha_softc), mhamatch, mhaattach
};

struct cfdriver mha_cd = {
	NULL, "mha", DV_DULL
};

struct scsipi_adapter mha_switch = {
	mha_scsi_cmd,
	mha_minphys,
	0,
	0,
};

struct scsipi_device mha_dev = {
	NULL,			/* Use default error handler */
	NULL,			/* have a queue, served by this */
	NULL,			/* have no async handler */
	NULL,			/* Use default 'done' routine */
};

/*
 * returns non-zero value if a controller is found.
 */
int
mhamatch(parent, match, aux)
	struct device *parent;
	void *match, *aux;
{
	struct cfdata *cf = match;

	if (strcmp(aux, "mha") || mha_find(cf->cf_unit) == 0)
		return 0;
	return 1;
}

/*
 * Find the board
 */
volatile void *
mha_find(unit)
	int unit;
{
	volatile void *addr;

	if (unit > 1)
		return 0;
	/* Find only on-board ROM */
  if (badaddr(IODEVbase->exscsirom)
      || bcmp((void *)&IODEVbase->exscsirom[0x24], "SCSIEX", 6))
    return 0;

  /* If bdid exists, this board is ``CZ-6BS1'' */
  if (!badbaddr(&IODEVbase->io_exspc.bdid))
    return 0;

	return (void *)(&IODEVbase->exscsirom[0x60]);
}

/*
 */

struct mha_softc *tmpsc;

void
mhaattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct mha_softc *sc = (void *)self;

	tmpsc = sc;	/* XXX */

	SPC_TRACE(("mhaattach  "));
	sc->sc_state = SPC_INIT;
	sc->sc_iobase = mha_find(sc->sc_dev.dv_unit); /* XXX */

	sc->sc_pc = (volatile u_char *)sc->sc_iobase;
	sc->sc_ps = (volatile u_short *)sc->sc_iobase;
	sc->sc_pcx = &sc->sc_pc[0x10];

	sc->sc_id = IODEVbase->io_sram[0x70] & 0x7; /* XXX */

	mha_init(sc);	/* Init chip and driver */
	sc->sc_phase  = BUSFREE_PHASE;

	/*
	 * Fill in the prototype scsi_link
	 */
	sc->sc_link.scsipi_scsi.channel = SCSI_CHANNEL_ONLY_ONE;
	sc->sc_link.adapter_softc = sc;
	sc->sc_link.scsipi_scsi.adapter_target = sc->sc_id;
	sc->sc_link.adapter = &mha_switch;
	sc->sc_link.device = &mha_dev;
	sc->sc_link.openings = 2;
	sc->sc_link.scsipi_scsi.max_target = 7;
	sc->sc_link.type = BUS_SCSI;

	sc->sc_spcinitialized = 0;
	WAR = WA_INITWIN;
#if 1
	CCR = 0x14;
	OIR = sc->sc_id;
	AMR = 0x00;
	SMR = 0x00;
	SRR = 0x00;
	STR = 0x20;
	RTR = 0x40;
	ATR = 0x01;
	PER = 0xc9;
#endif
	IER = IE_ALL;	/* $B$9$Y$F$N3d$j9~$_$r5v2D(B */
#if 1
	GLR = 0x00;
	DMR = 0x30;
	IMR = 0x00;
#endif
	WAR = WA_MCSBUFWIN;

	/* drop off */
	while (SSR & SS_IREQUEST)
	  {
	    unsigned a = ISCSR;
	  }

	CMR = CMD_SET_UP_REG;	/* setup reg cmd. */

	SPC_TRACE(("waiting for intr..."));
	while(!sc->sc_spcinitialized);

	tmpsc = NULL;

	printf("\n");

	config_found(self, &sc->sc_link, scsiprint);
}

void
mha_reset(sc)
	struct mha_softc *sc;
{
	u_short	dummy;
printf("reset...");
	CMR = CMD_SOFT_RESET;
	asm volatile ("nop");	/* XXX wait (4clk in 20mhz) ??? */
	dummy = sc->sc_ps[-1];
	dummy = sc->sc_ps[-1];
	dummy = sc->sc_ps[-1];
	dummy = sc->sc_ps[-1];
	asm volatile ("nop");
	CMR = CMD_SOFT_RESET;
	sc->sc_spcinitialized = 0;
	CMR = CMD_SET_UP_REG;	/* setup reg cmd. */
	while(!sc->sc_spcinitialized);

	sc->sc_id = IODEVbase->io_sram[0x70] & 0x7; /* XXX */
printf("done.\n");
}

/*
 * Pull the SCSI RST line for 500us.
 */
void
mha_scsi_reset(sc)	/* FINISH? */
	struct mha_softc *sc;
{

	CMR = CMD_SCSI_RESET;	/* SCSI RESET */
	WAIT;
}

/*
 * Initialize mha SCSI driver.
 */
void
mha_init(sc)
	struct mha_softc *sc;
{
	struct acb *acb;
	int r;

	if (sc->sc_state == SPC_INIT) {
		/* First time through; initialize. */
		TAILQ_INIT(&sc->ready_list);
		TAILQ_INIT(&sc->nexus_list);
		TAILQ_INIT(&sc->free_list);
		sc->sc_nexus = NULL;
		acb = sc->sc_acb;
		bzero(acb, sizeof(sc->sc_acb));
		for (r = 0; r < sizeof(sc->sc_acb) / sizeof(*acb); r++) {
			TAILQ_INSERT_TAIL(&sc->free_list, acb, chain);
			acb++;
		}
		bzero(&sc->sc_tinfo, sizeof(sc->sc_tinfo));
	} else {
		/* Cancel any active commands. */
		sc->sc_flags |= SPC_ABORTING;
		sc->sc_state = SPC_IDLE;
		if ((acb = sc->sc_nexus) != NULL) {
			acb->xs->error = XS_DRIVER_STUFFUP;
			mha_done(sc, acb);
		}
		while ((acb = sc->nexus_list.tqh_first) != NULL) {
			acb->xs->error = XS_DRIVER_STUFFUP;
			mha_done(sc, acb);
		}
	}

	sc->sc_phase = sc->sc_prevphase = INVALID_PHASE;
	for (r = 0; r < 8; r++) {
		struct spc_tinfo *ti = &sc->sc_tinfo[r];

		ti->flags = 0;
#if SPC_USE_SYNCHRONOUS
		ti->flags |= T_SYNCMODE;
		ti->period = sc->sc_minsync;
		ti->offset = SPC_SYNC_REQ_ACK_OFS;
#else
		ti->period = ti->offset = 0;
#endif
		ti->width = 0;
	}

	sc->sc_state = SPC_IDLE;
#if 0
	mha_reset(sc);
#endif
}

void
mha_free_acb(sc, acb, flags)
	struct mha_softc *sc;
	struct acb *acb;
	int flags;
{
	int s;

	s = splbio();

	acb->flags = 0;
	TAILQ_INSERT_HEAD(&sc->free_list, acb, chain);

	/*
	 * If there were none, wake anybody waiting for one to come free,
	 * starting with queued entries.
	 */
	if (acb->chain.tqe_next == 0)
		wakeup(&sc->free_list);

	splx(s);
}


/*
 * DRIVER FUNCTIONS CALLABLE FROM HIGHER LEVEL DRIVERS
 */

/*
 * Expected sequence:
 * 1) Command inserted into ready list
 * 2) Command selected for execution
 * 3) Command won arbitration and has selected target device
 * 4) Send message out (identify message, eventually also sync.negotiations)
 * 5) Send command
 * 5a) Receive disconnect message, disconnect.
 * 5b) Reselected by target
 * 5c) Receive identify message from target.
 * 6) Send or receive data
 * 7) Receive status
 * 8) Receive message (command complete etc.)
 * 9) If status == SCSI_CHECK construct a synthetic request sense SCSI cmd.
 *    Repeat 2-8 (no disconnects please...)
 */

/*
 * Start a selection.  This is used by mha_sched() to select an idle target,
 * and by mha_done() to immediately reselect a target to get sense information.
 */
void
mhaselect(sc, target, lun, cmd, clen)
	struct mha_softc *sc;
	u_char target, lun;
	u_char *cmd;
	u_char clen;
{
#if 0
	struct scsi_link *sc_link = acb->xs->sc_link;
#endif
	struct spc_tinfo *ti = &sc->sc_tinfo[target];
	int i;
	int s;

	s = splbio();	/* XXX */

	SPC_TRACE(("[mhaselect(t%d,l%d,cmd:%x)] ", target, lun, *(u_char *)cmd));

	/* CDB $B$r(B SPC $B$N(B MCS REG $B$K%;%C%H$9$k(B */
	/* Now the command into the FIFO */
	WAIT;
#if 1
	SPC_MISC(("[cmd:"));
	for (i = 0; i < clen; i++)
	  {
	    unsigned c = cmd[i];
	    if (i == 1)
	      c |= lun << 5;
	    SPC_MISC((" %02x", c));
	    sc->sc_pcx[i] = c;
	  }
	SPC_MISC(("], target=%d\n", target));
#else
	bcopy(cmd, sc->sc_pcx, clen);
#endif
	if (NSR & 0x80)
		panic("scsistart: already selected...");
	sc->sc_phase  = COMMAND_PHASE;

	/* new state ASP_SELECTING */
	sc->sc_state = SPC_SELECTING;

	SIR = target;
#if 0
	CMR = CMD_SELECT;
#else
	CMR = CMD_SEL_AND_CMD;	/* select & cmd */
#endif
	splx(s);
}

#if 0
int
mha_reselect(sc, message)
	struct mha_softc *sc;
	u_char message;
{
	u_char selid, target, lun;
	struct acb *acb;
	struct scsipi_link *sc_link;
	struct spc_tinfo *ti;

	/*
	 * The SCSI chip made a snapshot of the data bus while the reselection
	 * was being negotiated.  This enables us to determine which target did
	 * the reselect.
	 */
	selid = sc->sc_selid & ~(1 << sc->sc_id);
	if (selid & (selid - 1)) {
		printf("%s: reselect with invalid selid %02x; sending DEVICE RESET\n",
		    sc->sc_dev.dv_xname, selid);
		SPC_BREAK();
		goto reset;
	}

	/*
	 * Search wait queue for disconnected cmd
	 * The list should be short, so I haven't bothered with
	 * any more sophisticated structures than a simple
	 * singly linked list.
	 */
	target = ffs(selid) - 1;
	lun = message & 0x07;
	for (acb = sc->nexus_list.tqh_first; acb != NULL;
	     acb = acb->chain.tqe_next) {
		sc_link = acb->xs->sc_link;
		if (sc_link->scsipi_scsi.target == target &&
		    sc_link->scsipi_scsi.lun == lun)
			break;
	}
	if (acb == NULL) {
		printf("%s: reselect from target %d lun %d with no nexus; sending ABORT\n",
		    sc->sc_dev.dv_xname, target, lun);
		SPC_BREAK();
		goto abort;
	}

	/* Make this nexus active again. */
	TAILQ_REMOVE(&sc->nexus_list, acb, chain);
	sc->sc_state = SPC_HASNEXUS;
	sc->sc_nexus = acb;
	ti = &sc->sc_tinfo[target];
	ti->lubusy |= (1 << lun);
	mha_setsync(sc, ti);

	if (acb->flags & ACB_RESET)
		mha_sched_msgout(sc, SEND_DEV_RESET);
	else if (acb->flags & ACB_ABORTED)
		mha_sched_msgout(sc, SEND_ABORT);

	/* Do an implicit RESTORE POINTERS. */
	sc->sc_dp = acb->daddr;
	sc->sc_dleft = acb->dleft;
	sc->sc_cp = (u_char *)&acb->cmd;
	sc->sc_cleft = acb->clen;

	return (0);

reset:
	mha_sched_msgout(sc, SEND_DEV_RESET);
	return (1);

abort:
	mha_sched_msgout(sc, SEND_ABORT);
	return (1);
}
#endif
/*
 * Start a SCSI-command
 * This function is called by the higher level SCSI-driver to queue/run
 * SCSI-commands.
 */
int
mha_scsi_cmd(xs)
	struct scsipi_xfer *xs;
{
	struct scsipi_link *sc_link = xs->sc_link;
	struct mha_softc *sc = sc_link->adapter_softc;
	struct acb *acb;
	int s, flags;

	SPC_TRACE(("[mha_scsi_cmd] "));
	SPC_CMDS(("[0x%x, %d]->%d ", (int)xs->cmd->opcode, xs->cmdlen,
	    sc_link->scsipi_scsi.target));

	flags = xs->flags;

	/* Get a mha command block */
	s = splbio();
	acb = sc->free_list.tqh_first;
	if (acb) {
		TAILQ_REMOVE(&sc->free_list, acb, chain);
		ACB_SETQ(acb, ACB_QNONE);
	}
	splx(s);

	if (acb == NULL) {
		SPC_MISC(("TRY_AGAIN_LATER"));
		return TRY_AGAIN_LATER;
	}

	/* Initialize acb */
	acb->xs = xs;
	bcopy(xs->cmd, &acb->cmd, xs->cmdlen);
	acb->clen = xs->cmdlen;
	acb->daddr = xs->data;
	acb->dleft = xs->datalen;
	acb->stat = 0;

	s = splbio();
	ACB_SETQ(acb, ACB_QREADY);
	TAILQ_INSERT_TAIL(&sc->ready_list, acb, chain);
#if 1
	timeout(mha_timeout, acb, (xs->timeout*hz)/1000);
#endif

	/*
	 * $B%-%e!<$N=hM}Cf$G$J$1$l$P!"%9%1%8%e!<%j%s%03+;O$9$k(B
	 */
	if (sc->sc_state == SPC_IDLE)
		mha_sched(sc);

	splx(s);

	if (flags & SCSI_POLL) {
		/* Not allowed to use interrupts, use polling instead */
		return mha_poll(sc, acb);
	}

	SPC_MISC(("SUCCESSFULLY_QUEUED"));
	return SUCCESSFULLY_QUEUED;
}

/*
 * Adjust transfer size in buffer structure
 */
void
mha_minphys(bp)
	struct buf *bp;
{

	SPC_TRACE(("mha_minphys  "));
	minphys(bp);
}

/*
 * Used when interrupt driven I/O isn't allowed, e.g. during boot.
 */
int
mha_poll(sc, acb)
	struct mha_softc *sc;
	struct acb *acb;
{
	struct scsipi_xfer *xs = acb->xs;
	int count = xs->timeout * 100;
	int s = splbio();

	SPC_TRACE(("[mha_poll] "));

	while (count) {
		/*
		 * If we had interrupts enabled, would we
		 * have got an interrupt?
		 */
		if (SSR & SS_IREQUEST)
			mhaintr(sc->sc_dev.dv_unit);
		if ((xs->flags & ITSDONE) != 0)
			break;
		DELAY(10);
#if 1
		if (sc->sc_state == SPC_IDLE) {
			SPC_TRACE(("[mha_poll: rescheduling] "));
			mha_sched(sc);
		}
#endif
		count--;
	}

	if (count == 0) {
		SPC_MISC(("mha_poll: timeout"));
		mha_timeout((caddr_t)acb);
	}
	splx(s);
	return COMPLETE;
}

/*
 * LOW LEVEL SCSI UTILITIES
 */

/*
 * Set synchronous transfer offset and period.
 */
inline void
mha_setsync(sc, ti)
	struct mha_softc *sc;
	struct spc_tinfo *ti;
{
}


/*
 * Schedule a SCSI operation.  This has now been pulled out of the interrupt
 * handler so that we may call it from mha_scsi_cmd and mha_done.  This may
 * save us an unecessary interrupt just to get things going.  Should only be
 * called when state == SPC_IDLE and at bio pl.
 */
void
mha_sched(sc)
	register struct mha_softc *sc;
{
	struct scsipi_link *sc_link;
	struct acb *acb;
	int t;

	SPC_TRACE(("[mha_sched] "));
	if (sc->sc_state != SPC_IDLE)
		panic("mha_sched: not IDLE (state=%d)", sc->sc_state);

	if (sc->sc_flags & SPC_ABORTING)
		return;

	/*
	 * Find first acb in ready queue that is for a target/lunit
	 * combinations that is not busy.
	 */
	for (acb = sc->ready_list.tqh_first; acb ; acb = acb->chain.tqe_next) {
		struct spc_tinfo *ti;
		sc_link = acb->xs->sc_link;
		t = sc_link->scsipi_scsi.target;
		ti = &sc->sc_tinfo[t];
		if (!(ti->lubusy & (1 << sc_link->scsipi_scsi.lun))) {
			if ((acb->flags & ACB_QBITS) != ACB_QREADY)
				panic("mha: busy entry on ready list");
			TAILQ_REMOVE(&sc->ready_list, acb, chain);
			ACB_SETQ(acb, ACB_QNONE);
			sc->sc_nexus = acb;
			sc->sc_flags = 0;
			sc->sc_prevphase = INVALID_PHASE;
			sc->sc_dp = acb->daddr;
			sc->sc_dleft = acb->dleft;
			ti->lubusy |= (1<<sc_link->scsipi_scsi.lun);
			mhaselect(sc, t, sc_link->scsipi_scsi.lun,
				     (u_char *)&acb->cmd, acb->clen);
			break;
		} else {
			SPC_MISC(("%d:%d busy\n",
			    sc_link->scsipi_scsi.target,
			    sc_link->scsipi_scsi.lun));
		}
	}
}

void
mha_sense(sc, acb)
	struct mha_softc *sc;
	struct acb *acb;
{
	struct scsipi_xfer *xs = acb->xs;
	struct scsipi_link *sc_link = xs->sc_link;
	struct spc_tinfo *ti = &sc->sc_tinfo[sc_link->scsipi_scsi.target];
	struct scsipi_sense *ss = (void *)&acb->cmd;

	SPC_MISC(("requesting sense  "));
	/* Next, setup a request sense command block */
	bzero(ss, sizeof(*ss));
	ss->opcode = REQUEST_SENSE;
	ss->byte2 = sc_link->scsipi_scsi.lun << 5;
	ss->length = sizeof(struct scsipi_sense_data);
	acb->clen = sizeof(*ss);
	acb->daddr = (char *)&xs->sense;
	acb->dleft = sizeof(struct scsipi_sense_data);
	acb->flags |= ACB_CHKSENSE;
	ti->senses++;
	if (acb->flags & ACB_QNEXUS)
		ti->lubusy &= ~(1 << sc_link->scsipi_scsi.lun);
	if (acb == sc->sc_nexus) {
		mhaselect(sc, sc_link->scsipi_scsi.target,
			  sc_link->scsipi_scsi.lun,
			     (void *)&acb->cmd, acb->clen);
	} else {
		mha_dequeue(sc, acb);
		TAILQ_INSERT_HEAD(&sc->ready_list, acb, chain);
		if (sc->sc_state == SPC_IDLE)
			mha_sched(sc);
	}
}

/*
 * POST PROCESSING OF SCSI_CMD (usually current)
 */
void
mha_done(sc, acb)
	struct mha_softc *sc;
	struct acb *acb;
{
	struct scsipi_xfer *xs = acb->xs;
	struct scsipi_link *sc_link = xs->sc_link;
	struct spc_tinfo *ti = &sc->sc_tinfo[sc_link->scsipi_scsi.target];

	SPC_TRACE(("[mha_done(error:%x)] ", xs->error));

#if 1
	untimeout(mha_timeout, acb);
#endif

	/*
	 * Now, if we've come here with no error code, i.e. we've kept the
	 * initial XS_NOERROR, and the status code signals that we should
	 * check sense, we'll need to set up a request sense cmd block and
	 * push the command back into the ready queue *before* any other
	 * commands for this target/lunit, else we lose the sense info.
	 * We don't support chk sense conditions for the request sense cmd.
	 */
	if (xs->error == XS_NOERROR) {
		if ((acb->flags & ACB_ABORTED) != 0) {
			xs->error = XS_TIMEOUT;
		} else if (acb->flags & ACB_CHKSENSE) {
			xs->error = XS_SENSE;
		} else if ((acb->stat & ST_MASK) == SCSI_CHECK) {
			struct scsipi_sense *ss = (void *)&acb->cmd;
			SPC_MISC(("requesting sense "));
			/* First, save the return values */
			xs->resid = acb->dleft;
			xs->status = acb->stat;
			/* Next, setup a request sense command block */
			bzero(ss, sizeof(*ss));
			ss->opcode = REQUEST_SENSE;
			/*ss->byte2 = sc_link->lun << 5;*/
			ss->length = sizeof(struct scsipi_sense_data);
			acb->clen = sizeof(*ss);
			acb->daddr = (char *)&xs->sense;
			acb->dleft = sizeof(struct scsipi_sense_data);
			acb->flags |= ACB_CHKSENSE;
/*XXX - must take off queue here */
			if (acb != sc->sc_nexus) {
				panic("%s: mha_sched: floating acb %p",
					sc->sc_dev.dv_xname, acb);
			}
			TAILQ_INSERT_HEAD(&sc->ready_list, acb, chain);
			ACB_SETQ(acb, ACB_QREADY);
			ti->lubusy &= ~(1<<sc_link->scsipi_scsi.lun);
			ti->senses++;
			timeout(mha_timeout, acb, (xs->timeout*hz)/1000);
			if (sc->sc_nexus == acb) {
				sc->sc_nexus = NULL;
				sc->sc_state = SPC_IDLE;
				mha_sched(sc);
			}
#if 0
			mha_sense(sc, acb);
#endif
			return;
		} else {
			xs->resid = acb->dleft;
		}
	}

	xs->flags |= ITSDONE;

#if SPC_DEBUG
	if ((mha_debug & SPC_SHOWMISC) != 0) {
		if (xs->resid != 0)
			printf("resid=%d ", xs->resid);
		if (xs->error == XS_SENSE)
			printf("sense=0x%02x\n", xs->sense.error_code);
		else
			printf("error=%d\n", xs->error);
	}
#endif

	/*
	 * Remove the ACB from whatever queue it's on.
	 */
	switch (acb->flags & ACB_QBITS) {
	case ACB_QNONE:
		if (acb != sc->sc_nexus) {
			panic("%s: floating acb", sc->sc_dev.dv_xname);
		}
		sc->sc_nexus = NULL;
		sc->sc_state = SPC_IDLE;
		ti->lubusy &= ~(1<<sc_link->scsipi_scsi.lun);
		mha_sched(sc);
		break;
	case ACB_QREADY:
		TAILQ_REMOVE(&sc->ready_list, acb, chain);
		break;
	case ACB_QNEXUS:
		TAILQ_REMOVE(&sc->nexus_list, acb, chain);
		ti->lubusy &= ~(1<<sc_link->scsipi_scsi.lun);
		break;
	case ACB_QFREE:
		panic("%s: dequeue: busy acb on free list",
			sc->sc_dev.dv_xname);
		break;
	default:
		panic("%s: dequeue: unknown queue %d",
			sc->sc_dev.dv_xname, acb->flags & ACB_QBITS);
	}

	/* Put it on the free list, and clear flags. */
#if 0
	TAILQ_INSERT_HEAD(&sc->free_list, acb, chain);
	acb->flags = ACB_QFREE;
#else
	mha_free_acb(sc, acb, xs->flags);
#endif

	ti->cmds++;
	scsipi_done(xs);
}

void
mha_dequeue(sc, acb)
	struct mha_softc *sc;
	struct acb *acb;
{

	if (acb->flags & ACB_QNEXUS) {
		TAILQ_REMOVE(&sc->nexus_list, acb, chain);
	} else {
		TAILQ_REMOVE(&sc->ready_list, acb, chain);
	}
}

/*
 * INTERRUPT/PROTOCOL ENGINE
 */

/*
 * Schedule an outgoing message by prioritizing it, and asserting
 * attention on the bus. We can only do this when we are the initiator
 * else there will be an illegal command interrupt.
 */
#define mha_sched_msgout(m) \
	do {				\
		SPC_MISC(("mha_sched_msgout %d ", m)); \
		CMR = CMD_SET_ATN;	\
		sc->sc_msgpriq |= (m);	\
	} while (0)

#define IS1BYTEMSG(m) (((m) != 0x01 && (m) < 0x20) || (m) >= 0x80)
#define IS2BYTEMSG(m) (((m) & 0xf0) == 0x20)
#define ISEXTMSG(m) ((m) == 0x01)

/*
 * Precondition:
 * The SCSI bus is already in the MSGI phase and there is a message byte
 * on the bus, along with an asserted REQ signal.
 */
void
mha_msgin(sc)
	register struct mha_softc *sc;
{
	register int v;
	int n;

	SPC_TRACE(("[mha_msgin(curmsglen:%d)] ", sc->sc_imlen));

	/*
	 * Prepare for a new message.  A message should (according
	 * to the SCSI standard) be transmitted in one single
	 * MESSAGE_IN_PHASE. If we have been in some other phase,
	 * then this is a new message.
	 */
	if (sc->sc_prevphase != MESSAGE_IN_PHASE) {
		sc->sc_flags &= ~SPC_DROP_MSGI;
		sc->sc_imlen = 0;
	}

	WAIT;

	v = MBR;	/* modified byte */
	v = sc->sc_pcx[0];

	sc->sc_imess[sc->sc_imlen] = v;

	/*
	 * If we're going to reject the message, don't bother storing
	 * the incoming bytes.  But still, we need to ACK them.
	 */

	if ((sc->sc_flags & SPC_DROP_MSGI)) {
		CMR = CMD_SET_ATN;
/*		ESPCMD(sc, ESPCMD_MSGOK);*/
		printf("<dropping msg byte %x>",
			sc->sc_imess[sc->sc_imlen]);
		return;
	}

	if (sc->sc_imlen >= SPC_MAX_MSG_LEN) {
		mha_sched_msgout(SEND_REJECT);
		sc->sc_flags |= SPC_DROP_MSGI;
	} else {
		sc->sc_imlen++;
		/*
		 * This testing is suboptimal, but most
		 * messages will be of the one byte variety, so
		 * it should not effect performance
		 * significantly.
		 */
		if (sc->sc_imlen == 1 && IS1BYTEMSG(sc->sc_imess[0]))
			goto gotit;
		if (sc->sc_imlen == 2 && IS2BYTEMSG(sc->sc_imess[0]))
			goto gotit;
		if (sc->sc_imlen >= 3 && ISEXTMSG(sc->sc_imess[0]) &&
		    sc->sc_imlen == sc->sc_imess[1] + 2)
			goto gotit;
	}
#if 0
	/* Ack what we have so far */
	ESPCMD(sc, ESPCMD_MSGOK);
#endif
	return;

gotit:
	SPC_MSGS(("gotmsg(%x)", sc->sc_imess[0]));
	/*
	 * Now we should have a complete message (1 byte, 2 byte
	 * and moderately long extended messages).  We only handle
	 * extended messages which total length is shorter than
	 * SPC_MAX_MSG_LEN.  Longer messages will be amputated.
	 */
	if (sc->sc_state == SPC_HASNEXUS) {
		struct acb *acb = sc->sc_nexus;
		struct spc_tinfo *ti =
			&sc->sc_tinfo[acb->xs->sc_link->scsipi_scsi.target];

		switch (sc->sc_imess[0]) {
		case MSG_CMDCOMPLETE:
			SPC_MSGS(("cmdcomplete "));
			if (sc->sc_dleft < 0) {
				struct scsipi_link *sc_link = acb->xs->sc_link;
				printf("mha: %d extra bytes from %d:%d\n",
					-sc->sc_dleft,
					sc_link->scsipi_scsi.target,
				        sc_link->scsipi_scsi.lun);
				sc->sc_dleft = 0;
			}
			acb->xs->resid = acb->dleft = sc->sc_dleft;
			sc->sc_flags |= SPC_BUSFREE_OK;
			break;

		case MSG_MESSAGE_REJECT:
#if SPC_DEBUG
			if (mha_debug & SPC_SHOWMSGS)
				printf("%s: our msg rejected by target\n",
					sc->sc_dev.dv_xname);
#endif
#if 1 /* XXX - must remember last message */
scsi_print_addr(acb->xs->sc_link); printf("MSG_MESSAGE_REJECT>>");
#endif
			if (sc->sc_flags & SPC_SYNCHNEGO) {
				ti->period = ti->offset = 0;
				sc->sc_flags &= ~SPC_SYNCHNEGO;
				ti->flags &= ~T_NEGOTIATE;
			}
			/* Not all targets understand INITIATOR_DETECTED_ERR */
			if (sc->sc_msgout == SEND_INIT_DET_ERR)
				mha_sched_msgout(SEND_ABORT);
			break;
		case MSG_NOOP:
			SPC_MSGS(("noop "));
			break;
		case MSG_DISCONNECT:
			SPC_MSGS(("disconnect "));
			ti->dconns++;
			sc->sc_flags |= SPC_DISCON;
			sc->sc_flags |= SPC_BUSFREE_OK;
			if ((acb->xs->sc_link->quirks & SDEV_AUTOSAVE) == 0)
				break;
			/*FALLTHROUGH*/
		case MSG_SAVEDATAPOINTER:
			SPC_MSGS(("save datapointer "));
			acb->dleft = sc->sc_dleft;
			acb->daddr = sc->sc_dp;
			break;
		case MSG_RESTOREPOINTERS:
			SPC_MSGS(("restore datapointer "));
			if (!acb) {
				mha_sched_msgout(SEND_ABORT);
				printf("%s: no DATAPOINTERs to restore\n",
				    sc->sc_dev.dv_xname);
				break;
			}
			sc->sc_dp = acb->daddr;
			sc->sc_dleft = acb->dleft;
			break;
		case MSG_PARITY_ERROR:
			printf("%s:target%d: MSG_PARITY_ERROR\n",
				sc->sc_dev.dv_xname,
				acb->xs->sc_link->scsipi_scsi.target);
			break;
		case MSG_EXTENDED:
			SPC_MSGS(("extended(%x) ", sc->sc_imess[2]));
			switch (sc->sc_imess[2]) {
			case MSG_EXT_SDTR:
				SPC_MSGS(("SDTR period %d, offset %d ",
					sc->sc_imess[3], sc->sc_imess[4]));
				ti->period = sc->sc_imess[3];
				ti->offset = sc->sc_imess[4];
				if (sc->sc_minsync == 0) {
					/* We won't do synch */
					ti->offset = 0;
					mha_sched_msgout(SEND_SDTR);
				} else if (ti->offset == 0) {
					printf("%s:%d: async\n", "mha",
						acb->xs->sc_link->scsipi_scsi.target);
					ti->offset = 0;
					sc->sc_flags &= ~SPC_SYNCHNEGO;
				} else if (ti->period > 124) {
					printf("%s:%d: async\n", "mha",
						acb->xs->sc_link->scsipi_scsi.target);
					ti->offset = 0;
					mha_sched_msgout(SEND_SDTR);
				} else {
					int r = 250/ti->period;
					int s = (100*250)/ti->period - 100*r;
					int p;
#if 0
					p =  mha_stp2cpb(sc, ti->period);
					ti->period = mha_cpb2stp(sc, p);
#endif

#ifdef SPC_DEBUG
					scsi_print_addr(acb->xs->sc_link);
#endif
					if ((sc->sc_flags&SPC_SYNCHNEGO) == 0) {
						/* Target initiated negotiation */
						if (ti->flags & T_SYNCMODE) {
						    ti->flags &= ~T_SYNCMODE;
#ifdef SPC_DEBUG
						    printf("renegotiated ");
#endif
						}
						TMR=TM_ASYNC;
						/* Clamp to our maxima */
						if (ti->period < sc->sc_minsync)
							ti->period = sc->sc_minsync;
						if (ti->offset > 15)
							ti->offset = 15;
						mha_sched_msgout(SEND_SDTR);
					} else {
						/* we are sync */
						sc->sc_flags &= ~SPC_SYNCHNEGO;
						TMR = TM_SYNC;
						ti->flags |= T_SYNCMODE;
					}
#ifdef SPC_DEBUG
					printf("max sync rate %d.%02dMb/s\n",
						r, s);
#endif
				}
				ti->flags &= ~T_NEGOTIATE;
				break;
			default: /* Extended messages we don't handle */
				CMR = CMD_SET_ATN; /* XXX? */
				break;
			}
			break;
		default:
			SPC_MSGS(("ident "));
			/* thanks for that ident... */
			if (!MSG_ISIDENTIFY(sc->sc_imess[0])) {
				SPC_MISC(("unknown "));
printf("%s: unimplemented message: %d\n", sc->sc_dev.dv_xname, sc->sc_imess[0]);
				CMR = CMD_SET_ATN; /* XXX? */
			}
			break;
		}
	} else if (sc->sc_state == SPC_RESELECTED) {
		struct scsipi_link *sc_link = NULL;
		struct acb *acb;
		struct spc_tinfo *ti;
		u_char lunit;

		if (MSG_ISIDENTIFY(sc->sc_imess[0])) { 	/* Identify? */
			SPC_MISC(("searching "));
			/*
			 * Search wait queue for disconnected cmd
			 * The list should be short, so I haven't bothered with
			 * any more sophisticated structures than a simple
			 * singly linked list.
			 */
			lunit = sc->sc_imess[0] & 0x07;
			for (acb = sc->nexus_list.tqh_first; acb;
			     acb = acb->chain.tqe_next) {
				sc_link = acb->xs->sc_link;
				if (sc_link->scsipi_scsi.lun == lunit &&
				    sc->sc_selid == (1<<sc_link->scsipi_scsi.target)) {
					TAILQ_REMOVE(&sc->nexus_list, acb,
					    chain);
					ACB_SETQ(acb, ACB_QNONE);
					break;
				}
			}

			if (!acb) {		/* Invalid reselection! */
				mha_sched_msgout(SEND_ABORT);
				printf("mmespc: invalid reselect (idbit=0x%2x)\n",
				    sc->sc_selid);
			} else {		/* Reestablish nexus */
				/*
				 * Setup driver data structures and
				 * do an implicit RESTORE POINTERS
				 */
				ti = &sc->sc_tinfo[sc_link->scsipi_scsi.target];
				sc->sc_nexus = acb;
				sc->sc_dp = acb->daddr;
				sc->sc_dleft = acb->dleft;
				sc->sc_tinfo[sc_link->scsipi_scsi.target].lubusy
					|= (1<<sc_link->scsipi_scsi.lun);
				if (ti->flags & T_SYNCMODE) {
					TMR = TM_SYNC;	/* XXX */
				} else {
					TMR = TM_ASYNC;
				}
				SPC_MISC(("... found acb"));
				sc->sc_state = SPC_HASNEXUS;
			}
		} else {
			printf("%s: bogus reselect (no IDENTIFY) %0x2x\n",
			    sc->sc_dev.dv_xname, sc->sc_selid);
			mha_sched_msgout(SEND_DEV_RESET);
		}
	} else { /* Neither SPC_HASNEXUS nor SPC_RESELECTED! */
		printf("%s: unexpected message in; will send DEV_RESET\n",
		    sc->sc_dev.dv_xname);
		mha_sched_msgout(SEND_DEV_RESET);
	}

	/* Ack last message byte */
#if 0
	ESPCMD(sc, ESPCMD_MSGOK);
#endif

	/* Done, reset message pointer. */
	sc->sc_flags &= ~SPC_DROP_MSGI;
	sc->sc_imlen = 0;
}

/*
 * Send the highest priority, scheduled message.
 */
void
mha_msgout(sc)
	register struct mha_softc *sc;
{
	struct spc_tinfo *ti;
	int n;

	SPC_TRACE(("mha_msgout  "));

	if (sc->sc_prevphase == MESSAGE_OUT_PHASE) {
		if (sc->sc_omp == sc->sc_omess) {
			/*
			 * This is a retransmission.
			 *
			 * We get here if the target stayed in MESSAGE OUT
			 * phase.  Section 5.1.9.2 of the SCSI 2 spec indicates
			 * that all of the previously transmitted messages must
			 * be sent again, in the same order.  Therefore, we
			 * requeue all the previously transmitted messages, and
			 * start again from the top.  Our simple priority
			 * scheme keeps the messages in the right order.
			 */
			SPC_MISC(("retransmitting  "));
			sc->sc_msgpriq |= sc->sc_msgoutq;
			/*
			 * Set ATN.  If we're just sending a trivial 1-byte
			 * message, we'll clear ATN later on anyway.
			 */
			CMR = CMD_SET_ATN; /* XXX? */
		} else {
			/* This is a continuation of the previous message. */
			n = sc->sc_omp - sc->sc_omess;
			goto nextbyte;
		}
	}

	/* No messages transmitted so far. */
	sc->sc_msgoutq = 0;
	sc->sc_lastmsg = 0;

nextmsg:
	/* Pick up highest priority message. */
	sc->sc_currmsg = sc->sc_msgpriq & -sc->sc_msgpriq;
	sc->sc_msgpriq &= ~sc->sc_currmsg;
	sc->sc_msgoutq |= sc->sc_currmsg;

	/* Build the outgoing message data. */
	switch (sc->sc_currmsg) {
	case SEND_IDENTIFY:
		SPC_ASSERT(sc->sc_nexus != NULL);
		sc->sc_omess[0] =
		    MSG_IDENTIFY(sc->sc_nexus->xs->sc_link->scsipi_scsi.lun, 1);
		n = 1;
		break;

#if SPC_USE_SYNCHRONOUS
	case SEND_SDTR:
		SPC_ASSERT(sc->sc_nexus != NULL);
		ti = &sc->sc_tinfo[sc->sc_nexus->xs->sc_link->scsipi_scsi.target];
		sc->sc_omess[4] = MSG_EXTENDED;
		sc->sc_omess[3] = 3;
		sc->sc_omess[2] = MSG_EXT_SDTR;
		sc->sc_omess[1] = ti->period >> 2;
		sc->sc_omess[0] = ti->offset;
		n = 5;
		break;
#endif

#if SPC_USE_WIDE
	case SEND_WDTR:
		SPC_ASSERT(sc->sc_nexus != NULL);
		ti = &sc->sc_tinfo[sc->sc_nexus->xs->sc_link->scsipi_scsi.target];
		sc->sc_omess[3] = MSG_EXTENDED;
		sc->sc_omess[2] = 2;
		sc->sc_omess[1] = MSG_EXT_WDTR;
		sc->sc_omess[0] = ti->width;
		n = 4;
		break;
#endif

	case SEND_DEV_RESET:
		sc->sc_flags |= SPC_ABORTING;
		sc->sc_omess[0] = MSG_BUS_DEV_RESET;
		n = 1;
		break;

	case SEND_REJECT:
		sc->sc_omess[0] = MSG_MESSAGE_REJECT;
		n = 1;
		break;

	case SEND_PARITY_ERROR:
		sc->sc_omess[0] = MSG_PARITY_ERROR;
		n = 1;
		break;

	case SEND_INIT_DET_ERR:
		sc->sc_omess[0] = MSG_INITIATOR_DET_ERR;
		n = 1;
		break;

	case SEND_ABORT:
		sc->sc_flags |= SPC_ABORTING;
		sc->sc_omess[0] = MSG_ABORT;
		n = 1;
		break;

	default:
		printf("%s: unexpected MESSAGE OUT; sending NOOP\n",
		    sc->sc_dev.dv_xname);
		SPC_BREAK();
		sc->sc_omess[0] = MSG_NOOP;
		n = 1;
		break;
	}
	sc->sc_omp = &sc->sc_omess[n];

nextbyte:
	/* Send message bytes. */
	/* send TRANSFER command. */
	sc->sc_ps[3] = 1;
	sc->sc_ps[4] = n >> 8;
	sc->sc_pc[10] = n;
	sc->sc_ps[-1] = 0x000F;	/* burst */
	asm volatile ("nop");
	CMR = CMD_SEND_FROM_DMA;	/* send from DMA */
	for (;;) {
		if ((SSR & SS_BUSY) != 0)
			break;
		if (SSR & SS_IREQUEST)
			goto out;
	}
	for (;;) {
#if 0
		for (;;) {
			if ((PSNS & PSNS_REQ) != 0)
				break;
			/* Wait for REQINIT.  XXX Need timeout. */
		}
#endif
		if (SSR & SS_IREQUEST) {
			/*
			 * Target left MESSAGE OUT, possibly to reject
			 * our message.
			 *
			 * If this is the last message being sent, then we
			 * deassert ATN, since either the target is going to
			 * ignore this message, or it's going to ask for a
			 * retransmission via MESSAGE PARITY ERROR (in which
			 * case we reassert ATN anyway).
			 */
#if 0
			if (sc->sc_msgpriq == 0)
				CMR = CMD_RESET_ATN;
#endif
			goto out;
		}

#if 0
		/* Clear ATN before last byte if this is the last message. */
		if (n == 1 && sc->sc_msgpriq == 0)
			CMR = CMD_RESET_ATN;
#endif

		while ((SSR & SS_DREG_FULL) != 0)
			;
		/* Send message byte. */
		sc->sc_pc[0] = *--sc->sc_omp;
		--n;
		/* Keep track of the last message we've sent any bytes of. */
		sc->sc_lastmsg = sc->sc_currmsg;

		if (n == 0)
			break;
	}

	/* We get here only if the entire message has been transmitted. */
	if (sc->sc_msgpriq != 0) {
		/* There are more outgoing messages. */
		goto nextmsg;
	}

	/*
	 * The last message has been transmitted.  We need to remember the last
	 * message transmitted (in case the target switches to MESSAGE IN phase
	 * and sends a MESSAGE REJECT), and the list of messages transmitted
	 * this time around (in case the target stays in MESSAGE OUT phase to
	 * request a retransmit).
	 */

out:
	/* Disable REQ/ACK protocol. */
}


/***************************************************************
 *
 *	datain/dataout
 *
 */

int
mha_datain_pio(sc, p, n)
	register struct mha_softc *sc;
	u_char *p;
	int n;
{
	u_short d;
	int a;
	int total_n = n;

	SPC_TRACE(("[mha_datain_pio(%x,%d)", p, n));

	WAIT;
	sc->sc_ps[3] = 1;
	sc->sc_ps[4] = n >> 8;
	sc->sc_pc[10] = n;
	/* $BHa$7$-%=%U%HE>Aw(B */
	CMR = CMD_RECEIVE_TO_MPU;
	for (;;) {
		a = SSR;
		if (a & 0x04) {
			d = sc->sc_ps[0];
			*p++ = d >> 8;
			if (--n > 0) {
				*p++ = d;
				--n;
			}
			a = SSR;
		}
		if (a & 0x40)
			continue;
		if (a & 0x80)
			break;
	}
	SPC_TRACE(("...%d resd]", n));
	return total_n - n;
}

int
mha_dataout_pio(sc, p, n)
	register struct mha_softc *sc;
	u_char *p;
	int n;
{
	u_short d;
	int a;
	int total_n = n;

	SPC_TRACE(("[mha_dataout_pio(%x,%d)", p, n));

	WAIT;
	sc->sc_ps[3] = 1;
	sc->sc_ps[4] = n >> 8;
	sc->sc_pc[10] = n;
	/* $BHa$7$-%=%U%HE>Aw(B */
	CMR = CMD_SEND_FROM_MPU;
	for (;;) {
		a = SSR;
		if (a & 0x04) {
			d = *p++ << 8;
			if (--n > 0) {
				d |= *p++;
				--n;
			}
			sc->sc_ps[0] = d;
			a = SSR;
		}
		if (a & 0x40)
			continue;
		if (a & 0x80)
			break;
	}
	SPC_TRACE(("...%d resd]", n));
	return total_n - n;
}

static int
mha_dataio_dma(dw, cw, sc, p, n)
	int dw;		/* DMA word */
	int cw;		/* CMR word */
	register struct mha_softc *sc;
	u_char *p;
	int n;
{
  int ts;
  char *paddr, *vaddr;

  vaddr = p;
  paddr = (char *)kvtop(vaddr);
  DCFP((vm_offset_t)paddr);	/* XXX */
  for (ts = (NBPG - ((long)vaddr & PGOFSET));
       ts < n && (char *)kvtop(vaddr + ts + 4) == paddr + ts + 4;
       ts += NBPG)
    DCFP((vm_offset_t)paddr + ts);
  if (ts > n)
    ts = n;
#if 0
  printf("(%x,%x)->(%x,%x)\n", p, n, paddr, ts);
  PCIA();	/* XXX */
#endif
  sc->sc_pc[0x80 + (((long)paddr >> 16) & 0xFF)] = 0;
  sc->sc_pc[0x180 + (((long)paddr >> 8) & 0xFF)] = 0;
  sc->sc_pc[0x280 + (((long)paddr >> 0) & 0xFF)] = 0;
  WAIT;
  sc->sc_ps[3] = 1;
  sc->sc_ps[4] = ts >> 8;
  sc->sc_pc[10] = ts;
  /* DMA $BE>Aw@)8f$O0J2<$NDL$j!#(B
     3 ... short bus cycle
     2 ... MAXIMUM XFER.
     1 ... BURST XFER.
     0 ... R/W */
  sc->sc_ps[-1] = dw;	/* burst */
  asm volatile ("nop");
  CMR = cw;	/* receive to DMA */
  return ts;
}
int
mha_dataout(sc, p, n)
	register struct mha_softc *sc;
	u_char *p;
	int n;
{
  register struct acb *acb = sc->sc_nexus;

  if (n == 0)
    return n;

  if (((long)p & 1) || (n & 1))
    return mha_dataout_pio(sc, p, n);
  return mha_dataio_dma(0x000F, CMD_SEND_FROM_DMA, sc, p, n);
}

int
mha_datain(sc, p, n)
	register struct mha_softc *sc;
	u_char *p;
	int n;
{
  int ts;
  register struct acb *acb = sc->sc_nexus;
  char *paddr, *vaddr;

  if (n == 0)
    return n;
  if (acb->cmd.opcode == 0x03 || ((long)p & 1) || (n & 1))
    return mha_datain_pio(sc, p, n);
  return mha_dataio_dma(0x000E, CMD_RECEIVE_TO_DMA, sc, p, n);
}


/*
 * Catch an interrupt from the adaptor
 */
/*
 * This is the workhorse routine of the driver.
 * Deficiencies (for now):
 * 1) always uses programmed I/O
 */
int
mhaintr(unit)
	int unit;
{
	struct mha_softc *sc = mha_cd.cd_devs[unit]; /* XXX */
	u_char ints;
	struct acb *acb;
	struct scsipi_link *sc_link;
	struct spc_tinfo *ti;
	u_char ph;
	u_short r;
	int n;

	/* return if not configured */
	if (sc == NULL)
		return;

#if 1	/* XXX */
	if (tmpsc != NULL && tmpsc != sc) {
	    SPC_MISC(("[%x %x]\n", mha_cd.cd_devs, sc));
	    sc = tmpsc;
	  }
#endif

	/*
	 * $B3d$j9~$_6X;_$K$9$k(B
	 */
#if 0
	SCTL &= ~SCTL_INTR_ENAB;
#endif

	SPC_TRACE(("[mhaintr]"));

loop:
	/*
	 * $BA4E>Aw$,40A4$K=*N;$9$k$^$G%k!<%W$9$k(B
	 */
	/*
	 * First check for abnormal conditions, such as reset.
	 */
#if 0
#if 1 /* XXX? */
	while (((ints = SSR) & SS_IREQUEST) == 0)
		delay(1);
	SPC_MISC(("ints = 0x%x  ", ints));
#else /* usually? */
	ints = SSR;
#endif
#endif
  while (SSR & SS_IREQUEST)
    {
      acb = sc->sc_nexus;
      r = ISCSR;
      SPC_MISC(("[r=0x%x]", r));
      switch (r >> 8)
	{
	default:
	  printf("[addr=%x\n"
		 "result=0x%x\n"
		 "cmd=0x%x\n"
		 "ph=0x%x(ought to be %d)]\n",
		 &ISCSR,
		 r,
		 acb->xs->cmd->opcode,
		 SCR, sc->sc_phase);
	  panic("unexpected result.");
	case 0x82:	/* selection timeout */
	  SPC_MISC(("selection timeout  "));
	  sc->sc_phase = BUSFREE_PHASE;
	  SPC_ASSERT(sc->sc_nexus != NULL);
	  acb = sc->sc_nexus;
	  delay(250);
	  acb->xs->error = XS_SELTIMEOUT;
	  mha_done(sc, acb);
	  continue;	/* XXX ??? msaitoh */
	case 0x60:	/* command completed */
	  sc->sc_spcinitialized++;
	  if (sc->sc_phase == BUSFREE_PHASE)
	    continue;
	  ph = SCR;
	  if (ph & PSNS_ACK)
	    {
	      int s;
	      /* $B$U$D!<$N%3%^%s%I$,=*N;$7$?$i$7$$(B */
SPC_MISC(("0x60)phase = %x(ought to be %x)\n", ph & PHASE_MASK, sc->sc_phase));
# if 0
	      switch (sc->sc_phase)
#else
	      switch (ph & PHASE_MASK)
#endif
		{
		case STATUS_PHASE:
			if (sc->sc_state != SPC_HASNEXUS)
			  {
			    printf("stsin: !SPC_HASNEXUS->(%d)\n", sc->sc_state);
			  }
			SPC_ASSERT(sc->sc_nexus != NULL);
			acb = sc->sc_nexus;
			WAIT;
			s = MBR;
			SPC_ASSERT(s == 1);
			acb->stat = sc->sc_pcx[0]; /* XXX */
			SPC_MISC(("stat=0x%02x  ", acb->stat));
			sc->sc_prevphase = STATUS_PHASE;
			break;
		case MESSAGE_IN_PHASE:
			mha_msgin(sc);
			sc->sc_prevphase = MESSAGE_IN_PHASE;
			break;
		}
	      WAIT;
	      CMR = CMD_RESET_ACK;	/* reset ack */
	      /*mha_done(sc, acb);	XXX */
	      continue;
	    }
	  else if (NSR & 0x80)	/* nexus */
	    {
#if 1
		if (sc->sc_state == SPC_SELECTING)	/* XXX msaitoh */
		  sc->sc_state = SPC_HASNEXUS;
	      /* $B%U%'!<%:$N7h$aBG$A$r$9$k(B
		 $B30$l$?$i!"(Binitial-phase error(0x54) $B$,(B
		 $BJV$C$F$/$k$s$GCm0U$7$?$^$(!#(B
		 $B$G$b$J$<$+(B 0x65 $B$,JV$C$F$-$?$j$7$F$M!<$+(B? */
	      WAIT;
	      if (SSR & SS_IREQUEST)
		continue;
	      switch (sc->sc_phase)
		{
		default:
		  panic("$B8+CN$i$L(B phase $B$,Mh$A$^$C$?$@$h(B");
		case MESSAGE_IN_PHASE:
		  /* $B2?$b$7$J$$(B */
		  continue;
		case STATUS_PHASE:
		  sc->sc_phase = MESSAGE_IN_PHASE;
		  CMR = CMD_RECEIVE_MSG;	/* receive msg */
		  continue;
		case DATA_IN_PHASE:
		  sc->sc_prevphase = DATA_IN_PHASE;
		  if (sc->sc_dleft == 0)
		    {
		      /* $BE>Aw%G!<%?$O$b$&$J$$$N$G(B
			 $B%9%F!<%?%9%U%'!<%:$r4|BT$7$h$&(B */
		      sc->sc_phase = STATUS_PHASE;
		      CMR = CMD_RECEIVE_STS;	/* receive sts */
		      continue;
		    }
		  n = mha_datain(sc, sc->sc_dp, sc->sc_dleft);
		  sc->sc_dp += n;
		  sc->sc_dleft -= n;
		  continue;
		case DATA_OUT_PHASE:
		  sc->sc_prevphase = DATA_OUT_PHASE;
		  if (sc->sc_dleft == 0)
		    {
		      /* $BE>Aw%G!<%?$O$b$&$J$$$N$G(B
			 $B%9%F!<%?%9%U%'!<%:$r4|BT$7$h$&(B */
		      sc->sc_phase = STATUS_PHASE;
		      CMR = CMD_RECEIVE_STS;	/* receive sts */
		      continue;
		    }
		  /* data phase $B$NB3$-$r$d$m$&(B */
		  n = mha_dataout(sc, sc->sc_dp, sc->sc_dleft);
		  sc->sc_dp += n;
		  sc->sc_dleft -= n;
		  continue;
		case COMMAND_PHASE:
		  /* $B:G=i$O(B CMD PHASE $B$H$$$&$3$H$i$7$$(B */
		  if (acb->dleft)
		    {
		      /* $B%G!<%?E>Aw$,$"$j$&$k>l9g(B */
		      if (acb->xs->flags & SCSI_DATA_IN)
			{
			  sc->sc_phase = DATA_IN_PHASE;
			  n = mha_datain(sc, sc->sc_dp, sc->sc_dleft);
			  sc->sc_dp += n;
			  sc->sc_dleft -= n;
			}
		      else if (acb->xs->flags & SCSI_DATA_OUT)
			{
			  sc->sc_phase = DATA_OUT_PHASE;
			  n = mha_dataout(sc, sc->sc_dp, sc->sc_dleft);
			  sc->sc_dp += n;
			  sc->sc_dleft -= n;
			}
		      continue;
		    }
		  else
		    {
		      /* $B%G!<%?E>Aw$O$J$$$i$7$$(B?! */
		      WAIT;
		      sc->sc_phase = STATUS_PHASE;
		      CMR = CMD_RECEIVE_STS;	/* receive sts */
		      continue;
		    }
		}
#endif
	    }
	  continue;
	case 0x31:	/* disconnected in xfer progress. */
	  SPC_MISC(("[0x31]"));
	case 0x70:	/* disconnected. */
	  SPC_ASSERT(sc->sc_flags & SPC_BUSFREE_OK);
	  sc->sc_phase = BUSFREE_PHASE;
	  sc->sc_state = SPC_IDLE;
#if 1
	  acb = sc->sc_nexus;
	  SPC_ASSERT(sc->sc_nexus != NULL);
	  acb->xs->error = XS_NOERROR;
	  mha_done(sc, acb);
#else
	  TAILQ_INSERT_HEAD(&sc->nexus_list, acb, chain);
	  mha_sched(sc);
#endif
	  continue;
	case 0x32:	/* phase error in xfer progress. */
	  SPC_MISC(("[0x32]"));
	case 0x65:	/* invalid command.
			   $B$J$<$3$s$J$b$N$,=P$k$N$+(B
			   $B26$K$OA4$/M}2r$G$-$J$$(B */
#if 1
	  SPC_MISC(("[0x%04x]", r));
#endif
	case 0x54:	/* initial-phase error. */
	  SPC_MISC(("[0x54, ns=%x, ph=%x(ought to be %x)]",
		    NSR,
		    SCR, sc->sc_phase));
	  /* thru */
	case 0x71:	/* assert req */
	  WAIT;
	  if (SSR & 0x40)
	    {
	      printf("SPC sts=%2x, r=%04x, ns=%x, ph=%x\n",
		     SSR, r, NSR, SCR);
	      WAIT;
	    }
	  ph = SCR;
	  if (sc->sc_state == SPC_SELECTING)	/* XXX msaitoh */
	    {
	      sc->sc_state = SPC_HASNEXUS;
	    }
	  if (ph & 0x80)
	    {
	      switch (ph & PHASE_MASK)
		{
		default:
			printf("phase = %x\n", ph);
			panic("assert req: the phase I don't know!");
		case DATA_IN_PHASE:
			sc->sc_prevphase = DATA_IN_PHASE;
			SPC_MISC(("DATAIN(%d)...", sc->sc_dleft));
			n = mha_datain(sc, sc->sc_dp, sc->sc_dleft);
			sc->sc_dp += n;
			sc->sc_dleft -= n;
			SPC_MISC(("done\n"));
			continue;
		case DATA_OUT_PHASE:
			sc->sc_prevphase = DATA_OUT_PHASE;
			SPC_MISC(("DATAOUT\n"));
			n = mha_dataout(sc, sc->sc_dp, sc->sc_dleft);
			sc->sc_dp += n;
			sc->sc_dleft -= n;
			continue;
		case STATUS_PHASE:
			sc->sc_phase = STATUS_PHASE;
			SPC_MISC(("[RECV_STS]"));
			WAIT;
			CMR = CMD_RECEIVE_STS;	/* receive sts */
			continue;
		case MESSAGE_IN_PHASE:
			sc->sc_phase = MESSAGE_IN_PHASE;
			WAIT;
			CMR = CMD_RECEIVE_MSG;
			continue;
		}
	    }
	  continue;
	}
    }
}

void
mha_abort(sc, acb)
	struct mha_softc *sc;
	struct acb *acb;
{
	acb->flags |= ACB_ABORTED;

	if (acb == sc->sc_nexus) {
		/*
		 * If we're still selecting, the message will be scheduled
		 * after selection is complete.
		 */
		if (sc->sc_state == SPC_HASNEXUS) {
			sc->sc_flags |= SPC_ABORTING;
			mha_sched_msgout(SEND_ABORT);
		}
	} else {
		if (sc->sc_state == SPC_IDLE)
			mha_sched(sc);
	}
}

void
mha_timeout(arg)
	void *arg;
{
	int s = splbio();
	struct acb *acb = (struct acb *)arg;
	struct scsipi_xfer *xs = acb->xs;
	struct scsipi_link *sc_link = xs->sc_link;
	struct mha_softc *sc = sc_link->adapter_softc;

	scsi_print_addr(sc_link);
again:
	printf("%s: timed out [acb %p (flags 0x%x, dleft %x, stat %x)], "
	       "<state %d, nexus %p, phase(c %x, p %x), resid %x, msg(q %x,o %x) >",
		sc->sc_dev.dv_xname,
		acb, acb->flags, acb->dleft, acb->stat,
		sc->sc_state, sc->sc_nexus, sc->sc_phase, sc->sc_prevphase,
		sc->sc_dleft, sc->sc_msgpriq, sc->sc_msgout
		);
	printf("[%04x %02x]\n", sc->sc_ps[1], SCR);
	panic("timeout, ouch!");

	if (acb->flags & ACB_ABORTED) {
		/* abort timed out */
		printf(" AGAIN\n");
#if 0
		mha_init(sc, 1); /* XXX 1?*/
#endif
	} else {
		/* abort the operation that has timed out */
		printf("\n");
		xs->error = XS_TIMEOUT;
		mha_abort(sc, acb);
	}

	splx(s);
}

#ifdef SPC_DEBUG
/*
 * The following functions are mostly used for debugging purposes, either
 * directly called from the driver or from the kernel debugger.
 */

void
mha_show_scsi_cmd(acb)
	struct acb *acb;
{
	u_char  *b = (u_char *)&acb->cmd;
	struct scsipi_link *sc_link = acb->xs->sc_link;
	int i;

	scsi_print_addr(sc_link);
	if ((acb->xs->flags & SCSI_RESET) == 0) {
		for (i = 0; i < acb->clen; i++) {
			if (i)
				printf(",");
			printf("%x", b[i]);
		}
		printf("\n");
	} else
		printf("RESET\n");
}

void
mha_print_acb(acb)
	struct acb *acb;
{

	printf("acb@%x xs=%x flags=%x", acb, acb->xs, acb->flags);
	printf(" dp=%x dleft=%d stat=%x\n",
	    (long)acb->daddr, acb->dleft, acb->stat);
	mha_show_scsi_cmd(acb);
}

void
mha_print_active_acb()
{
	struct acb *acb;
	struct mha_softc *sc = mha_cd.cd_devs[0]; /* XXX */

	printf("ready list:\n");
	for (acb = sc->ready_list.tqh_first; acb != NULL;
	    acb = acb->chain.tqe_next)
		mha_print_acb(acb);
	printf("nexus:\n");
	if (sc->sc_nexus != NULL)
		mha_print_acb(sc->sc_nexus);
	printf("nexus list:\n");
	for (acb = sc->nexus_list.tqh_first; acb != NULL;
	    acb = acb->chain.tqe_next)
		mha_print_acb(acb);
}

void
mha_dump_driver(sc)
	struct mha_softc *sc;
{
	struct spc_tinfo *ti;
	int i;

	printf("nexus=%x prevphase=%x\n", sc->sc_nexus, sc->sc_prevphase);
	printf("state=%x msgin=%x msgpriq=%x msgoutq=%x lastmsg=%x currmsg=%x\n",
	    sc->sc_state, sc->sc_imess[0],
	    sc->sc_msgpriq, sc->sc_msgoutq, sc->sc_lastmsg, sc->sc_currmsg);
	for (i = 0; i < 7; i++) {
		ti = &sc->sc_tinfo[i];
		printf("tinfo%d: %d cmds %d disconnects %d timeouts",
		    i, ti->cmds, ti->dconns, ti->touts);
		printf(" %d senses flags=%x\n", ti->senses, ti->flags);
	}
}
#endif
