# Toplevel Makefile

VERSION="0.1"

BUILDDIR=,,build

PROJECT=lucheck-spp

all build test: configure
	make -C $(BUILDDIR) $@

dist: configure $(BUILDDIR)/$(PROJECT)-$(VERSION).tar.gz

$(BUILDDIR)/$(PROJECT)-$(VERSION).tar.gz: src/*.[ch] src/Makefile doc/COPYING \
	  doc/README* doc/TODO test/*.sh test/Makefile Makefile* cdb-0.75 \
	  test/aliases.cdb lucheck-spp.spec
	ln -s . $(PROJECT)-$(VERSION)
	tar cvfz $@ `for i in $^; do echo $(PROJECT)-$(VERSION)/$$i; done`
	rm $(PROJECT)-$(VERSION)

clean:
	rm -rf $(BUILDDIR)

configure: clean
	mkdir -p $(BUILDDIR)
	find . -name ,,build -prune -o \
	       -name '{arch}' -prune -o \
	       -name '.arch-ids' -prune -o -type f | \
	  grep -v '^\.$$' | sed 's=^\./==' | \
	  while read file; do \
	    mkdir -p $(BUILDDIR)/`dirname "$$file"`; \
	    ln -s "`pwd`/$$file" "$(BUILDDIR)/$$file"; \
	  done
	rm $(BUILDDIR)/Makefile*
	ln -s ../Makefile.in $(BUILDDIR)/Makefile
	rm $(BUILDDIR)/cdb-0.75/error.h
	sed 's/^extern int errno.*/#include <errno.h>/' <cdb-0.75/error.h \
		>$(BUILDDIR)/cdb-0.75/error.h

# Don't change the following line!
# arch-tag: 276c5abb-b479-44ce-b7fe-06c1dd99089c

