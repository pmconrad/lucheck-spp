%define QMAIL_DIR /var/qmail

Summary: A plugin for qmail-smtpd for checking if recipients can be delivered locally
Name: lucheck-spp
Version: 0.1
Release: 1
Copyright: P. Conrad <conrad@tivano.de>
Group: System/Mail
Source: %{name}-%{version}.tar.gz
Requires: qmail
BuildRoot: /var/tmp/%{name}-root
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
make

%install
mkdir -p $RPM_BUILD_ROOT%{QMAIL_DIR}/plugins
install -m 0755 ,,build/src/%{name} $RPM_BUILD_ROOT%{QMAIL_DIR}/plugins

%post
cd %{QMAIL_DIR}/control
if [ "$1" -ge 1 -a `grep -c plugins/%{name} smtpplugins` -eq 0 ]; then
    echo Trying to insert plugin into %{QMAIL_DIR}/control/smtpplugins...
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
    echo Trying to remove %{name} from %{QMAIL_DIR}/control/smtpplugins...
    cd %{QMAIL_DIR}/control
    grep -v '^plugins/%{name}$' smtpplugins >smtpplugins.new && \
      mv smtpplugins.new smtpplugins
fi

%clean
[ "$RPM_BUILD_ROOT" = "/" ] || rm -rf "$RPM_BUILD_ROOT"

%files
%defattr(-,root,qmail)
%doc doc/*
%dir %{QMAIL_DIR}/plugins
%{QMAIL_DIR}/plugins/%{name}

%changelog
* Wed Sep 29 2004 - conrad@tivano.de
- Adapted from mfcheck-spp to this project

# Do not change the following line:
# arch-tag: 26dd308a-26de-416a-ab01-c813f67a42e6

