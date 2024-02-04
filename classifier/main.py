from Train import Trainer
from dataUtils import get_dataloaders
from model import TransformerModel
import torch
from pytorch_metric_learning.losses import NTXentLoss

epochs = 250
batch_size = 64

loaders = get_dataloaders(batch_size)

encoder = TransformerModel()

weights = torch.zeros(3)
weights[0] = 0.0002
weights[1] = 0.4999
weights[2] = 0.4999

optimizer_contrastive = torch.optim.Adam(encoder.parameters(), lr=0.0001)
loss_classifier = [torch.nn.CrossEntropyLoss(weight=weights) for _ in range(11)]

Model_Trainer = Trainer(optimizer_contrastive, loss_classifier,
                        torch.device("cuda:0" if torch.cuda.is_available() else "cpu"), encoder, epochs)

Model_Trainer.Train_Test(loaders["Train"], loaders["Test"])
