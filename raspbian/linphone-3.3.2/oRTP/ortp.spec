# -*- rpm-spec -*-
# 
# ortp -- Real-time Transport Protocol Stack
# 
# Default is optimized for Pentium IV but will execute on Pentium II &
# later (i686).

%ifarch %ix86
%define		ortp_cpu	pentium4
%endif

Summary:	Real-time Transport Protocol Stack
Name:		ortp
Version:	0.16.3
Release:	1
License:	LGPL
Group:		Applications/Communications
URL:		http://linphone.org/ortp/
Source0:	%{name}-0.16.3.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot
%ifarch %ix86
BuildArch:	i686
%endif

%description
oRTP is a LGPL licensed C library implementing the RTP protocol
(rfc1889). It is available for most unix clones (primilarly Linux and
HP-UX), and Microsoft Windows.

%package        devel
Summary:        Headers, libraries and docs for the oRTP library
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
oRTP is a LGPL licensed C library implementing the RTP protocol
(rfc1889). It is available for most unix clones (primilarly Linux and
HP-UX), and Microsoft Windows.

This package contains header files and development libraries needed to
develop programs using the oRTP library.

%ifarch %ix86
%define	ortp_arch_cflags -malign-double -march=i686 -mcpu=%{ortp_cpu}
%else
# Must be non-empty
%define ortp_arch_cflags -Wall
%endif
%define ortp_cflags %ortp_arch_cflags -Wall -g -pipe -pthread -O3 -fomit-frame-pointer -fno-schedule-insns -fschedule-insns2 -fstrict-aliasing

%prep
%setup -q

%build
%configure \
	--enable-shared \
	--enable-static
%{__make} -j$RPM_BUILD_NCPUS CFLAGS="%ortp_cflags" CXXFLAGS="%ortp_cflags"

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README TODO
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%doc doc/html
%{_libdir}/*.la
%{_libdir}/*.a
%{_libdir}/*.so
%{_libdir}/pkgconfig/*.pc
%{_includedir}

%changelog
* Tue Oct 25 2005 Francois-Xavier Kowalski <fix@hp.com>
- Add to oRTP distribution with "make rpm" target
