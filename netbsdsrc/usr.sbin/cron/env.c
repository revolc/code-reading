/*	$NetBSD: env.c,v 1.6 1997/04/14 08:18:36 mrg Exp $	*/

/* Copyright 1988,1990,1993,1994 by Paul Vixie
 * All rights reserved
 *
 * Distribute freely, except: don't remove my name from the source or
 * documentation (don't take credit for my work), mark your changes (don't
 * get me blamed for your possible bugs), don't alter or remove this
 * notice.  May be sold if buildable source is provided to buyer.  No
 * warrantee of any kind, express or implied, is included with this
 * software; use at your own risk, responsibility for damages (if any) to
 * anyone resulting from the use of this software rests entirely with the
 * user.
 *
 * Send bug reports, bug fixes, enhancements, requests, flames, etc., and
 * I'll try to keep a version up to date.  I can be reached as follows:
 * Paul Vixie          <paul@vix.com>          uunet!decwrl!vixie!paul
 */

#if !defined(lint) && !defined(LINT)
/*static char rcsid[] = "Id: env.c,v 2.7 1994/01/26 02:25:50 vixie Exp";*/
static char rcsid[] = "$NetBSD: env.c,v 1.6 1997/04/14 08:18:36 mrg Exp $";
#endif


#include "cron.h"
#include <string.h>

char **
env_init()
{
	register char	**p = (char **) malloc(sizeof(char **));

	p[0] = NULL;
	return (p);
}


void
env_free(envp)
	char	**envp;
{
	char	**p;

	for (p = envp;  *p;  p++)
		free(*p);
	free(envp);
}


char **
env_copy(envp)
	register char	**envp;
{
	register int	count, i;
	register char	**p;

	for (count = 0;  envp[count] != NULL;  count++)
		;
	p = (char **) malloc((count+1) * sizeof(char *));  /* 1 for the NULL */
	for (i = 0;  i < count;  i++)
		p[i] = strdup(envp[i]);
	p[count] = NULL;
	return (p);
}


char **
env_set(envp, envstr)
	char	**envp;
	char	*envstr;
{
	register int	count, found;
	register char	**p;

	/*
	 * count the number of elements, including the null pointer;
	 * also set 'found' to -1 or index of entry if already in here.
	 */
	found = -1;
	for (count = 0;  envp[count] != NULL;  count++) {
		if (!strcmp_until(envp[count], envstr, '='))
			found = count;
	}
	count++;	/* for the NULL */

	if (found != -1) {
		/*
		 * it exists already, so just free the existing setting,
		 * save our new one there, and return the existing array.
		 */
		free(envp[found]);
		envp[found] = strdup(envstr);
		return (envp);
	}

	/*
	 * it doesn't exist yet, so resize the array, move null pointer over
	 * one, save our string over the old null pointer, and return resized
	 * array.
	 */
	p = (char **) realloc((void *) envp,
			      (unsigned) ((count+1) * sizeof(char **)));
	p[count] = p[count-1];
	p[count-1] = strdup(envstr);
	return (p);
}


/* return	ERR = end of file
 *		FALSE = not an env setting (file was repositioned)
 *		TRUE = was an env setting
 */
int
load_env(envstr, f)
	char	*envstr;
	FILE	*f;
{
	long	filepos;
	int	fileline, len;
	char	*name, *val, *s;

	filepos = ftell(f);
	fileline = LineNumber;
	skip_comments(f);
	if (EOF == get_string(envstr, MAX_ENVSTR, f, "\n"))
		return (ERR);

	Debug(DPARS, ("load_env, read <%s>\n", envstr))

	s = strchr(envstr, '=');
	if (s != NULL && s != envstr) {
		*s++ = '\0';
		val = s;
		name = strdup(envstr);
	} else {
		Debug(DPARS, ("load_env, didn't get valid string"));
		fseek(f, filepos, 0);
		Set_LineNum(fileline);
		return (FALSE);
	}

	/*
	 * process value string
	 */
	if (*val) {
		len = strdtb(val);
		if (len >= 2 && (val[0] == '\'' || val[0] == '"') &&
		    val[len-1] == val[0]) {
			val[len-1] = '\0';
			val++;
		}
	}

	(void) snprintf(envstr, MAX_ENVSTR, "%s=%s", name, val);
	Debug(DPARS, ("load_env, <%s> <%s> -> <%s>\n", name, val, envstr))
	free(name);
	return (TRUE);
}


char *
env_get(name, envp)
	register char	*name;
	register char	**envp;
{
	register int	len = strlen(name);
	register char	*p, *q;

	while (p = *envp++) {
		if (!(q = strchr(p, '=')))
			continue;
		if ((q - p) == len && !strncmp(p, name, len))
			return (q+1);
	}
	return (NULL);
}
