import numpy as np
from torch.utils.data import Dataset

# classes = {
#     "no_movement": 0,
#     "extending": 1,
#     "flexing": 2
# }
#
# y_indices = {
#     "t_distal",
#     "t_proximal",
#     "p0_distal",
#     "p0_proximal",
#     "p1_distal",
#     "p1_proximal",
#     "p2_distal",
#     "p2_proximal",
#     "p3_distal",
#     "p3_proximal",
#     "w"
# }

# 64, 128, 256, 512


def create_y_matrix(y_vec: np.ndarray, num_classes: int) -> np.ndarray:
    return np.take(np.eye(num_classes), y_vec, axis=0)


class SamplesDataset(Dataset):
    x_data: np.ndarray
    y_data: np.ndarray

    num_classes: int
    window_size_samples: int

    def __init__(self, x_source_file_path: str, y_source_file_path: str, num_classes: int, window_size_samples: int):
        self.x_data = np.load(x_source_file_path)
        self.y_data = np.load(y_source_file_path)

        self.num_classes = num_classes
        self.window_size_samples = window_size_samples

        if self.x_data.shape[1] != self.y_data.shape[1]:
            raise ValueError(f"Lengths of samples in x: {self.x_data.shape[1]} and y: {self.y_data.shape[1]} data are not equal")

    def __len__(self) -> int:
        return self.x_data.shape[1] - self.window_size_samples + 1

    def __getitem__(self, idx) -> tuple[np.ndarray, np.ndarray]:
        return self.x_data[:, idx:(self.window_size_samples + idx)], create_y_matrix(self.y_data[:, self.window_size_samples + idx - 1], self.num_classes)


class TimeWindowDataset(SamplesDataset):
    def __init__(self, x_source_file_path: str, y_source_file_path: str, num_classes: int, window_size_ms: int, sampling_rate_sps: int):
        super().__init__(x_source_file_path, y_source_file_path, num_classes, int(window_size_ms / 1000.0) * sampling_rate_sps)
