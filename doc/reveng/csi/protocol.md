# Transfer protocol for CSi (Connected Systems Inc.) devices.

## Base protocol
  - The base protocol is build upon SCSI through USB Mass Storage. Thus the device appears as a mountable USB flash drive. I hope, that I will able to send direct SCSI messages to these devices without root.
  - There are some standard SCSI messages that appear to setup and probe the device. 
  - The actual codeplug transfer then happens with invalid SCSI commands (OpCode: 0xff). 

## Custom SCSI message (SBC OpCode 0xff)
There are only two custom SCSI messages used 

```
 > ff 2a 00 00 00 00 00 00 00 00 00 00 00 00 47 50
```
Sends a command to the device. The actual command is send as a separate SCSI request-payload packet.

```
 > ff 28 00 00 00 00 00 00 00 00 00 00 00 00 47 50
```
Sends a response-query to the device. The actual response is then send by the device as a separate SCSI response-payload packet.


## Request and Response format
This is the format, requests and responses are transmitted as SCSI Request/Response Payloads.

```  
 +---+---+---+---+---+---+...+---+---+---+---+
 |A1h|FLG|CMD|Len BE | Payload   | CRC   |A4h|
 +---+---+---+---+---+---+...+---+---+---+---+
```
#### Start/end of message bytes
Each message starts with a A1h byte and ends with a A4h byte.

#### Flags
The first field contains some sort of flags. It is 00h for requests and 80h for responses. 

| Code | Description |
| ---- | ----------- |
| 00h  | Request |
| 80h  | Response |

#### Command code 
The second field contains the op-code of the request. The response copies that op-code.

| Code | Description |
| ---- | ----------- |
| 03h  | Read radio info |
| 14h  | Unknown, before write |
| 15h  | Unknown, before read | 
| 10h  | Read |
| 11h  | Write |

#### Payload length
The third field contains the payload length as an 16bit integer in big-endian. 

#### Payload
The fourth field is of variable length and contains the optional payload. 


## Read/Write sub-format
The read and write request and response payloads each have their own payload sub-format containing additional information about 
the memory region to read. 

### Read request (10h) payload format
The payload of a read request just contains the address and size of the sector to read. 
```
 +---+---+---+---+
 | Addr  | Size  |
 +---+---+---+---+
```
These fields are stored as 16bit big endian integers. The size is usually set to 0800h bytes. 
The sectors or blocks to read appear to be just numbered. E.g., 0000h, 00001h, ... 

### Read response (10h) payload format
A read response payload is usually 807h bytes long if a 800h bytes are requested. So there 
appears to be a 7h byte long header. 
```
 +---+---+---+---+---+---+---+---+...+---+
 |01h| Addr  |00h|07h|0eh|20h| Data      |
 +---+---+---+---+---+---+---+---+...+---+
``` 
The header contains some unknown but fixed fields as well as the address/sector/block number 
as a 16bit big endian integer, corresponding to the address in the request.

### Write request (11h) payload format
The write request payload is usually 808h bytes long, hence some 8byte header is expected for a data block of 800h.
``` 
 +---+---+---+---+---+---+---+---+---+...+---+
 | Addr  |00h|07h|08h|10h| Size  | Data      |
 +---+---+---+---+---+---+---+---+---+...+---+
```
The first field contains the address or sector number followed by 4 unknown but fixed bytes. 
The sixth field contains the size of the data to write (usually 800h bytes) followed by the actual data.

### Write response (11h) payload format
The write response payload contains a single byte. Probably indicating success/error. For now I've seen only 
```
 +---+
 |01h|
 +---+
```
probably indicating a success.


## Radio info (03h) sub-format
The radio info request has no payload.

#### Radio info response (03h) payload format
The response consist of a single status byte followed by the 100h bytes of information identifying the radio in detail.
```
 +---+---+...+---+
 |01h| Data      |
 +---+---+...+---+
```

#### Radio information format
The radio information blob consists of several ASCII strings and some binary information.
```  
     1   2   3   4   5   6   7   8   9   A   B   C   D   E   F  
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
00 | Model name, ASCII, 0-pad                                  |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
10 | FW Revision, ASCII, 0-pad | YYYYMMDDHHMM BCD      |00h|00h|
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
20 | Unknown vers., ASCII, 0-pad  E.g. "D00001"                |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
30 | Serial number?, ASCII, 0-pad                              |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
40 | HW Rev. ASCII, 0-pad      | Other vers., ASCII, 0-pad     |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
50 | Unknown binary data                                    ...
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
60  ...                        | Unknown ASCII info, 0-pad     |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
70 | Unknown binary data       | Unknown ASCII info, 0-pad     |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
80 | Unknown binary data       | Empty, filled with 0h      ...
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
f0  ...                                                        |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
```

## Unknown before-write (14h) and before-read (15h) commands
The purpose of these commands is unknown. They are send after the radio has been identified but 
before the first write or read command. To this end, one may assume, that this command prepares the
radio for writing or reading the codeplug.

The before-write (14h) and before-read (15h) requests have no payload.

#### Before-write (14h) and before-read (15h) response payload format
The response contains only two bytes
```
 +---+---+
 |01h|00h|
 +---+---+ 
```
Of cause, the meaning is not known and I've never seen anything than this response.