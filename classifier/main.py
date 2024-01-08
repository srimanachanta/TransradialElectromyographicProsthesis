from Train import Trainer
from dataUtils import get_dataloaders
from model import TransformerModel, Classifier
import torch
from pytorch_metric_learning.losses import NtXentLoss

epochs = 250
batch_size = 64

loaders = get_dataloaders(batch_size)

classifier = Classifier()

optimizer_contrastive = torch.optim.Adam(encoder.parameters(), lr=0.001)
optimizer_classifier = torch.optim.Adam(classifier.parameters(), lr=0.001)
loss_contrastive = NtXentLoss(temperature=0.1)
loss_classifier = torch.nn.BCELoss()()

Model_Trainer = Trainer(optimizer_contrastive, optimizer_classifier, loss_contrastive,
                        loss_classifier, torch.device("cuda:0" if torch.cuda.is_available() else "cpu"),
                        encoder, classifier, epochs)