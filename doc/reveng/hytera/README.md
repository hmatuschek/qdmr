# Reverse engineering of Hytera communication protocol and code-plugs.

To start reverse engineer the communication protocol, at least one capture of a code-plug write and read is necessary.

  1. Start Hytera CPS in VM
  2. Start wireshark and capture from *usbmon* device on host system
  3. Upload a small code-plug (only a few channels, contacts and a zone).
  4. Stop capture and save under recognizable name (e.g., *write_codeplug_base*) along with the code-plug with the same name (e.g., *codeplug_base*).
  5. Restart capture of usbmon on host.
  6. Read code-plug back from the radio.
  7. Stop capture and save under recognizable name (e.g., *read_codeplug_base*).

The transferred data can then be extracted using the *extract.py* script. For now, it only unpacks 
USB serial packets. This should help to reverse engineer the communication protocol. 

```
 $ ./extract.py serial write_codeplug_base.pcapng | less
```

## Communication protocol
Let see...
