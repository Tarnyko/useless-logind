%define systemd_compatibility 1

Name:           useless-logind
Version:        0.1
Release:        0
Summary:        A login and session management daemon for UNIX systems
License:        MIT
Group:          Base/Startup
Url:            https://github.com/Tarnyko/useless-logind.git

Source0:        %name-%version.tar.xz
Source1:        %name.manifest
BuildRequires:	autoconf >= 2.64, automake >= 1.11
BuildRequires:  libtool >= 2.2
BuildRequires:  pam-devel

%if %{systemd_compatibility} == 1
Conflicts:      systemd
%endif

%description
useless-logind is a login, session and seat management daemon compatible with the freedesktop.org multiseat recommendations (http://www.freedesktop.org/wiki/Software/systemd/multiseat/) and, partly, with the systemd-logind conventions (http://www.freedesktop.org/wiki/Software/systemd/logind/). It may be a drop-in replacement for systemd-logind. It works under GNU/Linux, FreeBSD and OpenBSD.

%package devel
Summary:        Development files for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}

%description devel
Development components for the %{name} package


%prep
%setup -q
cp %{SOURCE1} .


%build
%if %{systemd_compatibility} == 1
%reconfigure --enable-systemd-compatibility
%else
%reconfigure
%endif
make %{?_smp_mflags}


%install
%make_install
%install_service multi-user.target.wants data/useless-logind.service
install -m 0644 data/useless-logind.conf %{buildroot}%{_sysconfdir}
%if %{systemd_compatibility} == 1
ln -s loginctl %{buildroot}%{_bindir}/systemd-loginctl
%endif


%files
%manifest %{name}.manifest
%defattr(-,root,root)
%license COPYING
%{_sbindir}/useless-logind
%config %{_sysconfdir}/useless-logind.conf
%{_libdir}/libuseless-logind.so.*
%{_unitdir}/useless-logind.service
%{_unitdir}/multi-user.target.wants/useless-logind.service
%if %{systemd_compatibility} == 1
%{_bindir}/loginctl
%{_bindir}/systemd-loginctl
%{_libdir}/libsystemd-login.so.*
%endif

%files devel
%manifest %{name}.manifest
%defattr(-,root,root)
%{_includedir}/libuseless-logind.h
%{_libdir}/libuseless-logind.so
%{_libdir}/pkgconfig/libuseless-logind.pc
%if %{systemd_compatibility} == 1
%{_includedir}/systemd/sd-login.h
%{_libdir}/libsystemd-login.so
%{_libdir}/pkgconfig/libsystemd-login.pc
%endif

%changelog
