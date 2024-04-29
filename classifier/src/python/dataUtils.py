import os
import torch

from dataset import SamplesDataset
from torch.utils.data import DataLoader, ConcatDataset
import numpy as np

DATA_ROOT_PATH = "dataset"
X_FILE_NAME = "muscle_data_normalized.npy"
Y_FILE_NAME = "finger_state.npy"


def get_dataset(name: str) -> SamplesDataset:
    return SamplesDataset(os.path.join(DATA_ROOT_PATH, name, X_FILE_NAME),
                          os.path.join(DATA_ROOT_PATH, name, Y_FILE_NAME), 3, 512)


def get_dataloaders(batch_size: int) -> dict[str, DataLoader]:
    thumb_duo = get_dataset("thumb_duo")
    thumb_duo_press = get_dataset("thumb_duo_press")
    index_duo = get_dataset("index_duo")
    index_duo_press = get_dataset("index_duo_press")
    middle_duo = get_dataset("middle_duo")
    middle_duo_press = get_dataset("middle_duo_press")
    ring_duo_press = get_dataset("ring_duo_press")
    pinkie_duo = get_dataset("pinkie_duo")
    pinkie_duo_press = get_dataset("pinkie_duo_press")
    solo_finger = get_dataset("solo_finger")
    solo_finger_press = get_dataset("solo_finger_press")

    fist = get_dataset("fist")
    wrist = get_dataset("wrist")

    pickup_ball_palm_test = get_dataset("pickup_ball_palm")
    throw_ball_palm_test = get_dataset("throw_ball_palm")
    piano_test = get_dataset("throw_ball_palm")
    # piano_Test = get_dataset("piano_twinkle")

    train_dataset = ConcatDataset([thumb_duo, thumb_duo_press, index_duo, index_duo_press, middle_duo,
                                   middle_duo_press, ring_duo_press, pinkie_duo, pinkie_duo_press,
                                   solo_finger, solo_finger_press, fist, wrist])

    test_total = ConcatDataset([pickup_ball_palm_test, throw_ball_palm_test, piano_test])

    return {"Train": DataLoader(train_dataset, batch_size=batch_size, shuffle=True),
            "pickup_ball_palm": DataLoader(pickup_ball_palm_test, batch_size=batch_size, shuffle=True),
            "throw_ball_palm": DataLoader(throw_ball_palm_test, batch_size=batch_size, shuffle=True),
            "piano": DataLoader(piano_test, batch_size=batch_size, shuffle=True),
            "Test": DataLoader(test_total, batch_size=batch_size, shuffle=True)}


def get_normalization_per_muscle_coefficients() -> np.ndarray:
    dataset_data = []
    for dataset_name in os.listdir(DATA_ROOT_PATH):
        dataset_path = os.path.join(os.path.join(DATA_ROOT_PATH, dataset_name))
        dataset_x_data = np.load(os.path.join(dataset_path, "muscle_data.npy"))
        dataset_data.append(dataset_x_data)

    all_data = np.concatenate(dataset_data, axis=0)

    per_muscle_coefficients = np.array([[np.average(all_data[:, idx]), np.std(all_data[:, idx])] for idx in range(12)])

    return per_muscle_coefficients


def normalize_x_data() -> None:
    per_muscle_coefficients = get_normalization_per_muscle_coefficients()

    for dataset_name in os.listdir(DATA_ROOT_PATH):
        dataset_path = os.path.join(os.path.join(DATA_ROOT_PATH, dataset_name))
        dataset_x_data = np.load(os.path.join(dataset_path, "muscle_data.npy"))

        normalized_data = (dataset_x_data - per_muscle_coefficients[:, 0]) / per_muscle_coefficients[:, 1]

        np.save(os.path.join(dataset_path, "muscle_data_normalized.npy"), normalized_data)


def convert_to_tensors_and_save_to_fs():
    for dataset_name in os.listdir(DATA_ROOT_PATH):
        dataset_path = os.path.join(os.path.join(DATA_ROOT_PATH, dataset_name))

        dataset_x_data = np.load(os.path.join(dataset_path, "muscle_data.npy"))
        dataset_x_normalized_data = np.load(os.path.join(dataset_path, "muscle_data_normalized.npy"))
        dataset_y_data = np.load(os.path.join(dataset_path, "finger_state.npy"))

        class DatasetContainer(torch.nn.Module):
            def __init__(self, x_data: torch.Tensor, x_data_norm: torch.Tensor, y_data: torch.Tensor):
                super().__init__()
                setattr(self, "muscle_data", x_data)
                setattr(self, "muscle_data_normalized", x_data_norm)
                setattr(self, "finger_state", y_data)

        dataset_x_data_tensor = torch.from_numpy(dataset_x_data)
        dataset_x_normalized_data_tensor = torch.from_numpy(dataset_x_normalized_data)
        dataset_y_data_tensor = torch.from_numpy(dataset_y_data)

        container = torch.jit.script(DatasetContainer(dataset_x_data_tensor, dataset_x_normalized_data_tensor, dataset_y_data_tensor))
        container.save(os.path.join(dataset_path, "dataset.pt"))

