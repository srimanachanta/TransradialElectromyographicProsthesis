import os

import matplotlib.pyplot as plt
import numpy as np

measure_len_seconds = {
    "solo_finger": 12.0,
    "thumb_duo": 12.0,
    "index_duo": 10.0,
    "middle_duo": 10.0,
    "pinkie_duo": 10.0,
    "solo_finger_press": 10.0,
    "thumb_duo_press": 10.0,
    "index_duo_press": 10.0,
    "middle_duo_press": 10.0,
    "ring_duo_press": 10.0,
    "pinkie_duo_press": 10.0,
    "wrist": 10.0,
    "fist": 10.0,
    "pickup_ball_palm": 5.0,
    "throw_ball_palm": 10.0,
    "piano_twinkle": 30.0,
}

video_len_seconds = {
    "solo_finger": 11.628,
    "thumb_duo": 11.577,
    "index_duo": 9.605,
    "middle_duo": 9.573,
    "pinkie_duo": 9.647,
    "solo_finger_press": 9.678,
    "thumb_duo_press": 9.510,
    "index_duo_press": 9.578,
    "middle_duo_press": 9.745,
    "ring_duo_press": 9.635,
    "pinkie_duo_press": 9.737,
    "wrist": 9.643,
    "fist": 9.670,
    "pickup_ball_palm": 4.602,
    "throw_ball_palm": 9.620,
    "piano_twinkle": 30.102,
}

labels = ["Flexor Digitorum Superficialis", "Flexor Pollicis Longus", "Flexor Carpi Ulnaris", "Flexor Digitorum Profundus", "Extensor Digitorum Communis", "Extensor Digiti Minimi",
          "Extensor Indicis Proprius", "Extensor Pollicis Longus", "Extensor Pollicis Brevis", "Extensor Carpi Ulnaris", "Extensor Carpi Radialis Brevis",
          "Extensor Carpi Radialis Longus"]


# 0: no movement
# 1: extending
# 2: flexing

# "t_distal",
# "t_proximal",
# "p0_distal",
# "p0_proximal",
# "p1_distal",
# "p1_proximal",
# "p2_distal",
# "p2_proximal",
# "p3_distal",
# "p3_proximal",
# "w"

def generate_y_data(base_data_dir: str, video_start_frame_idx: int, video_len_s: float, annotations: list[list[tuple[float, int]]], crop_time_seconds=None):
    x_data = np.load(os.path.join(base_data_dir, "muscle_data.npy"))
    y_data = np.zeros((x_data.shape[0], 11))

    # Its vary likely the set sampling rate isn't accurate so calculate a rough approximate using the length of the video and the number of samples collected in that time
    video_sampling_rate = (x_data.shape[0] - video_start_frame_idx) // video_len_s

    for joint_idx, joint_annotations in enumerate(annotations):
        y_data[0:, joint_idx] = joint_annotations[0][1]

        # If there is a trailing annotation, fill the buffer with the value after that timestamp
        if len(joint_annotations) % 2 == 0:
            last_annotation = joint_annotations[-1]
            last_frame_idx = video_start_frame_idx + int(last_annotation[0] * video_sampling_rate)

            y_data[last_frame_idx:, joint_idx] = last_annotation[1]

        for i in range(len(joint_annotations) - 1):
            lower_sample = joint_annotations[i]
            upper_sample = joint_annotations[i + 1]

            lower_frame_idx = video_start_frame_idx + int(lower_sample[0] * video_sampling_rate)
            upper_frame_idx = video_start_frame_idx + int(upper_sample[0] * video_sampling_rate)

            y_data[lower_frame_idx:upper_frame_idx, joint_idx] = lower_sample[1]

    print(f"Saving y-data {base_data_dir} @ VidoSPS {video_sampling_rate}sps")
    np.save(os.path.join(base_data_dir, "finger_state.npy"), y_data)

    if crop_time_seconds is not None:
        crop_frame_idx = video_start_frame_idx + int(crop_time_seconds * video_sampling_rate)

        x_data = x_data[:crop_frame_idx, :]
        y_data = y_data[:crop_frame_idx, :]

        print("Also saving cropped data")

        np.save(os.path.join(base_data_dir, "muscle_data_cropped.npy"), x_data)
        np.save(os.path.join(base_data_dir, "finger_state_cropped.npy"), y_data)


def plot_data(data: np.ndarray):
    plt.plot(np.arange(data.shape[0]), data)
    plt.legend(labels, bbox_to_anchor=(1.02, 0.1), loc='upper left', borderaxespad=0)
    plt.show()


# plot_data(np.load("data/fist/muscle_data.npy"))

# solo_finger
# thumb_duo
# index_duo
# middle_duo
# pinkie_duo
# solo_finger_press

# The timestamp of the annotations are relative to the beginning the timestamp
# TODO find relationship between start of the video and what frame that is

generate_y_data("../../data/solo_finger", 0, video_len_seconds["solo_finger"], [
    [(-1, 2), (0.433, 1), (0.5, 0)],
    [(-1, 2), (0.267, 1), (0.733, 2), (1.700, 1), (2.867, 2)],

    [(-1, 2), (1.8, 1), (2.567, 2)],
    [(-1, 2), (1.733, 1), (2.667, 2)],

    [(-1, 2), (3.733, 1), (4.533, 2)],
    [(-1, 2), (3.533, 1), (4.433, 2)],

    [(-1, 2), ],
    [(-1, 2), ],

    [(-1, 2), (5.367, 1), (6.402, 2)],
    [(-1, 2), (5.233, 1), (6.268, 2)],

    [(-1, 1), ]
], 6.902)
generate_y_data("../../data/thumb_duo", 0, video_len_seconds["thumb_duo"], [
    [(-1, 2), (0.5, 1), (1.233, 2), (2.233, 1), (3.100, 2), (3.867, 1), (4.967, 2)],
    [(-1, 2), (0.5, 1), (1.233, 2), (2.233, 1), (3.100, 2), (3.867, 1), (4.967, 2)],

    [(-1, 2), (0.6, 1), (1.067, 2)],
    [(-1, 2), (0.533, 1), (1.267, 2)],

    [(-1, 2), (2.233, 1), (3.033, 2)],
    [(-1, 2), (2.133, 1), (3.033, 2)],

    [(-1, 2), ],
    [(-1, 2), ],

    [(-1, 2), (3.8, 1), (4.967, 2)],
    [(-1, 2), (3.967, 1), (4.8, 2)],

    [(-1, 1), ]
], 5.735)
generate_y_data("../../data/index_duo", 0, video_len_seconds["index_duo"], [
    [(-1, 2), (0.667, 1), (1.367, 2)],
    [(-1, 2), (0.667, 1), (1.367, 2)],

    [(-1, 2), (0.767, 1), (1.3, 2), (2.4, 1), (3.333, 2), (4.233, 1), (5.233, 2)],
    [(-1, 2), (0.733, 1), (1.5, 2), (2.467, 1), (2.233, 2), (4.3, 1), (5.2, 2)],

    [(-1, 2), (2.4, 1), (3.333, 2)],
    [(-1, 2), (2.3, 1), (2.233, 2)],

    [(-1, 2), ],
    [(-1, 2), ],

    [(-1, 2), (4.233, 1), (5.133, 2)],
    [(-1, 2), (4.167, 1), (5.133, 2)],

    [(-1, 1), ]
], 5.835)
generate_y_data("../../data/middle_duo", 0, video_len_seconds["middle_duo"], [
    [(-1, 2), (.233, 1), (1.0, 2)],
    [(-1, 2), (.233, 1), (1.0, 2), (2.867, 1), (3.2, 2)],

    [(-1, 2), (0.3, 1), (1.067, 2), (1.967, 1), (3.221, 2)],
    [(-1, 2), (.233, 1), (1.0, 2), (1.933, 1), (3.0, 2)],

    [(-1, 2), (0.3, 1), (1.1, 2), (1.967, 1), (3.123, 2)],
    [(-1, 2), (.233, 1), (1.0, 2), (1.933, 1), (2.867, 2)],

    [(-1, 2), ],
    [(-1, 2), ],

    [(-1, 2), ],
    [(-1, 2), ],

    [(-1, 1), ]
], 3.822)
generate_y_data("../../data/pinkie_duo", 0, video_len_seconds["pinkie_duo"], [
    [(-1, 2), (.6, 1), (1.3, 2)],
    [(-1, 2), (.6, 1), (1.3, 2), (2.567, 1), (3.633, 2)],

    [(-1, 2), (2.567, 1), (3.433, 2)],
    [(-1, 2), (2.6, 1), (3.467, 2)],

    [(-1, 2), (4.5, 1), (5.6, 2)],
    [(-1, 2), (4.443, 1), (5.735, 2)],

    [(-1, 2), (4.5, 1), (5.6, 2)],
    [(-1, 2), (4.443, 1), (5.768, 2)],

    [(-1, 2), (0.7, 1), (1.367, 2), (2.5, 1), (3.5, 2), (4.5, 1), (5.6, 2)],
    [(-1, 2), (0.633, 1), (1.3, 2), (2.367, 1), (3.433, 2), (4.443, 1), (5.668, 2)],

    [(-1, 1), ]
], 6.502)

generate_y_data("../../data/solo_finger_press", 0, video_len_seconds["solo_finger_press"], [
    [(-1, 0), (0.233, 1), (0.567, 2), (0.7, 0)],
    [(-1, 0), (0.233, 1), (0.567, 2), (0.7, 0)],
    [(-1, 0), (1.067, 1), (1.333, 2), (1.1467, 0)],
    [(-1, 0), (1.067, 1), (1.333, 2), (1.1467, 0)],
    [(-1, 0), (1.833, 1), (2.2, 2), (2.33, 0)],
    [(-1, 0), (1.833, 1), (2.2, 2), (2.33, 0)],
    [(-1, 0), (2.667, 1), (3.067, 2), (3.167, 0)],
    [(-1, 0), (2.667, 1), (3.067, 2), (3.167, 0)],
    [(-1, 0), (3.567, 1), (4, 2), (4.133, 0)],
    [(-1, 0), (3.567, 1), (4, 2), (4.133, 0)],
    [(-1, 0), ]
], 4.8)
generate_y_data("../../data/thumb_duo_press", 0, video_len_seconds["thumb_duo_press"], [
    [(-1, 0), (0.0, 1), (0.433, 2), (0.6, 0), (1.167, 1), (1.633, 2), (1.867, 0), (2.267, 1), (2.933, 2), (3.233, 0), (3.7, 1), (4.167, 2), (4.4, 0)],
    [(-1, 0), (0.0, 1), (0.433, 2), (0.6, 0), (1.167, 1), (1.633, 2), (1.867, 0), (2.267, 1), (2.933, 2), (3.233, 0), (3.7, 1), (4.167, 2), (4.4, 0)],
    [(-1, 0), (0.033, 1), (0.433, 2), (0.6, 0)],
    [(-1, 0), (0.033, 1), (0.433, 2), (0.6, 0)],
    [(-1, 0), (1.2, 1), (1.733, 2), (1.867, 0)],
    [(-1, 0), (1.2, 1), (1.733, 2), (1.867, 0)],
    [(-1, 0), (2.433, 1), (3.033, 2), (3.233, 0)],
    [(-1, 0), (2.433, 1), (3.033, 2), (3.233, 0)],
    [(-1, 0), (3.7, 1), (4.233, 2), (4.333, 0)],
    [(-1, 0), (3.7, 1), (4.233, 2), (4.333, 0)],
    [(-1, 0), ]
], 5.1)
generate_y_data("../../data/index_duo_press", 0, video_len_seconds["index_duo_press"], [
    [(-1, 0), (0.0, 1), (0.567, 2), (0.733, 0)],
    [(-1, 0), (0.0, 1), (0.567, 2), (0.733, 0)],
    [(-1, 0), (0.067, 1), (0.533, 2), (0.667, 0), (1.167, 1),  (1.667, 2), (1.833, 0), (2.133, 1), (2.767, 2), (2.933, 0), (3.233, 1), (3.9, 2), (4.067, 0)],
    [(-1, 0), (0.067, 1), (0.533, 2), (0.667, 0), (1.167, 1),  (1.667, 2), (1.833, 0), (2.133, 1), (2.767, 2), (2.933, 0), (3.233, 1), (3.9, 2), (4.067, 0)],
    [(-1, 0), (1.167, 1), (1.667, 2), (1.833, 0)],
    [(-1, 0), (1.167, 1), (1.667, 2), (1.833, 0)],
    [(-1, 0), (2.133, 1), (2.733, 2), (2.933, 0)],
    [(-1, 0), (2.133, 1), (2.733, 2), (2.933, 0)],
    [(-1, 0), (3.367, 1), (3.9, 2), (4.067, 0)],
    [(-1, 0), (3.367, 1), (3.9, 2), (4.067, 0)],
    [(-1, 0), ]
])
generate_y_data("../../data/middle_duo_press", 0, video_len_seconds["middle_duo_press"], [
    [(-1, 0), (0.07, 1), (1.633, 2), (1.8, 0)],
    [(-1, 0), (0.07, 1), (1.633, 2), (1.8, 0)],
    [(-1, 0), (2.133, 1), (2.933, 2), (3.1, 0), (3.3, 1), (4.333, 2), (4.533, 0)],
    [(-1, 0), (2.133, 1), (2.933, 2), (3.1, 0), (3.3, 1), (4.333, 2), (4.533, 0)],
    [(-1, 0), (0.087, 1), (1.6, 2), (1.767, 0), (2.233, 1), (2.9, 2), (3.067, 0), (5.5, 1), (6.802, 2), (7.002, 0)],
    [(-1, 0), (0.087, 1), (1.6, 2), (1.767, 0), (2.233, 1), (2.9, 2), (3.067, 0), (5.5, 1), (6.802, 2), (7.002, 0)],
    [(-1, 0), (3.467, 1), (4.067, 2), (4.533, 0)],
    [(-1, 0), (3.467, 1), (4.067, 2), (4.533, 0)],
    [(-1, 0), (5.5, 1), (6.835, 2), (7.068, 0)],
    [(-1, 0), (5.5, 1), (6.835, 2), (7.068, 0)],
    [(-1, 0), ]
], 7.368)
generate_y_data("../../data/ring_duo_press", 0, video_len_seconds["ring_duo_press"], [
    [(-1, 0), (0.433, 1), (1.333, 2), (1.533, 0)],
    [(-1, 0), (0.433, 1), (1.333, 2), (1.533, 0)],
    [(-1, 0), (2.367, 1), (2.933, 2), (3.267, 0)],
    [(-1, 0), (2.367, 1), (2.933, 2), (3.267, 0)],
    [(-1, 0), (3.6, 1), (4.233, 2), (4.4, 0)],
    [(-1, 0), (3.6, 1), (4.233, 2), (4.4, 0)],
    [(-1, 0), (0.5, 1), (1.367, 2), (1.567, 0), (2.367, 1), (3.0, 2), (3.233, 0), (3.567, 1), (4.233, 2), (4.433, 0), (4.8, 1), (5.367, 2), (5.533, 0)],
    [(-1, 0), (0.5, 1), (1.367, 2), (1.567, 0), (2.367, 1), (3.0, 2), (3.233, 0), (3.567, 1), (4.233, 2), (4.433, 0), (4.8, 1), (5.367, 2), (5.533, 0)],
    [(-1, 0), (4.8, 1), (5.367, 2), (5.636, 0)],
    [(-1, 0), (4.8, 1), (5.367, 2), (5.636, 0)],
    [(-1, 0), ]
], 5.668)
generate_y_data("../../data/pinkie_duo_press", 0, video_len_seconds["pinkie_duo_press"], [
    [(-1, 0), (0.233, 1), (0.933, 2), (1.1, 0)],
    [(-1, 0), (0.233, 1), (0.933, 2), (1.1, 0)],
    [(-1, 0), (1.467, 1), (2, 2), (2.133, 0)],
    [(-1, 0), (1.467, 1), (2, 2), (2.133, 0)],
    [(-1, 0), (2.8, 1), (3.567, 2), (3.667, 0)],
    [(-1, 0), (2.8, 1), (3.567, 2), (3.667, 0)],
    [(-1, 0), (4.267, 1), (4.9, 2), (5.033, 0)],
    [(-1, 0), (4.267, 1), (4.9, 2), (5.033, 0)],
    [(-1, 0), (0.4, 1), (0.9, 2), (1.033, 0), (1.467, 1), (2, 2), (2.133, 0), (2.9, 1), (3.5, 2), (3.633, 0), (4.1, 1), (4.9, 2), (5.033, 0)],
    [(-1, 0), (0.4, 1), (0.9, 2), (1.033, 0), (1.467, 1), (2, 2), (2.133, 0), (2.9, 1), (3.5, 2), (3.633, 0), (4.1, 1), (4.9, 2), (5.033, 0)],
    [(-1, 0), ]
])

generate_y_data("../../data/fist", 0, video_len_seconds["fist"], [
    [(-1, 2), (1.533, 1), (2.367, 2), (3.433, 1), (4.267, 2), (5.3, 1), (5.935, 2)],
    [(-1, 2), (1.533, 1), (2.367, 2), (3.433, 1), (4.267, 2), (5.3, 1),  (5.935, 2)],
    [(-1, 2), (1.533, 1), (2.2, 2), (3.433, 1), (4.133, 2), (5.3, 1),  (5.935, 2)],
    [(-1, 2), (1.533, 1), (2.2, 2), (3.433, 1), (4.133, 2), (5.3, 1),  (5.935, 2)],
    [(-1, 2), (1.533, 1), (2.2, 2), (3.433, 1), (4.133, 2), (5.3, 1),  (5.935, 2)],
    [(-1, 2), (1.533, 1), (2.2, 2), (3.433, 1), (4.133, 2), (5.3, 1),  (5.935, 2)],
    [(-1, 2), (1.533, 1), (2.2, 2), (3.433, 1), (4.133, 2), (5.3, 1),  (5.935, 2)],
    [(-1, 2), (1.533, 1), (2.2, 2), (3.433, 1), (4.133, 2), (5.3, 1),  (5.935, 2)],
    [(-1, 2), (1.533, 1), (2.2, 2), (3.433, 1), (4.133, 2), (5.3, 1),  (5.935, 2)],
    [(-1, 2), (1.533, 1), (2.2, 2), (3.433, 1), (4.133, 2), (5.3, 1),  (5.935, 2)],
    [(-1, 1), ]
], 7.268)
generate_y_data("../../data/wrist", 0, video_len_seconds["wrist"], [
    [(-1, 1), ],
    [(-1, 1), ],
    [(-1, 1), ],
    [(-1, 1), ],
    [(-1, 1), ],
    [(-1, 1), ],
    [(-1, 1), ],
    [(-1, 1), ],
    [(-1, 1), ],
    [(-1, 1), ],
    [(-1, 1), (0.5, 2), (1.2, 1), (2.6, 2), (2.867, 1), (3.533, 2), (4.233, 1), (5.433, 2), (5.935, 1), (6.502, 2), (7.168, 1), (8.402, 2), (8.768, 1)]
])

generate_y_data("../../data/pickup_ball_palm", 0, video_len_seconds["pickup_ball_palm"], [
    [(-1, 1), (1.4, 2), (3.967, 1)],
    [(-1, 1), (1.4, 2), (3.967, 1)],

    [(-1, 1), (1.867, 1), (4.033, 1)],
    [(-1, 1), (1.4, 2), (4.067, 1)],

    [(-1, 1), (1.867, 1), (4.033, 1)],
    [(-1, 1), (1.4, 2), (4.067, 1)],

    [(-1, 1), (1.867, 1), (4.033, 1)],
    [(-1, 1), (1.4, 2), (4.067, 1)],

    [(-1, 1), (1.867, 1), (4.033, 1)],
    [(-1, 1), (1.4, 2), (4.067, 1)],

    [(-1, 1)],
])
generate_y_data("../../data/throw_ball_palm", 0, video_len_seconds["throw_ball_palm"], [
    [(-1, 2), (2.233, 1), (2.7, 0)],
    [(-1, 2), (2.333, 1), (2.7, 0)],

    [(-1, 2), (2.233, 1), (2.7, 0)],
    [(-1, 2), (2.333, 1), (2.7, 0)],

    [(-1, 2), (2.233, 1), (2.7, 0)],
    [(-1, 2), (2.333, 1), (2.7, 0)],

    [(-1, 2), (2.233, 1), (2.7, 0)],
    [(-1, 2), (2.333, 1), (2.7, 0)],

    [(-1, 2), (2.333, 1), (2.7, 0)],
    [(-1, 2), (1.8, 1), (2.7, 0)],

    [(-1, 1), (2.4, 2), (3.033, 0)]
], 3.533)

# generate_y_data("../../data/piano_twinkle", 0, video_len_seconds["piano_twinkle"], [
#     [(-1, 0), ],
#     [(-1, 0), ],
#
#     [(-1, 0), ],
#     [(-1, 0), ],
#
#     [(-1, 0), ],
#     [(-1, 0), ],
#
#     [(-1, 0), ],
#     [(-1, 0), ],
#
#     [(-1, 0), ],
#     [(-1, 0), ],
#
#     [(-1, 0), ]
# ])
