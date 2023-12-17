import numpy as np
from ampd import ampd
from Slope2 import Slope2
from scipy.signal import argrelextrema
import matplotlib.pyplot as plt

def Slope(fftmax, output, hr_gt, fs):
    k = np.ravel(np.asarray(argrelextrema(fftmax, np.greater)))
    #plt.plot(fftmax)
    #plt.scatter(k, fftmax[k])
    #plt.savefig('test.png')
    print('shape of k:', k.shape)
    fftmax_begin = fftmax
    slope = []
    for m in range(1, len(k) - 1):
        start = k[m]
        endd = k[m + 1]
        print('endd-start:', endd-start, 'm is:', m)
        HR = k[m + 1] - k[m]
        if start <= 0 or endd >= len(fftmax) or HR > hr_gt + 30 or HR < hr_gt - 30:
            continue
        Abs = fftmax[start:endd]
        Abs = (Abs - np.min(Abs)) / (np.max(Abs) - np.min(Abs))
        if len(Abs) < 24:
            continue
        slope.append(np.mean(Abs[14:24]))
    print('slope:', slope)
    if np.mean(slope) > 0:
        output = -output
        fftmax_begin = -fftmax_begin
    fftmax_begin, output = Slope2(fftmax_begin, output, hr_gt, fs)
    return fftmax_begin, output
