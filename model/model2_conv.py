# -*- coding: UTF-8 -*-

import torch
from torch import nn


class Attention(nn.Module):
    def __init__(self, channel_num):
        super().__init__()
        self.name = 'Attention'
        self.tanh = nn.Tanh()
        self.channel_num = channel_num
        self.sigmoid = nn.Sigmoid()
        self.softmax = nn.Softmax(dim=-1)
        self.fc = nn.Sequential(
            nn.Linear(self.channel_num, 1),
            nn.Tanh(),
            nn.Sigmoid()
        )

    def forward(self, inputs):
        score = inputs.permute(0, 2, 1)
        score = score.contiguous().view(-1, self.channel_num)
        score = self.fc(score)
        output_score = score

        score = score.contiguous().view(inputs.shape[0], 1, -1)
        # score = self.softmax(score)

        score = score.expand_as(inputs)
        out = score * inputs

        return out, output_score


class Block(nn.Module):
    def __init__(self, in_channel, out_channel, stride, kernel_size=3):
        super().__init__()
        self.name = 'Block'
        self.block0 = nn.Conv1d(in_channel, out_channel, kernel_size, stride=stride, padding=(kernel_size - 1) // 2)
        self.bn = nn.BatchNorm1d(out_channel)
        self.relu = nn.ReLU()
        self.drop = nn.Dropout(p=0.2)

        for m in self.modules():
            if isinstance(m, nn.Conv1d):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
            if isinstance(m, nn.BatchNorm1d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

    def forward(self, inputs):
        x = self.block0(inputs)
        x = self.bn(x)
        x = self.relu(x)
        # x = self.drop(x)
        return x


class LayerBlock(nn.Module):
    def __init__(self, in_channel, out_channel, stride, kernel_size=3):
        super().__init__()
        self.name = 'LayerBlock'
        self.block0 = Block(in_channel, out_channel, 1, kernel_size)
        self.block1 = Block(out_channel, out_channel, 1, kernel_size)
        self.m = Block(out_channel, out_channel, stride, 1)

    def forward(self, inputs):
        x = self.block0(inputs)
        x = self.block1(x)
        x = self.m(x)

        return x


class RefineBlock(nn.Module):
    def __init__(self, in_channel, out_channel):
        super().__init__()
        self.name = 'RefineBlock'
        self.block0 = nn.Linear(in_channel, out_channel)
        self.bn = nn.BatchNorm1d(out_channel)
        self.relu = nn.ReLU()

        for m in self.modules():
            if isinstance(m, nn.Linear):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
            if isinstance(m, nn.BatchNorm1d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

    def forward(self, inputs):
        x = self.block0(inputs)
        x = self.bn(x)
        x = self.relu(x)
        return x


class SNet(nn.Module):
    def __init__(self):
        super().__init__()
        self.name = 'SNet'

        self.position_embedding = nn.Embedding(128, 1)
        self.block0 = LayerBlock(1, 16, 4, 7)
        self.attention1 = Attention(16)
        self.block1 = LayerBlock(16, 32, 2, 3)
        self.attention2 = Attention(32)
        self.block2 = LayerBlock(32, 64, 2, 3)
        self.attention3 = Attention(64)
        self.block3 = LayerBlock(64, 128, 2, 3)
        self.attention4 = Attention(128)
        self.block4 = LayerBlock(128, 128, 2, 3)
        self.attention5 = Attention(128)
        # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        # neural network input dimension matters here
        # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        # self.layer1 = RefineBlock(256 * 2 + 5, 128)
        self.layer1 = RefineBlock(256 * 2, 128)
        self.layer2 = RefineBlock(128, 256)
        self.fc = nn.Linear(256, 2)

    def pre_forward(self, inputs):
        # inputs: tensor(256,256)
        inputs = inputs.unsqueeze(1)  # (256, 1, 256)       N*64, 1, 128
        x = self.block0(inputs)  # (256, 16, 64)
        x, _ = self.attention1(x)  # (256, 16, 64)
        x = self.block1(x)  # (256, 32, 32)
        x, _ = self.attention2(x)  # (256, 32, 32）
        x = self.block2(x)  # (256, 64, 16)
        x, _ = self.attention3(x)  # (256, 64, 16)
        x = self.block3(x)  # (256, 128, 8)
        x, _ = self.attention4(x)  # (256, 128, 8)
        x = self.block4(x)  # (256, 128, 4)
        x, _ = self.attention5(x)  # (256, 128, 8)
        x = x.view(x.shape[0], -1)  # (256, 512) ???????????????????????????????????????????????????????????????????????
        return x

    def forward(self, inputs):
        # inputs: tensor(256,256)
        x = self.pre_forward(inputs)  # (256, 512)
        x = self.layer1(x)  # (256, 128)
        x = self.layer2(x)  # (256, 256)
        x = self.fc(x)  # (256, 2)
        return x
