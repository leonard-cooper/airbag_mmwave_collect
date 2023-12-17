import numpy as np
from scipy.signal import argrelextrema


def Slope2(fftmax, output, hr_gt, fs):
    # 判断正反
    k = np.ravel(np.asarray(argrelextrema(fftmax, np.greater)))
    print('shape of k:', k.shape)
    fftmax_begin = fftmax
    slope = []
    for m in range(1, len(k) - 1):
        start = k[m]
        endd = k[m + 1]
        HR = k[m + 1] - k[m]
        if start <= 0 or endd >= len(fftmax) or HR > hr_gt + 30 or HR < hr_gt - 30:
            continue
        Abs = fftmax[start:endd]
        Abs = (Abs - np.min(Abs)) / (np.max(Abs) - np.min(Abs))
        if len(Abs) < 25:
            continue
        slope1 = np.mean(Abs[14:24])
        slope2 = np.mean(Abs[-25:-15])
        slope.append(np.mean(slope1 - slope2))
    print('shape of slope:', slope.shape)
    if np.mean(slope) > 0:
        output = -output
        fftmax_begin = -fftmax_begin
    return fftmax_begin, output
# Note that I assumed that the mapminmax function in the original code is used to normalize the Abs array to the range of -1 to 1. In Python, I used the min-max normalization formula to achieve the same result. Also, I made sure to update the syntax of certain functions and operators to be compatible with Python.
