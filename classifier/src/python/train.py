import torch
from tqdm import tqdm
from torchmetrics.classification import MulticlassAccuracy
import numpy as np


class Trainer:
    def __init__(self, optimizer_classifier, loss_classifier, device, encoder, epochs):
        self.device = device
        self.encoder = encoder.float().to(self.device)
        self.optimizer_classifier = optimizer_classifier
        self.loss_classifier = [i.to(self.device) for i in loss_classifier]
        self.epochs = epochs

    def train(self, train):
        running_loss = 0.0
        step = 0
        ACC = MulticlassAccuracy(num_classes=3).to(self.device)
        accurracy = [0.0 for i in range(6)]
        for _, batch in enumerate(tqdm(train)):
            x = batch[0].to(self.device)
            y = batch[1].to(self.device)[:, [1, 3, 5, 7, 9, 10]]

            self.optimizer_classifier.zero_grad()

            pred = self.encoder(x)

            # print(pred[0], y[0])

            loss = self.loss_classifier[0](pred[:, 0], y[:, 0])

            for i in range(1, y.shape[1]):
                loss += self.loss_classifier[i](pred[:, i], y[:, i])

            loss.backward()
            self.optimizer_classifier.step()
            running_loss += loss.item()
            step += 1

            # print(pred, y)

            for i in range(6):
                accurracy[i] += ACC(pred[:, i], y[:, i])

        return running_loss / step, [i/step for i in accurracy]

    def test(self, test):
        running_loss = 0.0
        step = 0
        ACC = MulticlassAccuracy(num_classes=3).to(self.device)
        accurracy = [0.0 for i in range(6)]
        for _, batch in enumerate(tqdm(test)):
            x = batch[0].to(self.device)
            y = batch[1].to(self.device)[:, [1, 3, 5, 7, 9, 10]]
            with torch.no_grad():
                pred = self.encoder(x)

            loss = self.loss_classifier[0](pred[:, 0], y[:, 0])

            for i in range(1, y.shape[1]-1):
                loss += self.loss_classifier[i](pred[:, i], y[:, i])

            running_loss += loss.item()
            step += 1

            # accurracy += ACC(pred, y)

            for i in range(6):
                accurracy[i] += ACC(pred[:, i], y[:, i])

        return running_loss / step, [i/step for i in accurracy]

    def Train_Test(self, train, test):
        print("Training Encoder")
        for i in range(self.epochs):
            loss_train, acc = self.train(train)
            print(f"Epoch {i+1}: Train Loss: {str(loss_train)}, Train Accuracy: {str(acc)}")

            if i % 2 == 0:
                loss_train, acc = self.test(test)
                print(f"Epoch {i + 1}: Test Loss: {str(loss_train)}, Test Accuracy: {str(acc)}")

            torch.save(self.encoder.state_dict(), r"C:\Users\kesch\Desktop\TransradialElectromyographicProsthesis\classifier\encoding_weights\weight.pt")

