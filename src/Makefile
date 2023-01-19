# lucheck-spp - A qmail-spp plugin implementing greylisting

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

CDB=../cdb-0.75

#CFLAGS=-O6 -Wall -pedantic -Wstrict-prototypes -I$(CDB)
CFLAGS=-g -Wall -pedantic -Wstrict-prototypes -I$(CDB)
LDFLAGS=-g

all: lucheck-spp

$(CDB)/cdb.a $(CDB)/byte.a $(CDB)/unix.a $(CDB)/buffer.a \
$(CDB)/uint32.h:
	rm -f $(CDB)/conf-cc $(CDB)/conf-ld
	echo "$(CC) $(CFLAGS)" >$(CDB)/conf-cc
	echo "$(CC) $(LDFLAGS)" >$(CDB)/conf-ld
	make -C $(CDB) `basename $@`

lucheck-spp.o: lucheck-spp.c $(CDB)/uint32.h

lucheck-spp: lucheck-spp.o commonstuff.o \
	     $(CDB)/cdb.a $(CDB)/byte.a $(CDB)/unix.a \
	     $(CDB)/buffer.a

clean:
	rm -f lucheck-spp *.o

