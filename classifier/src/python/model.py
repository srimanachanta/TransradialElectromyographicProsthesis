from torch import nn
import torch
import math
import torch.nn.functional as F


class PositionalEncoding(nn.Module):
    def __init__(self, dimensions_model, dropout=0.0, max_len=12, position=torch.zeros(512)):
        super(PositionalEncoding, self).__init__()
        self.dropout = nn.Dropout(p=dropout)

        self.dimensions_model = dimensions_model
        self.max_len = max_len

        position = torch.arange(max_len).unsqueeze(1)
        div_term = torch.exp(torch.arange(0, dimensions_model, 2) * (-math.log(10000.0) / dimensions_model))
        pe = torch.zeros(max_len, 1, dimensions_model)
        pe[:, 0, 0::2] = torch.sin(position * div_term)
        pe[:, 0, 1::2] = torch.cos(position * div_term)
        self.register_buffer('pe', pe)

    def forward(self, x):
        x = x + self.pe[:x.size(1)].view(1, self.max_len, self.dimensions_model).repeat(x.shape[0], 1, 1)
        return self.dropout(x)


class TransformerModel(nn.Module):
    def __init__(self, input_dim=1, d_model=32, heads=1, activation="relu", dropout=0.2):
        super(TransformerModel, self).__init__()

        self.d_model = d_model
        self.heads = heads
        self.signal_dim = 256

        self.encoder_whole = nn.Linear(512, d_model)
        self.encoder_whole_vertical = nn.Linear(384, d_model)

        self.pos_encoder1 = PositionalEncoding(d_model, dropout)
        self.pos_encoder2 = PositionalEncoding(d_model, dropout, max_len=16)

        self.encoder_layer1 = nn.TransformerEncoderLayer(d_model, heads, activation=activation, dim_feedforward=256)
        self.encoder_layer2 = nn.TransformerEncoderLayer(d_model, heads, activation=activation, dim_feedforward=256)

        self.head_transform1 = nn.Linear(d_model, int(d_model / heads))
        self.head_transform2 = nn.Linear(d_model, int(d_model / heads))

        self.signal_linear = nn.Linear(int(d_model / heads), self.signal_dim)

        self.Linear1 = nn.Sequential(nn.Linear(self.signal_dim * 3 + 16 * int(d_model / heads), 256),
                                     # self.Linear1 = nn.Sequential(nn.Linear(self.signal_dim * 3, 256),
                                     nn.ELU(),
                                     nn.Linear(256, 128),
                                     nn.ELU(),
                                     nn.Linear(128, 3)
                                     )
        self.Linear2 = nn.Sequential(nn.Linear(self.signal_dim * 4 + 16 * int(d_model / heads), 256),
                                     # self.Linear2 = nn.Sequential(nn.Linear(self.signal_dim * 4, 256),
                                     nn.ELU(),
                                     nn.Linear(256, 128),
                                     nn.ELU(),
                                     nn.Linear(128, 3)
                                     )

        self.Linear3 = nn.Sequential(nn.Linear(self.signal_dim * 3 + 16 * int(d_model / heads), 256),
                                     # self.Linear3 = nn.Sequential(nn.Linear(self.signal_dim * 3, 256),
                                     nn.ELU(),
                                     nn.Linear(256, 128),
                                     nn.ELU(),
                                     nn.Linear(128, 6)
                                     )

        self.Linear4 = nn.Sequential(nn.Linear(self.signal_dim * 4 + 16 * int(d_model / heads), 256),
                                     # self.Linear4 = nn.Sequential(nn.Linear(self.signal_dim * 4, 256),
                                     nn.ELU(),
                                     nn.Linear(256, 128),
                                     nn.ELU(),
                                     nn.Linear(128, 3)
                                     )

        self.Linear5 = nn.Sequential(nn.Linear(self.signal_dim * 4 + 16 * int(d_model / heads), 256),
                                     # self.Linear5 = nn.Sequential(nn.Linear(self.signal_dim * 4, 256),
                                     nn.ELU(),
                                     nn.Linear(256, 128),
                                     nn.ELU(),
                                     nn.Linear(128, 3)
                                     )

    def forward(self, x):
        data_encoded = F.relu(self.encoder_whole(x.view(x.shape[0], 12, 512)))
        data_encoded_positional = self.pos_encoder1(data_encoded)
        x_embedded = self.encoder_layer1(data_encoded_positional)
        signal_list = self.head_transform1(x_embedded)

        data_encoded = F.relu(self.encoder_whole_vertical(torch.flatten(x.view(x.shape[0], 16, 32, 12), 2, 3)))
        data_encoded_positional = self.pos_encoder2(data_encoded)
        x_embedded = self.encoder_layer2(data_encoded_positional)
        vertical_signals = self.head_transform2(x_embedded)

        signal_list = self.signal_linear(signal_list.view(signal_list.shape[0], 12, -1)).view(signal_list.shape[0], -1,
                                                                                              12)

        signal_list1 = torch.cat([signal_list[:, :, [1, 7, 8]].view(signal_list.shape[0], -1),
                                  vertical_signals.view(signal_list.shape[0], -1)], dim=1)
        signal_list2 = torch.cat([signal_list[:, :, [0, 3, 4, 6]].view(signal_list.shape[0], -1),
                                  vertical_signals.view(signal_list.shape[0], -1)], dim=1)
        signal_list3 = torch.cat([signal_list[:, :, [0, 3, 4]].view(signal_list.shape[0], -1),
                                  vertical_signals.view(signal_list.shape[0], -1)], dim=1)
        signal_list4 = torch.cat([signal_list[:, :, [0, 3, 4, 5]].view(signal_list.shape[0], -1),
                                  vertical_signals.view(signal_list.shape[0], -1)], dim=1)
        signal_list5 = torch.cat([signal_list[:, :, [2, 9, 10, 11]].view(signal_list.shape[0], -1),
                                  vertical_signals.view(signal_list.shape[0], -1)], dim=1)

        out1 = self.Linear1(signal_list1)
        out2 = self.Linear2(signal_list2)
        out3 = self.Linear3(signal_list3)
        out4 = self.Linear4(signal_list4)
        out5 = self.Linear5(signal_list5)

        return torch.concat([F.softmax(out1.view(signal_list.shape[0], -1, 3), dim=-1),
                             F.softmax(out2.view(signal_list.shape[0], -1, 3), dim=-1),
                             F.softmax(out3.view(signal_list.shape[0], -1, 3), dim=-1),
                             F.softmax(out4.view(signal_list.shape[0], -1, 3), dim=-1),
                             F.softmax(out5.view(signal_list.shape[0], -1, 3), dim=-1)], dim=1)
