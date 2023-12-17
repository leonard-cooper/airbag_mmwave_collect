import matplotlib
import numpy as np
import statsmodels.tsa.api as smt

import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg  # NavigationToolbar2TkAgg

from scipy import signal
import tkinter as tk

matplotlib.use('TkAgg')


def smooth2nd(input_x, M):
    """
    实现一个简单的移动平均平滑。移动平均是一种常见的数据平滑技术，用于消除数据中的噪声和短期波动，并突出更长期的趋势或周期。
    """
    K = round(M / 2 - 0.1)  # M应为奇数，如果是偶数，则取大1的奇数
    len_input = len(input_x)
    if len_input < 2 * K + 1:
        print('数据长度小于平滑点数')
        return False
    else:
        res = np.zeros(len_input)
        for x in range(0, len_input, 1):
            startInd = max([0, x - K])
            endInd = min(x + K + 1, len_input)
            res[x] = np.mean(input_x[startInd:endInd])
        return res


def plotfft(raw_data, fs, ui):
    wl = 0.4 * 2 / fs
    wh = 6 * 2 / fs
    b, a = signal.butter(4, [wl, wh], 'bandpass')
    ratio_max = -1

    samples_per_chirp = 32

    fft1dmax = np.zeros(shape=(3, 6900))

    raw_data = np.transpose(raw_data, (1, 0, 3, 2))  # 3*7000*32*1
    raw_data = raw_data[:, 100:7000, :, :]
    raw_data = np.squeeze(raw_data)  # 3*6900*32

    for rxn in range(0, 3):
        x2 = raw_data[rxn, :, :]

        x1 = np.squeeze(x2)  # 6900*32

        meanx1 = np.mean(x1, 1)

        meanx1 = np.tile(meanx1, (32, 1))
        meanx1 = np.transpose(meanx1, (1, 0))

        x1 = x1 - meanx1
        range_win = signal.windows.hann(samples_per_chirp).reshape(samples_per_chirp, 1)  # (32,1)
        ScaleHannWin = 1 / sum(range_win)
        range_win = np.tile(range_win, (1, 6900))
        x1 = np.multiply(x1, np.transpose(range_win))

        fft1d = np.fft.fft(x1) * ScaleHannWin
        end_index = round(samples_per_chirp / 2)
        fft1d = fft1d[:, 2:end_index]
        fft1dmaxtemp = np.max(abs(fft1d), axis=1)
        filtedData = signal.filtfilt(b, a, fft1dmaxtemp, axis=0)  # fftMax为要过滤的信号
        temp = smooth2nd(filtedData, np.round(fs))
        result = np.subtract(filtedData, temp)

        fft1dmax[rxn, :] = result

        jiange_up = round(fs * 60 / 50)
        jiange_low = round(fs * 60 / 160)

        acf1 = smt.stattools.acf(result, nlags=round(jiange_up))

        acf2 = acf1[jiange_low:jiange_up]

        for i in range(len(acf2)):
            if acf2[i] > ratio_max:
                ratio_max = acf2[i]
                rx_chosen = rxn

    fft1dmaxplot = fft1dmax[rx_chosen, :]
    ratio_max = round(ratio_max, 3)

    accprint = tk.ttk.Label(ui.labels['ACC:'])
    accprint.place(relx=0, rely=0, relwidth=1)
    accprint['text'] = ratio_max
    # accprint['font'] = ('宋体', 30, 'bold')
    accprint['justify'] = 'left'
    ui.root.update()

    f = plt.figure(figsize=(12, 5))
    plt.plot(fft1dmaxplot)

    figure = f
    canvas = FigureCanvasTkAgg(figure, master=ui.label)
    canvas.draw()
    canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)

    canvas._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=1)
