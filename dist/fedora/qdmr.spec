#
# spec file for package qdmr
#
# Copyright (c) 2021-2025, Martin Hauke <mardnh@gmx.de>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.


%define ext_man .gz
%define sover   0
%define realver 0.13.1
Name:           qdmr
Version:        %{realver}
Release:        0fedora
Summary:        Graphical code-plug programming tool for DMR radios
License:        GPL-3.0-or-later
Group:          Productivity/Hamradio/Other
URL:            https://dm3mat.darc.de/qdmr/
#Git-Clone:     https://github.com/hmatuschek/qdmr.git
Source:         https://github.com/hmatuschek/qdmr/archive/refs/tags/v%{realver}.tar.gz
BuildRequires:  cmake
BuildRequires:  docbook5-style-xsl
BuildRequires:  xsltproc
BuildRequires:  gcc-c++
BuildRequires:  hicolor-icon-theme
BuildRequires:  librsvg2-tools
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(Qt6Core)
BuildRequires:  pkgconfig(Qt6Network)
BuildRequires:  pkgconfig(Qt6Positioning)
BuildRequires:  pkgconfig(Qt6SerialPort)
BuildRequires:  pkgconfig(Qt6Test)
BuildRequires:  pkgconfig(Qt6UiTools)
BuildRequires:  pkgconfig(Qt6Widgets)
BuildRequires:  pkgconfig(libusb-1.0)
BuildRequires:  pkgconfig(yaml-cpp)


%description
qdmr is a Qt application to program DMR radios. DMR is a digital modulation
standard used in amateur and commercial radio. To this end, qdmr is an
alternative codeplug programming software (CPS) that supports several radios
across several manufacturers.

Currently supported devices are:
 * Radioddity/Baofeng RD-5R, GD-73, GD-77
 * TYT MD-380, MD-390, MD-UV380, MD-UV390, MD-2017
 * Retevis RT8, RT3S, RT82
 * Open GD77 firmware (GD77, RD-5R, DM-1801, MD-UV390, RT-3S & DM1701)
 * AnyTone AT-D868UVE, AT-D878UV, AT-878UVII, AT-D578UV, AT-D578UVII
 * BTech DMR-6X2, DMR-6X2PRO, DM-1701, DR-1801UVA6

%package -n libdmrconf%{sover}
Summary:        Graphical code-plug programming tool for DMR radios
Group:          System/Libraries

%description -n libdmrconf%{sover}
qDMR is a simple to use and feature-rich code-plug programming software
(CPS) for cheap DMR radios.

This subpackage contains shared library part of libdmrconf.

%package devel
Summary:        Development files for dmrconf
Group:          Development/Libraries/C and C++
Requires:       libdmrconf%{sover} = %{version}

%description devel
qDMR is a simple to use and feature-rich code-plug programming software
(CPS) for cheap DMR radios.

This subpackage contains libraries and header files for developing
applications that want to make use of libdmrconf.

%prep
%setup -q -n %{name}-%{realver}

%build
%cmake -DBUILD_MAN=ON -DDOCBOOK2MAN_XSLT=docbook_man.fedora.xsl -DINSTALL_UDEV_PATH=%{_udevrulesdir}
%cmake_build

%install
%cmake_install

%post -n libdmrconf%{sover} -p /sbin/ldconfig
%postun -n libdmrconf%{sover} -p /sbin/ldconfig

%files
%license LICENSE
%doc README.md
%{_bindir}/dmrconf
%{_bindir}/qdmr
%{_udevrulesdir}/99-qdmr.rules
%{_datadir}/applications/qdmr.desktop
%{_metainfodir}/de.darc.dm3mat.qdmr.metainfo.xml
%{_datadir}/icons/hicolor/*/apps/qdmr.png
%{_mandir}/man1/dmrconf.1%{?ext_man}
%{_mandir}/man1/qdmr.1%{?ext_man}

%files -n libdmrconf%{sover}
%{_libdir}/libdmrconf.so.*

%files devel
%{_includedir}/libdmrconf/
%{_libdir}/libdmrconf.so

%changelog
* Tue Oct 21 2025 Hannes Matuschek <dm3mat@darc.de>
  - Updated to 0.13.1
* Sun Oct 19 2025 Hannes Matuschek <dm3mat@darc.de>
  - Updated to 0.13.0
* Thu Jul 25 2024 Hannes Matuschek <dm3mat@darc.de>
  - Updated to 0.12.0
  - added translations
  - updated description
  - fixed so-version
  - adapted from openSUSE spec file
