/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.22 1997/10/16 23:55:50 christos Exp 
 */

#define	syscallarg(x)	union { x datum; register_t pad; }

struct ultrix_sys_open_args {
	syscallarg(char *) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};

struct ultrix_sys_creat_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
};

struct ultrix_sys_execv_args {
	syscallarg(char *) path;
	syscallarg(char **) argp;
};

struct ultrix_sys_mknod_args {
	syscallarg(char *) path;
	syscallarg(int) mode;
	syscallarg(int) dev;
};

struct ultrix_sys_mount_args {
	syscallarg(char *) special;
	syscallarg(char *) dir;
	syscallarg(int) rdonly;
	syscallarg(int) type;
	syscallarg(caddr_t) data;
};

struct ultrix_sys_access_args {
	syscallarg(char *) path;
	syscallarg(int) flags;
};

struct ultrix_sys_stat_args {
	syscallarg(char *) path;
	syscallarg(struct stat43 *) ub;
};

struct ultrix_sys_lstat_args {
	syscallarg(char *) path;
	syscallarg(struct stat43 *) ub;
};

struct ultrix_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(caddr_t) data;
};

struct ultrix_sys_execve_args {
	syscallarg(char *) path;
	syscallarg(char **) argp;
	syscallarg(char **) envp;
};

struct ultrix_sys_mmap_args {
	syscallarg(caddr_t) addr;
	syscallarg(size_t) len;
	syscallarg(int) prot;
	syscallarg(u_int) flags;
	syscallarg(int) fd;
	syscallarg(long) pos;
};

struct ultrix_sys_setpgrp_args {
	syscallarg(int) pid;
	syscallarg(int) pgid;
};

struct ultrix_sys_wait3_args {
	syscallarg(int *) status;
	syscallarg(int) options;
	syscallarg(struct rusage *) rusage;
};

struct ultrix_sys_select_args {
	syscallarg(u_int) nd;
	syscallarg(fd_set *) in;
	syscallarg(fd_set *) ou;
	syscallarg(fd_set *) ex;
	syscallarg(struct timeval *) tv;
};

struct ultrix_sys_setsockopt_args {
	syscallarg(int) s;
	syscallarg(int) level;
	syscallarg(int) name;
	syscallarg(caddr_t) val;
	syscallarg(int) valsize;
};

struct ultrix_sys_sigcleanup_args {
	syscallarg(struct sigcontext *) sigcntxp;
};

struct ultrix_sys_cacheflush_args {
	syscallarg(char *) addr;
	syscallarg(int) nbytes;
	syscallarg(int) whichcache;
};

struct ultrix_sys_cachectl_args {
	syscallarg(char *) addr;
	syscallarg(int) nbytes;
	syscallarg(int) cacheop;
};

struct ultrix_sys_nfssvc_args {
	syscallarg(int) fd;
};

struct ultrix_sys_statfs_args {
	syscallarg(char *) path;
	syscallarg(struct ultrix_statfs *) buf;
};

struct ultrix_sys_fstatfs_args {
	syscallarg(int) fd;
	syscallarg(struct ultrix_statfs *) buf;
};

struct ultrix_sys_quotactl_args {
	syscallarg(int) cmd;
	syscallarg(char *) special;
	syscallarg(int) uid;
	syscallarg(caddr_t) addr;
};

struct ultrix_sys_exportfs_args {
	syscallarg(char *) path;
	syscallarg(char *) ex;
};

struct ultrix_sys_uname_args {
	syscallarg(struct ultrix_utsname *) name;
};

struct ultrix_sys_shmsys_args {
	syscallarg(u_int) shmop;
	syscallarg(u_int) a2;
	syscallarg(u_int) a3;
	syscallarg(u_int) a4;
};

struct ultrix_sys_ustat_args {
	syscallarg(int) dev;
	syscallarg(struct ultrix_ustat *) buf;
};

struct ultrix_sys_getmnt_args {
	syscallarg(int *) start;
	syscallarg(struct ultrix_fs_data *) buf;
	syscallarg(int) bufsize;
	syscallarg(int) mode;
	syscallarg(char *) path;
};

struct ultrix_sys_sigpending_args {
	syscallarg(int *) mask;
};

struct ultrix_sys_waitpid_args {
	syscallarg(int) pid;
	syscallarg(int *) status;
	syscallarg(int) options;
};

struct ultrix_sys_getsysinfo_args {
	syscallarg(unsigned) op;
	syscallarg(char *) buffer;
	syscallarg(unsigned) nbytes;
	syscallarg(int *) start;
	syscallarg(char *) arg;
};

struct ultrix_sys_setsysinfo_args {
	syscallarg(unsigned) op;
	syscallarg(char *) buffer;
	syscallarg(unsigned) nbytes;
	syscallarg(unsigned) arg;
	syscallarg(unsigned) flag;
};

/*
 * System call prototypes.
 */

int	sys_nosys	__P((struct proc *, void *, register_t *));
int	sys_exit	__P((struct proc *, void *, register_t *));
int	sys_fork	__P((struct proc *, void *, register_t *));
int	sys_read	__P((struct proc *, void *, register_t *));
int	sys_write	__P((struct proc *, void *, register_t *));
int	ultrix_sys_open	__P((struct proc *, void *, register_t *));
int	sys_close	__P((struct proc *, void *, register_t *));
int	compat_43_sys_wait	__P((struct proc *, void *, register_t *));
int	ultrix_sys_creat	__P((struct proc *, void *, register_t *));
int	sys_link	__P((struct proc *, void *, register_t *));
int	sys_unlink	__P((struct proc *, void *, register_t *));
int	ultrix_sys_execv	__P((struct proc *, void *, register_t *));
int	sys_chdir	__P((struct proc *, void *, register_t *));
int	ultrix_sys_mknod	__P((struct proc *, void *, register_t *));
int	sys_chmod	__P((struct proc *, void *, register_t *));
int	sys_chown	__P((struct proc *, void *, register_t *));
int	sys_obreak	__P((struct proc *, void *, register_t *));
int	compat_43_sys_lseek	__P((struct proc *, void *, register_t *));
int	sys_getpid	__P((struct proc *, void *, register_t *));
int	ultrix_sys_mount	__P((struct proc *, void *, register_t *));
int	sys_setuid	__P((struct proc *, void *, register_t *));
int	sys_getuid	__P((struct proc *, void *, register_t *));
int	ultrix_sys_access	__P((struct proc *, void *, register_t *));
int	sys_sync	__P((struct proc *, void *, register_t *));
int	sys_kill	__P((struct proc *, void *, register_t *));
int	ultrix_sys_stat	__P((struct proc *, void *, register_t *));
int	ultrix_sys_lstat	__P((struct proc *, void *, register_t *));
int	sys_dup	__P((struct proc *, void *, register_t *));
int	sys_pipe	__P((struct proc *, void *, register_t *));
int	sys_profil	__P((struct proc *, void *, register_t *));
int	sys_getgid	__P((struct proc *, void *, register_t *));
int	sys_acct	__P((struct proc *, void *, register_t *));
int	ultrix_sys_ioctl	__P((struct proc *, void *, register_t *));
int	sys_reboot	__P((struct proc *, void *, register_t *));
int	sys_symlink	__P((struct proc *, void *, register_t *));
int	sys_readlink	__P((struct proc *, void *, register_t *));
int	ultrix_sys_execve	__P((struct proc *, void *, register_t *));
int	sys_umask	__P((struct proc *, void *, register_t *));
int	sys_chroot	__P((struct proc *, void *, register_t *));
int	compat_43_sys_fstat	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getpagesize	__P((struct proc *, void *, register_t *));
int	sys_vfork	__P((struct proc *, void *, register_t *));
int	sys_sbrk	__P((struct proc *, void *, register_t *));
int	sys_sstk	__P((struct proc *, void *, register_t *));
int	ultrix_sys_mmap	__P((struct proc *, void *, register_t *));
int	sys_ovadvise	__P((struct proc *, void *, register_t *));
int	sys_munmap	__P((struct proc *, void *, register_t *));
int	sys_mprotect	__P((struct proc *, void *, register_t *));
int	sys_madvise	__P((struct proc *, void *, register_t *));
int	ultrix_sys_vhangup	__P((struct proc *, void *, register_t *));
int	sys_mincore	__P((struct proc *, void *, register_t *));
int	sys_getgroups	__P((struct proc *, void *, register_t *));
int	sys_setgroups	__P((struct proc *, void *, register_t *));
int	sys_getpgrp	__P((struct proc *, void *, register_t *));
int	ultrix_sys_setpgrp	__P((struct proc *, void *, register_t *));
int	sys_setitimer	__P((struct proc *, void *, register_t *));
int	ultrix_sys_wait3	__P((struct proc *, void *, register_t *));
int	compat_12_sys_swapon	__P((struct proc *, void *, register_t *));
int	sys_getitimer	__P((struct proc *, void *, register_t *));
int	compat_43_sys_gethostname	__P((struct proc *, void *, register_t *));
int	compat_43_sys_sethostname	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getdtablesize	__P((struct proc *, void *, register_t *));
int	sys_dup2	__P((struct proc *, void *, register_t *));
int	sys_fcntl	__P((struct proc *, void *, register_t *));
int	ultrix_sys_select	__P((struct proc *, void *, register_t *));
int	sys_fsync	__P((struct proc *, void *, register_t *));
int	sys_setpriority	__P((struct proc *, void *, register_t *));
int	sys_socket	__P((struct proc *, void *, register_t *));
int	sys_connect	__P((struct proc *, void *, register_t *));
int	compat_43_sys_accept	__P((struct proc *, void *, register_t *));
int	sys_getpriority	__P((struct proc *, void *, register_t *));
int	compat_43_sys_send	__P((struct proc *, void *, register_t *));
int	compat_43_sys_recv	__P((struct proc *, void *, register_t *));
int	sys_sigreturn	__P((struct proc *, void *, register_t *));
int	sys_bind	__P((struct proc *, void *, register_t *));
int	ultrix_sys_setsockopt	__P((struct proc *, void *, register_t *));
int	sys_listen	__P((struct proc *, void *, register_t *));
int	compat_43_sys_sigvec	__P((struct proc *, void *, register_t *));
int	compat_43_sys_sigblock	__P((struct proc *, void *, register_t *));
int	compat_43_sys_sigsetmask	__P((struct proc *, void *, register_t *));
int	sys_sigsuspend	__P((struct proc *, void *, register_t *));
int	compat_43_sys_sigstack	__P((struct proc *, void *, register_t *));
int	compat_43_sys_recvmsg	__P((struct proc *, void *, register_t *));
int	compat_43_sys_sendmsg	__P((struct proc *, void *, register_t *));
int	sys_gettimeofday	__P((struct proc *, void *, register_t *));
int	sys_getrusage	__P((struct proc *, void *, register_t *));
int	sys_getsockopt	__P((struct proc *, void *, register_t *));
int	sys_readv	__P((struct proc *, void *, register_t *));
int	sys_writev	__P((struct proc *, void *, register_t *));
int	sys_settimeofday	__P((struct proc *, void *, register_t *));
int	sys_fchown	__P((struct proc *, void *, register_t *));
int	sys_fchmod	__P((struct proc *, void *, register_t *));
int	compat_43_sys_recvfrom	__P((struct proc *, void *, register_t *));
int	sys_setreuid	__P((struct proc *, void *, register_t *));
int	sys_setregid	__P((struct proc *, void *, register_t *));
int	sys_rename	__P((struct proc *, void *, register_t *));
int	compat_43_sys_truncate	__P((struct proc *, void *, register_t *));
int	compat_43_sys_ftruncate	__P((struct proc *, void *, register_t *));
int	sys_flock	__P((struct proc *, void *, register_t *));
int	sys_sendto	__P((struct proc *, void *, register_t *));
int	sys_shutdown	__P((struct proc *, void *, register_t *));
int	sys_socketpair	__P((struct proc *, void *, register_t *));
int	sys_mkdir	__P((struct proc *, void *, register_t *));
int	sys_rmdir	__P((struct proc *, void *, register_t *));
int	sys_utimes	__P((struct proc *, void *, register_t *));
int	ultrix_sys_sigcleanup	__P((struct proc *, void *, register_t *));
int	sys_adjtime	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getpeername	__P((struct proc *, void *, register_t *));
int	compat_43_sys_gethostid	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getrlimit	__P((struct proc *, void *, register_t *));
int	compat_43_sys_setrlimit	__P((struct proc *, void *, register_t *));
int	compat_43_sys_killpg	__P((struct proc *, void *, register_t *));
int	compat_43_sys_getsockname	__P((struct proc *, void *, register_t *));
#ifdef __mips
int	ultrix_sys_cacheflush	__P((struct proc *, void *, register_t *));
int	ultrix_sys_cachectl	__P((struct proc *, void *, register_t *));
#else	/* !mips */
#endif	/* !mips */
#ifdef NFSSERVER
int	ultrix_sys_nfssvc	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_43_sys_getdirentries	__P((struct proc *, void *, register_t *));
int	ultrix_sys_statfs	__P((struct proc *, void *, register_t *));
int	ultrix_sys_fstatfs	__P((struct proc *, void *, register_t *));
#ifdef NFS
int	async_daemon	__P((struct proc *, void *, register_t *));
int	sys_getfh	__P((struct proc *, void *, register_t *));
#else
#endif
int	compat_09_sys_getdomainname	__P((struct proc *, void *, register_t *));
int	compat_09_sys_setdomainname	__P((struct proc *, void *, register_t *));
int	ultrix_sys_quotactl	__P((struct proc *, void *, register_t *));
int	ultrix_sys_exportfs	__P((struct proc *, void *, register_t *));
int	ultrix_sys_uname	__P((struct proc *, void *, register_t *));
int	ultrix_sys_shmsys	__P((struct proc *, void *, register_t *));
int	ultrix_sys_ustat	__P((struct proc *, void *, register_t *));
int	ultrix_sys_getmnt	__P((struct proc *, void *, register_t *));
int	ultrix_sys_sigpending	__P((struct proc *, void *, register_t *));
int	sys_setsid	__P((struct proc *, void *, register_t *));
int	ultrix_sys_waitpid	__P((struct proc *, void *, register_t *));
int	ultrix_sys_getsysinfo	__P((struct proc *, void *, register_t *));
int	ultrix_sys_setsysinfo	__P((struct proc *, void *, register_t *));
