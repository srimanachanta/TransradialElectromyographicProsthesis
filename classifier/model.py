from torch import nn
import torch
import numpy as np


class PositionalEncoding(nn.Module):
    def __init__(self, dimensions_model, dropout=0.1, max_len=512, position=torch.zeros(512)):
        super(PositionalEncoding, self).__init__()
        self.dropout = nn.Dropout(p=dropout)
        pe = torch.zeros(max_len, dimensions_model)
        position = position.unsqueeze(1)
        div_term = torch.exp(torch.arange(0, dimensions_model, 2) * (-np.log(10000.0) / dimensions_model))
        pe[:, 0::2] = torch.sin(position * div_term)
        pe[:, 1::2] = torch.cos(position * div_term)
        self.register_buffer('pe', pe)

    def forward(self, x):
        x = x + self.pe[:x.size(0)]
        return self.dropout(x)


class TransformerModel(nn.Module):
    def __init__(self, input_dim=1, d_model=128, heads=6, activation="relu", dropout=0.2):
        super(TransformerModel, self).__init__()

        self.encoder = nn.Linear(input_dim, d_model)
        self.pos_encoder = PositionalEncoding(d_model, dropout)
        encoder_layer1 = nn.TransformerEncoderLayer(d_model, heads, activation=activation, dim_feedforward=512)
        encoder_layer2 = nn.TransformerEncoderLayer(d_model, heads, dim_feedforward=256)

    def forward(self, x):
        x = self.encoder(x)
        x = self.pos_encoder(x)
        x = self.transformer_encoder(x)
        return x


class Classifier(nn.Module):
    def __init__(self):
        super(Classifier, self).__init__()
        self.decoder = nn.Linear(256, 1)

    def forward(self, x):
        out = self.decoder(x)
        return out
