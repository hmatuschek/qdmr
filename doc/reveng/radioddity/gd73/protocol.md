# GD73 Serial Protocol

## General Request/Response Frame Format
```
     0   1   2   3   4   5   6   7   8      n-2 n-1  bytes
   +---+---+---+---+---+---+---+---+---+...+---+---+
00 |68h|FLG|CMD|SUB| CRC?  | Size  |  Payload  |10h|
   +---+---+---+---+---+---+---+---+---+...+---+---+ 
```

| Bytes  | Value | Description |
|:-------|:------|:------------|
| 0      | 68    | Start of frame byte. Fixed. |
| 1      | 0f    | Flags? Fixed. |
| 2:3    | 01 04 | First command send on read. Probably *enter prog mode*. No payload. |
|        | 00 02 | Response to *enter prog mode* |
|        | 01 02 | Start read. |
|        | 00 00 | Read response, contains sequence number and 35h bytes codeplug data. |
|        | 04 01 | Read ACK, triggers read of next segment. |
|        | 01 00 | Write request, usually 55bytes payload. |
|        | 00 01 | Write ACK. |
| 4:5    |       | CRC |
| 6:7    |       | The payload size in little endian. |
| 8:(n-2)|       | The request/response payload. |
| n-1    | 10    | The end-of-frame byte. |


### CRC
The CRC is computed over the entire packet, including start and end bytes. For the computation, 
the packet is interpreted as a sequence of little-endian unsigned 16-bit integer. If the packet 
does not align with 16bits, it gets 0-padded to a multiple of 16bit. 

Let `v[i]` the i-th of `N` unsigned 16bit integer formed by the 0-padded packet, the CRC is then 
computed as
```
uint32_t crc = 0xffff;
for (int i=0; i<N; i++) {
  if (crc < v[i])
    crc += 0xffff;
  crc -= v[i];
}
```
To check the CRC, the resulting value of `crc` must be 0, to compute the CRC, just set the CRC field
to 0, compute the CRC over the entire packet and then set the field within the packet to the value 
of the result.


## Enter program mode request (0104h)
The request is send before reading and writing the codeplug. The radio responses with 21h bytes in a 
0002h response. The content/format of the response is unknown

## Start read request (0102h)
This request initializes the codeplug read process. The radio responds with the first (seq. number 
0000h) 35h byte-segment of the codeplug. To receive further segments, the host must ACK the received
segment with a read-ACK request containing the number of the received segment.
```
 Host                                Device
  | --- Read request (no payload)  ---> |
  | <----- Response 0000 + Data ------- |
  | --------- ACK seq. 0000 ----------> |
  | <----- Response 0001 + Data ------- |
  | --------- ACK seq. 0001 ----------> |
 ...                                   ...
  | <----- Response 0a42 + Data ------- |
  | --------- ACK seq. 0a42 ----------> |
  | <----- Response 0a43 + Data ------- |
```

## Write request (0100h)
The write request sends a sequence number and codeplug segment to the device. The device then ACKs 
the written segment.


