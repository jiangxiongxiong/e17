%define use_gnome %{?_with_gnome}0

Summary: Enlightenment Epplets
Name: enlightenment-epplets
Version: 0.8
Release: 0.%(date '+%Y%m%d')
License: BSD
Group: User Interface/X
URL: http://www.enlightenment.org/
Source0: http://prdownloads.sourceforge.net/enlightenment/%{name}-%{version}.tar.gz
Packager: %{?_packager:%{_packager}}%{!?_packager:%{_vendor}}
Vendor: %{?_vendorinfo:%{_vendorinfo}}%{!?_vendorinfo:%{_vendor}}
Distribution: %{?_distribution:%{_distribution}}%{!?_distribution:%{_vendor}}
#BuildSuggests: freeglut-devel xorg-x11-devel
BuildRequires: imlib-devel XFree86-devel
Requires: enlightenment >= 0.16.0
Provides: epplets = %{version}
Obsoletes: epplets
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
Epplets are small, handy Enlightenment applets, similar to "dockapps"
or "applets" for other packages.  The epplets package contains the
base epplet API library and header files, as well as the core set of
epplets, including CPU monitors, clocks, a mail checker, mixers, a
slideshow, a URL grabber, a panel-like toolbar, and more.

%prep
%setup -q

%build
CFLAGS="$RPM_OPT_FLAGS"
export CFLAGS

%{configure} --prefix=%{_prefix} --bindir=%{_bindir} --datadir=%{_datadir} \
%if %{use_gnome}
    --disable-autorespawn \
%endif
    --enable-fsstd %{?acflags}
%{__make} %{?_smp_mflags} %{?mflags}

%install
%{__make} install DESTDIR=$RPM_BUILD_ROOT %{?mflags_install}

%ifos linux
%post -p /sbin/ldconfig
%endif

%ifos linux
%postun -p /sbin/ldconfig
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc ChangeLog
%{_includedir}/*
%{_libdir}/*
%{_bindir}/*
%{_datadir}/enlightenment/epplet_icons/*
%{_datadir}/enlightenment/epplet_data/*

%changelog
