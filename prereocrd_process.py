import scipy.io as sio
from utils.plotfft import smooth2nd
from scipy import signal
import statsmodels.tsa.api as smt
import numpy as np
from utils.model_bp_pred import model_bp_pred

num_rx = 3
xn= 7000
ratio_max = -1
test_file_path = './UnitTest/102_70_60_9_test1.mat'
test_file = sio.loadmat(test_file_path)
time_list = np.squeeze(test_file['time_list'])  # (7000,)
data = test_file['feature_list']  # (3,7000,32)
data = data[:, 25:xn, :]
samples_per_chirp = 32
fs = xn / time_list[-1]
wl = 0.4 * 2 / fs
wh = 6 * 2 / fs
b, a = signal.butter(4, [wl, wh], 'bandpass')  # b=1
fft1dmax = np.zeros(shape=(3, xn-25))
for rxn in range(0, num_rx):
    x2 = data[rxn, :, :]  # 1*175*32
    x1 = np.squeeze(x2)
    meanx1 = np.mean(x1, 1)
    meanx1 = np.tile(meanx1, (32, 1))
    meanx1 = np.transpose(meanx1, (1, 0))
    x1 = x1 - meanx1
    range_win = signal.windows.hann(samples_per_chirp).reshape(samples_per_chirp, 1)  # (32,1)
    ScaleHannWin = 1 / sum(range_win)
    range_win = np.tile(range_win, (1, xn - 25))  # 32*175
    x1 = np.multiply(x1, np.transpose(range_win))  # 175*32
    fft1d = np.fft.fft(x1, samples_per_chirp, axis=1) * ScaleHannWin  # 175*32
    # 选取信号最强range_bin
    end_index = round(samples_per_chirp / 2)
    fft1d = fft1d[:, 2:end_index]
    fft1dmaxtemp = np.max(abs(fft1d), axis=1)
    # 对fft结果进行带通滤波
    filtedData = signal.filtfilt(b, a, fft1dmaxtemp, axis=0)
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

# calculate Acc HR SBP DBP plot
fft1dmax = fft1dmax[rx_chosen, :]
#plt.plot(fft1dmax[200:1200])
sbp_pred, dbp_pred = model_bp_pred(fft1dmax)
print('sbp_pred:', sbp_pred)
print('dbp_pred:', dbp_pred)



