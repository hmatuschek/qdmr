# Reverse engineering of Hytera communication protocol and code-plugs.

To start reverse engineer the communication protocol, at least one capture of a
code-plug write and read is necessary.

  1. Start Hytera CPS in VM
  2. Start wireshark and capture from *usbmon* device on host system
  3. Upload a small code-plug (only a few channels, contacts and a zone).
  4. Stop capture and save under recognizable name (e.g., *write_codeplug_base*) along with the code-plug with the same name (e.g., *codeplug_base*).
  5. Restart capture of usbmon on host.
  6. Read code-plug back from the radio.
  7. Stop capture and save under recognizable name (e.g., *read_codeplug_base*).

The transferred data can then be extracted using the *extract.py* script. For
now, it only unpacks USB serial packets. This should help to reverse engineer
the communication protocol.

```
 $ ./extract.py serial write_codeplug_base.pcapng | less
```

## Reading Code Plugs

Preliminary work has been done to establish how the CPS reads a code plug from
the radio. This will hopefully allow us to reverse engineer the USB protocol
that is used.

Unlike most other DMR radios, Hytera radios doesn't appear as a USB serial
device. Instead, direct communication with the USB enpoint is required. A
wireshark capture of a CPS code plug read has been obtained and is located in
`read/read.pcapng`. The capture indicates that there is a series of USB Bulk
transfers to endpoint number 04. You can then extract these packets as JSON and
then use a small script `read/splitter.js` to create two files:

 - `read/host_to_dev.txt` - Data that is transferred from the host to the device.
 - `read/dev_to_host.txt` - Data that is transferred from the device to the host.

 If we replay the `host_to_dev.txt` data back to the device, this results in the
 code plug being read from the radio. You can use the small Haskell program to
 do this with:

```sh
cd read/replay/
cabal v2-run reader -- ../host-to-dev.txt
```

## Communication protocol
We can use data from `host_to_dev.txt` to analyze the protocol. Here are some
preliminary findings/assumptions:

In order to start reading a CP from the radio, the following 4 packets are sent
to the radio:

```
7e:00:00:fe:20:10:00:00:00:0c:60:e5
7e:01:00:00:20:10:00:00:00:14:31:d3:02:03:02:01:00:00:2c:03    (A)
7e:01:00:00:20:10:00:00:00:24:02:f1:02:c5:01:11:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:5b:03
7e:01:00:00:20:10:00:00:00:14:41:c3:02:01:02:01:00:12:1c:03    (B)
```

After these packets are sent, the radio shows `Reading` on the screen. Following
this, a series of packets are now sent. These packets seem to ask for a
particular address and size of the CP memory to be sent back to the host:

```
7e:01:00:00:20:10:00:00:00:1f:53:a4:02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05:e0:03
7e:01:00:00:20:10:00:00:00:1f:4e:a9:02:c7:01:0c:00:00:00:00:01:00:00:78:05:00:00:78:05:63:03
    K      |  J  |    I   |H |G |    F      |  E  |       D         |     C     |  B  |A |S
```

The following can be observed:

  - There appears to be a communications protocol that facilitates the sending
    of 'payloads'. In the packet above, bytes K,J,I,H,G,F,E,A,S correspond to
    the comms protocol and bytes D,C and B correspond to the payload for reading
    the CP.

  - Protocol Fields:

    - `S` - This appears to be a static value denoting the end of a packet and
      always seems to have a value of `0x03`.
    - `A` - This appears to be a checksum. It is unclear whether this if of the
      payload or also includes the protocol header. Our best chance at
      understanding this is to look at packets (A) and (B) above. They both have
      a payload of only one byte `0x00` and `0x12` respectively. Maybe we can
      use this to figure out how the checksum is calculated.
    - `E` - This is the size of the payload, in bytes.
    - `F` - Unknown, but seems to be static when sending a read command. But
      this value does change with packets (A) and (B), maybe some kind of
      address to send the payload to?
    - `G` and `H` - Unknown, but `H` appears to decrement by 5 or 6 with each
      command and `G` appears to increase. Maybe some sequencing number?
    - `I` - Padding, always seems to be four bytes of `0x0`.
    - `J` - Transfer Direction? seems to be `0x2010` when host-to-radio and
      `0x1020` when radio-to-host.
    - `K` -  Packet header, always has value `0x7e010000`.


  - Read Command Fields:

      - `D` - Unknown, but seems to have a static value across all read
        commands.
      - `C` - The address to read data from.
      - `B` - The size of data to read.


