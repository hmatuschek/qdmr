# Reverse engineering of the Alinco DJ-MD5X 

The radio is very similar to the AnyTone AT-D878UV. Uses the same protocol and likely a similar 
codeplug. 

The `dj-md5x_emulator.py` script implements a simple emulator of the radio to be used with a 
*virtual com port* (e.g., com0com). This can then be used for the differential codeplug analysis. A 
simple base codeplug is generated and then written to the emulator. In a next step, a single 
setting is changed and the resulting codeplug is written to the emulator too. Then, the differences 
can be inspected.