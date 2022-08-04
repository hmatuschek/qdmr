# Transfer protocol for CSi (Connected Systems Inc.) devices.

# Base protocol
  - The base protocol is build upon SCSI through USB Mass Storage. Thus the device appears as a mountable USB flash drive. I hope, that I will able to send direct SCSI messages to these devices without root.
  - There are some standard SCSI messages that appear to setup and probe the device. 
  - The actual codeplug transfer then happens with invalid SCSI commands (OpCode: 0xff). 

## Custom SCSI message (SBC OpCode 0xff)
There are only two custom SCSI messages used 

```
 > ff 2a 00 00 00 00 00 00 00 00 00 00 00 00 47 50
```
Sends a command to the device. The actual command is send as a separate SCSI Request Payload packet.

```
 > ff 28 00 00 00 00 00 00 00 00 00 00 00 00 47 50
```
Sends a response query to the device. The actual response is then send by the device as a separate SCSI Response Payload packet.


## Request and Response format
This is the format, requests and responses are transmitted as SCSI Request/Response Payloads.

```  
 +---+---+---+---+---+.........+---+---+---+
 |A1h|Rsp|CMD|Len BE | Payload | CRC   |A4h|
 +---+---+---+---+---+.........+---+---+---+
```

#### Rsp code
| Code | Description |
| ---- | ----------- |
| 00h  | Request |
| 80h  | Response |

#### Command code 
| Code | Description |
| ---- | ----------- |
| 03h  | Unknown |
| 15h  | Unknown | 
| 10h  | Read |
| 11h  | Write |


### Read/Write sub-format
When reading/writing data (cmd 10h) the request and response payload follow a sub-format containing the length and address of the data read/written.
```
 +---+---+---+---+---+.......+---+
 | Addr  | Size  | Optional data |
 +---+---+---+---+---+.......+---+
```
Address and size are stored in big endian. For a read request, the optional data is missing.

