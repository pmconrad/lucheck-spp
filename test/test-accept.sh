#!/bin/sh

#   Copyright (C) 2004 Peter Conrad <conrad@tivano.de>

#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License (version 2) as
#   published by the Free Software Foundation.

#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

reject=`./lucheck-spp 2>/dev/null`
if [ $? != 0 ]; then
    echo "The plugin unexpectedly exited with an error code!"
    exit -1
fi

if [ "$reject" != "" ]; then
    echo "The plugin unexpectedly rejected the test case with '$reject'!"
    exit -1
fi

exit 0

