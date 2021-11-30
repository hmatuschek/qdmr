# DM1702 Protocol

The communication protocol is a simple serial-port-over-USB protocol. It follows a strict 
request/response structure. That is, all communication is initiated by the host and each request 
from the host gets replied by the device. 

The following sections will outline all requests and their responses.

## Read Memory/Registers

### Request
```
+-----+-----+-----+-----+-----+-----+
| 'R' | Address         | Length    |
+-----+-----+-----+-----+-----+-----+
```
All read requests are of length 6bytes. It starts with the fixed byte `R = 0x52`. 
The address is a 24bit little-endian field. The length field is 16bit little-endian field.

### Response
```
+-----+-----+-----+-----+-----+-----+-----+ ... +-----+
| 'W' |Flags| Address   | Length    | Payload         |
+-----+-----+-----+-----+-----+-----+-----+-...-+-----+
```
The read response has a similar structure. Here, however, the prefix is `W = 0x57`. The address 
and length fields are copied from the request. The header is followed by the payload of the
specified length.


## Get Memory/Registers
This request and response appears to be similar to the read command but may address a different 
memory region or status information.

### Request
```
+-----+-----+-----+-----+-----+-----+
| 'G' | Address         | Length    |
+-----+-----+-----+-----+-----+-----+
```
The request is 6bytes long. It starts with a fixed prefix `G=0x47`. The address is a 24bit 
little-endian field. The length field is 16bit little-endian field.

### Response
```
+-----+-----+-----+-----+-----+-----+-----+ ... +-----+
| 'S' | Address         | Length    | Payload         |
+-----+-----+-----+-----+-----+-----+-----+-...-+-----+
```
Like for the read response, the flags, address and length fields are copied from the request. The 
fixed prefix is `S=0x53`. The header is followed by the requested payload of specified length.

## View (?!?)
The purpose and format of this request and its responses is still unknown.

### Request
```
+-----+-----+-----+-----+-----+
| 'V' | Address ?             |
+-----+-----+-----+-----+-----+
```
These requests are of fixed size (5 bytes). The fixed prefix is `V=0x56`. The rest of the packet is
still unknown but appears to at least contain some sort of an address in little endian.

### Response 
```
+-----+-----+-----+-----+ ... +-----+
| 'V' | LSB | Len | Payload         |
+-----+-----+-----+-----+ ... +-----+
```
The response header is pretty short. It contains the same prefix `V=0x56` followed by the 
least-significant byte of the address. I.e., the last byte of the request packet. Then there is a
single-byte length of the payload. 

## Write 
### Request 
```
+-----+-----+-----+-----+-----+-----+-----+ ... +-----+
| 'W' | Address         | Length    | Payload         |
+-----+-----+-----+-----+-----+-----+-----+-...-+-----+
```
The write request starts with the fixed byte `W=0x57` followed by the 24bit, little-endian address
and 16bit little-endian length of the payload. The actual payload of the request is simply appended
to the request header and is of the specified length.

### Response 
The device simply replies with an ACK.
``` 
+-----+
| 06H |
+-----+
```

## ACK/Status request
There are frequent 1-byte messages send to and from the device. This byte appears to be a kind of 
status request and acknowledgement responses. For example, after each read, get, view requests and
response, a single `0x06` byte is send by the host which the device responses with the same byte.
Also some control requests are ACKed by the device with this byte.

