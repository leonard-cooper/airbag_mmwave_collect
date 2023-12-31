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

import argparse
import numpy as np

from ifxRadarSDK import *
from scipy import signal
from scipy import constants
import matplotlib.pyplot as plt

from internal.fft_spectrum import *

# -------------------------------------------------
# Computation
# -------------------------------------------------
class DistanceFFT_Algo:
    # Algorithm for computation of distance fft from raw data

    def __init__(self, cfg):
        # Common values initiation
        # cfg: dictionary with configuration for device used by set_config() as input

        self._numchirps = cfg["num_chirps_per_frame"]
        chirpsamples = cfg["num_samples_per_chirp"]

        # compute Blackman-Harris Window matrix over chirp samples(range)
        self._range_window = signal.blackmanharris(chirpsamples).reshape(1, chirpsamples)

        lower_frequency_Hz = cfg["lower_frequency_Hz"]
        upper_frequency_Hz = cfg["upper_frequency_Hz"]
        bandwith_hz = upper_frequency_Hz-lower_frequency_Hz
        fft_size = chirpsamples * 2
        self._range_bin_length = (constants.c) / (2 * bandwith_hz * fft_size / chirpsamples)

    def compute_distance(self, data):
        # Computes a distance for one chirp of data
        # data: single chirp data for single antenna

        # Step 1 - calculate range fft spectrum of the frame
        range_fft = fft_spectrum(data, self._range_window)

        # Step 2 - convert to absolute spectrum
        fft_spec_abs = abs(range_fft)

        # Step 3 - coherent integration of all chirps
        data = np.divide(fft_spec_abs.sum(axis=0), self._numchirps)

        # Step 4 - peak search and distance calculation
        skip = 8
        max = np.argmax(data[skip:])

        dist = self._range_bin_length * (max + skip)
        return dist, data

# -------------------------------------------------
# Presentation
# -------------------------------------------------
class Draw:
    # Draws plots for data - each antenna is in separeted plot

    def __init__(self, cfg, max_range_m, num_ant):
        # Common values init
        # cfg:          dictionary with configuration for device used by set_config() as input
        # max_range_m:  maximum supported range
        # num_ant:      number of available antennas

        self._num_ant = num_ant;
        chirpsamples = cfg["num_samples_per_chirp"]
        self._pln = []

        self._fig, self._axs = plt.subplots(nrows=1, ncols=num_ant, figsize=((num_ant+1)//2,2))
        self._fig.canvas.manager.set_window_title("Range FFT")
        self._fig.set_size_inches(17/3*num_ant, 4)

        self._dist_points = np.linspace(
            0,
            max_range_m,
            chirpsamples)

    def _draw_first_time(self, data_all_antennas):
        # Create common plots as well scale it in same way
        # data_all_antennas: array of raw data for each antenna
        minmin = min([np.min(data) for data in data_all_antennas])
        maxmax = max([np.max(data) for data in data_all_antennas])

        for i_ant in range(0, self._num_ant):
            # This is a workaround: If there is only one plot then self._axs is
            # an object of the blass type AxesSubplot. However, if multiple
            # axes is available (if more than one RX antenna is activated),
            # then self._axs is an numpy.ndarray of AxesSubplot.
            # The code above gets in both cases the axis.
            print(type(self._axs))
            if type(self._axs) == np.ndarray:
                ax = self._axs[i_ant]
            else:
                ax = self._axs

            data = data_all_antennas[i_ant]
            pln, = ax.plot(self._dist_points, data)
            ax.set_ylim(minmin, 1.05 *  maxmax)
            self._pln.append(pln)

            ax.set_xlabel("distance (m)")
            ax.set_ylabel("FFT magnitude")
            ax.set_title("Antenna #"+str(i_ant))
        self._fig.tight_layout()
        plt.ion()
        plt.show()

    def _draw_next_time(self, data_all_antennas):
        # Update plots
        # data_all_antennas: array of raw data for each antenna

        for i_ant in range(0, self._num_ant):
            data = data_all_antennas[i_ant]
            self._pln[i_ant].set_ydata(data)

    def draw(self, data_all_antennas):
        # Draw plots for all antennas
        # data_all_antennas: array of raw data for each antenna

        if(len(self._pln)==0):
            self._draw_first_time(data_all_antennas)
        else:
            self._draw_next_time(data_all_antennas)
        plt.pause(1e-3)

# -------------------------------------------------
# Helpers
# -------------------------------------------------
def parse_attr_nframes_frate(
        description,
        def_nframes,
        def_frate):
    # Parse all program attributes
    # description:   describes program
    # def_nframes:   default number of frames
    # def_frate:     default frame rate in Hz

    parser = argparse.ArgumentParser(
        description=description)

    parser.add_argument('-n', '--nframes', type=int,
                        default=def_nframes, help="number of frames, default "+str(def_nframes))
    parser.add_argument('-f', '--frate', type=int, default=def_frate,
                        help="frame rate in Hz, default "+str(def_frate))

    return parser.parse_args()

# -------------------------------------------------
# Main logic
# -------------------------------------------------
if __name__ == '__main__':
    args = parse_attr_nframes_frate(
        '''Displays distance plot from Radar Data''',
        def_nframes = 50,
        def_frate = 5)

    with Device() as device:
        # activate all RX antennas
        num_rx_antennas = device.get_device_information()["num_rx_antennas"]
        rx_mask = (1 << num_rx_antennas) - 1

        metric = {
            'sample_rate_Hz':           1000000,
            'range_resolution_m':       0.05,
            'max_range_m':              1.6,
            'max_speed_m_s':            3,
            'speed_resolution_m_s':     0.2,
            'frame_repetition_time_s':  1/args.frate,
            'center_frequency_Hz':      60750000000,
            'rx_mask':                  rx_mask,
            'tx_mask':                  1,
            'tx_power_level':           31,
            'if_gain_dB':               33}

        cfg = device.translate_metrics_to_config(**metric);
        device.set_config(**cfg)

        frame = device.create_frame_from_device_handle()
        num_ant = frame.get_num_rx()

        distance = DistanceFFT_Algo(cfg)
        draw = Draw(cfg, metric["max_range_m"], num_ant)

        for frame_number in range(args.nframes): # For each frame
            device.get_next_frame(frame)

            dist_data_all_antennas = []
            dist_peak_m_4_all_ant = []

            for i_ant in range(0, num_ant): #For each antenna
                data = frame.get_mat_from_antenna(i_ant)
                dist_peak_m, dist_data = distance.compute_distance(data)

                dist_data_all_antennas.append(dist_data)
                dist_peak_m_4_all_ant.append(dist_peak_m)

                print("Distance antenna # " + str(i_ant) + ": " +
                    format(dist_peak_m, "^05.3f") + "m")
            draw.draw(dist_data_all_antennas)
