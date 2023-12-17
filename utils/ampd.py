import numpy as np


def ampd(Signal):
    if not isinstance(Signal, np.ndarray) or Signal.ndim != 1:
        raise ValueError('Input argument \'Signal\' must be a 1D numpy array')
    elif Signal.size == 1:
        raise ValueError('Input argument \'Signal\' must contain at least 2 elements')
    elif not np.issubdtype(Signal.dtype, np.floating):
        raise ValueError('Data type of input argument \'Signal\' must be \'float\'')
    elif Signal.ndim == 2 and Signal.shape[0] == 1:
        Signal = Signal[0]

    time = np.arange(Signal.size)
    fitPolynomial = np.polyfit(time, Signal, 1)
    fitSignal = np.polyval(fitPolynomial, time)
    dtrSignal = Signal - fitSignal

    N = dtrSignal.size
    L = np.ceil(N / 2.0).astype(int) - 1

    RNG = np.random.RandomState()
    LSM = 1 + RNG.rand(L, N)
    for k in range(L):
        for i in range(k + 2, N - k + 1):
            if Signal[i - 1] > Signal[i - k - 1] and Signal[i - 1] > Signal[i + k - 1]:
                LSM[k, i] = 0

    G = np.sum(LSM, axis=1)
    l = np.argmin(G)
    LSM = LSM[0:l, :]

    S = np.std(LSM, axis=1)
    maxima = np.where(S == 0)[0] - 1
    return maxima
