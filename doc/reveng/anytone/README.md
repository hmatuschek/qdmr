# Reverse engineering of AnyTone codeplugs
The transfer protocol of the AnyTone devices is based on the USB-Serial protocol (ACM-CDC) and is
well known. To this end, it is possible to *emulate* the AnyTone devices using a simple python 
script. You will find a matching script in every subdirectory.

## Emulation under windows
These scripts expect a serial loopback device to be present. Under windows, such a device can be
created using a *virtual null-modem cable*. For example [com0com](http://com0com.sourceforge.net/).
This will create a pair of COM ports (e.g., COM5 and COM6). One will be used by the emulation and 
the other will be used by the manufacturer CPS. 

First, start the emulation
```
python3 at_dxxx_emulator.py COM5
```
Here, the COM5 port will be used by the emulation. Then, start the CPS and select `COM6` as the 
port to be used.

## Emulation under Linux
It might be possible to emulate the device under Linux and using Wine to run the CPS. First, a pair
of *serial ports* (pts) must be created. This can be done using `socat` by calling 
```
socat -d -d pty,raw,echo=0,b4000000 pty,raw,echo=0,b4000000
```
This call creates a pair of pseudo-terminals, for example `/dev/pts/3` and `/dev/pts/4`. The 
emulation may then use one of them by calling
```
python3 at_dxxx_emulator.py /dev/pts/3
```

Now, the second must be associated with a COM port in the Wine emulation. This is done using the
registry editor. Run `wine regedit` and change 
```
HKEY_LOCAL_MACHINE/Software/Wine/Port/COM1  /dev/pts/4
```
You may need to create that key first. This will bind the Wine emulated COM port to the second 
pseudo-terminal. 

Now, start the manufacturer CPS under Wine and select COM1.

## Differential Codeplug Analysis
Create a small codeplug containing at least two channels, two contacts, a group list and a zone. 
Save that codeplug and write it to the device. The emulator will save that codeplug as a hex dump 
in a file called `codeplug_0000.hex`. Any further codeplug write will be saved as a separate 
hex dump with an incremented number. 

The differential analysis of the codeplug allows to figure out how and where a specific setting is
encoded within the binary codeplug. To do that, make a single change in the codeplug. E.g. changing
the first radio ID. Then write that changed codeplug to the emulation. The hexdump of the codeplug
is then save as `codeplug_0001.hex`. 

You can then use a diff tool to inspect the changes. E.g.
```
$ diff codeplug_0000.hex codeplug_0001.hex
1530c1530
< 02580000 : 01 23 45 67 00 52 61 64 69 6f 20 31 00 00 00 00 | .#Eg.Radio 1....
---
> 02580000 : 00 12 34 56 00 52 61 64 69 6f 20 31 00 00 00 00 | ..4V.Radio 1....
```
Here the first radio ID 1234567 was changed to 123456. The only differences between the codeplugs
are at the address 2580000. Apparently, the first radio ID is stored there together with the radio
name. The first 4 bytes at this address have changed from `01 23 45 67` to `00 12 34 56`. As the 
numbers are readable in hex, they are stored in BCD and the most-significant number is stored 
first. So the radio ID is stored in an 8-digit BCD in big-endian. 

This step is then repeated for all settings within the CPS to figure out the entire codeplug.