"""
get filtered pulse wave from raw data
save the filtered pulse wave(only select 0~5s piece) to pictures
"""
import os

import matplotlib.pyplot as plt
import numpy as np
import scipy.io as sio
import statsmodels.tsa.api as smt
from scipy import signal
import shutil
from utils.plotfft import smooth2nd

file_path = r'./UnitTest'
output_path = './output/data_SBP_change'
if os.path.exists(output_path):
    shutil.rmtree(output_path)
os.mkdir(output_path)

for file_name in os.listdir(file_path):
    print('processing:', file_name)
    data = sio.loadmat(os.path.join(file_path, file_name))
    time_list = np.squeeze(data['time_list'])  # (7000,)
    feature_list = data['feature_list']  # (3,7000,32)

    lentocope = 7000
    fs = lentocope / time_list[-1]
    fft1dmax = np.zeros(shape=(3, lentocope))
    ratio_max = -1
    samples_per_chirp = 32
    # set band pass filter parameters
    wl = 0.4 * 2 / fs
    wh = 6 * 2 / fs
    b, a = signal.butter(4, [wl, wh], 'bandpass')
    rx_chosen = -1
    for rxn in range(0, 3):
        # 消除直流分量，消除基线漂移
        x1 = np.squeeze(feature_list[rxn, :, :])
        x1_mean = np.transpose(np.tile(np.mean(x1, 1), (32, 1)), (1, 0))
        x1 = x1 - x1_mean

        # 乘Hann窗以减少频谱的旁瓣泄露 做fft并选择结果中的一部分
        range_win = signal.windows.hann(samples_per_chirp).reshape(samples_per_chirp, 1)
        ScaleHannWin = 1 / sum(range_win)
        range_win = np.tile(range_win, (1, lentocope))  # 32 * 7000
        x1 = np.multiply(x1, np.transpose(range_win))  # 7000 * 32
        fft1d = np.fft.fft(x1, samples_per_chirp, axis=1) * ScaleHannWin  # 7000 * 32
        fft1d = fft1d[:, 2:round(samples_per_chirp / 2)]

        # 取最大值
        fft1dmaxtemp = np.max(abs(fft1d), axis=1)

        # band pass filter
        filtedData = signal.filtfilt(b, a, fft1dmaxtemp, axis=0)

        # 去除数据中的低频趋势，并强调数据中的高频波动
        temp = smooth2nd(filtedData, np.round(fs))
        result = np.subtract(filtedData, temp)

        fft1dmax[rxn, :] = result
        jiange_up = round(fs * 60 / 50)
        jiange_low = round(fs * 60 / 160)
        acf1 = smt.stattools.acf(result, nlags=round(jiange_up))
        Acc_rxn = max(acf1[jiange_low:jiange_up])
        if Acc_rxn > ratio_max:
            ratio_max = Acc_rxn
            rx_chosen = rxn

    pulse_wave = fft1dmax[rx_chosen, :]
    plt.plot(pulse_wave[0:1000])
    # plt.show()
    plt.savefig(os.path.join(output_path, file_name.replace('.mat', '.png')))
    plt.clf()
