from threading import Thread, Lock

import h5py
import numpy as np


class HDF5Recorder:
    sampling_rate: int
    filepath: str
    dataset: str

    sample_buffer: list[tuple[np.ndarray, float]]
    buffer_latest_time: float
    buffer_lock: Lock

    def __init__(self, sampling_rate_sps: int, filepath: str, dataset: str):
        self.sampling_rate = sampling_rate_sps
        self.filepath = filepath
        self.dataset = dataset
        self.sample_buffer = []
        self.buffer_latest_time = 0
        self.buffer_lock = Lock()

        Thread(target=self.main, name="Data_Recorder").start()

    def add_sample(self, data: np.ndarray, timestamp_s: float):
        if self.buffer_latest_time == 0 or (
            (1.0 / self.sampling_rate) <= (timestamp_s - self.buffer_latest_time)
        ):
            self.buffer_lock.acquire()
            self.sample_buffer.append((data, timestamp_s))
            self.buffer_latest_time = timestamp_s
            self.buffer_lock.release()

    def main(self):
        with h5py.File(self.filepath, "a") as f:
            dataset = f.create_dataset(
                self.dataset,
                data=np.zeros((1, 12), dtype=np.float32),
                maxshape=(None, 12),
            )

            while True:
                self.buffer_lock.acquire()

                if len(self.sample_buffer) != 0:
                    current_shape = dataset.shape

                    # Resize the dataset with the new frame
                    new_shape = (current_shape[0] + 1, current_shape[1])
                    dataset.resize(new_shape)

                    # Add the new frame to the dataset
                    result = self.sample_buffer.pop(0)[0]
                    dataset[-1, :] = result

                self.buffer_lock.release()
