# GD73 Serial Protocol

# General Frame Format
Irrespective of request or response, the frame has always (except for a bug on the first response) the same format.

```
     0   1   2   3   4   5   6   7   8      n-2 n-1  bytes
   +---+---+---+---+---+---+---+---+---+...+---+---+
00 |68h|FLG|Command| CRC?  | Size  |  Payload  |10h|
   +---+---+---+---+---+---+---+---+---+...+---+---+ 
```

| Bytes  | Value | Description |
|:-------|:------|:------------|
| 0      | 68    | Start of frame byte. Fixed. |
| 1      | 0f    | Flags? |
| 2:3    | 0104  | First command send on read. Probably *enter prog mode*. No payload. |
|        | 0102  | Second command send on read. Probably *identify*. No payload. |
|        | 0100  | Write request. |
|        | 0401  | Read command, 2bytes payload likely address or block number in LE. |
|        | 0000  | On *read* responses. |
|        | 0002  | On *enter prog mode* response. This response also has two additional bytes appended to the end after the EOF byte. |  
|        | 0001  | On *write* response. This response also has one additional byte appended to the end after the EOF byte. |
| 4:5    |       | Likely some sort of checksum. |
| 6:7    |       | The payload size. |
| 8:(n-2)|       | The request/response payload. |
| n-1    | 10    | The end-of-frame byte. |
| n:(n+1)|       | Additional byte observed on the *write* response. |
| n:(n+2)| 0000  | Additional 2-bytes observed on the *enter prog mode* response. |

