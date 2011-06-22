%define QMAIL_DIR /var/qmail

Summary: A plugin for qmail-smtpd for checking if recipients can be delivered locally
Name: lucheck-spp
Version: 1.0
Release: 1
License: GPLv2
Group: Productivity/Networking/Email/Servers
URL: http://www.unix-ag.uni-kl.de/~conrad/lucheck/
Source: http://www.unix-ag.uni-kl.de/~conrad/lucheck/%{name}-%{version}.tar.gz
Requires: qmail
%if 0%{?suse_version} >= 1100 && 0%{?suse_version} < 1150
BuildRequires: licenses
%endif
BuildRoot: %{_tmppath}/%{name}-%{version}-build
%description
This package implements a plugin for D. J. Bernstein's "qmail" MTA 
(http://qmail.org/ ). It requires qmail to be compiled with the 
smtp-plugin-patch (http://qmail-spp.sourceforge.net/ ), hence the name.
The plugin will perform a configurable number of checks on every envelope
recipient of every incoming email. These checks are currently implemented:
 - matching against local users (via getpwnam())
 - matching against local aliases in ~alias/.qmail-<alias>
 - matching against a fastforward-style aliases database (optional)

%prep
%setup

%build
make CFLAGS="%{optflags} -I\$(CDB)" LDFLAGS="%{optflags}" %{?_smp_mflags}
lic="`md5sum doc/COPYING | cut -d' ' -f 1`"
if [ -r "/usr/share/doc/licenses/md5/$lic" ]; then
    ln -sf /usr/share/doc/licenses/md5/"$lic" doc/COPYING
fi

%install
mkdir -p "%{buildroot}%{QMAIL_DIR}/plugins"
install -m 0755 -s ",,build/src/%{name}" "%{buildroot}%{QMAIL_DIR}/plugins"

%post
cd "%{QMAIL_DIR}/control"
if [ "$1" -ge 1 -a `grep -c "plugins/%{name}" smtpplugins` -eq 0 ]; then
    echo "Trying to insert plugin into %{QMAIL_DIR}/control/smtpplugins..."
    if grep '^\[rcpt\]$' smtpplugins >/dev/null; then
	i=1
	while [ `head -$i smtpplugins | grep -c '^\[rcpt\]$'` -lt 1 ]; do
	    i=$(($i + 1))
	done
	head -$i smtpplugins >smtpplugins.new
	echo "plugins/%{name}" >>smtpplugins.new
	tail +$(($i + 1)) smtpplugins >>smtpplugins.new && \
	  mv smtpplugins.new smtpplugins
    else
	echo "[rcpt]" >>smtpplugins
	echo "plugins/%{name}" >>smtpplugins
    fi
    echo "Don't forget to set the required environment variables"
    echo "when invoking qmail-smtpd."
fi

%preun
if [ "$1" = 0 ]; then
    echo "Trying to remove %{name} from %{QMAIL_DIR}/control/smtpplugins..."
    cd "%{QMAIL_DIR}/control"
    grep -v '^plugins/%{name}$' smtpplugins >smtpplugins.new && \
      mv smtpplugins.new smtpplugins
fi

%clean
[ "%{buildroot}" = "/" ] || rm -rf "%{buildroot}"

%files
%defattr(-,root,qmail,-)
%doc doc/*
%dir %{QMAIL_DIR}/plugins
%{QMAIL_DIR}/plugins/%{name}

%changelog
* Sun Jun 19 2011 - conrad@tivano.de
- Minor modifications
* Fri Feb  1 2008 - conrad@tivano.de
- Version 1.0
* Mon Apr  4 2005 - conrad@tivano.de
- Bugfix, version 0.2
* Wed Sep 29 2004 - conrad@tivano.de
- Adapted from mfcheck-spp to this project

# Do not change the following line:
# arch-tag: 26dd308a-26de-416a-ab01-c813f67a42e6

