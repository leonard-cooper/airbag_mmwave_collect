import time
import torch
from scipy import signal
import numpy as np
from scipy.interpolate import interp1d
from model.model2_conv import SNet

def model_bp_pred(fft1dmax):
    # 反转波形 未实现

    # load model
    model = SNet()
    model_dict = model.state_dict()
    checkpoint = torch.load(r'C:\Users\14806\Desktop\matlab\blood_pressure\artificial_limb\real_time_cal\tablet_breath_data_recording\model\ckpt\ckpt_2023-04-26-09-57.t7')
    pretrained_dict = checkpoint['net']
    pretrained_dict = {k: v for k, v in pretrained_dict.items() if k in model_dict}
    model_dict.update(pretrained_dict)
    model.load_state_dict(model_dict)
    # model.cuda()
    model.eval()
    sbp_list = []
    dbp_list = []
    # 选取波峰
    fft1dmax = fft1dmax[20:]
    peaks, _ = signal.find_peaks(fft1dmax, prominence=0.9 * np.max(fft1dmax))
    for i in range(len(peaks) - 1):
        print(i)
        onecycle = fft1dmax[peaks[i]:peaks[i + 1]]
        onecycle_scaled = 2.0 * (onecycle - np.min(onecycle)) / (np.max(onecycle) - np.min(onecycle)) - 1
        # plt.plot(onecycle_scaled)
        print('type of onecyle variable', type(onecycle))
        # 输入神经网络
        y = onecycle_scaled  # {ndarray: (201,)}
        x = np.linspace(0, 1, y.shape[0])  # {ndarray: (201,)}
        xnew = np.linspace(0, 1, 256)  # 128 is neural network input dimension {ndarray: (258,)}
        f = interp1d(x, y)
        ynew = f(xnew)  # {ndarray: (256,)}
        data = torch.tensor(ynew, dtype=torch.float).squeeze()  # {Tensor: (256,)}
        output = model(data.unsqueeze(0))
        sbp_list.append(float(output[0, 0] * 200))
        dbp_list.append(float(output[0, 1] * 200))
        # plt.plot(onecycle)
        time.sleep(1)
    # plt.plot(peaks, fft1dmax[peaks])
    # print(peaks)
    # plt.show()
    sbp_pred = np.mean(sbp_list)
    dbp_pred = np.mean(dbp_list)
    return sbp_pred, dbp_pred