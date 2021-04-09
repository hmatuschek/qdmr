import struct
import usb.core
import usb.util
import packet
from abc import ABC, abstractmethod

class Radio(ABC):
    READ_SZ = 0x400
    TIMEOUT = 1500
    OUT_EP = 0x01
    IN_EP = 0x81

    def __init__(self, productId, vendorId, ep, verbose):
        self._pid = productId
        self._vid = vendorId
        self._outEp = ep
        self._inEp = ep | 0x80
        self._verbose = verbose
        pass

    def __enter__(self):
        self._dev = usb.core.find(idVendor=self._vid, idProduct=self._pid)

        if self._dev is None:
            raise ValueError('Radio not found')

        self._dev.set_configuration()

        return self

    @abstractmethod
    def getTotalPacketLength(self, partialPacket):
        pass

    @abstractmethod
    def unpack(self, data):
        pass

    def xfer(self, cmd):
        if self._verbose:
            print('-'*80)
            print(cmd)

        self._dev.write(self._outEp, cmd.pack(), Radio.TIMEOUT)
        data = bytes(self._dev.read(self._inEp, Radio.READ_SZ, Radio.TIMEOUT))

        dataSz = self.getTotalPacketLength(data)

        while len(data) != dataSz:
            data += bytes(self._dev.read(self._inEp, Radio.READ_SZ, Radio.TIMEOUT))

        pkt = self.unpack(data)

        if self._verbose:
            print(pkt)

        return pkt

class FirmwareRadio(Radio):
    def __init__(self, verbose):
        super().__init__(0x1234, 0x8765, 0x01, verbose)

    def getTotalPacketLength(self, partialPacket):
        return struct.unpack('<H', partialPacket[:2])[0]

    def unpack(self, data):
        return packet.FirmwarePacket.unpack(data)

    def __enter__(self):
        super().__enter__()
        response = self.xfer(packet.FirmwarePacket.setMemoryAccess(True))
        openStatus = response._content._content._status

        if openStatus != packet.FwMemoryAccessResponse.STATUS_OK:
            raise RuntimeError("Failed to enable write on radio (status={:02X})".format(openStatus))

        return self

    def __exit__(self, exc_type, exc_value, traceback):
        response = self.xfer(packet.FirmwarePacket.setMemoryAccess(False))
        openStatus = response._content._content._status

        if openStatus != packet.FwMemoryAccessResponse.STATUS_OK:
            raise RuntimeError("Failed to disable write on radio (status={:02X})".format(openStatus))

class CPSRadio(Radio):
    def __init__(self, verbose):
        super().__init__(0x0a11, 0x238b, 0x04, verbose)

    def getTotalPacketLength(self, partialPacket):
        return struct.unpack('>H', partialPacket[8:10])[0]

    def unpack(self, data):
        return packet.Packet.unpack(data)

    def __enter__(self):
        super().__enter__()
        self.xfer(packet.Packet.connect())
        self.xfer(packet.Packet.enterProgMode(packet.EnterProgModeRequest.READ_MODE))

        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.xfer(packet.Packet.leaveProgMode())
        pass
