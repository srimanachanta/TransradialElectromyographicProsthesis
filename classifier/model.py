from torch import nn
import torch
import math
import torch.nn.functional as F

class PositionalEncoding(nn.Module):
    def __init__(self, dimensions_model, dropout=0.0, max_len=512, position=torch.zeros(512)):
        super(PositionalEncoding, self).__init__()
        self.dropout = nn.Dropout(p=dropout)

        position = torch.arange(max_len).unsqueeze(1)
        div_term = torch.exp(torch.arange(0, dimensions_model, 2) * (-math.log(10000.0) / dimensions_model))
        pe = torch.zeros(max_len, 1, dimensions_model)
        pe[:, 0, 0::2] = torch.sin(position * div_term)
        pe[:, 0, 1::2] = torch.cos(position * div_term)
        self.register_buffer('pe', pe)

    def forward(self, x):

        x = x + self.pe[:x.size(0)]
        return self.dropout(x)


class TransformerModel(nn.Module):
    def __init__(self, input_dim=1, d_model=64, heads=1, activation="relu", dropout=0.2):
        super(TransformerModel, self).__init__()

        self.d_model = d_model
        self.heads = heads
        self.signal_dim = 256

        self.encoder = nn.Linear(input_dim, d_model)
        self.pos_encoder = PositionalEncoding(d_model, dropout)
        self.encoder_layer1 = nn.TransformerEncoderLayer(d_model, heads, activation=activation, dim_feedforward=256)
        self.head_transform = nn.Linear(d_model, int(d_model / heads))

        self.query = nn.Linear(12, 12)
        self.key = nn.Linear(12, 12)
        self.softmax = nn.Softmax(dim=2)

        self.signal_linear = nn.Linear(512 * int(d_model/heads), self.signal_dim)

        self.Linear1 = nn.Sequential(nn.Linear(self.signal_dim * 3, 256),
                                     nn.ELU(),
                                    # nn.Linear(512, 256),
                                    # nn.ELU(),
                                    nn.Linear(256, 3))

        self.Linear2 = nn.Sequential(nn.Linear(self.signal_dim * 4, 256),
                                     nn.ELU(),
                                    # nn.Linear(512, 256),
                                    # nn.ELU(),
                                    nn.Linear(256, 3))

        self.Linear3 = nn.Sequential(nn.Linear(self.signal_dim * 3, 256),
                                    nn.ELU(),
                                    # nn.Linear(512, 256),
                                    # nn.ELU(),
                                    nn.Linear(256, 6))

        self.Linear4 = nn.Sequential(nn.Linear(self.signal_dim * 4, 256),
                                    nn.ELU(),
                                    # nn.Linear(512, 256),
                                    # nn.ELU(),
                                    nn.Linear(256, 3))

        self.Linear5 = nn.Sequential(nn.Linear(self.signal_dim * 4, 256),
                                    nn.ELU(),
                                    # nn.Linear(512, 256),
                                    # nn.ELU(),
                                    nn.Linear(256, 3))

    def forward(self, x):
        x_list = [i for i in range(12)]

        for i in range(12):
            data_encoded = F.relu(self.encoder(x[:, :, i].view(x.shape[0], 512, 1)))
            data_encoded_positional = self.pos_encoder(data_encoded)
            x_embedded = self.encoder_layer1(data_encoded_positional)
            x_list[i] = self.head_transform(x_embedded)

        x = torch.stack(x_list, dim=-1)

        query = self.query(x.view(x.shape[0], -1, 12)).view(x.shape[0], 12, -1)
        key = self.key(x.view(x.shape[0], -1, 12)).view(x.shape[0], 12, -1)

        scores = torch.bmm(query, key.transpose(1, 2)) / (x.shape[-1] ** 0.5)

        x = torch.bmm(self.softmax(scores), x.view(x.shape[0], -1, 12).transpose(1, 2)).view(x.shape[0], -1, 12)

        x = self.signal_linear(x.view(x.shape[0], 12, -1)).view(x.shape[0], -1, 12)

        out1 = F.softmax(self.Linear1(x[:, :, [1, 7, 8]].view(x.shape[0], -1)).view(x.shape[0], -1, 3), dim=-1)
        out2 = F.softmax(self.Linear2(x[:, :, [0, 3, 4, 6]].view(x.shape[0], -1)).view(x.shape[0], -1, 3), dim=-1)
        out3 = F.softmax(self.Linear3(x[:, :, [0, 3, 4]].view(x.shape[0], -1)).view(x.shape[0], -1, 3), dim=-1)
        out4 = F.softmax(self.Linear4(x[:, :, [0, 3, 4, 5]].view(x.shape[0], -1)).view(x.shape[0], -1, 3), dim=-1)
        out5 = F.softmax(self.Linear5(x[:, :, [2, 9, 10, 11]].view(x.shape[0], -1)).view(x.shape[0], -1, 3), dim=-1)

        return torch.concat([out1, out2, out3, out4, out5], dim=1)
