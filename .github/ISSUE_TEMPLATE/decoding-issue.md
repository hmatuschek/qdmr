---
name: Decoding issue
about: If qdmr fails to read/decode a codeplug from a device.
title: ''
labels: bug
assignees: ''

---

First, check if qdmr/dmrconf can read the device. That is, read the binary codeplug one the command-line using 
```
 > dmrcond read --verbose DEVICE_codeplug.dfu
```
If this succeeds, qdmr/dmrconf can talk to the device just fine. To help with identifying the decoding issue, consider adding this binary codeplug to the issue. You may need to zip it first. 

Also report your qdmr version, the device firmware version and the output of
```
 > dmrconf decode --verbose --radio=YOUR_DEVICE DFU_FILE_NAME decoded.yaml
```
