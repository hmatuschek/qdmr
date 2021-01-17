# QDMR, a GUI application and command line tool to program DMR radios

![qdmr channel editor](https://raw.githubusercontent.com/hmatuschek/qdmr/master/doc/fig/qdmr-channels.png "The qdmr CPS software.")

*qdmr* is a graphical user interface (GUI) application that allows to program several types of DMR radios.
To this end, it aims at being a more universal codeplug programming software (CPS) compared to the
device and even revision specific CPSs provided by the manufacturers. The goal of this project is to
provide a **single**, **comfortable**, [**well-documented**](https://dm3mat.darc.de/qdmr/manual/) 
and **platform-independent** CPS for several types of (mainly Chinese) DMR radios.

## Supported Radios 
Currently, there are only few radios that are supported

  * Open GD77 firmware (since version 0.4.0)
  * Radioddity GD77 (untested)
  * Baofeng/Radioddity RD-5R & RD-5R+
  * TYT MD-UV390 / Retevis RT3S
  * Anytone AT-D878UV (since version 0.5.0)

A more [detailed list](https://dm3mat.darc.de/qdmr/#dev) is also available. 
The limited amount of supported radios is due to the fact that I only
own these radios to test the software with.


## Releases
 * **[Version 0.5.1](https://github.com/hmatuschek/qdmr/releases/tag/v0.5.1)** -- Bugfix release.
 * **[Version 0.5.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.5.0)** -- Added support for Anytone AT-D878UV.
 * **[Version 0.4.7](https://github.com/hmatuschek/qdmr/releases/tag/v0.4.7)** -- Feature release.
 * **[Version 0.4.6](https://github.com/hmatuschek/qdmr/releases/tag/v0.4.6)** -- Bugfix release.
 * **[Version 0.4.5](https://github.com/hmatuschek/qdmr/releases/tag/v0.4.5)** -- Bugfix release.
 * **[Version 0.4.3](https://github.com/hmatuschek/qdmr/releases/tag/v0.4.3)** -- Bugfix release.
 * **[Version 0.4.1](https://github.com/hmatuschek/qdmr/releases/tag/v0.4.1)** -- Bugfix release.
 * **[Version 0.4.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.4.0)** -- Added Open GD77 support.
 * **[Version 0.3.0](https://github.com/hmatuschek/qdmr/releases/tag/v0.3.0)** -- Bugfix release.
 * **[Version 0.2.3](https://github.com/hmatuschek/qdmr/releases/tag/v0.2.3)** -- Bugfix release.
 * **[Version 0.2.2](https://github.com/hmatuschek/qdmr/releases/tag/v0.2.2)** -- Bugfix release.
 * **[Version 0.2.1](https://github.com/hmatuschek/qdmr/releases/tag/v0.2.1)** -- First public release.


## Install
There currently are only binaries for MacOS X and Ubuntu Linux. The MacOS X binary can be downloaded
from the [current release](https://github.com/hmatuschek/qdmr/releases/).

Under Ubuntu Linux (bionic and later), you may add my
[PPA](https://launchpad.net/~hmatuschek/+archive/ubuntu/ppa) to your list of software repositories with

    sudo add-apt-repository ppa:hmatuschek/ppa
    sudo apt-get update

after this, you may install the GUI application with

    sudo apt-get install qdmr

or the command line tool with

    sudo apt-get install dmrconf

[![qdmr](https://snapcraft.io//qdmr/badge.svg)](https://snapcraft.io/qdmr)

### Permissions
When running *qdmr* or *dmrconf* under Linux, you may need to change the permissions for accessing USB devices.  
Create a file /etc/udev/rules.d/99-dmr.rules with the following content:

    # TYT MD-UV380
    SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="df11", MODE="666"
    
    # Baofeng RD-5R, TD-5R
    SUBSYSTEM=="usb", ATTRS{idVendor}=="15a2", ATTRS{idProduct}=="0073", MODE="666"

Finally execute `sudo udevadm control --reload-rules` to activate these new rules.

### Snap package and USB interface
If you use the snap image of *qdmr*, you must (for now) grant access to the raw USB interface for
*qdmr*. Otherwise, *qdmr* will not find any USB devices. That is, execute

    snap connect qdmr:raw-usb

in a terminal. I will request an auto-connect for this snap image, so that this step will not be necessary in
the future. For the time being, however, you have to do that manually.

### Snap package and serial interface
Please note that it is very hard to gain access to a serial port for a snap image, hence it is 
(for now) *impossible to access OpenGD77 and AnyTone devices using the snap 
package of qdmr*. Please resort to the DEB package from my PPA. Any other direct USB (non-serial) 
interface should work (i.e., RD5-R, GD77 and Retevis/TyT).

## Building from sources
To build the lastest development version of QDMR, you may build it and the command line tool `dmrconf` from 
sources.

First install git, the compiler and all needed dependencies. These package names may differ between 
Linux distributions.  
#### Ubuntu
Under Ubuntu run
```
$ sudo apt-get install build-essential git cmake 
$ sudo apt-get install libusb-1.0-0-dev qtbase5-dev qttools5-dev qttools5-dev-tools qtpositioning5-dev libqt5serialport5-dev
```
#### Arch Linux
Under Arch Linux run
```
$ sudo pacman -S git base-devel cmake libusb qt5-tools qt5-serialport qt5-location
```

Then clone the repository with
```
$ git clone https://github.com/hmatuschek/qdmr.git
```
If you have already cloned that repository, you can update the sources to the latest version with
```
$ git pull
```
from within the `qdmr` directory.
 
This should create a new directory named `qdmr`. Enter this directory, create and enter a build directory and configure the build with
```
$ cd qdmr
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_INSTALL_PREFIX=/usr/
```
The last call configures CMAKE to install everything under the system path `/usr/`. The previous steps has to be done only once.

Finally, to build and install the binaries, run
```
$ make
$ sudo make install
```
after every update.

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
