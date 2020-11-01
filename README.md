# QDMR a GUI application and command-line-tool to programm DMR radios

![qdmr channel editor](https://raw.githubusercontent.com/hmatuschek/qdmr/master/doc/fig/qdmr-channels.png "The qdmr CPS software.")

*qdmr* is a graphical user interface (GUI) application that allows to program several DMR radios.
To this end, it aims at being a more universal code-plug programming software (CPS) compared to the
device and even revision specific CPSs provided by the manufactures. The goal of this project is to
provide a **single**, **comfortable**, **well-documented** and **platform-independent** CPS for
several (mainly Chinese) DMR radios.

## Supported Radios 
Currently, there are only two supported radios

  * Baofeng/Radioddity RD-5R
  * TYT MD-UV390 / Retevis RT3S

The limited amount of supported radios is due to the fact, that I only own these two radios to test
the software with.


## Releases
 * **[Version 0.4.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.4.0)** -- Added Open GD77 support.
 * **[Version 0.3.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.3.0)** -- Bugfix release.
 * **[Version 0.2.3](https://github.com/hmatuschek/qdmr/releases/tag/v0.2.3)** -- Bugfix release.
 * **[Version 0.2.2](https://github.com/hmatuschek/qdmr/releases/tag/v0.2.2)** -- Bugfix release.
 * **[Version 0.2.1](https://github.com/hmatuschek/qdmr/releases/tag/v0.2.1)** -- First public release.


## Install
Currently there is only a binary for MacOS X and Ubuntu Linux. The MacOS X binary can be downloaded
from the [current release](https://github.com/hmatuschek/qdmr/releases/).

Under Ubuntu Linux, you may add my [PPA](https://launchpad.net/~hmatuschek/+archive/ubuntu/ppa) to
your list of Software sources with

    sudo add-apt-repository ppa:hmatuschek/ppa
    sudo apt-get update

after this, you may install the GUI application with

    sudo apt-get install qdmr

or the command-line-tool with

    sudo apt-get install dmrconf

[![qdmr](https://snapcraft.io//qdmr/badge.svg)](https://snapcraft.io/qdmr)

### Permissions
When running *qdmr* or *dmrconf* under Linux, you may need to change the permissions to access USB devices.  Create a file /etc/udev/rules.d/99-dmr.rules with the following contents:

    # TYT MD-UV380
    SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", MODE="666"
    
    # Baofeng RD-5R, TD-5R
    SUBSYSTEM=="usb", ATTRS{idVendor}=="15a2", ATTRS{idProduct}=="0073", MODE="666"


## License
qdmr - A GUI application and command-line-tool to programm DMR radios.
Copyright (C) 2019 Hannes Matuschek, DM3MAT

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
