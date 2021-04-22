from reader import CPReader
from typing import Dict, List
import struct


class CPSSectionHeader:
    STRUCT_FMT = '<HHHHHHHII'

    def __init__(self, reader: CPReader, addr: int) -> None:
        self.__addr = addr
        fields = struct.unpack(CPSSectionHeader.STRUCT_FMT,
                               reader.read(addr, CPSSectionHeader.size()))

        self.section_type = fields[0]
        self.element_capacity = fields[1] & 0xfff
        self.unk1 = fields[1] >> 12
        self.no_elements = fields[2]

        assert fields[3] == 0x0020
        assert fields[4] == 0x0000
        self.unk2 = fields[5]
        self.unk3 = fields[6]

        self.data_size = fields[7]

        assert fields[8] == self.data_size + 0x16

    def __str__(self):
        return "Section: address=0x{:02X} type={:02X} cap={} " \
               "elem_in_use={} size={} " \
               " unk1=0x{:02X} unk2=0x{:02X}" \
               " unk3=0x{:02X}".format(self.__addr,
                                       self.section_type,
                                       self.element_capacity,
                                       self.no_elements,
                                       self.data_size,
                                       self.unk1,
                                       self.unk2,
                                       self.unk3)

    @staticmethod
    def size() -> int:
        return struct.calcsize(CPSSectionHeader.STRUCT_FMT)



class CPSSection:
    def __init__(self, reader: CPReader, addr: int) -> None:
        self.header = CPSSectionHeader(reader, addr)
        self.data = reader.read(addr + CPSSectionHeader.size(),
                                self.header.data_size)
        self.mappings = self.__read_mappings(reader,
                                             addr + self.header.size() + self.header.data_size,
                                             self.header.element_capacity)

    @staticmethod
    def __read_mappings(reader: CPReader, addr: int,
                        elem: int) -> List[int]:
        mappings = []
        mapping_format = '<HI'
        mapping_sz = struct.calcsize(mapping_format)

        for i, addr in enumerate(range(addr,
                                       addr + (mapping_sz * elem),
                                       mapping_sz)):
            idx, offset = struct.unpack(mapping_format, reader.read(addr, mapping_sz))
            mappings.append(idx)

        return mappings

    def __len__(self):
        return self.header.size() + self.header.data_size + \
            6 * self.header.element_capacity

    def __str__(self):
        return self.header.__str__()


def read_codeplug_sections(reader: CPReader) -> Dict[int, CPSSection]:
    addr = 0x37e
    struct_fmt = '<HBBBBBBBBBBII'
    fields = struct.unpack(struct_fmt, reader.read(addr, 0x14))

    assert fields[0] == 0x40

    end_addr = addr + fields[12]

    section_addr = addr + struct.calcsize(struct_fmt)

    sections = []

    while section_addr < end_addr:
        section = CPSSection(reader, section_addr)

        sections.append(section)

        section_addr += len(section)

    return dict([(s.header.section_type, s) for s in sections])
