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

all: run-tests

lucheck-spp:
	make -C ../src
	ln -sf ../src/lucheck-spp

cdbmake:
	make -C ../cdb-0.75
	ln -sf ../cdb-0.75/cdbmake

run-tests: lucheck-spp cdbmake
	mkdir -p control1
	echo local.domain >control1/locals
	echo .sub.domain >>control1/locals
	echo virtual.domain:v >control1/virtualdomains
	mkdir -p control2
	touch control2/locals
	mkdir -p control3
	echo local.domain >control3/me
	mkdir -p alias
	touch alias/.qmail-thatsalocalalias
	./test-error.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@local.domain ./test-error.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@local.domain LUCHECK_CONTROL=control1 ./test-error.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@local.domain LUCHECK_CONTROL=. ./test-error.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@local.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-reject.sh
	SMTPRCPTTO=IhOpEtHaTsNoTaLoCaLuSeRnAmE@LoCaL.DoMaIn LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-reject.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@local.domain LUCHECK_CONTROL=control3 LUCHECK_ALIAS=alias ./test-reject.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@local.domain LUCHECK_CONTROL=control2 LUCHECK_ALIAS=alias ./test-accept.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@virtual.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-accept.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@local.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias RELAYCLIENT= ./test-accept.sh
	SMTPRCPTTO=`id -un` LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-accept.sh
	SMTPRCPTTO=ihopethatsnotalocalusername LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-reject.sh
	SMTPRCPTTO=`id -un`@local.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-accept.sh
	SMTPRCPTTO=`id -un`@local.domain LUCHECK_CONTROL=control3 LUCHECK_ALIAS=alias ./test-accept.sh
	SMTPRCPTTO=`id -un`@thats.a.sub.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-accept.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@thats.a.sub.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-reject.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@ThAtS.A.SuB.DoMaIn LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-reject.sh
	SMTPRCPTTO=`id -un`-test@thats.a.sub.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-accept.sh
	SMTPRCPTTO=thatsafastforwardalias LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias ./test-reject.sh
	SMTPRCPTTO=thatsafastforwardalias LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias LUCHECK_FFDB=aliases.cdb ./test-accept.sh
	SMTPRCPTTO=ThAtSaFaStFoRwArDaLiAs LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias LUCHECK_FFDB=aliases.cdb ./test-accept.sh
	SMTPRCPTTO=thatsalocalalias LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias LUCHECK_FFDB=aliases.cdb ./test-accept.sh
	SMTPRCPTTO=fastforwardalias@local.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias LUCHECK_FFDB=aliases.cdb ./test-reject.sh
	SMTPRCPTTO=fastforwardalias@specific.sub.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias LUCHECK_FFDB=aliases.cdb ./test-accept.sh
	SMTPRCPTTO=FaStFoRwArDaLiAs@SpEcIfIc.sUb.DoMaIn LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias LUCHECK_FFDB=aliases.cdb ./test-accept.sh
	SMTPRCPTTO=ihopethatsnotalocalusername@aliased.sub.domain LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias LUCHECK_FFDB=aliases.cdb ./test-accept.sh
	SMTPRCPTTO=IhOpEtHaTsNotAlOcAlUseRnaMe@AlIaSeD.sUb.dOmAin LUCHECK_CONTROL=control1 LUCHECK_ALIAS=alias LUCHECK_FFDB=aliases.cdb ./test-accept.sh
	mkdir -p control4
	echo short.name >control4/locals
	echo a.looooong.name >>control4/locals
	SMTPRCPTTO=ihopethatsnotalocalusername@test.name LUCHECK_CONTROL=control4 LUCHECK_ALIAS=alias ./test-accept.sh

clean:
	rm -rf lucheck-spp control[1234]

# Do not change the following line:
# arch-tag: c23deea8-3210-4fc7-97ff-247520c2bbfa
