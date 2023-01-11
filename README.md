# QDMR, a GUI application and command line tool to program DMR radios
<a href="https://translate.codeberg.org/projects/qdmr/">
 <img src="https://translate.codeberg.org/widgets/qdmr/-/graphical-user-interface/svg-badge.svg"
      alt="Translation status">
</a>

![qdmr channel editor](https://raw.githubusercontent.com/hmatuschek/qdmr/master/doc/fig/qdmr-channels.png "The qdmr CPS software.")

*qdmr* is a graphical user interface (GUI) application that allows to program several types of DMR radios.
To this end, it aims at being a more universal codeplug programming software (CPS) compared to the
device and even revision specific CPSs provided by the manufacturers. The goal of this project is to
provide a **single**, **comfortable**, [**well-documented**](https://dm3mat.darc.de/qdmr/manual/) 
and **platform-independent** CPS for several types of (mainly Chinese) DMR radios.

## Supported Radios 
Currently, there are only few radios that are supported

  * Open GD77 firmware (since version 0.4.0)
  * Radioddity GD77 (since version 0.8.1)
  * Baofeng/Radioddity RD-5R & RD-5R+ (since version 0.2.0)
  * TYT MD-390 / Retevis RT8 (since version 0.9.0)
  * TYT MD-UV380 (since version 0.9.0)
  * TYT MD-UV390 / Retevis RT3S (since version 0.3.0)
  * TYT MD-2017 / Retevis RT82 (since version 0.9.0)
  * Anytone AT-D878UV (since version 0.5.0)
  * Anytone AT-D868UVE (since version 0.7.0)
  * Anytone AT-D878UVII (since version 0.8.0)
  * Anytone AT-D578UV (since version 0.8.0)
  * BTECH DM-1701 / Retevis RT84 (since version 0.10.0)
  * BTECH DMR-6x2 (since version 0.11.0)

A more [detailed list](https://dm3mat.darc.de/qdmr/#dev) is also available. 

## Questions?

  * If you find any bugs or have suggestions to improve qdmr, consider [opening an issue](https://github.com/hmatuschek/qdmr/issues/new) or participate in one of the [discussions](https://github.com/hmatuschek/qdmr/discussions).
  * There is also a *Matrix* chat at [#qdmr:darc.de](https://matrix.to/#/#qdmr:darc.de).

## Releases
<a href="https://repology.org/project/qdmr/versions">
 <img src="https://repology.org/badge/vertical-allrepos/qdmr.svg" alt="Packaging status" align="right">
</a>
 
 * **[Version 0.11.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.11.0)** -- Added proper support for BTech DMR-6X2UV, some bugfixes.
 * **[Version 0.10.4](https://github.com/hmatuschek/qdmr/releases/tag/v0.10.4)** -- Added support for BTech DM1701, some bugfixes.
 * **[Version 0.9.3](https://github.com/hmatuschek/qdmr/releases/tag/v0.9.3)** -- Reworked core library, added support for TyT MD-2017/Retevis RT82, TyT MD-390/Retevis RT8 & TyT MD-UV380.
 * **[Version 0.8.1](https://github.com/hmatuschek/qdmr/releases/tag/v0.8.1)** -- Fixed Radioddity GD-77 support (callsign db still buggy).
 * **[Version 0.7.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.7.0)** -- Added AT-D868UVE support and many bugfixes.
 * **[Version 0.6.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.6.4)** -- Added APRS & roaming for AT-D878UV.
 * **[Version 0.5.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.5.0)** -- Added support for Anytone AT-D878UV.
 * **[Version 0.4.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.4.0)** -- Added Open GD77 support.
 * **[Version 0.2.1](https://github.com/hmatuschek/qdmr/releases/tag/v0.2.1)** -- First public release.


## Install
There are several ways to install qdmr on your system ranging from simple app-package downloads to building qdmr from its sources. For a detailed list of instructions for your system, read the [install instructions](https://dm3mat.darc.de/qdmr/install.html). Some distributions (see badge above) already added qdmr, thus easing the install using the system package manager.

## License
qdmr - A GUI application and command-line-tool to program DMR radios.
Copyright (C) 2019-2020 Hannes Matuschek, DM3MAT

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
