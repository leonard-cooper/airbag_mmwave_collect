"""
实现一个比较简单的数据录制程序
"""
from radarlib.ifxRadarSDK import *
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

if __name__ == '__main__':
    # 1. 获得Radar SDK的版本，也是一个开始录制数据的信号
    print("Radar SDK Version: " + get_version_full())
    with Device() as device:
        # set device config
        # 使用一个大型的tuple设置数据，可以直接设置对应的max range等参数
        metric = {
            'sample_rate_Hz': 1000000,  # 信号的采样率
            'range_resolution_m': 0.05,  # 距离分辨率
            'max_range_m': 1.6,  # 最大距离
            'max_speed_m_s': 3,  # 最大速度
            'speed_resolution_m_s': 0.2,  # 速度分辨率
            'frame_repetition_time_s': 1 / 0.82,
            'center_frequency_Hz': 60750000000,  # 中频频率（看看是否根据需要进行更改）
            'rx_mask': 7,  # 接收天线对应id的掩码
            'tx_mask': 1,
            'tx_power_level': 31,
            'if_gain_dB': 33}
        cfg = device.translate_metrics_to_config(**metric)
        device.set_config(**cfg)

        # 获得快时间采样率和慢时间采样率
        samples_per_chirp = cfg['num_samples_per_chirp']
        chirps_per_frame = cfg['num_chirps_per_frame']

        # create frame 这个函数没有什么参数可以设置的，直接调用就可以
        frame = device.create_frame_from_device_handle()
        # 获取到对应的接收天线的个数
        # number of virtual active receiving antennas
        num_rx = frame.get_num_rx()
        print('rx num ', num_rx)
        # 获取到固定数量的frame数据，并且完成相应的处理
        num_frames = 10
        rPad = 4  # 设置fft延拓的倍数
        # 指定存储range-FFT的矩阵
        range_fft_metrix = np.zeros(shape=(num_rx, num_frames, chirps_per_frame, samples_per_chirp * rPad),
                                    dtype=complex)
        for frame_index in range(num_frames):
            print('load and save frame ',frame_index+1)
            try:
                device.get_next_frame(frame)
            except ErrorFifoOverflow:
                print("Fifo Overflow")
                exit(1)

            # Do some processing with the obtained frame.
            # In this example we just dump it into the console
            # print("Got frame " + format(frame_number) + ", num_antennas={}".format(num_rx))

            for iAnt in range(0, num_rx):
                mat = frame.get_mat_from_antenna(iAnt)  # 得到采样的实数原信号
                # 采出来的数据是这样一个维度的：(sample_per_chirp, chirp_per_frame)
                print('mat length：', len(mat))
                print("Antenna", iAnt, "\n", mat)

                # 对一个range的信号进行fft操作
                # 首先需要对数据进行加窗操作(先尝试加汉宁窗)
                # range_win = signal.windows.blackmanharris(chirps_per_frame).reshape(chirps_per_frame, 1)
                # mat = np.multiply(range_win, mat)
                # # 加窗之后对数据进行fft操作
                # per_range_fft = np.fft.fft(mat, rPad * len(mat[0]))
                # range_fft_metrix[iAnt, frame_index, :, :] = per_range_fft
        # 随便取一帧的数据画出来，观察看对应的效果如何
        # 首先可以使用手机振动的数据进行测试


