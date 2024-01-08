import torch
from tqdm import tqdm
from torchmetrics.classification import BinaryAccuracy


class Trainer:
    def __init__(self, optimizer_contrastive, loss_contrastive, optimizer_classifier, loss_classifier, device, encoder, classifier, epochs):
        self.device = device
        self.encoder = encoder.to(self.device)
        self.classifier = classifier.to(self.device)
        self.optimizer_contrastive = optimizer_contrastive
        self.loss_contrastive = loss_contrastive
        self.optimizer_classifier = optimizer_classifier
        self.loss_classifier = loss_classifier
        self.epochs = epochs

    def train_encoder(self, train):
        running_loss = 0.0
        step = 0
        self.encoder.train(mode=True)
        for _, batch in enumerate(tqdm(train)):
            x = batch[0].to(self.device)
            y = batch[0].to(self.device)
            self.optimizer_contrastive.zero_grad()
            pred = self.encoder(x)
            loss = self.loss_contrastive(pred, y)

            loss.backward()
            self.optimizer_contrastive.step()
            running_loss += loss.item()
            step += 1
        return running_loss/step

    def train_classifier(self, train):
        running_loss = 0.0
        step = 0
        self.encoder.train(mode=False)
        self.classifier.train(mode=True)
        acc = 0.0
        for _, batch in enumerate(tqdm(train)):
            x = batch[0].to(self.device)
            y = batch[0].to(self.device)
            self.optimizer_classifier.zero_grad()
            with torch.no_grad:
                pred = self.encoder(x)
            out = self.classifier(pred)
            loss = self.loss_classifier(out, y)

            loss.backward()
            self.optimizer_classifier.step()
            running_loss += loss.item()
            step += 1
            ACC = BinaryAccuracy(multidim_average='samplewise')
            acc += ACC(out, y)
        return running_loss / step, acc / step

    def test_classifier(self, test):
        running_loss = 0.0
        step = 0
        self.encoder.train(mode=False)
        self.classifier.train(mode=False)
        acc = 0.0
        with torch.no_grad:
            for _, batch in enumerate(tqdm(test)):
                x = batch[0].to(self.device)
                y = batch[0].to(self.device)

                pred = self.encoder(x)
                out = self.classifier(pred)

                loss = self.loss_classifier(out, y)
                running_loss += loss.item()
                step += 1
                ACC = BinaryAccuracy(multidim_average='samplewise')
                acc += ACC(out, y)
        return running_loss / step, acc / step

    def Train_Test(self, train, test):
        print("Training Encoder")
        for i in range(self.epochs):
            loss_train = self.train_encoder(train)
            print(f"Epoch {i+1}: Encoder Loss: {str(loss_train)}")
        torch.save(self.encoder.state_dict(), f"weights/encoder/epoch-{self.epochs}-classifier")

        print("Training and Testing Classifier")
        for i in range(25):
            loss_train, auc_train = self.train_classifier(train)
            loss_test, auc_test = self.test_classifier(test)
            print(f"Epoch {i+1}: Training Classifier Loss: {str(loss_train)}, Train ACC: {str(auc_train)}")
            print(f"Epoch {i + 1}: Testing Classifier Loss: {str(loss_test)}, Testing ACC: {str(auc_test)}")
            torch.save(self.classifier.state_dict(), f"weights/classifier/epoch-{self.epochs}-classifier")

