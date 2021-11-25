import serial
import yaml

def test_heartbeat():
    with open('test_config.yaml') as config:
        config = yaml.safe_load(config)
    assert(isinstance(config, dict))
    assert('serial' in config)
    with serial.Serial(config['serial'], baudrate=115200) as uib:
        uib.timeout = 1
        print(uib.read(1024))