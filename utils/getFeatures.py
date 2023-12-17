import numpy as np
import scipy.signal as signal
from utils.utils import ampd
from utils.utils import VMD


def getFeatures(fftmax, fs):
    K = 18
    alpha = 2000
    tau = 1e-6
    vmd = VMD(K, alpha, tau)
    imf, omega_K = vmd(fftmax)
    # vmd/smooth平滑滤波器/带通滤波器
    # imf, _ = vmd(fftmax, NumIMF=18)
    fftmax = np.sum(imf[-3:-1, :], axis=0)
    fftmax = fftmax - signal.savgol_filter(fftmax, round(fs), 3)
    print('round(fs)', round(fs))
    b, a = signal.butter(4, [0.5 / fs, 7.0 / fs], btype='band')  # 0.5~7 Hz
    print('shape of b', b.shape)
    print('shape of a', a.shape)
    fftmax = signal.filtfilt(b, a, fftmax)

    hr_gt = fs
    print('hr_get', hr_gt)
    print('after butter filtering fftmax shape:', fftmax.shape)
    print('--------------------step into slope function------------------')
    #fftmax, _ = Slope(fftmax, fftmax, hr_gt, fs)
    Peaks = ampd(fftmax)

    import matplotlib.pyplot as plt
    plt.plot(fftmax)
    plt.subplots(figsize=(8, 6))
    plt.subplot(3, 1, 1)
    plt.plot(fftmax)
    plt.plot(Peaks, fftmax[Peaks], 'r*')
    plt.show()

    limit = np.mean(np.diff(Peaks))
    interval = 0.1 * limit

    # 波幅相关
    Aa = []  # 波谷
    Ab = []  # 波峰
    Ac = []
    Ad = []
    Ae = []
    Af = []
    Hab = []  # Ab-Aa
    Hae = []  # Ae-Aa
    AI = []  # 增量指数 (Ad-Aa)/(Ab-Aa) 反射波波幅占主波波幅百分比
    pk = []  # 压力常数K 周期平均值/Ab 血管外周阻力大小
    # 时间间隔相关
    tab = []  # 上升时间
    tba = []  # 怎么找下一周期的a点位置？
    tae = []  # RWTT
    # 波形面积相关
    area_ratio = []  # Te,a+ /Ta,e

    a = b = c = d = e = f = 0
    for i in range(len(Peaks) - 2):
        if (Peaks[i + 1] - Peaks[i]) > limit * 1.2 or (Peaks[i + 1] - Peaks[i]) < limit * 0.8:
            continue
        fftmax1 = fftmax[(Peaks[i] - int(interval)):(Peaks[i + 1] + int(interval))]
        fftmax2d = np.diff(fftmax1, 2)

        pks, locs = signal.find_peaks(fftmax2d[0: int(len(fftmax2d) / 1.2)])
        I = np.argmax(pks)
        b = locs[I]
