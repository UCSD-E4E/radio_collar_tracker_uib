import random

import serial
import time
from packet import DataPayload, Packet


def main():
    payload = DataPayload(-1, -1, -1, -1, -1, -1, -1)
    packet = Packet(payload)

    with serial.Serial(port="", baudrate=115200) as ser:
        payload.time = time.time()
        payload.latitude = random.randint(-2**32, (2**32)-1)
        payload.longitude = random.randint(-2**32, (2**32)-1)
        payload.altitude = random.randint(0, (2**16)-1)
        payload.heading = random.randint(-2**16, (2**16)-1)
        payload.voltage = random.randint(0, (2**16)-1)
        payload.fix_type = random.randint(0, (2**8)-1)

        ser.write(packet.to_bytes())


if __name__ == "__main__":
    main()
