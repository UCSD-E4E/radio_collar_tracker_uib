import abc

from crccheck.crc import Crc16
from dataclasses import dataclass


class Payload(abc.ABC):
    def __init__(self):
        self.packet_class: int = -1
        self.packet_id: int = -1

    @staticmethod
    def from_bytearray(byte_array) -> 'Payload':
        raise NotImplementedError()

    def to_bytearray(self) -> bytearray:
        raise NotImplementedError()

    def to_bytes(self) -> bytes:
        byte_array = self.to_bytearray()
        return bytes(byte_array)


@dataclass
class DataPayload(Payload):
    time: int
    latitude: int
    longitude: int
    altitude: int
    heading: int
    voltage: int
    fix_type: int

    version: int = 0x01
    packet_class: int = 0x05
    packet_id: int = 0x03

    @staticmethod
    def from_bytearray(byte_array) -> 'DataPayload':
        version = byte_array[0]
        t = int.from_bytes(byte_array[1:9], byteorder="big")
        latitude = int.from_bytes(byte_array[9:13], byteorder="big", signed=True)
        longitude = int.from_bytes(byte_array[13:17], byteorder="big", signed=True)
        altitude = int.from_bytes(byte_array[17:19], byteorder="big")
        heading = int.from_bytes(byte_array[19:21], byteorder="big", signed=True)
        voltage = int.from_bytes(byte_array[21:23], byteorder="big")
        fix_type = byte_array[23]

        payload = DataPayload(t, latitude, longitude, altitude, heading, voltage, fix_type, version=version)
        return payload

    def to_bytearray(self) -> bytearray:
        packet = bytearray()

        packet.extend(self.version.to_bytes(1, byteorder="big"))
        packet.extend(self.time.to_bytes(8, byteorder="big"))
        packet.extend(self.latitude.to_bytes(4, byteorder="big", signed=True))
        packet.extend(self.longitude.to_bytes(4, byteorder="big", signed=True))
        packet.extend(self.altitude.to_bytes(2, byteorder="big"))
        packet.extend(self.heading.to_bytes(2, byteorder="big", signed=True))
        packet.extend(self.voltage.to_bytes(2, byteorder="big"))
        packet.extend(self.fix_type.to_bytes(1, byteorder="big"))

        return packet


@dataclass
class Packet:
    payload: 'Payload'

    SYNC_CHAR_1: int = 0xE4
    SYNC_CHAR_2: int = 0xEB

    def to_bytearray(self) -> bytearray:
        packet = bytearray()

        # Packet headers
        packet.extend(self.SYNC_CHAR_1.to_bytes(1, byteorder="big"))
        packet.extend(self.SYNC_CHAR_2.to_bytes(1, byteorder="big"))
        packet.extend(self.payload.packet_class.to_bytes(1, byteorder="big"))
        packet.extend(self.payload.packet_id.to_bytes(1, byteorder="big"))

        # Packet payload
        payload_bytes = self.payload.to_bytearray()
        packet.extend(len(payload_bytes).to_bytes(2, byteorder="big"))
        packet.extend(payload_bytes)

        # Checksum
        packet.extend(Crc16.calc(packet).to_bytes(2, byteorder="big"))

        return packet

    def to_bytes(self) -> bytes:
        byte_array = self.to_bytearray()
        return bytes(byte_array)

    @staticmethod
    def from_bytearray(byte_array) -> 'Packet':
        assert byte_array[0] == Packet.SYNC_CHAR_1
        assert byte_array[1] == Packet.SYNC_CHAR_2
        if byte_array[2] == DataPayload.packet_class and byte_array[3] == DataPayload.packet_id:
            payload_len = int.from_bytes(byte_array[4:6], byteorder="big")
            payload = DataPayload.from_bytearray(byte_array[6:6 + payload_len])

            checksum = Crc16.calc(byte_array[:6 + payload_len])
            assert checksum == int.from_bytes(byte_array[-2:], byteorder="big")
        else:
            raise ValueError(f"Unknown packet class {byte_array[2]} and ID {byte_array[3]}")

        return Packet(payload)
