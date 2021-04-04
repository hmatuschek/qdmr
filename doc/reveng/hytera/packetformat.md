# Packet format

## Basic packet format (Layer 0)
This format appears to be a nested packet format. That is, there is a common
header and end of packet with some payload area in between. This payload area
than contains another packet format. These formats usually imply that there are
several redundant length fields.

```
    0         1         2         3         4         5         6         7 
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x7e    | Type    |  0x00   | Flags?  | Source  | Dest.   |  Resp. Count      |
   +---------+---------+---------+---------+---------+---------+---------+---------+
08 | TLen., 16bit, BE  | CRC, LE           | Payload, variable length           ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
  - Preamble: Fixed to 0x7e (binary 01111110).
  - Packet type:
    - 00 -> Command (no payload)
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
  - CRC computed over complete packet, LE
  - Optional content, not present in command request and responses.
  
### CRC
The CRC is computed over the complete packet. That is, every 16bits of the packet is 
interpreted as a little-endian value (except for the CRC field itself) and summed up. 
For packets of odd length, the last byte is added as a 8bit value. The sum
is then turned back into 16bit value by adding the upper 16bit to the lower one. 
That is,
```c
 sum = (sum >> 16) + (sum&0xffff);
```
As the last sum may cause the value to exceed 16bit again, the operation is repeated.
The *negated* CRC is then stored as a little-end value.

### Example
```
7e:01:00:00:20:10:00:00:00:1f:53:a4:02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05:e0:03
```
This can then be interpreted as a read request with payload
```
02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05:e0:03
```
The CRC is then computed as the sum of all 16bit interpreted as little-endian values 
except for the crc itself. That is (excluding 0x0000)
```cpp
uint32_t crc = 0x017e + 0x1020 + 0x1f00 + 0xc702 + 0x0c01 + 0x0001 + 0x7800 + 0xe005 + 0x03; // 0x25baa
// turn back to 16bit value
crc = (crc >> 16) + (crc & 0xffff); // = 0x5bac
// the last sum may result in a crc > 0xffff, so repeat once more
crc = (crc >> 16) + (crc & 0xffff);
// negate 
crc ^= 0xffff; // = 0xa453
// in LE 0x53a4
```


## Request format (Layer 1)
This protocol is certainly nested. For now I have identified 3 layers. This is layer 1 and 
describes the content of the payload of layer 0 for requests (Layer 0, type 0x1/0x4). This 
layer itself contains of a header and payload. The payload of this layer is layer 2 and is 
described below.

### Request & Response
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x02    | ReqType LE        | ReqLen 16b LE     |                          ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ... Payload, variable length                               | CRC     | 0x03    |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - Appears to be fixed to 0x02 for all requests.
 - Request type (Device identifier, firmware version, code-plug?). This value gets 
   repeated in the response, except for bit 8 being set. See below.
 - Request payload length (aka layer 2). 16bit little-endian integer.
 - Request payload, variable length. Contains a further layer 2 data for various operations.
 - CRC computed over header and payload
 - Fixed to 0x03

#### Example
```
02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05:e0:03
```
Here the request type is 0x01c7, request length is 0x000c, CRC is 0xe0 and the payload 
```
00:00:00:01:00:00:00:00:00:00:78:05
```
The CRC can be computed by first adding all bytes of the request type, length and payload
(ignoring all 0x00), negate the sum finally add 0x33 
```cpp
uint8_t crc = 0xc7+0x01+0x0c+0x01+0x78+0x05; // = 0x52
// negate
crc ^= 0xff; // = 0xad
// add 0x33
crc += 0x33; // = 0xe0
```


### Request type field
 The request type field appears to be a 16bit wide bit field encoding several informations about the request and expected response.
```
 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 | R | 0 | 0 | 0 | Type          | Target        | Operation     | 
 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
```
 - **Response** flag, set to 0 on requests and 1 on responses.
 - Second field specifies the **type** or class of operation or the expected response format. Seen 
   0x1, 0x2, 0xc. When the type is 0x1, the response contains a fixed amount of payload and there 
   is no payload length field. Consequently, I would interpret this type as *get* or *put*. As an 
   example, the *identify radio request* returns 0x48 bytes containing a single unicode string. If this 
   field is 0x01, however, the response (and also the request) has a length field. This means the 
   response will have a variable length payload. This is certainly true for read/write code plug
   requests but also for the *read firmware version* request. The 0xC type request is still fully 
   unknown.
 - The next 4 bits appear to specify the **target** of the operation, seen 0x0 and 0xC. 0xC is used 
   when reading and writing the code-plug. 0x0 is used when identifying the radio. Maybe 0xC means
   *flash* and 0x0 *rom*. Interestingly, 0xC is used when reading the firmware version.
 - The last 4bit field contains some sort of an **op-code**. Seen 0x1,0x3,0x5,0x6,0x7,0x8 and 0xe.
 

ReqType | Meaning
--------|------------------------------------------------------------------------------------------
 0x01c5 | Likely read firmware version, variable length response.
 0x01c6 | Last request when reading/writing code-plug, flush or reboot?
 0x01c7 | Read code-plug memory
 0x01c8 | Write code-plug memory
 0x01ce | Several requests before writing to memory
 0x0C01 | Unknown, before reading/writing code-plug, fixed length response.
 0x0201 | Unknown, before reading/writing code-plug, fixed length response 0x48 bytes.
 0x0203 | Likely get device identification, fixed length response 0x48 bytes, unicode string.
 0x0205 | Likely get device identification, fixed length response 0x48 bytes, ASCII string.

#### Example
```
02:c7:01:0c:00:00:00:00:01:00:00:00:00:00:00:78:05:e0:03
```
Means 
  - Fixed value 0x02
  - Request type is 0x01c7 -> read code-plug memory
  - Payload length is 0x000c (12)
  - Payload 00:00:00:01:00:00:00:00:00:00:78:05
  - CRC is 0xe0
  - End of packet flag 0x03


## Code-plug memory read (Layer 2)

### Request (ReqType=0x01c7)
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

### Response (ReqType=0x81c7)
As for all 0x2-type request responses, the response header is just a 0x00 (means success?!?) 
followed by the request header.
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x00    | 0x00    | 0x00    | 0x00    | 0x01    | 0x00    | 0x00    |      ... 
   +---------+---------+---------+---------+---------+---------+---------+---------+
08  ... Addr 32bit LE            | Size, 16bit, LE   | The actual data ...           
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - The first 1+6 bytes appear to be constant. 6 bytes identical to request header.
 - The address read from or write to, 32bit little endian, identical to request header.
 - The amount of data read, 16bit little endian, identical to request header.
 - The actual data read.


## Code-plug memory write (Layer 2)

### Request (ReqType=0x01c8)
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
 - The address to write to, 32bit little endian.
 - The amount of data to write, 16bit little endian.
 - The actual data to write. 

### Response (ReqType=0x81c8)
As for all 0x2-type request responses, the response header is just a 0x00 (means success?!?) 
followed by the request header.
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x00    | 0x00    | 0x00    | 0x00    | 0x01    | 0x00    | 0x00    |      ... 
   +---------+---------+---------+---------+---------+---------+---------+---------+
08  ... Addr 32bit LE            | Size, 16bit, LE   | 
   +---------+---------+---------+---------+---------+
```
 - The first 1+6 bytes appears to be constant. 6 bytes identical to request header.
 - The address written to, 32bit little endian, identical to request header.
 - The amount of data written, 16bit little endian, identical to request header.
 

## Read firmware version request (Layer 2)
The request does not contain any length information in the request but gets a response 
with a length field (type 0x1 request). This implies that the response to this request 
may return data of different length.

### Request (ReqType=0x01C5) 
The request is much longer that all other read requests, but contains only 0x00.

The *what* field in layer 1 is set to 0x01c5. The payload is simply
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


## Unknown read request 0x01CE
This request is send to the radio before writing the firmware. This is certainly a read 
request of some kind, including address and length fields. The memory being read is 
unknown.

### Request (ReqType=0x01CE) 
```
   +---------+---------+---------+---------+---------+---------+
00 | A 16bit LE        | B 16bit LE        | Length, 16bit BE  |
   +---------+---------+---------+---------+---------+---------+
```

### Response (ReqType=0x81CE) 
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | 0x00    | A 16bit LE        | B 16bit LE        | Length, 16bit BE  |      ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ... Payload, variable length                                                   |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```



## Get String (Type 0x2) requests (Layer 2)
Whenever the request *type* field is set to 0x2, there is a fixed size response. The request 
payload the only contains a single byte probably specifying what to get. The response to
these requests appear to contain a 8byte header consisting of almost all 0x00 but containing 
the request byte at position 4. 

### Request (ReqType=0x0201,0x0203,0x0205) 
```
   +---------+
00 | What    |
   +---------+
```
- Some magic command byte seems to specify what to get.
  - 0x00 - Seen in *identify radio* request (0x0302, 0x0502)
  - 0x12 - Seen in code-plug download, 0x48b response mostly 0x00.
  - 0x09 - Seen in code-plug upload, 0x48b response mostly 0x00. A single uni-code 
    string.

### Response (ReqType=0x8201,0x8203,0x8205) 
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | UKN1    | 0x00    | 0x00    | 0x00    | What    | 0x00    | 0x00    | 0x00    |
   +---------+---------+---------+---------+---------+---------+---------+---------+
08 | Response data                                                              ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```

## Unknown get data (Type 0xC) request (Layer 2)
# Request (ReqType=0x0C01)
```
   +---------+---------+---------+---------+
00 | What, 32bit little-endian?            |
   +---------+---------+---------+---------+
```
 - Appears to be an address or code what to get.
   - 0x00000000 in code-plug write, response 0x14 bytes of 0x00.
   - 0x00000001 in code-plug write, response 0x10 bytes of 0x00.
 
# Response (ReqType=0x8C01)
```
   +---------+---------+---------+---------+---------+---------+---------+---------+
00 | Data                                                                       ...
   +---------+---------+---------+---------+---------+---------+---------+---------+
    ...                                                                            |
   +---------+---------+---------+---------+---------+---------+---------+---------+
```
 - No length field. Response length depends on the *what* field of the request. For 0x00000000, 
   0x14 bytes (160 bits) and for 0x00000001 0x10 bytes (128 bits) are returned. Smells like 
   crypto stuff, maybe related to the suspicious HMAC fields in the code-plug?!?