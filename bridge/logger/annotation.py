import os.path
import numpy as np


def generate_and_save_y_data(data_dir: str, y_file_name: str, sample_count: int, sampling_rate_sps: int, joint_actions: list[list[tuple[float, int]]]):
    data = np.zeros((11, sample_count))

    for i, joint_action_list in enumerate(joint_actions):
        for timestamp_seconds, action_class in joint_action_list:
            frame_idx = int(timestamp_seconds * sampling_rate_sps)
            data[i, frame_idx:] = action_class

    out_path = os.path.join(data_dir, y_file_name)
    np.save(out_path, data)


def get_dataset_meta(data_dir: str, x_file_name: str, video_len_seconds: float):
    data = np.load(os.path.join(data_dir, x_file_name))
    num_samples = data.shape[1]
    return {
        "sample_count": num_samples,
        "sampling_rate_sps": num_samples // video_len_seconds
    }


if __name__ == "__main__":
    y_name = "finger_state.npy"

