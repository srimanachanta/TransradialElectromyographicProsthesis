import numpy as np
import random
import matplotlib.pyplot as plt

all_data = np.load("dataset/solo_finger/muscle_data_normalized.npy")

WINDOW_SIZE = 512

def get_rand_range(len: int):
    start = random.randint(500, all_data.shape[0]-max(len, 500))
    end = start + len
    
    return all_data[start:end, 4]

def save_data_to_image(data: np.ndarray, img_name: str, fig_size=tuple[float,float]):
    plt.figure(dpi=800, figsize=fig_size)
    plt.plot(data, color="black")
    plt.axis("off")
    plt.savefig(img_name + ".png")
    plt.clf()


for i in range(10):
    large = get_rand_range(WINDOW_SIZE * 3)
    save_data_to_image(large, f"images/large_{i}", (12, 4))
    
    for k in range(10):
        small = get_rand_range(WINDOW_SIZE)
        save_data_to_image(small, f"images/small_{i}_{k}", (4,4))
