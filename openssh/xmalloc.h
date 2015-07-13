/* $OpenBSD: xmalloc.h,v 1.13 2006/08/03 03:34:42 deraadt Exp $ */

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * Created: Mon Mar 20 22:09:17 1995 ylo
 *
 * Versions of malloc and friends that check their results, and never return
 * failure (they call fatal if they encounter an error).
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#if defined(_TOH_) && defined(_PFDBG_)

#define xmalloc(s)  xmalloc_dbg(s, __FILE__, __LINE__)
#define xcalloc(n, s)  xrealloc_dbg(n, s, __FILE__, __LINE__)
#define xrealloc(p, s)  xrealloc_dbg(p, s, __FILE__, __LINE__)
#define xfree(p)    xfree_dbg(p, __FILE__, __LINE__)
#define xstrdup(p)  xstrdup_dbg(p, __FILE__, __LINE__)

void	*xmalloc_dbg(size_t, const char *, int);
void	xcalloc_dbg(size_t, size_t, const char *, int);
void	*xrealloc_dbg(void *, size_t, const char *, int);
void     xfree_dbg(void *, const char *, int);
char	*xstrdup_dbg(const char *, const char *, int);

#else  /* _TOH_ && _PFDBG_ */

void	*xmalloc(size_t);
void	*xcalloc(size_t, size_t);
void	*xrealloc(void *, size_t, size_t);
void     xfree(void *);
char	*xstrdup(const char *);

#endif /* _TOH_ && _PFDBG */

int	 xasprintf(char **, const char *, ...)
                __attribute__((__format__ (printf, 2, 3)))
                __attribute__((__nonnull__ (2)));
