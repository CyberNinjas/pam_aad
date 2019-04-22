%define version 0.0.1

Name: pam_aad
Version: %{version}
Release: 1%{?dist}
Summary: Azure Active Directory PAM Module

Group: System Environment/Base
License: GPLv3+
URL: https://github.com/CyberNinjas/pam_aad
Source0: https://github.com/CyberNinjas/pam_aad/archive/%{version}.tar.gz

BuildRequires: jansson-devel libcurl-devel libuuid-devel openssl-devel pam-devel

%define _pamlibdir %{_libdir}/security
%define _pamconfdir %{_sysconfdir}/pam.d

%description
This PAM module aims to provide Azure Active Directory authentication for Linux.

%prep
%setup -q -n pam_aad-${version}


%build
./bootstrap.sh
%configure --with-pam-dir=%{_pamlibdir}
make

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc README.md
%license COPYING


%changelog
# Create RPM for easier installation.
