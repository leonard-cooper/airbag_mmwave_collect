import numpy as np
import datetime as dt
import random
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from matplotlib.pylab import mpl
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk  # NavigationToolbar2TkAgg


from scipy import signal, fft
import tkinter as tk
from radarlib.internal.fft_spectrum import fft_spectrum

data1 = np.load("-230103_233236.npy")
file1 = open("-230103_233236.txt")
timelist = float(file.read())
# （100,1,7,32）
print("----type----")
print(type(data))
print("----shape----")
print(data.shape)
chirps_per_frame = 32

def smooth2nd(x, M):  ##x 为一维数组
    K = round(M / 2 - 0.1)  ##M应为奇数，如果是偶数，则取大1的奇数
    lenX = len(x)
    if lenX < 2 * K + 1:
        print('数据长度小于平滑点数')
    else:
        y = np.zeros(lenX)
        for NN in range(0, lenX, 1):
            startInd = max([0, NN - K])
            endInd = min(NN + K + 1, lenX)
            y[NN] = np.mean(x[startInd:endInd])
            # #y[0]=x[0]
            # #首部保持一致
            # #y[-1]=x[-1]
            # #尾部也保持一致
    return (y)

data = np.transpose(data, (1, 0, 3, 2))
    # (1,100,32,7)

x1 = np.zeros(shape=(1, 7000, 32))
for i in range(0, 7000):
        k = np.mod(i, 7)
        # if k == 0:
        #     k = 7
        # for j in range(0, 32):
        x1[0, i, :] = data[0, int(np.floor((i - 1) / 7)), :, k]
x1 = np.squeeze(x1)
    # print(x1.shape)
meanx1 = np.mean(x1, 1)
    # print(meanx1)
    # meanx1 = np.reshape(meanx1, 700, 1)
meanx1 = np.tile(meanx1, (32, 1))
meanx1 = np.transpose(meanx1, (1, 0))
    # print(meanx1.shape)
x1 = x1 - meanx1
range_win = signal.windows.hann(chirps_per_frame).reshape(chirps_per_frame, 1)  # (32,1)
ScaleHannWin = 1 / sum(range_win)
range_win = np.tile(range_win, (1, 7000))
x1 = np.multiply(x1, np.transpose(range_win))*ScaleHannWin
    # print(x1.shape)
fft1d = np.fft.fft(x1)
fft1d = np.abs(fft1d)
fftMax = fft1d[:, 2]
    # print(fftmax)
    # plt.ion()
    # plt.figure(figsize=(10, 8))
    # plt.plot(fftmax)
    # # plt.pause(10)
    # plt.show()

fs = 7000 / timelist
wl = 0.4 * 2 / fs
wh = 6 * 2 / fs
b, a = signal.butter(4, [wl, wh], 'bandpass')
filtedData = signal.filtfilt(b, a, fftMax, axis=0)  # fftMax为要过滤的信号
    # plt.ion()
    # plt.figure(figsize=(10, 8))
    # plt.plot(filtedData)
    # plt.show()

firstchrip = random.randint(99, 6000)
print(firstchrip)
filtedData = filtedData[firstchrip: firstchrip+699]

temp = smooth2nd(filtedData, np.round(fs))

result = np.subtract(filtedData, temp)

f = plt.figure(figsize=(12, 5))
plt.plot(result)
plt.show()