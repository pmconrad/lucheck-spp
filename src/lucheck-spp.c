/* lucheck-spp - A qmail-spp plugin for checking on the user part of recipient
 *               addresses
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

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <ctype.h>
#include <cdb.h>
#include "commonstuff.h"

#define	ENV_ALIAS	"LUCHECK_ALIAS"
#define	ENV_FFDB	"LUCHECK_FFDB"
#define	ENV_DEBUG	"LUCHECK_DEBUG"
#define	ENV_VERBOSE	"LUCHECK_VERBOSE"

#define ERR_MISSING_VARIABLE	"Missing environment variable "
#define ERR_OPEN		"Can't open file "

static const char *CMD_REJECT = "E551 Unknown user.\n";

static char *cmd_reject;

/** Retrieve the environment variable with the given name. If there is no
 *  such variable, write an error message to stderr and exit with a non-zero
 *  exit code. */
static char *get_required_env(char *name) {
char *result = getenv(name);

    if (!result) {
	write_error_string(progname);
	write_error_string(": ");
	write_error_string(ERR_MISSING_VARIABLE);
	write_error_string(name);
	write_error_string("\n");
	exit(1);
    }
    return result;
}

static void err_open(char *file) {
char *err = strerror(errno);

    write_error_string(progname);
    write_error_string(": ");
    write_error_string(ERR_OPEN);
    write_error_string(file);
    write_error_string(": ");
    write_error_string(err);
    write_error_string("\n");
    exit(1);
}

/** Write a reject command to stdout */
static void reject(void) {
    write(STDOUT_FILENO, cmd_reject, strlen(cmd_reject));
}

/** Check if the given domain is in the virtualdomains file.
 *  Return the prefix if yes, NULL otherwise.
 */
static char *is_virtual(char *domain) {
char *line, *dot, *success = NULL, *vdomains;
int f, eof = 0, dlen = strlen(domain), rc, i, colon;

    line = malloc(dlen + 10);
    if (!line) { err_memory(); }

    vdomains = "control/virtualdomains";
    f = open(vdomains, O_RDONLY);
    if (f < 0) {
	if (errno != ENOENT) {
	   err_open(vdomains);
	}
    } else {
	while (!eof && !success) {
	    i = 0;
	    colon = 0;
	    do 	{
		if (i > dlen + 2) {
		    /* No need to remember the whole line */
		    i--;
		}
		rc = read(f, &line[i], 1);
		if (rc < 0) { err_reading(vdomains, f); }
		if (rc == 0) {
		    eof = 1;
		    line[i] = 0;
		} else {
		    if (line[i] == ':') { colon = i; }
		    i++;
		}
	    } while (!eof && line[i - 1] != '\n');
	    if (line[colon] == ':' && i > 0
		    && (eof || line[--i] == '\n') && colon <= dlen) {
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
    return success;
}

/** Check if the given domain is in the locals file */
static int is_local(char *domain) {
char *line, *dot, *locals;
int f, eof = 0, dlen = strlen(domain), rc, i, success = 0;

    line = malloc(dlen + 10);
    if (!line) { err_memory(); }

    locals = "control/locals";
    f = open(locals, O_RDONLY);
    if (f < 0) {
	if (errno != ENOENT) {
	    err_open(locals);
	}
	locals = "control/me";
	f = open(locals, O_RDONLY);
	if (f < 0) {
	    err_open(locals);
	}
    }

    while (!eof && !success) {
	i = 0;
	do {
	    if (i > dlen + 2) {
		/* No need to remember the whole line */
		i--;
	    }
	    rc = read(f, &line[i], 1);
	    if (rc < 0) { err_reading(locals, f); }
	    if (rc == 0) {
		eof = 1;
		line[i] = 0;
	    } else {
		i++;
	    }
	} while (!eof && line[i - 1] != '\n');
	if (i > 0 && (eof || line[--i] == '\n') && i <= dlen) {
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
    return success;
}

/** main function of the lucheck plugin */
int main(int argc, char **argv) {
char *ffdb = getenv(ENV_FFDB), *recipient;
char *alias, *at, *dotqmail;
int i, rc, alen, rlen;
struct cdb cdb = {0};
struct passwd *pw;
struct stat statbuf;

    progname = argv[0];
    debug = getenv(ENV_DEBUG);
    if (debug) {
	cmd_reject = strdup(CMD_REJECT);
	if (!cmd_reject) { err_memory(); }
	cmd_reject[1] = '4';
	verbose = debug;
    } else {
	cmd_reject = (char *) CMD_REJECT;
	verbose = getenv(ENV_VERBOSE);
    }

    if (getenv(ENV_RELAYCLIENT)) {
	DEBUG("RELAYCLIENT set - exiting")
	exit(0);
    }

    alias = get_required_env(ENV_ALIAS);
    recipient = strdup(get_required_env(ENV_RCPT_TO));
    if (!recipient) { err_memory(); }
    for (rlen = 0; recipient[rlen]; rlen++) {
	recipient[rlen] = tolower(recipient[rlen]);
    }

    DEBUG3("Started for '",recipient,"'")

    at = index(recipient, '@');
    if (at) {
	char *domain = &at[1];
	if (is_virtual(domain)) {
	    DEBUG3("'",recipient,"' is virtual - exiting.")
	    exit(0);
	}
	if (!is_local(domain)) {
	    DEBUG3("'",recipient,"' is not local - exiting.")
	    exit(0);
	}
    }

    alen = strlen(alias);

    if (ffdb) {
	char *helper = (char *) malloc(rlen + 5);
	if (!helper) { err_memory(); }
	helper[0] = ':';
	strcpy(&helper[1], recipient);
	if (!at) {
	    strcat(helper, "@");
	}
	DEBUG4("Checking alias DB '",ffdb,"' for ",recipient)
	i = open(ffdb, O_RDONLY);
	if (i < 0) { err_open(ffdb); }
	cdb_init(&cdb, i);
	rc = cdb_find(&cdb, helper, strlen(helper));
	if (rc == -1) { err_reading(ffdb, i); }
	if (!rc && at) {
	    helper[at - recipient + 2] = 0;
	    DEBUG4("Checking alias DB '",ffdb,"' for ",&helper[1])
	    rc = cdb_find(&cdb, helper, strlen(helper));
	    if (rc == -1) { err_reading(ffdb, i); }
	}
	if (!rc && at) {
	    strcpy(&helper[1], at);
	    DEBUG4("Checking alias DB '",ffdb,"' for ",&helper[1])
	    rc = cdb_find(&cdb, helper, strlen(helper));
	    if (rc == -1) { err_reading(ffdb, i); }
	}
	cdb_free(&cdb);
	close(i);
	if (rc) {
	    DEBUG3("Alias '",&helper[1],"' found - exiting.")
	    free(helper);
	    exit(0);
	}
	free(helper);
    }

    if (at) {
	*at = 0;
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
	DEBUG3("Looking up local user '",recipient,"'")
	pw = getpwnam(recipient);
	if (pw) {
	    DEBUG3("Local user '",recipient,"' found - exiting.")
	    exit(0);
	}
	DEBUG3("Checking ~alias for '",recipient,"'")
	if (!stat(dotqmail, &statbuf)) {
	    DEBUG3("Alias file '",dotqmail,"' found - exiting.")
	    exit(0);
	}
	if (errno != ENOENT) {
	    err_open(dotqmail);
	}
	at = rindex(recipient, '-');
	if (at) { *at = 0; }
    } while (at);

    VERBOSE3("'",recipient,"' doesn't seem to be local - REJECTING.")

    reject();

    /* We should free(cmd_reject) here in debug mode.
     * We don't, on purpose.
     */

    return 0;
}

/* Do not change the following line:
 * arch-tag: cc3f1dfa-3073-45cc-bdd4-c2e1355a9df5
 */
