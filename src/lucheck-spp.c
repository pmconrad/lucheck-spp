/* lucheck-spp - A qmail-spp plugin for checking on the user part of recipient
 *               addresses
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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <cdb.h>

#define	ENV_RELAYCLIENT	"RELAYCLIENT"
#define ENV_RCPT_TO	"SMTPRCPTTO"
#define	ENV_CONTROL	"LUCHECK_CONTROL"
#define	ENV_ALIAS	"LUCHECK_ALIAS"
#define	ENV_FFDB	"LUCHECK_FFDB"

#define CMD_REJECT	"E451 Unknown user.\n"

char *progname;

#define ERR_MISSING_VARIABLE	"Missing environment variable "
#define ERR_OPEN		"Can't open file "
#define ERR_READ		"Can't read file "
#define ERR_MEMORY		"Can't allocate memory: "

/** Retrieve the environment variable with the given name. If there is no
 *  such variable, write an error message to stderr and exit with a non-zero
 *  exit code. */
static char *get_required_env(char *name) {
char *result = getenv(name);

    if (!result) {
	write(STDERR_FILENO, progname, strlen(progname));
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, ERR_MISSING_VARIABLE,
	      strlen(ERR_MISSING_VARIABLE));
	write(STDERR_FILENO, name, strlen(name));
	write(STDERR_FILENO, "\n", 1);
	exit(1);
    }
    return result;
}

static void err_open(char *file) {
char *err = strerror(errno);

    write(STDERR_FILENO, progname, strlen(progname));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, ERR_OPEN, strlen(ERR_OPEN));
    write(STDERR_FILENO, file, strlen(file));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, err, strlen(err));
    write(STDERR_FILENO, "\n", 1);
    exit(1);
}

static void err_reading(char *file, int fd) {
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

static void err_memory() {
char *err = strerror(errno);

    write(STDERR_FILENO, progname, strlen(progname));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, ERR_MEMORY, strlen(ERR_MEMORY));
    write(STDERR_FILENO, err, strlen(err));
    write(STDERR_FILENO, "\n", 1);
    exit(1);
}

/** Write a reject command to stdout */
static void reject() {
    write(STDOUT_FILENO, CMD_REJECT, strlen(CMD_REJECT));
}

/** Check if the given domain is in the virtualdomains file.
 *  Return the prefix if yes, NULL otherwise.
 */
static char *is_virtual(char *domain, char *control) {
char *line, *dot, *vdomains, *success = NULL;
int f, eof = 0, dlen = strlen(domain), rc, i, colon;

    vdomains = malloc(strlen(control) + 15);
    if (!vdomains) { err_memory(); }
    strcpy(vdomains, control);
    strcat(vdomains, "/virtualdomains");

    line = malloc(dlen + 10);
    if (!line) { err_memory(); }

    f = open(vdomains, O_RDONLY);
    if (f < 0) {
	if (errno != ENOENT) {
	   err_open(vdomains);
	}
    } else {
	while (!eof && !success) {
	    i = 0;
	    colon = 0;
	    while (!eof && line[i] != '\n') {
		rc = read(f, &line[i], 1);
		if (rc < 0) { err_reading(vdomains, f); }
		if (rc == 0) { eof = 1; i--; }
		else if (line[i] != '\n' && i < dlen + 2) {
		    if (line[i] == ':') { colon = i; }
		    i++;
		}
	    }
	    if (line[colon] == ':' && line[i] == '\n' && colon <= dlen) {
		line[colon] = 0;
		line[i] = 0;
		if (!strcmp(line, domain)) {
		    if (!(success = strdup(&line[colon + 1]))) {
			err_memory();
		    }
		}
		if (line[0] == '.') {
		    dot = domain;
		    while (!success && (dot = index(dot, '.'))) {
			if (!strcmp(line, dot)) {
			    if (!(success = strdup(&line[colon + 1]))) {
				err_memory();
			    }
			} else {
			    dot++;
			}
		    }
		}
	    }
	}
	close(f);
    }

    free(line);
    free(vdomains);
    return success;
}

/** Check if the given domain is in the locals file */
static int is_local(char *domain, char *control) {
char *line, *dot, *locals;
int f, eof = 0, dlen = strlen(domain), rc, i, success = 0;

    locals = malloc(strlen(control) + 7);
    if (!locals) { err_memory(); }
    strcpy(locals, control);
    strcat(locals, "/locals");

    line = malloc(dlen + 10);
    if (!line) { err_memory(); }

    f = open(locals, O_RDONLY);
    if (f < 0) {
	if (errno != ENOENT) {
	    err_open(locals);
	}
	strcpy(&locals[strlen(control)], "/me");
	f = open(locals, O_RDONLY);
	if (f < 0) {
	    err_open(locals);
	}
    }

    while (!eof && !success) {
	i = 0;
	while (!eof && line[i] != '\n') {
	    rc = read(f, &line[i], 1);
	    if (rc < 0) { err_reading(locals, f); }
	    if (rc == 0) { eof = 1; i--; }
	    else if (line[i] != '\n' && i < dlen + 2) { i++; }
	}
	if (line[i] == '\n' && i <= dlen) {
	    line[i] = 0;
	    if (!strcmp(line, domain)) { success = 1; }
	    if (line[0] == '.') {
		dot = domain;
		while ((dot = index(dot, '.'))) {
		    success |= !strcmp(line, dot);
		    dot++;
		}
	    }
	}
    }

    close(f);
    free(line);
    free(locals);
    return success;
}

/** main function of the lucheck plugin */
int main(int argc, char **argv) {
char *ffdb = getenv(ENV_FFDB), *recipient;
char *control, *alias, *at, *dotqmail;
int i, rc, alen, rlen;
struct cdb cdb;
struct passwd *pw;
struct stat statbuf;

    progname = argv[0];

    if (getenv(ENV_RELAYCLIENT)) { exit(0); }

    control = get_required_env(ENV_CONTROL);
    alias = get_required_env(ENV_ALIAS);
    recipient = get_required_env(ENV_RCPT_TO);

    at = index(recipient, '@');
    if (at) {
	char *domain = &at[1];
	*at = 0;
	if (is_virtual(domain, control) || !is_local(domain, control)) {
	    exit(0);
	}
    }

    rlen = strlen(recipient);
    alen = strlen(alias);

    if (ffdb) {
	i = open(ffdb, O_RDONLY);
	if (i < 0) { err_open(ffdb); }
	cdb_init(&cdb, i);
	rc = cdb_find(&cdb, recipient, rlen);
	if (rc == -1) { err_reading(ffdb, i); }
	cdb_free(&cdb);
	close(i);
	if (rc) { exit(0); }
    }

    dotqmail = malloc(alen + 1 + 7 + rlen + 1);
    if (!dotqmail) { err_memory(); }
    strcpy(dotqmail, alias);
    if (alias[alen - 1] == '/') { alen--; }
    dotqmail[alen] = '/';
    strcpy(&dotqmail[alen + 1], ".qmail-");
    strcat(dotqmail, recipient);
    recipient = &dotqmail[alen + 1 + 7];
    do {
	pw = getpwnam(recipient);
	if (pw) { exit(0); }
	if (!stat(dotqmail, &statbuf)) {
	    exit(0);
	}
	if (errno != ENOENT) {
	    err_open(dotqmail);
	}
	at = rindex(recipient, '-');
	if (at) { *at = 0; }
    } while (at);

    reject();

    return 0;
}

/* Do not change the following line:
 * arch-tag: cc3f1dfa-3073-45cc-bdd4-c2e1355a9df5
 */
