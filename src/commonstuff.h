/* mfcheck-spp - A qmail-spp plugin for checking envelope sender addresses
 *
 *  Copyright (C) 2004 Peter Conrad <conrad@tivano.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _COMMONSTUFF_H_
#define _COMMONSTUFF_H_

#include <unistd.h>

#define ENV_SMTPMAILFROM	"SMTPMAILFROM"
#define ENV_RCPT_TO		"SMTPRCPTTO"
#define ENV_RELAYCLIENT		"RELAYCLIENT"

extern char *progname, *debug;

#define DEBUG4(m1,m2,m3,m4)	\
    if (debug) { \
	write(STDERR_FILENO, progname, strlen(progname)); \
	write(STDERR_FILENO, ": ", 2); \
	write(STDERR_FILENO, m1, strlen(m1)); \
	write(STDERR_FILENO, m2, strlen(m2)); \
	write(STDERR_FILENO, m3, strlen(m3)); \
	write(STDERR_FILENO, m4, strlen(m4)); \
	write(STDERR_FILENO, "\n", 1); \
    }

#define DEBUG(m)		DEBUG4(m,"","","")
#define DEBUG2(m1,m2)		DEBUG4(m1,m2,"","")
#define DEBUG3(m1,m2,m3)	DEBUG4(m1,m2,m3,"")

/** Write the error message for memory problems to stderr and exit with
 *  an error code */
extern void err_memory();

#endif /* _COMMONSTUFF_H_ */

/* Do not change the following line:
 * arch-tag: ba924322-56f6-401a-97f2-64d6f1962a75
 */
