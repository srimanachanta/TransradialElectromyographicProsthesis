import os.path

from dataset import SamplesDataset
from torch.utils.data import DataLoader, ConcatDataset

data_root_path = "../bridge/logger/data"
x_file_name = "muscle_data.npy"
y_file_name = "finger_state.npy"


def get_dataset(name: str) -> SamplesDataset:
    return SamplesDataset(os.path.join(data_root_path, name, x_file_name), os.path.join(data_root_path, name, y_file_name), 3, 512)


def get_dataloaders(batch_size):

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

    pickup_ball_palm_Test = get_dataset("pickup_ball_palm")
    throw_ball_palm_Test = get_dataset("throw_ball_palm")
    piano_Test = get_dataset("throw_ball_palm")
    # piano_Test = get_dataset("piano_twinkle")

    Train_Dataset = ConcatDataset([thumb_duo, thumb_duo_press, index_duo, index_duo_press, middle_duo,
                                        middle_duo_press, ring_duo_press, pinkie_duo, pinkie_duo_press,
                                        solo_finger, solo_finger_press, fist, wrist])

    Test_Total = ConcatDataset([pickup_ball_palm_Test, throw_ball_palm_Test, piano_Test])

    return {"Train": DataLoader(Train_Dataset, batch_size=batch_size, shuffle=False),
            "pickup_ball_palm": DataLoader(pickup_ball_palm_Test, batch_size=batch_size, shuffle=False),
            "throw_ball_palm": DataLoader(throw_ball_palm_Test, batch_size=batch_size, shuffle=False), "piano": DataLoader(piano_Test, batch_size=batch_size, shuffle=False),
            "Test": DataLoader(Test_Total, batch_size=batch_size, shuffle=False)}

