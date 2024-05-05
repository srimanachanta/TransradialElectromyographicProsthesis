import torch

from dataUtils import *
from model import TransformerModel
from train import Trainer

epochs = 250
batch_size = 64

loaders = get_dataloaders(batch_size)

device = torch.device(
    "cuda"
    if torch.cuda.is_available()
    else "mps" if torch.backends.mps.is_available() else "cpu"
)
encoder = TransformerModel().to(device)
encoder.load_state_dict(torch.load("../../model_weights/weights.pt"))

weights = torch.zeros(3)
weights[0] = 0.2
weights[1] = 0.7
weights[2] = 0.2

optimizer_contrastive = torch.optim.Adam(encoder.parameters(), lr=0.0001)
loss_classifier = [torch.nn.CrossEntropyLoss(weight=weights) for _ in range(5)]

weights[0] = 0.49
weights[1] = 0.2
weights[2] = 0.49

loss_classifier.append(torch.nn.CrossEntropyLoss(weight=weights))
trainer = Trainer(
    optimizer_contrastive,
    loss_classifier,
    torch.device("cuda:0" if torch.cuda.is_available() else "cpu"),
    encoder,
    epochs,
)

trainer.Train_Test(loaders["Train"], loaders["Test"])
