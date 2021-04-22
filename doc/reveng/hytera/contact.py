#!/usr/bin/env python3
import struct
from enum import Enum
from section import CPSSection
from typing import Dict, List
from prettytable import PrettyTable  # type: ignore
from util import chunks


class DigitalContact:
    """A class that represents a digitial channel."""
    STRUCT_FMT = '<H32sBBHIIH'

    class CALL_TYPE(Enum):
        """Type of call that is made when this contact is called.

        If the contact type is IGNORE, the contact is ignored by the radio and
        the CPS.
        """
        PRIVATE = 0
        GROUP = 1
        IGNORE = 0x11

    def __init__(self, data: bytes) -> None:
        fields = struct.unpack(DigitalContact.STRUCT_FMT, data)
        self.idx = fields[0]
        self.name = fields[1].decode('utf-8')
        self.call_type = DigitalContact.CALL_TYPE(fields[2])
        self.is_ref = bool(fields[3])
        self.id = fields[5]
        self.link = fields[7]

    @staticmethod
    def size() -> int:
        """Get the size of the contact structure."""
        return struct.calcsize(DigitalContact.STRUCT_FMT)


def parse_digital_contacts(cps: Dict[int, CPSSection]) -> List[DigitalContact]:

    contact_section = cps.get(0x2a)

    if contact_section is None:
        raise RuntimeError("No contact section found in codeplug.")

    num_contacts = contact_section.header.no_elements
    contact_sz = DigitalContact.size()

    contacts = []

    # NOTE: For some reason, contacts are referenced by index. The mappings
    # table in the section is ignored.
    for contact_data in chunks(contact_section.data[2:], contact_sz)[:num_contacts]:
        contacts.append(DigitalContact(contact_data))

    return contacts


def print_contacts(cps: Dict[int, CPSSection]) -> None:
    contacts = parse_digital_contacts(cps)

    table = PrettyTable([
        'Index', 'Name', 'Type', 'Is referenced', 'id', 'link'])

    for contact in contacts:
        if contact.call_type != DigitalContact.CALL_TYPE.IGNORE:
            table.add_row([contact.idx, contact.name, contact.call_type,
                           contact.is_ref, contact.id, contact.link])

    print(table)
