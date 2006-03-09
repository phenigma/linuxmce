Summary: A Wrapper for run-time loading of XvMC libraries.
Name: libXvMCW
Version: 0.9.0
Release: 1
License: MIT-Style, or GPL
Group: X11
URL: http://sourceforge.net/projects/unichrome
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
A Wrapper for XvMC libraries that allows the X server or 
user to specify the hardware dependent library at run-time

%prep
%setup -q
./configure

%build
make

%install
make DESTDIR=%buildroot install


%clean
[ ${RPM_BUILD_ROOT} != "/" ] && rm -rf ${RPM_BUILD_ROOT}


%files
%defattr(-,root,root,-)
%doc README
/usr/X11R6/lib/libXvMCW.a
/usr/X11R6/lib/libXvMCW.la
/usr/X11R6/lib/libXvMCW.so
/usr/X11R6/lib/libXvMCW.so.1
/usr/X11R6/lib/libXvMCW.so.1.0.0
/etc/X11/XvMCConfig
/usr/X11R6/include/X11/extensions/vldXvMC.h

%changelog
* Thu Sep 23 2004 Thomas Hellström <sthomas@shipmail.org>
- Initial release

