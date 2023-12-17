import numpy as np
from scipy import signal
from statsmodels.tsa import api as smt

from utils.plotfft import smooth2nd


def get_pulse(raw_data, fs, range_bin=0, start_minus=0):
    # this code assume chirps_per_frame = 1 by default
    # typical observations: 7000, 3, 1, 32
    # n_frame. n_rx, chirps_per_frame, samples_per_chirp
    n_frame = raw_data.shape[0]
    n_rx = raw_data.shape[1]
    samples_per_chirp = raw_data.shape[3]
    ratio_max = -1
    pulse_rxs = np.zeros(shape=(3, n_frame - start_minus))
    # filter parameters
    wl = 0.4 * 2 / fs
    wh = 6 * 2 / fs
    b, a = signal.butter(4, [wl, wh], 'bandpass')  # b=1
    data = np.transpose(raw_data, (1, 0, 3, 2))
    data = data[:, start_minus:n_frame, :, :]  # 去除起始噪声
    data = np.squeeze(data)  # (n_rx, n_frame-start_minus, samples_per_chirp)
    rx_chosen = 0
    for rx in range(0, n_rx):
        rx_data = np.squeeze(data[rx, :, :])  # (n_frame-start_minus, samples_per_chirp)
        # 减去均值
        chirp_mean = np.tile(np.mean(rx_data, 1), (32, 1))
        rx_data = rx_data - np.transpose(chirp_mean, (1, 0))

        range_win = signal.windows.hann(samples_per_chirp).reshape(samples_per_chirp, 1)  # (32,1)
        ScaleHannWin = 1 / sum(range_win)
        range_win = np.tile(range_win, (1, n_frame - start_minus))  # 32*(n_frame - start_minus)
        rx_data = np.multiply(rx_data, np.transpose(range_win))  # 175*32

        fft1d = np.fft.fft(rx_data, samples_per_chirp, axis=1) * ScaleHannWin  # 175*32
        # 选取信号最强range_bin
        end_index = round(samples_per_chirp / 2)
        fft1d = fft1d[:, 2:end_index]
        fft1dmaxtemp = np.max(abs(fft1d), axis=1)
        # 对fft结果进行带通滤波
        filtedData = signal.filtfilt(b, a, fft1dmaxtemp, axis=0)
        temp = smooth2nd(filtedData, np.round(fs))
        pulse_rx = np.subtract(filtedData, temp)
        pulse_rxs[rx, :] = pulse_rx

        if range_bin == 0:
            jiange_up = round(fs * 60 / 50)
            jiange_low = round(fs * 60 / 160)
            acf1 = smt.stattools.acf(pulse_rx, nlags=round(jiange_up))
            acf2 = acf1[jiange_low:jiange_up]
            rx_max_acc = np.max(acf2)
            if rx_max_acc > ratio_max:
                ratio_max = rx_max_acc
                rx_chosen = rx
        else:
            pass
    pulse_best = pulse_rxs[rx_chosen, :]
    return pulse_best, ratio_max, rx_chosen


def CalHR(fs, pulse):
    max_bpm = 60 * fs
    velocity = np.linspace(0, max_bpm, 512)
    range_win = signal.windows.hann(len(pulse))
    ScaleHannWin = 1 / sum(range_win)
    x1 = np.multiply(pulse, np.transpose(range_win))
    fft2 = abs(np.fft.fft(x1, 512) * 1 * ScaleHannWin)  # 175*512
    it = []
    for val in range(0, 512):
        if (50 < velocity[val]) & (velocity[val] < 120):
            it.append(val)
    sigTemp = []
    for val2 in it:
        sigTemp.append(fft2[val2])
    maxindex = np.argmax(sigTemp)
    HR = velocity[it[0] + maxindex]
    HR = round(HR - 2.2)
    return HR
