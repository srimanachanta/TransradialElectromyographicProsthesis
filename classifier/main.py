from Train import Trainer
from dataUtils import get_dataloaders
from model import TransformerModel
import torch

epochs = 250
batch_size = 64

loaders = get_dataloaders(batch_size)

encoder = TransformerModel()
encoder.load_state_dict(torch.load("encoding_weights/weight.pt"))

weights = torch.zeros(3)
weights[0] = 0.2
# weights[0] = 0.02
weights[1] = 0.49
weights[2] = 0.2

optimizer_contrastive = torch.optim.Adam(encoder.parameters(), lr=0.0001)
loss_classifier = [torch.nn.CrossEntropyLoss(weight=weights) for _ in range(11)]

Model_Trainer = Trainer(optimizer_contrastive, loss_classifier,
                        torch.device("cuda:0" if torch.cuda.is_available() else "cpu"), encoder, epochs)

Model_Trainer.Train_Test(loaders["Train"], loaders["Test"])
