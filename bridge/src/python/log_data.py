import numpy as np
import serial


class SensorMatrix:
    port_one: serial.Serial
    port_two: serial.Serial

    open_message = 0x1
    close_message = 0x2

    def __init__(self, port_one_addr: str, port_two_addr: str):
        self.port_one = serial.Serial(port_one_addr, 115200)
        self.port_two = serial.Serial(port_two_addr, 115200)

    def __enter__(self):
        self.port_one.write(self.open_message.to_bytes())
        self.port_two.write(self.open_message.to_bytes())
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.port_one.write(self.close_message.to_bytes())
        self.port_two.write(self.close_message.to_bytes())

        self.port_one.close()
        self.port_two.close()

    def get_data(self) -> tuple[int, list[float]]:
        first = self.port_one.readline().strip().split(b",")
        second = self.port_two.readline().strip().split(b",")

        # Only get timestamp from first sensor
        timestamp = int(first[0])
        concat_sensor_data = [int(v) * (5.0 / 4095.0) for sensor_data in [first, second] for v in sensor_data[1:]]

        return timestamp, concat_sensor_data


with SensorMatrix("/dev/cu.usbmodem11101", "/dev/cu.usbmodem11201") as matrix:
    # last_time = 0
    # i = 0
    # data = np.zeros((30000, 12))
    # while True:
    #     sensor_data = matrix.get_data()
    #     if last_time == 0 or sensor_data[0] >= last_time + 1000:
    #         last_time = sensor_data[0]
    #         data[i, :] = np.array(sensor_data[1])
    #         i += 1
    #
    #         if i == 30000:
    #             print([np.mean(data[:, idx]) for idx in range(12)])
    #             print([np.std(data[:, idx]) for idx in range(12)])
    #             exit()

    while True:
        sensor_data = matrix.get_data()
        print([round(v / (5.0 / 4095.0)) for v in sensor_data[1]])
