import time

import numpy as np

from recorder import HDF5Recorder
from sensor import Sensor

SENSOR_A_ADDRESS = "/dev/cu.usbmodem1101"
SENSOR_B_ADDRESS = "/dev/cu.usbmodem1201"

SAMPLING_RATE_SPS = 1000
DATA_FILE = "data.hdf5"
DATASET_NAME = "test"


def main():
    with Sensor(SENSOR_A_ADDRESS) as sensor_a, Sensor(SENSOR_B_ADDRESS) as sensor_b:
        recorder = HDF5Recorder(SAMPLING_RATE_SPS, DATA_FILE, DATASET_NAME)
        while True:
            sensor_group_a_data = sensor_a.get_data()
            sensor_group_b_data = sensor_b.get_data()

            if abs(sensor_group_b_data.timestamp - sensor_group_a_data.timestamp) > 500:
                sensor_a.sync_clock()
                sensor_b.sync_clock()

            full_data = np.concatenate(
                (sensor_group_a_data.data, sensor_group_b_data.data)
            )

            recorder.add_sample(
                full_data,
                time.time()
            )


if __name__ == "__main__":
    main()
