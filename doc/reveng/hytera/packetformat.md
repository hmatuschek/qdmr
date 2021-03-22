# Packet format

## Basic packet format (Layer 0)
This format appears to be a nested packet format. That is, there is a common
header and end of packet with some payload area in between. This payload area
than contains another packet format. These formats usually imply that there are
several redundant length fields.

```
    0         1         2         3         4         5         6         7 
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | Preamb  | Type    |  0x00   | Flags?  | Source  | Dest.   |  Resp. Count      |
   +---------+---------+---------+---------+---------+---------+---------+---------+
08 | TLen., 16bit, BE  | Payload, variable length                               ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
  - Preamble: Fixed to 0x7e (binary 01111110).
  - Packet type:
    - 00 -> Command
    - 01 -> Request
    - 04 -> Response/Success
  - Byte 3 appears to be reserved or part of the next byte (flags). Only observed as 0x00.
  - Flags: Possible packet flags or additional command option
    - Observed 0xfe for command packet, answered with flag 0xfd
    - 0x00 for all read/write requests
  - Source tag: 
    - 0x20 Host
    - 0x10 Device
  - Destination tag: 
    - 0x20 Host
    - 0x10 Device
  - Response counter, 16 bit big endian. 
    - Only set on responses, incrementing by 1 for each response.
    - 0x0000 on requests.
  - Total length of packet including headers etc. 16bit big endian.
  - Optional content, not present in command request and responses.
  
### Example
```
7e:01:00:00:20:10:00:00:00:1f:53:a4:02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05
```
This can then be interpreted as a read request with payload
```
53:a4:02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05:e0:03
```


## Request format (Layer 1)
This protocol is certainly nested. For now I have identified 3 layers. This is layer 1 and 
describes the content of the payload of layer 0 for requests (Layer 0, type 0x1/0x4). This 
layer itself contains of a header and payload. The payload of this layer is layer 2 and is 
described below.

### Request & Response
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | MSB CRC | Count?  | 0x02    | ReqType           | ReqLen 16b LE     |      ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ... Payload, variable length                               | CRC LSB | 0x03    |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - MSB of CRC.
 - Some weird counter, appears to increment by a certain amount from request to request.
 - Appears to be fixed to 0x02 for all read requests.
 - Request type (Device identifier, firmware version, code-plug?). This value gets 
   repeated in the response, except for bit 7 being set. That is a 0x0102 gets responded 
   with type 0x0182.
   - 0x0102 = 0b0000 0001 0000 0010: unknown, before reading/writing code-plug
   - 0x010C = 0b0000 0001 0000 1100: unknown, before reading/writing code-plug
   - 0x0302 = 0b0000 0011 0000 0010: likely device identification
   - 0xc501 = 0b0000 0101 0000 0001: likely read firmware version
   - 0x0502 = 0b0000 0101 0000 0010: during handshake before writing code-plug
   - 0xc601 = 0b1100 0110 0000 0001: last request when reading/writing code-plug, reboot?
   - 0xc701 = 0b1100 0111 0000 0001: likely read code-plug memory
   - 0xc801 = 0b1100 1000 0000 0001: likely write code-plug memory
   - 0xce01 = 0b1100 1110 0000 0001: several requests before writing to memory
 - Read-request payload length (aka layer 2). 16bit little-endian integer.
 - Payload, variable length. Contains a further layer 2 for read and write operations.
 - LSB of CRC
 - fixed to 0x03

#### Example
```
53:a4:02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05:e0:03
```
Means 
  - CRC is 0x53e0,
  - Sequence number 0xa4,
  - Fixed unknown value 0x02
  - Request type is 0xc701 -> read code-plug memory
  - Payload length is 0x000c (12)
  - Payload 00:00:00:01:00:00:00:00:00:00:78:05
  - End of packet flag 0x03



## Memory read/write request & response (Layer 2)

### Request (ReqType=0xc701/0xc801)
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x00    | 0x00    | 0x00    | 0x01    | 0x00    | 0x00    | Addr 32bit LE  ... 
   +---------+---------+---------+---------+---------+---------+---------+---------+
08  ...                | Size, 16bit, LE   | The actual data                    ...           
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - The first 6 bytes appear to be constant.
 - The address to read from or write to, 32bit little endian.
 - The amount of data to read/write, 16bit little endian.
 - The actual data to write. Not present on read requests.

### Response (ReqType=0xc781/0xc881)
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
 - The address read from or write to, 32bit little endian.
 - The amount of data read, 16bit little endian.
 - The actual data, only present for read requests.


## Identify radio request
The very first request to the radio (after being put into program mode) appears to be a 
request for identification from the radio. It returns with 16bit unicode strings.
The request type field in layer 1 is 0x0302, likely requesting the radio identifier.

### Request (ReqType=0x0302) 
```
   +---------+
00 | 0x00    |
   +---------+
```
   - Payload a single byte 0x00.

### Response (ReqType=0x0382) 
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | Data ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - Contains 0x48 bytes. Containing 16bit unicode strings. The first and last bytes 
 of the response are 0x00.


## Firmware version request
The second read request appears to request the firmware version and other information from the
radio. Also this request does not follow the general "read-code-plug" read request format above.

### Request (ReqType=0xC501) 
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

### Response (ReqType=0xC581)
The response is then pretty simple
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | Data                                                                      ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```


## Another weird read-request 
This request is send to the radio before writing the firmware. This is certainly a read 
request including address and length fields. The memory being read is unknown, however.

### Request (ReqType=0xCE01) 
```
   +---------+---------+---------+---------+---------+---------+
00 | Address? 32bit little endian          | Length, 16bit BE  |
   +---------+---------+---------+---------+---------+---------+
```

### Response (ReqType=0xCE81) 
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x00    | Address? 32bit little endian          | Length, 16bit BE  | ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ... Payload, variable length                                                   |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```

