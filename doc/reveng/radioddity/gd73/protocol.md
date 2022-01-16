# GD73 Serial Protocol

## General Request Frame Format
```
     0   1   2   3   4   5   6   7   8      n-2 n-1  bytes
   +---+---+---+---+---+---+---+---+---+...+---+---+
00 |68h|FLG|CMD|SUB| CRC?  | Size  |  Payload  |10h|
   +---+---+---+---+---+---+---+---+---+...+---+---+ 
```

| Bytes  | Value | Description |
|:-------|:------|:------------|
| 0      | 68    | Start of frame byte. Fixed. |
| 1      | 0f    | Flags? |
| 2:3    | 01 04 | First command send on read. Probably *enter prog mode*. No payload. |
|        | 01 02 | Second command send on read. Probably *identify*. No payload. |
|        | 01 00 | Write request, usually 55bytes payload. |
|        | 04 01 | Read request, 2bytes payload likely address or block number in LE. |
| 4:5    |       | Likely some sort of checksum. |
| 6:7    |       | The payload size in little endian. |
| 8:(n-2)|       | The request/response payload. |
| n-1    | 10    | The end-of-frame byte. |


## General Response Frame Format
```
     0   1   2   3   4   5   6   7   8      n-2 n-1  n      n+m  bytes
   +---+---+---+---+---+---+---+---+---+...+---+---+---+...+---+
00 |68h|FLG|00h|PAD| CRC?  | Size  |  Payload  |10h| Pad Bytes |
   +---+---+---+---+---+---+---+---+---+...+---+---+---+...+---+
```

| Bytes  | Value | Description |
|:-------|:------|:------------|
| 0      | 68    | Start of frame byte. Fixed. |
| 1      | 0f    | Flags? |
| 2      | 00    | Indicates a response, probably success. |
| 3      |       | Number of additional bytes added to the end of the frame after the EOF byte. |
| 4:5    |       | Likely some sort of checksum. |
| 6:7    |       | The payload size in little endian. |
| 8:(n-2)|       | The request/response payload. |
| n-1    | 10    | The end-of-frame byte. |
| n:n+m  |       | Additional bytes observed on the *write* and *enter prog mode* responses. |

### CRC
Some sort of sum on 16bit integers over at least CMD, SUB, Size and payload.

## Enter program mode
