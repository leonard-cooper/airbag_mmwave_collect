import queue
import tkinter.ttk
import datetime
import tkinter as tk
from threading import Thread
from tkinter import messagebox

from matplotlib import pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from radarlib.ifxRadarSDK import *
from ui_utils import UpdateHR, UpdateAcc
from utils.mmwave_utils import get_pulse, CalHR
from utils.plotfft import plotfft
import matplotlib
import numpy as np

from utils.stop_recording import StopFlag
import tkinter.ttk
matplotlib.use('TkAgg')


def recording_data(ui):
    username = ui.entries['NAME'].get()
    print("Radar SDK Version: " + get_version_full())
    with Device() as device:
        device.set_config(num_chirps_per_frame=1,
                          num_samples_per_chirp=32,
                          frame_repetition_time_s=0.005,
                          chirp_repetition_time_s=0.003,
                          rx_mask=0b111)
        Dev = device.get_config()
        samples_per_chirp = Dev['num_samples_per_chirp']
        chirps_per_frame = Dev['num_chirps_per_frame']

        # range_list = np.linspace(0, range_res * samples_per_chirp, samples_per_chirp)
        frame = device.create_frame_from_device_handle()
        n_rx = frame.get_num_rx()
        frame_idx = 1
        n_frame = 7000
        # plt.ion()
        # plt.figure(figsize=(10, 8))
        raw_data = np.zeros(shape=(n_frame, n_rx, chirps_per_frame, samples_per_chirp))
        progressbarF = tkinter.ttk.Progressbar(ui.progress_bar_label1)
        progressbarF.place(relx=0, rely=0, relwidth=1, height=20)
        progressbarF['maximum'] = 7000
        progressbarF['value'] = 0

        time_start = datetime.datetime.now()
        time_list = np.zeros(shape=(7001, 1))
        while True:
            # plt.cla()  # 清除上一帧绘制的图像
            try:
                device.get_next_frame(frame)
            except ErrorFifoOverflow:
                print("Fifo Overflow")
                exit(1)
            # Do some processing with the obtained frame.
            for rx in range(0, n_rx):
                raw_data[frame_idx, rx, :, :] = np.array(frame.get_mat_from_antenna(rx))
            frame_idx += 1
            if frame_idx >= n_frame:
                break
            progressbarF['value'] += 1
            ui.root.update()
            time_frame = datetime.datetime.now()
            time_list[frame_idx] = (time_frame - time_start).total_seconds()
        time_end = datetime.datetime.now()
        timedelta = time_end - time_start
        timedelta = str(timedelta.total_seconds())
        print('录制完成')
        messagebox.showinfo('Message', '录制完成')

        timestamp = time_end.strftime("%y%m%d_%H%M%S")
        np.save("./data/" + username + "-" + timestamp + ".npy", raw_data)
        np.save("./data/" + username + "-" + "-time.npy", time_list)
        with open("./data/" + username + "-" + timestamp + '.txt', 'w') as f:
            f.write(timedelta)
        print(timedelta)

        time0 = float(timedelta)
        fs = n_frame/time0
        plotfft(raw_data, fs, ui)


class RecordingProcessor:
    def __init__(self, ui):
        self.ui = ui

    def prerecord(self, _):
        self.ui.label.destroy()
        self.ui.label = tk.Label(self.ui.frames['plot'], text="No signal", font=('default', 40),
                                 width=44, height=2, bg="red", highlightbackground="grey", highlightthickness=0.7)
        self.ui.label.grid(row=3, columnspan=12)
        pre_recording(self.ui)

    def record(self, _):
        self.ui.label.destroy()
        self.ui.label = tk.Label(self.ui.frames['plot'], text="No signal", font=('default', 40),
                                 width=44, height=2, bg="red", highlightbackground="grey", highlightthickness=0.7)
        self.ui.label.grid(row=3, columnspan=12)
        recording_data(self.ui)
        # RecordAndUpdateUI(self.ui, 7000)

    def RealTimeMMWAVE(self, _):
    #  实时录制并展示的程序
        realtimemmave(self.ui, 660)


def pre_recording(ui):
    RecordAndUpdateUI(ui, 200, 25)
    # global t
    # t = Thread(target=func, daemon=True)
    # t.start()


def RecordAndUpdateUI(ui, n_frame, start_minus):
    with Device() as device:
        device.set_config(num_chirps_per_frame=1,
                          num_samples_per_chirp=32,
                          frame_repetition_time_s=0.030,
                          chirp_repetition_time_s=0.004,
                          rx_mask=0b111)
        Dev = device.get_config()
        samples_per_chirp = Dev['num_samples_per_chirp']
        chirps_per_frame = Dev['num_chirps_per_frame']

        frame = device.create_frame_from_device_handle()
        n_rx = frame.get_num_rx()

        figure = plt.figure(figsize=(12, 2))
        canvas = FigureCanvasTkAgg(figure, master=ui.label)
        canvas.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)
        canvas._tkcanvas.pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

        StopFlag.flag = False
        while not StopFlag.flag:
            frame_num = 0  # pre version: frame_num = 1
            raw_data = np.zeros(shape=(n_frame, n_rx, chirps_per_frame, samples_per_chirp))  # (200,3,1,32)
            print(raw_data.shape)
            #  progress bar
            progressbarF = tkinter.ttk.Progressbar(ui.progress_bar_label1)
            progressbarF.place(relx=0, rely=0, relwidth=1, height=20)
            progressbarF['maximum'] = 200
            progressbarF['value'] = 0
            plt.cla()

            time_start = datetime.datetime.now()
            while True:
                try:
                    device.get_next_frame(frame)
                except ErrorFifoOverflow:
                    print("Fifo Overflow")
                    exit(1)
                # Do some processing with the obtained frame.
                for rx in range(0, n_rx):
                    raw_data[frame_num, rx, :, :] = np.array(frame.get_mat_from_antenna(rx))
                frame_num += 1
                if frame_num >= n_frame:
                    break
                progressbarF['value'] += 1
                ui.root.update()

            time_end = datetime.datetime.now()
            timedelta = time_end - time_start
            timedelta = str(timedelta.total_seconds())
            print('录制完成')
            print(timedelta)
            time0 = float(timedelta)
            fs = n_frame / time0
            print(f'fs: {fs}')
            # fs raw_data
            pulse, ratio_max, rx_chosen = get_pulse(raw_data, fs, start_minus=start_minus)

            UpdateAcc(ui, ratio_max)
            plt.plot(pulse)
            canvas.draw()
            plt.clf()
            HR = CalHR(fs, pulse)
            UpdateHR(ui, HR)



def realtimemmave(ui, n_frame):
    def update_plot():
        # 绘制 pulse
        ax.clear()
        ax.plot(buffer_pulse)
        canvas.draw()
        ui.root.after(100, update_plot)
    def record_data():
        global buffer_raw_data, buffer_pulse, buffer_pulse_refresh_flag
        frame_num = 0
        with Device() as device:
            device.set_config(num_chirps_per_frame=1,
                              num_samples_per_chirp=32,
                              frame_repetition_time_s=0.030,
                              chirp_repetition_time_s=0.004,
                              rx_mask=0b111)
            Dev = device.get_config()
            samples_per_chirp = Dev['num_samples_per_chirp']
            chirps_per_frame = Dev['num_chirps_per_frame']

            frame = device.create_frame_from_device_handle()
            n_rx = frame.get_num_rx()
            new_raw_data = np.zeros((1, n_rx, chirps_per_frame, samples_per_chirp))  # 获取数据
            fs = 33.7
            while not StopFlag.flag:
                # 获取雷达数据的代码
                try:
                    device.get_next_frame(frame)
                except ErrorFifoOverflow:
                    print("Fifo Overflow")
                    exit(1)
                # print('running1')
                # Do some processing with the obtained frame.
                for rx in range(0, n_rx):
                    new_raw_data[:, rx, :, :] = np.array(frame.get_mat_from_antenna(rx))

                # 处理数据并更新缓冲区
                buffer_raw_data = np.concatenate((buffer_raw_data[1:], new_raw_data), axis=0)

                buffer_pulse, _, _ = get_pulse(buffer_raw_data, fs, range_bin=1) # get new pulse
                frame_num += 1
                # UpdateAcc(ui, ratio_max)
                # HR = CalHR(fs, buffer_pulse)
                # print(f'HR:{HR}')
                # UpdateHR(ui, HR)

    # 初始化缓冲区和绘图
    n_rx = 3
    chirps_per_frame = 1
    samples_per_chirp = 32
    global buffer_raw_data, buffer_pulse
    buffer_raw_data = np.zeros((n_frame, n_rx, chirps_per_frame, samples_per_chirp))  # 根据雷达数据的实际结构来确定
    buffer_pulse = np.zeros(n_frame)
    figure, ax = plt.subplots(figsize=(12, 3))
    canvas = FigureCanvasTkAgg(figure, master=ui.label)
    canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)
    ui.root.after(100, update_plot)  # 每 500ms 检查一次队列

    # 启动数据录制线程
    recording_thread = Thread(target=record_data, daemon=True)
    recording_thread.start()