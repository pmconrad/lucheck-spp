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

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "commonstuff.h"

#define ERR_MEMORY	"malloc failed: "
#define ERR_READ	"Can't read "

char *progname, *debug;

/** Write the error message for memory problems to stderr and exit with
 *  an error code */
void err_memory(void) {
char *err = strerror(errno);

    write(STDERR_FILENO, progname, strlen(progname));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, ERR_MEMORY, strlen(ERR_MEMORY));
    write(STDERR_FILENO, err, strlen(err));
    exit(1);
}

/** Write the error message for read errors on the given filename, close the
 *  given file descriptor and exit with an error code */
void err_reading(char *file, int fd) {
char *err = strerror(errno);

    if (fd >= 0) { close(fd); }

    write(STDERR_FILENO, progname, strlen(progname));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, ERR_READ, strlen(ERR_READ));
    write(STDERR_FILENO, file, strlen(file));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, err, strlen(err));
    write(STDERR_FILENO, "\n", 1);
    exit(1);
}

/* Do not change the following line:
 * arch-tag: a0e9e2e8-347c-415b-be10-598af7961e27
 */
