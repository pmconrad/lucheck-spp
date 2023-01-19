/* mfcheck-spp - A qmail-spp plugin for checking envelope sender addresses
 *
 *  Copyright (C) 2004,2011 Peter Conrad <conrad@tivano.de>
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

extern char *progname, *debug, *verbose;

#define DEBUG4(m1,m2,m3,m4)	\
    if (debug) { \
	write_error_string(progname); \
	write_error_string(": "); \
	write_error_string(m1); \
	write_error_string(m2); \
	write_error_string(m3); \
	write_error_string(m4); \
	write_error_string("\n"); \
    }

#define DEBUG(m)		DEBUG4(m,NULL,NULL,NULL)
#define DEBUG2(m1,m2)		DEBUG4(m1,m2,NULL,NULL)
#define DEBUG3(m1,m2,m3)	DEBUG4(m1,m2,m3,NULL)

#define VERBOSE4(m1,m2,m3,m4)	\
    if (verbose) { \
	write_error_string(progname); \
	write_error_string(": "); \
	write_error_string(m1); \
	write_error_string(m2); \
	write_error_string(m3); \
	write_error_string(m4); \
	write_error_string("\n"); \
    }

#define VERBOSE(m)		VERBOSE4(m,NULL,NULL,NULL)
#define VERBOSE2(m1,m2)		VERBOSE4(m1,m2,NULL,NULL)
#define VERBOSE3(m1,m2,m3)	VERBOSE4(m1,m2,m3,NULL)

/** Safely write the given string to STDERR */
extern void write_error_string(const char *message);

/** Write the error message for memory problems to stderr and exit with
 *  an error code */
extern void err_memory(void);

/** Write the error message for read errors on the given filename, close the
 *  given file descriptor and exit with an error code */
extern void err_reading(char *file, int fd);

#endif /* _COMMONSTUFF_H_ */
