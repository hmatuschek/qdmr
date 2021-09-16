Summary: A GUI application for configuring and programming cheap DMR radios 

%define version 0.8.1

License: GPL-2.0+
Group: Applications/Communications
Name: qdmr
Prefix: /usr
Release: 1
Source: qdmr-%{version}.tar.gz
URL: https://dm3mat.darc.de/qdmr
Version: %{version}
Buildroot: /tmp/qdmrrpm
BuildRequires: gcc-c++, cmake
%if 0%{?suse_version}
BuildRequires: libqt5-qtbase-devel, libqt5-qtlocation-devel libqt5-qtserialport-devel libusb-1.0-devel update-desktop-files
Requires: libqt5-qtbase, libqt5-qtlocation libqt5-qtserialport libusb-1.0 xsltproc docbook-xsl
%endif

%description
A GUI application for configuring and programming cheap DMR radios. 
Including: Radioddity/Baofeng RD-5R, Radioddity GD77, TYT MD-UV390,
Retevis RT3S, Open GD77 firmware (GD77, RD-5R & DM-1801),
AnyTone AT-D868UVE, AT-D878UV, AT-D878UVII, AT-D578UV,


%prep
%setup -q


%build
cmake -DCMAKE_BUILD_TYPE=RELEASE \
      -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr \
      -DBUILD_MAN=ON
make


%install
make install
#%if 0%{?suse_version}
#%suse_update_desktop_file  Education Teaching
#%endif

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-, root, root, -)
%attr(755, root, root) %{_prefix}/bin/qdmr
%{_prefix}/share/applications/qdmr.desktop
%{_prefix}/share/icons/hicolor/64x64/qdmr.png
