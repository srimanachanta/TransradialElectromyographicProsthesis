from dataclasses import dataclass

import numpy as np
from serial import Serial


@dataclass(frozen=True)
class SensorData:
    # Time in μs data was collected from the sensor
    timestamp: int
    # (6,) ndarray of sensor data as float32. Voltage [0, 5]
    data: np.ndarray


class Sensor:
    ENABLE_MESSAGE = b'\x01'
    DISABLE_MESSAGE = b'\x02'
    SYNC_CLOCK_MESSAGE = b'\x03'

    port: Serial
    enabled: bool = False

    def __init__(self, port_address: str) -> None:
        self.port = Serial(port_address, 115_200)

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()

        if self.port.is_open:
            self.port.close()

    def start(self):
        self.port.write(self.ENABLE_MESSAGE)
        self.enabled = not self.enabled

    def stop(self):
        self.port.write(self.DISABLE_MESSAGE)
        self.enabled = not self.enabled

    def sync_clock(self):
        self.port.write(self.SYNC_CLOCK_MESSAGE)

    def get_data(self) -> SensorData:
        data = self.port.readline().decode().strip().split(",")

        timestamp = int(data[0])
        adc_data = np.array(data[1:], dtype=np.uint32)

        # Convert from ADC values to voltage
        converted_data = adc_data * (5.0 / 4095.0)

        return SensorData(timestamp, converted_data)
