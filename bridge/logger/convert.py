import numpy as np
import os

for dataset_dir in os.listdir("data"):
    dataset_dir_path = os.path.join("data", dataset_dir)

    data = np.genfromtxt(os.path.join(dataset_dir_path, "x.txt"), delimiter=",")[:, :12]
    np.save(os.path.join(dataset_dir_path, "muscle_data.npy"), data)
