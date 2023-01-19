# lucheck-spp - A qmail-spp plugin for checking local users

## License

All files in this package except those in the "cdb-0.75" subdirectory are licensed under the GPL (v2.0, see below). Those in the "cdb-0.75" subdirectory are public domain and were created by [D. J. Bernstein](https://cr.yp.to/djb.html), see the README in that subdirectory.

> Copyright (C) 2004-2005,2011 Peter Conrad <conrad@quisquis.de>
> 
> This program is free software; you can redistribute it and/or modify it under
> the terms of the GNU General Public License (version 2) as published by the Free
> Software Foundation.
> 
> This program is distributed in the hope that it will be useful, but WITHOUT
> ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
> FOR A PARTICULAR PURPOSE.  See the [GNU General Public License](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html#SEC1) for
> more details.
> 
> You should have received a copy of the GNU General Public License along with
> this program; if not, write to the Free Software Foundation, Inc., 59 Temple
> Place, Suite 330, Boston, MA  02111-1307  USA

## Overview

[qmail-spp](https://qmail-spp.sourceforge.net) is a patch for D. J. Bernstein's "[qmail](https://qmail.org)" MTA package. The patch enables "plugin" programs to be run at various stages during an SMTP protocol exchange.

This package implements a qmail-spp-style plugin that performs certain checks on the *user* part of envelope recipient addresses. In particular, if the *domain* part of the envelope recipient indicates that the mail should be delivered locally, it checks if *user* exists locally and if not rejects
the mail.

## How it works

First, the plugin checks if the recipient address *user*@*domain* should be checked at all by looking at the `RELAYCLIENT` environment variable and comparing the *domain* part of the address to the domains listed in `control/locals`. If `RELAYCLIENT` is set or the domain is not listed in `control/locals`, it exits immediately.

If `control/virtualdomains` exists and *domain* is listed in it, the plugin exits as well (assuming the local user for handling the virtual domain is set up properly).

If the environment variable `LUCHECK_FFDB` is set, it uses its contents as the pathname of a
[fastforward](https://cr.yp.to/fastforward.html) style database. If the database contains *user*, it accepts the address and exit.

It repeats the following steps:

1. If *user* is a local system user, it accepts the address and exits.
2. If `$LUCHECK_ALIASDIR/.qmail-*user*` exists, it accepts the address and exits.
3. If *user* contains a dash ('-'), it strips the rightmost '-' and everything after it and repeats steps 1-3 with the new *user*.

If the address has not yet been accepted at this point, it is rejected with an error message.

If the environment variable `LUCHECK_DEBUG` is set (to any value) some debugging output will be sent to wherever your qmail-smtpd logs are written.  Also, if the recipient is rejected, it will be rejected with a temporary (4*xy*) SMTP response code instead of a permanent (5*xy*) code.

If the environment variable `LUCHECK_VERBOSE` is set (to any value), rejected addresses will be logged.

## Status

The current implementation (with only minor modifications) has been in use on a productive system for several years now and can be considered stable.

## Downloading

Check our releases on [github](https://github.com/pmconrad/lucheck-spp/tags).

## Building

Simply unpack the sources and type `make` in the top-level directory of the distribution. This will create a subdirectory `,,build`, in which the build will take place.

**Hint:** apparently you need [GNU make](https://savannah.gnu.org/projects/make), which may be installed under a different name on your platform. Or not at all.

The resulting executable will reside in `,,build/src/lucheck-spp`.

## Installing

A "make install" target is **not** provided, because the installation process requires some manual intervention as well as some decisions you'll have to make yourself. Basically, you'll have to perform the steps enumerated below.  For RPM-based systems, a .spec file is provided (assuming a qmail installation under `/var/qmail`). The RPM-based installation requires some manual work as well.

1. As has been said above, this program is a **plugin** for a Qmail installation that was built with the qmail-spp patch. So, as the first step, you'll have to build and install Qmail with that patch.
    In the following, we'll assume that the patched Qmail is installed in `/var/qmail` (the default location).
2. If you don't have a directory where your plugins reside, create it. It must be situated somewhere within the Qmail installation dir.
    In the following, we'll assume that the plugin dir is `/var/qmail/plugins`.
3. After building this plugin with `make`, copy the resulting executable to the plugin directory `/var/qmail/plugins`.
4. Modify whatever process you use for running qmail-smtpd to set the environment variable `LUCHECK_ALIASDIR` and optionally `LUCHECK_FFDB`, `LUCHECK_DEBUG` or `LUCHECK_VERBOSE` to appropriate values.
    For example:
```
     LUCHECK_ALIASDIR=/var/qmail/alias
     LUCHECK_VERBOSE=yes
     export LUCHECK_ALIASDIR LUCHECK_VERBOSE
     tcpserver -R -v -x /etc/tcprules.smtpd.cdb -c 20 0 smtp qmail-smtpd 2&gt;&amp;1
```
    Don't forget to restart the service if required!
5. Enable the plugin by adding the path (relative to the qmail installation dir) to the `[rcpt]` section of the qmail-spp configuration file `/var/qmail/control/smtpplugins`.
    Minimal example:
```
[auth]

[helo]

[mail]

[rcpt]
plugins/lucheck-spp
```

6. Keep an eye on your log files.
