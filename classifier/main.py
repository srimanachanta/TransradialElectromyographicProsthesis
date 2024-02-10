from Train import Trainer
from dataUtils import *
from model import TransformerModel
import torch
import timeit
import matplotlib.pyplot as plt

data = get_dataset("pickup_ball_palm")

epochs = 250
batch_size = 64

loaders = get_dataloaders(batch_size)

encoder = TransformerModel().to("cuda:0")
encoder.load_state_dict(torch.load("encoding_weights/weight.pt"))


# start_time = timeit.default_timer()
#
# step = 0
#
# for _, batch in enumerate(loaders["Train"]):
#     x = batch[0].to("cuda:0")
#
#     with torch.no_grad():
#         pred = encoder(x)
#     step += 1
#
# end_time = timeit.default_timer()

# print(end_time - start_time)
# print((end_time - start_time)/step)

# weights = torch.zeros(3)
# weights[0] = 0.2
# # weights[0] = 0.02
# weights[1] = 0.7
# weights[2] = 0.2
#
# optimizer_contrastive = torch.optim.Adam(encoder.parameters(), lr=0.0001)
# loss_classifier = [torch.nn.CrossEntropyLoss(weight=weights) for _ in range(5)]
#
# weights[0] = 0.49
# weights[1] = 0.2
# weights[2] = 0.49
#
# loss_classifier.append(torch.nn.CrossEntropyLoss(weight=weights))
# Model_Trainer = Trainer(optimizer_contrastive, loss_classifier,
#                         torch.device("cuda:0" if torch.cuda.is_available() else "cpu"), encoder, epochs)
#
# Model_Trainer.Train_Test(loaders["Train"], loaders["Test"])

# data_root_path = "../bridge/logger/data"
# x_file_name = "muscle_data_normalized.npy"
# y_file_name = "finger_state.npy"
#
messups = []

# for i in range(len(data)):
#     if not torch.equal(torch.round(encoder(torch.from_numpy(data[i][0]).unsqueeze(0).to("cuda:0"))).cpu(), torch.from_numpy(data[i][1][[1, 3, 5, 7, 9, 10]]).cpu()):
#         print(encoder(torch.from_numpy(data[i][0]).unsqueeze(0).to("cuda:0")).cpu(), torch.from_numpy(data[i][1][[1, 3, 5, 7, 9, 10]]).cpu())


for i in range(len(data)):
    pred = encoder(torch.from_numpy(data[i][0]).unsqueeze(0).to("cuda:0")).detach().cpu()[:, :].type(torch.float64).squeeze()
    y = torch.from_numpy(data[i][1][[3, 5, 7, 9, 10]]).cpu()

    if not torch.equal(torch.round(pred), y):
        messups.append(i)

plt.plot(data.x_data, color="blue")

for i in messups:
    plt.axvline(i, color="red")

plt.show()