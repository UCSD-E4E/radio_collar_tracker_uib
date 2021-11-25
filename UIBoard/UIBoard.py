from typing import Optional
import serial
import threading
import logging
import time

class UIBoard:
    def __init__(self, port: str, baudrate:int=115200) -> None:
        self.__device = port
        self.__baurate = baudrate
        self.__serial: Optional[serial.Serial] = None
        self.__runFlag = False

    def __enter__(self):
        self.__serial = serial.Serial(self.__device, baudrate=self.__baurate)
        self.__runFlag = True
        self.__readThread = threading.Thread(target=self.__reader)
        self.__readThread.start()
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.__runFlag = False
        self.__readThread.join()
        if self.__serial is not None:
            self.__serial.close()
        self.__serial = None

    def __reader(self):
        if self.__serial is None:
            raise RuntimeError()
        logger = logging.getLogger(f'{self.__device}.UIB.reader')
        
        self.__serial.timeout = 0.5
        while self.__runFlag:
            data_read = self.__serial.read(1)
            if self.__serial.inWaiting() > 0:
                data_read += self.__serial.read(self.__serial.inWaiting())
            if len(data_read):
                logger.debug(f"Received {data_read.hex()}")

if __name__ == '__main__':
    root_logger = logging.getLogger()
    # Log to root to begin
    root_logger.setLevel(logging.DEBUG)

    console_handler = logging.StreamHandler()
    console_handler.setLevel(logging.DEBUG)

    error_formatter = logging.Formatter('%(asctime)s.%(msecs)03d - %(name)s - %(levelname)s - %(message)s', datefmt="%Y-%m-%d %H:%M:%S")
    console_handler.setFormatter(error_formatter)
    root_logger.addHandler(console_handler)
    with UIBoard('/dev/ttyACM0') as uib:
        time.sleep(15)