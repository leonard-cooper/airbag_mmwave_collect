# ===========================================================================
# Copyright (C) 2021 Infineon Technologies AG
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# ===========================================================================

import numpy as np
from ifxRadarSDK import *

print("Radar SDK Version: " + get_version_full())

# open device: The device will be closed at the end of the block. Instead of
# the with-block you can also use:
#   device = Device()
# However, the with block gives you better control when the device is closed.
with Device() as device:
    # set device config
    device.set_config()  # 如果需要设置参数，在这里面设置相关的参数，都有哪些参数直接点开看就可以

    # create frame
    # 这个函数没有什么参数可以设置的，直接调用就可以
    frame = device.create_frame_from_device_handle()

    # 获取到对应的接收天线的个数
    # number of virtual active receiving antennas
    num_rx = frame.get_num_rx()

    # 获取到固定数量的frame数据，并将它们打印出来
    # A loop for fetching a finite number of frames comes next..
    for frame_number in range(10):
        try:
            device.get_next_frame(frame)
        except ErrorFifoOverflow:
            print("Fifo Overflow")
            exit(1)

        # Do some processing with the obtained frame.
        # In this example we just dump it into the console
        print("Got frame " + format(frame_number) + ", num_antennas={}".format(num_rx))

        for iAnt in range(0, num_rx):
            mat = frame.get_mat_from_antenna(iAnt) # 这里面得出来的是一个chirp的数据，可以直接对取出来的数据做FFT
            print('mat length：', len(mat))
            print("Antenna", iAnt, "\n", mat)

