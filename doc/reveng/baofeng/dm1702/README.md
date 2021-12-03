# Baofeng DM1701 / Retevis RT72 Protocol and Codeplug Reverse Engineering

This directory contains the tools and documentation of the reverse engineering of the Baofeng 
DM1701 communication protocol and binary codeplug.

The device interface is a simple Serial-Over-USB interface using the STM USB-COM pid/vid 
`0483:5780`. To this end, the radio can be emulated by implementing the protocol. This eases 
the reverse engineering of the codeplug a lot. Fortunately, the CPS file format contains the 
memory blocks that are written to the device. Although these blocks are not in the same order,
it enables to reverse engineer the codeplug elements (i.e., channels, zones, etc.) using the 
CPS file format. The memory mapping into the device, however, must be inferred using wireshark
or a device emulation.

