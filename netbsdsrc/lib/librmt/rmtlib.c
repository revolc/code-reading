/*	$NetBSD: rmtlib.c,v 1.8.2.1 1997/10/21 20:00:18 thorpej Exp $	*/

/*
 *	rmt --- remote tape emulator subroutines
 *
 *	Originally written by Jeff Lee, modified some by Arnold Robbins
 *
 *	WARNING:  The man page rmt(8) for /etc/rmt documents the remote mag
 *	tape protocol which rdump and rrestore use.  Unfortunately, the man
 *	page is *WRONG*.  The author of the routines I'm including originally
 *	wrote his code just based on the man page, and it didn't work, so he
 *	went to the rdump source to figure out why.  The only thing he had to
 *	change was to check for the 'F' return code in addition to the 'E',
 *	and to separate the various arguments with \n instead of a space.  I
 *	personally don't think that this is much of a problem, but I wanted to
 *	point it out.
 *	-- Arnold Robbins
 *
 *	Redone as a library that can replace open, read, write, etc, by
 *	Fred Fish, with some additional work by Arnold Robbins.
 */
 
/*
 *	MAXUNIT --- Maximum number of remote tape file units
 *
 *	READ --- Return the number of the read side file descriptor
 *	WRITE --- Return the number of the write side file descriptor
 */

#define RMTIOCTL	1
/* #define USE_REXEC	1 */	/* rexec code courtesy of Dan Kegel, srs!dan */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#ifdef RMTIOCTL
#include <sys/ioctl.h>
#include <sys/mtio.h>
#endif

#ifdef USE_REXEC
#include <netdb.h>
#endif

#include <errno.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

#define __RMTLIB_PRIVATE
#include <rmt.h>		/* get prototypes for remapped functions */

#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

static	int	_rmt_close __P((int));
static	int	_rmt_ioctl __P((int, unsigned long, char *));
static	off_t	_rmt_lseek __P((int, off_t, int));
static	int	_rmt_open __P((const char *, int, int));
static	int	_rmt_read __P((int, char *, unsigned int));
static	int	_rmt_write __P((int, const void *, unsigned int));
static	int	command __P((int, char *));
static	int	remdev __P((const char *));
static	void	rmtabort __P((int));
static	int	status __P((int));

	int	isrmt __P((int));


#define BUFMAGIC	64	/* a magic number for buffer sizes */
#define MAXUNIT	4

#define READ(fd)	(Ctp[fd][0])
#define WRITE(fd)	(Ptc[fd][1])

static int Ctp[MAXUNIT][2] = { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} };
static int Ptc[MAXUNIT][2] = { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} };


/*
 *	rmtabort --- close off a remote tape connection
 */

static void
rmtabort(fildes)
	int fildes;
{
	close(READ(fildes));
	close(WRITE(fildes));
	READ(fildes) = -1;
	WRITE(fildes) = -1;
}



/*
 *	command --- attempt to perform a remote tape command
 */

static int
command(fildes, buf)
	int fildes;
	char *buf;
{
	int blen;
	void (*pstat) __P((int));

/*
 *	save current pipe status and try to make the request
 */

	blen = strlen(buf);
	pstat = signal(SIGPIPE, SIG_IGN);
	if (write(WRITE(fildes), buf, blen) == blen)
	{
		signal(SIGPIPE, pstat);
		return(0);
	}

/*
 *	something went wrong. close down and go home
 */

	signal(SIGPIPE, pstat);
	rmtabort(fildes);

	errno = EIO;
	return(-1);
}



/*
 *	status --- retrieve the status from the pipe
 */

static int
status(fildes)
	int fildes;
{
	int i;
	char c, *cp;
	char buffer[BUFMAGIC];

/*
 *	read the reply command line
 */

	for (i = 0, cp = buffer; i < BUFMAGIC; i++, cp++)
	{
		if (read(READ(fildes), cp, 1) != 1)
		{
			rmtabort(fildes);
			errno = EIO;
			return(-1);
		}
		if (*cp == '\n')
		{
			*cp = 0;
			break;
		}
	}

	if (i == BUFMAGIC)
	{
		rmtabort(fildes);
		errno = EIO;
		return(-1);
	}

/*
 *	check the return status
 */

	for (cp = buffer; *cp; cp++)
		if (*cp != ' ')
			break;

	if (*cp == 'E' || *cp == 'F')
	{
		errno = atoi(cp + 1);
		while (read(READ(fildes), &c, 1) == 1)
			if (c == '\n')
				break;

		if (*cp == 'F')
			rmtabort(fildes);

		return(-1);
	}

/*
 *	check for mis-synced pipes
 */

	if (*cp != 'A')
	{
		rmtabort(fildes);
		errno = EIO;
		return(-1);
	}

	return(atoi(cp + 1));
}

#ifdef USE_REXEC

/*
 * _rmt_rexec
 *
 * execute /etc/rmt on a remote system using rexec().
 * Return file descriptor of bidirectional socket for stdin and stdout
 * If username is NULL, or an empty string, uses current username.
 *
 * ADR: By default, this code is not used, since it requires that
 * the user have a .netrc file in his/her home directory, or that the
 * application designer be willing to have rexec prompt for login and
 * password info. This may be unacceptable, and .rhosts files for use
 * with rsh are much more common on BSD systems.
 */

static	int	_rmt_rexec __P((const char *, const char *));

static int
_rmt_rexec(host, user)
	const char *host;
	const char *user;		/* may be NULL */
{
	struct servent *rexecserv;

	rexecserv = getservbyname("exec", "tcp");
	if (NULL == rexecserv) {
		fprintf (stderr, "? exec/tcp: service not available.");
		exit (-1);
	}
	if ((user != NULL) && *user == '\0')
		user = (char *) NULL;
	return rexec (&host, rexecserv->s_port, user, NULL,
			"/etc/rmt", (int *)NULL);
}
#endif /* USE_REXEC */

/*
 *	_rmt_open --- open a magtape device on system specified, as given user
 *
 *	file name has the form [user@]system:/dev/????
#ifdef COMPAT
 *	file name has the form system[.user]:/dev/????
#endif
 */

#define MAXHOSTLEN	257	/* BSD allows very long host names... */

static int
_rmt_open(path, oflag, mode)
	const char *path;
	int oflag;
	int mode;
{
	int i, rc;
	char buffer[BUFMAGIC];
	char system[MAXHOSTLEN];
	char device[BUFMAGIC];
	char login[BUFMAGIC];
	char *sys, *dev, *user;

	sys = system;
	dev = device;
	user = login;

/*
 *	first, find an open pair of file descriptors
 */

	for (i = 0; i < MAXUNIT; i++)
		if (READ(i) == -1 && WRITE(i) == -1)
			break;

	if (i == MAXUNIT)
	{
		errno = EMFILE;
		return(-1);
	}

/*
 *	pull apart system and device, and optional user
 *	don't munge original string
 *	if COMPAT is defined, also handle old (4.2) style person.site notation.
 */

	while (*path != '@'
#ifdef COMPAT
			&& *path != '.'
#endif
			&& *path != ':') {
		*sys++ = *path++;
	}
	*sys = '\0';
	path++;

	if (*(path - 1) == '@')
	{
		(void)strncpy(user, system, sizeof(login) - 1);
				/* saw user part of user@host */
		sys = system;			/* start over */
		while (*path != ':') {
			*sys++ = *path++;
		}
		*sys = '\0';
		path++;
	}
#ifdef COMPAT
	else if (*(path - 1) == '.')
	{
		while (*path != ':') {
			*user++ = *path++;
		}
		*user = '\0';
		path++;
	}
#endif
	else
		*user = '\0';

	while (*path) {
		*dev++ = *path++;
	}
	*dev = '\0';

#ifdef USE_REXEC
/* 
 *	Execute the remote command using rexec 
 */
	READ(i) = WRITE(i) = _rmt_rexec(system, login);
	if (READ(i) < 0)
		return -1;
#else
/*
 *	setup the pipes for the 'rsh' command and fork
 */

	if (pipe(Ptc[i]) == -1 || pipe(Ctp[i]) == -1)
		return(-1);

	if ((rc = fork()) == -1)
		return(-1);

	if (rc == 0)
	{
		close(0);
		dup(Ptc[i][0]);
		close(Ptc[i][0]); close(Ptc[i][1]);
		close(1);
		dup(Ctp[i][1]);
		close(Ctp[i][0]); close(Ctp[i][1]);
		(void) setuid (getuid ());
		(void) setgid (getgid ());
		if (*login)
		{
			execl("/usr/bin/rsh", "rsh", system, "-l", login,
				"/etc/rmt", (char *) 0);
		}
		else
		{
			execl("/usr/bin/rsh", "rsh", system,
				"/etc/rmt", (char *) 0);
		}

/*
 *	bad problems if we get here
 */

		perror("exec");
		exit(1);
	}

	close(Ptc[i][0]); close(Ctp[i][1]);
#endif

/*
 *	now attempt to open the tape device
 */

	(void)snprintf(buffer, sizeof(buffer), "O%s\n%d\n", device, oflag);
	if (command(i, buffer) == -1 || status(i) == -1)
		return(-1);

	return(i);
}



/*
 *	_rmt_close --- close a remote magtape unit and shut down
 */

static int
_rmt_close(fildes)
	int fildes;
{
	int rc;

	if (command(fildes, "C\n") != -1)
	{
		rc = status(fildes);

		rmtabort(fildes);
		return(rc);
	}

	return(-1);
}



/*
 *	_rmt_read --- read a buffer from a remote tape
 */

static int
_rmt_read(fildes, buf, nbyte)
	int fildes;
	char *buf;
	unsigned int nbyte;
{
	int rc, i;
	char buffer[BUFMAGIC];

	(void)snprintf(buffer, sizeof buffer, "R%d\n", nbyte);
	if (command(fildes, buffer) == -1 || (rc = status(fildes)) == -1)
		return(-1);

	for (i = 0; i < rc; i += nbyte, buf += nbyte)
	{
		nbyte = read(READ(fildes), buf, rc);
		if (nbyte <= 0)
		{
			rmtabort(fildes);
			errno = EIO;
			return(-1);
		}
	}

	return(rc);
}



/*
 *	_rmt_write --- write a buffer to the remote tape
 */

static int
_rmt_write(fildes, buf, nbyte)
	int fildes;
	const void *buf;
	unsigned int nbyte;
{
	char buffer[BUFMAGIC];
	void (*pstat) __P((int));

	(void)snprintf(buffer, sizeof buffer, "W%d\n", nbyte);
	if (command(fildes, buffer) == -1)
		return(-1);

	pstat = signal(SIGPIPE, SIG_IGN);
	if (write(WRITE(fildes), buf, nbyte) == nbyte)
	{
		signal (SIGPIPE, pstat);
		return(status(fildes));
	}

	signal (SIGPIPE, pstat);
	rmtabort(fildes);
	errno = EIO;
	return(-1);
}



/*
 *	_rmt_lseek --- perform an imitation lseek operation remotely
 */

static off_t
_rmt_lseek(fildes, offset, whence)
	int fildes;
	off_t offset;
	int whence;
{
	char buffer[BUFMAGIC];

	(void)snprintf(buffer, sizeof buffer, "L%qd\n%d\n", (long long)offset,
	    whence);
	if (command(fildes, buffer) == -1)
		return(-1);

	return(status(fildes));
}


/*
 *	_rmt_ioctl --- perform raw tape operations remotely
 */

#ifdef RMTIOCTL
static int
_rmt_ioctl(fildes, op, arg)
	int fildes;
	unsigned long op;
	char *arg;
{
	char c;
	int rc, cnt;
	char buffer[BUFMAGIC];

/*
 *	MTIOCOP is the easy one. nothing is transfered in binary
 */

	if (op == MTIOCTOP)
	{
		(void)snprintf(buffer, sizeof buffer, "I%d\n%d\n",
		    ((struct mtop *)arg)->mt_op,
		    ((struct mtop *)arg)->mt_count);
		if (command(fildes, buffer) == -1)
			return(-1);
		return(status(fildes));
	}

/*
 *	we can only handle 2 ops, if not the other one, punt
 */

	if (op != MTIOCGET)
	{
		errno = EINVAL;
		return(-1);
	}

/*
 *	grab the status and read it directly into the structure
 *	this assumes that the status buffer is (hopefully) not
 *	padded and that 2 shorts fit in a long without any word
 *	alignment problems, ie - the whole struct is contiguous
 *	NOTE - this is probably NOT a good assumption.
 */

	if (command(fildes, "S") == -1 || (rc = status(fildes)) == -1)
		return(-1);

	for (; rc > 0; rc -= cnt, arg += cnt)
	{
		cnt = read(READ(fildes), arg, rc);
		if (cnt <= 0)
		{
			rmtabort(fildes);
			errno = EIO;
			return(-1);
		}
	}

/*
 *	now we check for byte position. mt_type is a small integer field
 *	(normally) so we will check its magnitude. if it is larger than
 *	256, we will assume that the bytes are swapped and go through
 *	and reverse all the bytes
 */

	if (((struct mtget *) arg)->mt_type < 256)
		return(0);

	for (cnt = 0; cnt < rc; cnt += 2)
	{
		c = arg[cnt];
		arg[cnt] = arg[cnt+1];
		arg[cnt+1] = c;
	}

	return(0);
  }
#endif /* RMTIOCTL */

/*
 *	Added routines to replace open(), close(), lseek(), ioctl(), etc.
 *	The preprocessor can be used to remap these the rmtopen(), etc
 *	thus minimizing source changes:
 *
 *		#ifdef <something>
 *		#  define access rmtaccess
 *		#  define close rmtclose
 *		#  define creat rmtcreat
 *		#  define dup rmtdup
 *		#  define fcntl rmtfcntl
 *		#  define fstat rmtfstat
 *		#  define ioctl rmtioctl
 *		#  define isatty rmtisatty
 *		#  define lseek rmtlseek
 *		#  define lstat rmtlstat
 *		#  define open rmtopen
 *		#  define read rmtread
 *		#  define stat rmtstat
 *		#  define write rmtwrite
 *		#endif
 *
 *	-- Fred Fish
 *
 *	ADR --- I set up a <rmt.h> include file for this
 *
 */

/*
 *	Note that local vs remote file descriptors are distinquished
 *	by adding a bias to the remote descriptors.  This is a quick
 *	and dirty trick that may not be portable to some systems.
 */

#define REM_BIAS 128


/*
 *	Test pathname to see if it is local or remote.  A remote device
 *	is any string that contains ":/dev/".  Returns 1 if remote,
 *	0 otherwise.
 */
 
static int
remdev(path)
	const char *path;
{
	if ((path = strchr (path, ':')) != NULL)
	{
		if (strncmp (path + 1, "/dev/", 5) == 0)
		{
			return (1);
		}
	}
	return (0);
}


/*
 *	Open a local or remote file.  Looks just like open(2) to
 *	caller.
 */
 
int
#ifdef __STDC__
rmtopen(const char *path, int oflag, ...)
#else
rmtopen(va_alist)
	va_decl
#endif
{
	mode_t mode;
	int fd;
	va_list ap;
#if __STDC__
	va_start(ap, oflag);
#else
	const char *path;
	int oflag;

	va_start(ap);
	path = va_arg(ap, const char *);
	oflag = va_arg(ap, int);
#endif

	mode = va_arg(ap, mode_t);
	va_end(ap);

	if (remdev (path))
	{
		fd = _rmt_open (path, oflag, mode);

		return (fd == -1) ? -1 : (fd + REM_BIAS);
	}
	else
	{
		return (open (path, oflag, mode));
	}
}

/*
 *	Test pathname for specified access.  Looks just like access(2)
 *	to caller.
 */
 
int
rmtaccess(path, amode)
	const char *path;
	int amode;
{
	if (remdev (path))
	{
		return (0);		/* Let /etc/rmt find out */
	}
	else
	{
		return (access (path, amode));
	}
}


/*
 *	Isrmt. Let a programmer know he has a remote device.
 */

int
isrmt(fd)
	int fd;
{
	return (fd >= REM_BIAS);
}


/*
 *	Read from stream.  Looks just like read(2) to caller.
 */
  
ssize_t
rmtread(fildes, buf, nbyte)
	int fildes;
	void *buf;
	size_t nbyte;
{
	if (isrmt (fildes))
	{
		return (_rmt_read (fildes - REM_BIAS, buf, nbyte));
	}
	else
	{
		return (read (fildes, buf, nbyte));
	}
}


/*
 *	Write to stream.  Looks just like write(2) to caller.
 */
 
ssize_t
rmtwrite(fildes, buf, nbyte)
	int fildes;
	const void *buf;
	size_t nbyte;
{
	if (isrmt (fildes))
	{
		return (_rmt_write (fildes - REM_BIAS, buf, nbyte));
	}
	else
	{
		return (write (fildes, buf, nbyte));
	}
}

/*
 *	Perform lseek on file.  Looks just like lseek(2) to caller.
 */

off_t
rmtlseek(fildes, offset, whence)
	int fildes;
	off_t offset;
	int whence;
{
	if (isrmt (fildes))
	{
		return (_rmt_lseek (fildes - REM_BIAS, offset, whence));
	}
	else
	{
		return (lseek (fildes, offset, whence));
	}
}


/*
 *	Close a file.  Looks just like close(2) to caller.
 */
 
int
rmtclose(fildes)
	int fildes;
{
	if (isrmt (fildes))
	{
		return (_rmt_close (fildes - REM_BIAS));
	}
	else
	{
		return (close (fildes));
	}
}

/*
 *	Do ioctl on file.  Looks just like ioctl(2) to caller.
 */
 
int
#ifdef __STDC__
rmtioctl(int fildes, unsigned long request, ...)
#else
rmtioctl(va_alist)
	va_decl
#endif
{
	char *arg;
	va_list ap;
#if __STDC__
	va_start(ap, request);
#else
	int fildes;
	unsigned long request;

	va_start(ap);
	filedes = va_arg(ap, int);
	request = va_arg(ap, unsigned long);
#endif

	arg = va_arg(ap, char *);
	va_end(ap);

	if (isrmt (fildes))
	{
#ifdef RMTIOCTL
		return (_rmt_ioctl (fildes - REM_BIAS, request, arg));
#else
		errno = EOPNOTSUPP;
		return (-1);		/* For now  (fnf) */
#endif
	}
	else
	{
		return (ioctl (fildes, request, arg));
	}
}


/*
 *	Duplicate an open file descriptor.  Looks just like dup(2)
 *	to caller.
 */
 
int
rmtdup(fildes)
	int fildes;
{
	if (isrmt (fildes))
	{
		errno = EOPNOTSUPP;
		return (-1);		/* For now (fnf) */
	}
	else
	{
		return (dup (fildes));
	}
}

/*
 *	Get file status.  Looks just like fstat(2) to caller.
 */
 
int
rmtfstat(fildes, buf)
	int fildes;
	struct stat *buf;
{
	if (isrmt (fildes))
	{
		errno = EOPNOTSUPP;
		return (-1);		/* For now (fnf) */
	}
	else
	{
		return (fstat (fildes, buf));
	}
}


/*
 *	Get file status.  Looks just like stat(2) to caller.
 */
 
int
rmtstat(path, buf)
	const char *path;
	struct stat *buf;
{
	if (remdev (path))
	{
		errno = EOPNOTSUPP;
		return (-1);		/* For now (fnf) */
	}
	else
	{
		return (stat (path, buf));
	}
}



/*
 *	Create a file from scratch.  Looks just like creat(2) to the caller.
 */

int
rmtcreat(path, mode)
	const char *path;
	mode_t mode;
{
	if (remdev (path))
	{
		return (rmtopen (path, 1 | O_CREAT, mode));
	}
	else
	{
		return (creat (path, mode));
	}
}

/*
 *	Rmtfcntl. Do a remote fcntl operation.
 */

int
#ifdef __STDC__
rmtfcntl(int fd, int cmd, ...)
#else
rmtfcntl(va_alist)
	va_decl
#endif
{
	void *arg;
	va_list ap;
#if __STDC__
	va_start(ap, cmd);
#else
	int fd, cmd;

	va_start(ap);
	fd = va_arg(ap, int);
	cmd = va_arg(ap, int);
#endif

	arg = va_arg(ap, void *);
	va_end(ap);

	if (isrmt (fd))
	{
		errno = EOPNOTSUPP;
		return (-1);
	}
	else
	{
		return (fcntl (fd, cmd, arg));
	}
}

/*
 *	Rmtisatty.  Do the isatty function.
 */

int
rmtisatty(fd)
	int fd;
{
	if (isrmt (fd))
		return (0);
	else
		return (isatty (fd));
}


/*
 *	Get file status, even if symlink.  Looks just like lstat(2) to caller.
 */
 
int
rmtlstat(path, buf)
	const char *path;
	struct stat *buf;
{
	if (remdev (path))
	{
		errno = EOPNOTSUPP;
		return (-1);		/* For now (fnf) */
	}
	else
	{
		return (lstat (path, buf));
	}
}
