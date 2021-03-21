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


## Basic packet format (Layer 0)
This format appears to be a nested packet format. That is, there is a common
header and end of packet with some payload area in between. This payload area
than contains another packet format. These formats usually imply that there are
several redundant length fields.

```
+---------+---------+---------+---------+---------+---------+---------+---------+
| Preamb  | Cmd/Res |  0x00?  | Flags?  | Source  | Dest.   |  Resp. Count      |
+---------+---------+---------+---------+---------+---------+---------+---------+
| TLen., 16bit, BE  | Payload layer 1, optional, variable size ...
+---------+---------+---------+---------+---------+---------+---------+---------+
 ...                                                        | LSB CRC | PktEnd  |
+---------+---------+---------+---------+---------+---------+---------+---------+
```
  - Preamble: Fixed to 0x7e (binary 01111110).
  - Command/Type:
    - 00 -> Command request
    - 01 -> Read request
    - 04 -> Read/Command response on success
  - Byte 3 appears to be reserved or part of the next byte (flags). Only observed as 0x00.
  - Flags: Possible command/packet flags. Observed as
  - Source tag: 0x20 Host, 0x10 Device
  - Destination tag: 0x20 Host, 0x10 Device
  - Response counter, 16 bit big endian. Only present on responses and 0x0000 on requests.
  - Total length of packet, including headers etc.
  - Optional content, not present in command request and responses.
  - least significant byte of CRC?!?. MSB stored in layer 1?!?
  - End of packet flag (observed 0xe5, 0xd2, 0x03).
    - Command request to enter read/program mode 0xe5
    - Command response entered read/program mode 0xd2
    - Read command and response 0x03.

### Example
```
7e:01:00:00:20:10:00:00:00:1f:53:a4:02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05:e0:03
```
This can then be interpreted as a read request with payload
```
53:a4:02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05
```


## Read request & response (Layer 1)
This protocol is certainly nested. For now I have identified 3 layers. This is layer 1 and 
describes the content of the payload of layer 0. This layer itself contains of a header and 
payload. The payload of this layer is layer 2 and is described below.

### Request 
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | MSB CRC | Count?  | 0x02    | What    | ???     | ReqLen 16b LE     |      ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ... Payload of layer 2                                                         |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - Most significant byte of CRC? Breaks structure as CRC is element of layer 0. **This is
   really a problem, as for the CMD requests, there is no payload but a CRC, thus the MSB 
   would be missing.**
 - Some weird counter, appears to increment by a certain amount from request to request.
 - Appears to be fixed to 0x02 for all read requests.
 - Unknown, but appears to specify what should be read (Device identifier, firmware version,
   code-plug?). This value gets repeated in the response
   - 0x03: likely device identification
   - 0xc5: unknown, likely firmware version
   - 0x01: unknown
   - 0xc7: this means likely "read code-plug"
 - Unknown but relates to value in the response at the same place.
 - Read-request payload length (aka layer 2). 16bit little-endian integer. This is 
   the length of the rest of the request.
 - Payload read request layer 2.

### Response
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | MSB CRC | Count?  | 0x02    | What    | ???     | ResLen 16b LE     |      ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ... Payload of layer 2                                                         |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - Most significant byte of CRC? Breaks structure as CRC is element of layer 0.
 - Some weird counter, appears to increment by a certain amount from request to request.
 - Appears to be fixed to 0x02 for all read responses.
 - Unknown, but appears to specify what should be read (Device identifier, firmware version,
   code-plug?). This value is copied from the request.
 - Unknown byte, however if 0x01 was send in the request at that position, the response contains
   0x81. And when 0x02 was send in the request, the response contains 0x82. Maybe bit flags?
 - Read-response payload length, a 16bit little endian integer. This is the length of the 
   response.
 - Payload request-response layer 2


## Memory read request & response (Layer 2)

### Request 
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x00    | 0x00    | 0x00    | 0x01    | 0x00    | 0x00    | Addr 32bit LE  ... 
   +---------+---------+---------+---------+---------+---------+---------+---------+
08  ...                | Size, 16bit, LE   | 
   +---------+---------+---------+---------+
```
 - The first 6 bytes appear to be constant.
 - The address to read from, 32bit little endian.
 - The amount of data to read, 16bit little endian.

### Response
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x00    | 0x00    | 0x00    | 0x00    | 0x01    | 0x00    | 0x00    |      ... 
   +---------+---------+---------+---------+---------+---------+---------+---------+
08  ... Addr 32bit LE            | Size, 16bit, LE   | The actual data ...           
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - The first 7 bytes appear to be constant.
 - The address read from, 32bit little endian.
 - The amount of data read, 16bit little endian.
 - The actual data, size specified in previous field.

## Identify radio read-request (Layer 2)
The very first read request to the radio appears to be a request for identification from the radio.
It returns with 16bit unicode strings. Containing strings like
```
X1p05-000G0000-MB0000-U1-0-B
```
So I guess you read the code-plug from your X1p?

### Request
This request however, does not follow the *read-code-plug* requests and responses above (layer 2). 

The *what* field in layer 1 is 0x03, likely requesting the radio identifier. The unknown field is
0x02. The layer 2 content is then pretty simple:
```
   +---------+
00 | 0x00    |
   +---------+
```
   - Payload a single byte 0x00.

### Response
The response also does not follow the typical format of a memory-read response above. 

The response content, however is quiet simple:
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | Data ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - Contains 0x48 bytes. Containing 16bit unicode strings.

## Special read request/response: Firmware version
The second read request appears to request the firmware version and other information from the
radio. Also this request does not follow the general "read-code-plug" read request format above.

### Request
The request is much longer that all other read requests, but contains only 0x00.

The *what* field in layer 1 is set to 0xc5, the unknown field is set to 0x01. The payload is simply
17 times 0x00.

```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 17 x 0                                                                     ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
18  ...      |
   +---------+
```

### Response
The response is then pretty simple
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | Data                                                                      ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
