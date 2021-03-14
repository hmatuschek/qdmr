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
    gets send to the device. 
  * Once the code-plug is written to the device, stop capturing in wireshark and  save the data 
    under a recognizable filename, e.g. *base.pcapng*. 
  * Change a single thing within the CPS, e.g., enable GPS. Remember, we are doing a differential 
    analysis, so change only one setting at a time.
  * Restart capturing in wireshark and upload the modified code-plug to the device.
  * Once the code-plug is written, stop capturing and save the data in another file, e.g. 
    *gps_enabled.pcapng*.
  * Extract the data written to the device using the *extract.py* script with 
    `python3 extract.py PCAPNG_FILE > HEX_FILE` for both captured code-plugs.
  * Using *diff* you can now compare the two hex files. 

Repeat the steps above over and over again to find the memory location of each option, setting 
and feature of the original CPS. Over time, you will be able to reverse engineer the entire 
code-plug. 

But keep in mind, that you will not be able to reverse engineer every single bit of the code-plug.
At the end, there will be some reserved, fixed or hidden options left.
