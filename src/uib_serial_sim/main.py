import os
import pty
import random
import time

import serial

from packet import DataPayload, Packet


def test():
    master, child = pty.openpty()

    payload = DataPayload(-1, -1, -1, -1, -1, -1, -1)
    packet = Packet(payload)

    with serial.Serial(port=os.ttyname(child), baudrate=115200) as ser:
        payload.time = int(time.time())
        payload.latitude = random.randint(-2 ** 31, (2 ** 31) - 1)
        payload.longitude = random.randint(-2 ** 31, (2 ** 31) - 1)
        payload.altitude = random.randint(0, (2 ** 16) - 1)
        payload.heading = random.randint(-2 ** 15, (2 ** 15) - 1)
        payload.voltage = random.randint(0, (2 ** 16) - 1)
        payload.fix_type = random.randint(0, (2 ** 8) - 1)

        original_packet_bytes = packet.to_bytes()
        ser.write(original_packet_bytes)

    raw_bytes = bytearray(os.read(master, 1000))
    packet = Packet.from_bytearray(raw_bytes)
    assert (original_packet_bytes == packet.to_bytes())


def main():
    payload = DataPayload(-1, -1, -1, -1, -1, -1, -1)
    packet = Packet(payload)
    port_name = ""

    with serial.Serial(port=port_name, baudrate=115200) as ser:
        while True:
            payload.time = int(time.time())
            payload.latitude = random.randint(-2 ** 31, (2 ** 31) - 1)
            payload.longitude = random.randint(-2 ** 31, (2 ** 31) - 1)
            payload.altitude = random.randint(0, (2 ** 16) - 1)
            payload.heading = random.randint(-2 ** 15, (2 ** 15) - 1)
            payload.voltage = random.randint(0, (2 ** 16) - 1)
            payload.fix_type = random.randint(0, (2 ** 8) - 1)

            ser.write(packet.to_bytes())
            time.sleep(1)


if __name__ == "__main__":
    main()
