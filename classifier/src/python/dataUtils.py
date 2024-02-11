import os
from dataset import SamplesDataset
from torch.utils.data import DataLoader, ConcatDataset
import numpy as np

data_root_path = "../../../dataset"
x_file_name = "muscle_data_normalized.npy"
y_file_name = "finger_state.npy"


def get_dataset(name: str) -> SamplesDataset:
    return SamplesDataset(os.path.join(data_root_path, name, x_file_name),
                          os.path.join(data_root_path, name, y_file_name), 3, 512)


def get_dataloaders(batch_size: int):
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


def normalize_x_data():
    data = np.zeros((1, 12))
    data_cropped = np.zeros((1, 12))

    for dataset_name in os.listdir("../bridge/logger/data"):
        dataset_path = os.path.join(os.path.join("../bridge/logger/data", dataset_name))
        dataset_x_data = np.load(os.path.join(dataset_path, "muscle_data.npy"))
        dataset_x_data_cropped = np.load(os.path.join(dataset_path, "muscle_data_cropped.npy"))

        data = np.concatenate([data, dataset_x_data], axis=0)
        data_cropped = np.concatenate([data_cropped, dataset_x_data_cropped], axis=0)

    data = data[1:]
    data_cropped = data_cropped[1:]

    stuff = [(np.average(data[:, idx]), np.std(data[:, idx])) for idx in range(12)]
    stuff_cropped = [(np.average(data_cropped[:, idx]), np.std(data_cropped[:, idx])) for idx in range(12)]

    for dataset_name in os.listdir("../bridge/logger/data"):
        dataset_path = os.path.join(os.path.join("../bridge/logger/data", dataset_name))

        for idx in range(12):
            dataset_x_data = np.load(os.path.join(dataset_path, "muscle_data.npy"))
            dataset_x_data_cropped = np.load(os.path.join(dataset_path, "muscle_data_cropped.npy"))

            dataset_x_data = (dataset_x_data - stuff[idx][0]) / (stuff[idx][1])
            dataset_x_data_cropped = (dataset_x_data_cropped - stuff_cropped[idx][0]) / (stuff_cropped[idx][1])

            np.save(os.path.join(dataset_path, "muscle_data_normalized.npy"), dataset_x_data)
            np.save(os.path.join(dataset_path, "muscle_data_cropped_normalized.npy"), dataset_x_data_cropped)
