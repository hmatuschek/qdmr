# How to reverse engineer the AnyTone code-plugs
Unfortunately, AnyTone CPSs store the code-plug in a completely different format on the disc 
compared to the content being written to the device. This requires to observe the communication
between the host running the original CPS and the device. Moreover, a complete code-plug must 
be programmed to the device for every option in the CPS, to be able to reverse engineer the 
format. 

So to reverse engineer the code-plug format on the device, you first need to get the original
CPS running in a virtual machine on a Linux host (I use VirtualBox for that, but any other will
do too). Then the *usbmon* kernel module must be loaded to be able to grab all traffic between
the host and the radio. So call
```
 $ sudo modprobe usbmon
```

Then fire up wireshark 
```
 $ sudo wireshark
```
and start monitoring on the *usbmon* device.

  * Then start to upload a base code-plug to the device. Wireshark will capture everything that 
    gets sent to the device. 
  * Once the code-plug is written to the device, stop capturing in wireshark and  save the data 
    under a recognizable filename, e.g. *base.pcapng*. 
  * Change a single thing within the CPS, e.g., enable GPS. Remember, we are doing a differential 
    analysis, so change only one setting at a time.
  * Restart capturing in wireshark and upload the modified code-plug to the device.
  * Once the code-plug is written, stop capturing and save the data in another file, e.g. 
    *gps_enabled.pcapng*.
  * Extract the data written to the device using the *extract.py* script with 
    `python3 extract.py PCAPNG_FILE > HEX_FILE` for both captured code-plugs.
  * Using *diff* you can now compare the two hex files. As only one feature of the code-plug was
    changed, the difference between the two hex files should be tiny. In fact if only a single 
    setting is changed, frequently only a single bit/byte will change in the code-plug. 

Repeat the steps above over and over again to find the memory location of each option, setting 
and feature of the original CPS. Over time, you will be able to reverse engineer the entire 
code-plug. 

But keep in mind, that you will not be able to reverse engineer every single bit of the code-plug.
At the end, there will be some reserved, fixed or hidden options left.

## An example
In this directory, you will find two examples. The captured and extracted base code-plug 
(*capture_base.pcapng* and *d878uv_base.hex*) as well as a captured and extracted 
code-plug, where only a single setting was changed (*capture_set_gps_on.pcapng* and 
*d878uv_set_gps_on.hex*). In this example, the GPS was enabled in the CPS. 

The *diff* of the two hex files reveals
```
3029c3029
< 02500020 : 01 00 00 00 00 00 04 01 00 01 01 00 00 01 02 01 | ................
---
> 02500020 : 01 00 00 00 00 00 04 01 01 01 01 00 00 01 02 01 | ................
```
This tells me, that at the address 0x02500020 + 8, a single byte changed from 00 to 01. The entire
remaining code-plug (about 100kB) stayed the same. Comparing the address 0x02500028 with the 
[coarse memory layout](https://dm3mat.darc.de/qdmr/libdmrconf/class_d878_u_v_codeplug.html) 
of the code-plug tells me, that byte 40 (hex 0x28) of the general settings section of the 
code-plug enables or disables the GPS.