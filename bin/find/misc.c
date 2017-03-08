/*	$OpenBSD: misc.c,v 1.3 1996/10/24 03:46:05 tholo Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Cimarron D. Taylor of the University of California, Berkeley.
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

#ifndef lint
/*static char sccsid[] = "from: @(#)misc.c	8.1 (Berkeley) 6/6/93";*/
static char rcsid[] = "$OpenBSD: misc.c,v 1.3 1996/10/24 03:46:05 tholo Exp $";
#endif /* not lint */

#include <sys/types.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "find.h"
 
/*
 * brace_subst --
 *	Replace occurrences of {} in s1 with s2 and return the result string.
 */
void
brace_subst(orig, store, path, len)
	char *orig, **store, *path;
	int len;
{
	register int plen;
	register char ch, *p;

	plen = strlen(path);
	for (p = *store; ch = *orig; ++orig)
		if (ch == '{' && orig[1] == '}') {
			while ((p - *store) + plen > len)
				if (!(*store = realloc(*store, len *= 2)))
					err(1, NULL);
			memmove(p, path, plen);
			p += plen;
			++orig;
		} else
			*p++ = ch;
	*p = '\0';
}

/*
 * queryuser --
 *	print a message to standard error and then read input from standard
 *	input. If the input is 'y' then 1 is returned.
 */
int
queryuser(argv)
	register char **argv;
{
	int ch, first, nl;

	(void)fprintf(stderr, "\"%s", *argv);
	while (*++argv)
		(void)fprintf(stderr, " %s", *argv);
	(void)fprintf(stderr, "\"? ");
	(void)fflush(stderr);

	first = ch = getchar();
	for (nl = 0;;) {
		if (ch == '\n') {
			nl = 1;
			break;
		}
		if (ch == EOF)
			break;
		ch = getchar();
	}

	if (!nl) {
		(void)fprintf(stderr, "\n");
		(void)fflush(stderr);
	}
        return (first == 'y');
}
 
/*
 * emalloc --
 *	malloc with error checking.
 */
void *
emalloc(len)
	u_int len;
{
	void *p;

	if (p = malloc(len))
		return (p);
	err(1, NULL);
}

/*
 * show_path --
 *	called on SIGINFO
 */
/* ARGSUSED */
void
show_path(sig)
	int sig;
{
	extern FTSENT *entry;

	write(STDERR_FILENO, "find path: ", 11);
	write(STDERR_FILENO, entry->fts_path, entry->fts_pathlen);
	write(STDERR_FILENO, "\n", 1);
}