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
 - Read-request payload length (aka layer 2). 16bit little-endian integer.
 - Payload, variable length. Contains a further layer 2 for read and write operations.
 - LSB of CRC
 - fixed to 0x03

### Request type field
 The request type field appears to be a 16bit wide bit field encoding several informations about the request and expected response.
```
 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 | Target        | Operation     | R | 0 | 0 | 0 | Type          |
 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
```
 - The first 4 bit appear to specify the target of the operation, seen 0x0 and 0xC. 0xC is used 
   when reading and writing the code-plug. 0x0 is used when identifying the radio. Maybe 0xC means
   *flash* and 0x0 *rom*. Interestingly, 0xC is used when reading the firmware version.
 - The second 4bit field contains some sort of an op-code. Seen 0x1,0x3,0x5,0x6,0x7,0x8 and 0xe.

ReqType | Meaning
--------|------------------------------------------------------------------------------------------
 0x0102 | Unknown, before reading/writing code-plug, fixed length response 0x48 bytes.
 0x010C | Unknown, before reading/writing code-plug.
 0x0302 | Likely get device identification, fixed length response 0x48 bytes.
 0xc501 | Likely read firmware version
 0x0502 | During handshake before writing code-plug, fixed length response 0x28 bytes.
 0xc601 | Last request when reading/writing code-plug, flush or reboot?
 0xc701 | Read code-plug memory
 0xc801 | Write code-plug memory
 0xce01 | Several requests before writing to memory

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
00 | Code    | 16 x 0                                                           ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
18  ...      |
   +---------+
```
- Unknown code byte. Seen 0x00 in code-plug read and 0x02 in code-plug write. Code does not affect
  response. 

### Response (ReqType=0xC581)
The response is then pretty simple but appears to have a length field. The content consists of
several ASCII strings and some few unknown data fields.
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x00    | Code    | Length, 16bit LE  | Content                            ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
- The unknown code of the request is repeated in the response.
- The response payload length is encoded as 16bit little-endian integer.


## Unknown request 0x0102
This request is send directly before reading the code plug from the device. 

### Request (ReqType=0x0102) 
```
   +---------+
00 | Unknown |
   +---------+
```
- Some magic command byte.
  - 0x12 - Seen in code-plug download, 0x48b response mostly 0x00.
  - 0x09 - Seen in code-plug upload, 0x48b response mostly 0x00. A few single uni-code 
    chars.

### Response (ReqType=0x0182) 
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x48 bytes unknown data, mostly 0x00                                       ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
40  ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```

## Unknown memory read-request 
This request is send to the radio before writing the firmware. This is certainly a read 
request of some kind, including address and length fields. The memory being read is 
unknown.

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

## Unknown request 0x0502
