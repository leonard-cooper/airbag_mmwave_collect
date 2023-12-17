import matplotlib.pyplot as plt

import numpy as np
from scipy.signal import find_peaks


# interval = 0.1 * limit
# locs, _ = find_peaks(fftmax2d[0:int(len(fftmax2d))])
def getFeatures_test5(fftmax, fs):
    fftmax = -fftmax
    plt.plot(fftmax)
    Peaks, _ = find_peaks(fftmax)
    plt.scatter(Peaks, fftmax[Peaks])
    plt.show()
    limit = np.mean(np.diff(Peaks))
    interval = limit
    Aa = []
    Ab = []
    Ac = []
    Ad = []
    Ae = []
    Af = []
    Hab = []
    Hae = []
    AI = []
    pk = []
    tab = []
    tba = []
    tae = []
    area_ratio = []
    a = 0
    b = 0
    c = 0
    d = 0
    e = 0
    f = 0
    for i in range(len(Peaks) - 2):
        if (Peaks[i + 1] - Peaks[i]) > limit * 1.2 or (Peaks[i + 1] - Peaks[i]) < limit * 0.8:
            continue
        fftmax1 = fftmax[(Peaks[i] - int(interval)):(Peaks[i + 1] + int(interval))]
        plt.plot(fftmax1)
        fftmax2d = np.diff(fftmax1, 2)
        locs, _ = find_peaks(fftmax2d[0:int(len(fftmax2d))])
        pks = []
        for loc in locs:
            pks.append(fftmax2d[loc])
        if not len(locs):
            continue
        I = np.argmax(pks)
        b = locs[I]
        if len(locs) < (I + 2):
            continue
        d = locs[I + 1]
        f = locs[I + 2]

        locs = find_peaks(-fftmax2d)[0]
        if not len(locs[locs < b]):
            continue
        a = locs[locs < b][-1]
        c = locs[locs < b][-1] + 1
        if len(locs) < (np.where(locs == a)[0][0] + 2):
            continue
        e = locs[np.where(locs == a)[0][0] + 2]
        if not len(locs[locs > e]):
            continue
        nexta = locs[locs > e][0]

        Aa.append(fftmax2d[a])
        Ab.append(fftmax2d[b])
        Ac.append(fftmax2d[c])
        Ad.append(fftmax2d[d])
        Ae.append(fftmax2d[e])
        Af.append(fftmax2d[f])
        Hab.append(Ab[-1] - Aa[-1])
        Hae.append(fftmax2d[e] - Aa[-1])
        AI.append((fftmax2d[d] - Aa[-1]) / Hab[-1])
        tab.append(b - a)
        tba.append(nexta - b)
        tae.append(e - a)
        fftmax2d = fftmax2d - np.min(fftmax2d)
        pk.append(np.mean(fftmax2d) / fftmax2d[b])
        area_ratio.append(0.1)

        start = a - 1
        fftmax2d = fftmax2d[start:]

        a = a - start
        b = b - start
        c = c - start
        d = d - start
        e = e - start
        f = f - start
        nexta = nexta - start

        plt.subplot(3, 1, 2)
        plt.plot(fftmax1)

        plt.subplot(3, 1, 3)
        plt.plot(fftmax2d)

        plt.scatter(a, fftmax2d[a], 40, 'red', 'filled')
        plt.scatter(b, fftmax2d[b], 40, 'green', 'filled')
        plt.scatter(c, fftmax2d[c], 40, 'blue', 'filled')
        plt.scatter(d, fftmax2d[d], 40, 'cyan', 'filled')
        plt.scatter(e, fftmax2d[e], 40, 'magenta', 'filled')
        plt.scatter(f, fftmax2d[f], 40, 'yellow', 'filled')
        plt.scatter(nexta, fftmax2d[nexta], 40, 'black', 'filled')

    Aa = np.mean(Aa)
    Ab = np.mean(Ab)
    Ac = np.mean(Ac)
    Ad = np.mean(Ad)
    Ae = np.mean(Ae)
    Af = np.mean(Af)
    Hab = np.mean(Hab)
    Hae = np.mean(Hae)
    AI = np.mean(AI)
    pk = np.mean(pk)
    area_ratio = np.mean(area_ratio)
    tba = np.mean(tba) / fs

    features = [b - a, c - a, d - a, e - a, f - a, Aa, Ab, Ac, Ad, Ae, Af, Hab, Hae, AI, pk, tba, area_ratio]
    plt.show()
    return features
