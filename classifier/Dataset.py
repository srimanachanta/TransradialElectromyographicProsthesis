from torch.utils.data import Dataset
import os


class EmgDataset(Dataset):
    def __init__(self, filepath: str):
        if not filepath.endswith('.hdf5'):
            raise ValueError("Dataset must be an hdf5 encoded file")

        if not os.path.exists(filepath) or os.path.isdir(filepath):
            raise ValueError("Dataset file does not exist")

    def __len__(self):
        pass

    def __getitem__(self, idx: int):
        pass
