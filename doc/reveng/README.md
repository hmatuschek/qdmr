# How to reverse engineer code-plugs?

This question is certainly not easy to answer. It heavily depends on the particular device. In 
general, reverse engineering the code-plug requires two steps. First, the communication protocol
between the host and the device must be known. This step can be skipped if the protocol is already
known (e.g., for Radioddity, Baofeng, Tytera, Retevis or AnyTone devices) or when the protocol is 
actually an industry standard (e.g., DFU). 

Once you know how the host communicates with the device, the actual code-plug can be tackled: The 
general approach is a so-called differential analysis. That is, using the original CPS, you start 
with a compact code-plug that covers all features for the radio. Then you change only ONE thing 
using the original CPS and compare the two results. 

Sometimes, the manufacturer is nice to you and the binary code-plug saved by the original CPS is 
basically a one-to-one image of the memory content written to the device. This is true for the 
Radioddity GD-77 code-plugs and to some lesser extend for the RD-5R, TyT MD-UV390 code-plugs. 

Sometimes, however, the manufacturer CPS saves something that has basically nothing to do with 
what is written to the device. For example the AnyTone D-878UV. In these cases, the content 
being written to the divice must be captured. This can be done unsing wireshark on a linux 
host. That is, fire up a virtual machine running the original CPS. Start wireshark on the host 
system and monitor the USB interface using *usbmon*. Then, write a small script that extracts 
the actual data written to the device from the capture files. These can then be compared to 
perform the differential analysis. An example script including captured traffic and extracted 
data can be found in the anytone directory.