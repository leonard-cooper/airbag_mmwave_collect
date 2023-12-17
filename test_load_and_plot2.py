"""
load all artificial limb raw data from mat file,
process the file
save the processed data to pictures
"""
import os
import sys

import scipy.io as sio
import numpy as np
from matplotlib import pyplot as plt

from utils.utils import get_pulse

# data path that store original artificial limb data
# test_file_path = './UnitTest/100_70_60_9_test1.mat'
file_path = r'C:\Users\14806\Desktop\matlab\blood_pressure\artificial_limb\data\data_collected\data_DBP_change'
lentocope = 7000
# 遍历出来的结果不一样 butterworth滤波器的参数不一致很无语
# find best pulse wave that has the highest correlation coefficient
for file_name in os.listdir(file_path):
    file_data = sio.loadmat(os.path.join(file_path, file_name))
    feature_list = file_data['feature_list']
    time_list = np.squeeze(file_data['time_list'])
    fs = lentocope / time_list[-1]
    temp_fftmax, temp_bin, rx = get_pulse(feature_list, time_list)
    plt.plot(temp_fftmax[0:1000])
    plt.show()
    sys.exit(0)


