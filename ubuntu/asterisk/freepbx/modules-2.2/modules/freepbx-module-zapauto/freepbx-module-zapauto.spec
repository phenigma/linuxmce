Summary: FreePBX module - Zaptel AutoConfiguration
Name: freepbx-module-zapauto
Version: 0.6.4
Release: 1
License: GPL
Group: Applications/System
URL: http://www.freepbx.org/
Source: freepbx-module-zapauto-r%{release}.tar.gz
Packager: Diego Iastrubni <diego.iastrubni@xorcom.com>
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildArch: noarch
Requires: asterisk, sudo

# TODO:
# when freepbx will be packaged into RPMS, this will be used
#Requires: asterisk, sudo, freepbx-common 

%description 
This is a freepbx-module for autoconfiguration of Zaptel hardware.

%prep
%setup -n freepbx-module-zapauto-r%{release}

%install
[ "%{buildroot}" != '/' ] && rm -rf %{buildroot}
pwd
make DESTDIR=%{buildroot} installfiles
make DESTDIR=%{buildroot} patchfiles

%post
SUDOERS=/etc/sudoers
LINE="asterisk ALL=NOPASSWD:/var/lib/asterisk/bin/detect_zap"
if ! fgrep -q "$LINE" /etc/sudoers; then echo "$LINE" >> $SUDOERS; fi

#LINE2="asterisk ALL=NOPASSWD:/etc/init.d/asterisk"
#if ! fgrep -q "$LINE2" /etc/sudoers; then echo "$LINE2" >> $SUDOERS; fi

%preun
# this should be run only on removals, not on upgrades
# see http://www.rpm.org/hintskinks/buildtree/mdk-rpm/#UPGRADE
if [ $1 = 0 ]; then
	SUDOERS=/etc/sudoers
        LINE="asterisk ALL=NOPASSWD:/var/lib/asterisk/bin/detect_zap"
	fgrep -v "$LINE"  $SUDOERS > ${SUDOERS}.tmp && cp ${SUDOERS}.tmp $SUDOERS && rm ${SUDOERS}.tmp
	
#	LINE2="asterisk ALL=NOPASSWD:/etc/init.d/asterisk"
#	fgrep -v "$LINE2" $SUDOERS > ${SUDOERS}.tmp && cp ${SUDOERS}.tmp $SUDOERS && rm ${SUDOERS}.tmp
fi

%files 
%defattr(-,root,root)
%doc LICENSE
%doc README
/var/www/html/admin/modules/zapauto/
/var/lib/asterisk/bin/detect_zap 
/var/lib/asterisk/bin/fix_ast_db 
/var/lib/asterisk/bin/zap2amp 
/var/lib/asterisk/bin/zap.template

%changelog
* Wed 18 Apr 2007  Diego Iastrubni <diego.iastrubni@xorcom.com> - 0.6.4-1
- Restart of asterisk + zaptel is done only once
- Script will change colors:
     - blue means running
     - green means ended ok
     - red means ended with a problem
- Updated freePBX configuration to reflect new version (forgotten in last update)

* Sun Apr 15 2007 Diego Iastrubni <diego.iastrubni@xorcom.com> - 0.6.3-1
- Code supports freePBX 2.1 and 2.2

* Sun Apr 1 2007 Diego Iastrubni <diego.iastrubni@xorcom.com> - 0.6.2-1
- New release
- Better, smarter configuration bootstrap
- Astribank BRI detection has been added

* Tue Feb 20 2007 Diego Iastrubni <diego.iastrubni@xorcom.com> - 0.6.1-1
- New release
- Less vebose configuration
- Default of the echo canceller is now "yes" instead of "8"

* Thu Nov 22 2006 Diego Iastrubni <diego.iastrubni@xorcom.com> - 0.6.0-1
- New release
- Targetting version 2.2 of freePBX

* Thu Nov 22 2006 Diego Iastrubni <diego.iastrubni@xorcom.com> - 0.5.5-1
- New release
- Code + spec cleanups

* Thu Oct 8 2006 Diego Iastrubni <diego.iastrubni@xorcom.com>
- New release
- zaptel.conf and zapata-*.conf are set a+r on detection
- When new hardware is discovered, you will hear 2 beeps (if the application "beep" is installed)

* Thu Aug 13 2006 Diego Iastrubni <diego.iastrubni@xorcom.com>
- First release

