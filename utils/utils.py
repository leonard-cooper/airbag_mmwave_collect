"""
python implementation of matlab data processing code
useful function modules that can be used in other modules
data processing flow:
for every antenna:
    1. minus mean value
    2. Hann_window and get amplitude of FFT transform
    3. chose range bin from 32 range bins which has the highest ACC(auto correlation coefficient)
chose the antenna which has the highest ACC
"""
from plotfft import smooth2nd
from scipy import signal
import statsmodels.tsa.api as smt
import numpy as np
from scipy.signal import butter


def get_pulse(feature_list, time_list):
    for rx_chosen in range(1, 4):
        # get fft signal
        fftmax, ratio, bin = data_pretreatment(feature_list, time_list, rx_chosen)
        # get pulse from three antennas
        if rx_chosen == 1:
            temp = ratio
            temp_fftmax = fftmax
            temp_bin = bin
            rx = rx_chosen
        elif temp < ratio:
            temp = ratio
            temp_fftmax = fftmax
            temp_bin = bin
            rx = rx_chosen
        # filter the pulse

    return temp_fftmax, temp_bin, rx


def data_pretreatment(feature_list, time_list, rx_chosen):
    lentocope = 7000
    x1 = feature_list[rx_chosen - 1, 0:lentocope, :]  # rx1，51到550个
    meanx1 = np.mean(x1, axis=1)
    meanx1 = np.tile(meanx1, (32, 1)).T
    x1 = x1 - meanx1  # 7000, 32

    Hann_window = np.hanning(32)
    ScaleHannWin = 1 / np.sum(Hann_window)
    Hann_window = np.tile(Hann_window, (lentocope, 1))

    fft1dc = np.abs(np.fft.fft(x1 * Hann_window, 32) * 1 * ScaleHannWin)
    k1 = 0
    k2 = 15
    fft1dmax, ratio, bestBin = observation(fft1dc, 7e3 / time_list[-1], k1, k2)

    return fft1dmax, ratio, bestBin


def observation(fft1dmax, fs, k1, k2):
    for rangeBin in range(k1, k2 + 1):
        fftmax, ratio = corr_fun(fft1dmax[:, rangeBin], fs)
        if rangeBin == k1:
            temp = ratio
            temp_fftmax = fftmax
            bin = rangeBin
        elif temp < ratio:
            temp = ratio
            temp_fftmax = fftmax
            bin = rangeBin
    bestSignal = temp_fftmax
    maxRatio = temp
    bestBin = bin
    return bestSignal, maxRatio, bestBin


def corr_fun(fft1dmax, fs):
    o_fft1dmax = fft1dmax
    b, a = butter(4, [0.4, 8] * 2 / fs, 'bandpass')
    filtedData = signal.filtfilt(b, a, fft1dmax, axis=0)
    fft1dmax = filtedData[1000:]
    temp = smooth2nd(fft1dmax, np.round(fs))
    fft1dmax = np.subtract(fft1dmax, temp)
    jiange_up = round(fs * 60 / 50)
    jiange_low = round(fs * 60 / 160)
    acf1 = smt.stattools.acf(fft1dmax, nlags=round(jiange_up))
    ratio = np.max(acf1[jiange_low:jiange_up])
    return o_fft1dmax, ratio
