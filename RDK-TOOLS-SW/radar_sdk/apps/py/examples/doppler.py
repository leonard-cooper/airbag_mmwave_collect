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
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

from ifxRadarSDK import *
from internal.fft_spectrum import *

# -------------------------------------------------
# Computation
# -------------------------------------------------
class DopplerAlgo:
    # Doppler algorithm for raw data

    def __init__(self, cfg, num_ant):
        # Initiate common values for all computation
        # cfg:      dictionary with configuration for device used by set_config() as input
        # num_ant:  number of available antennas 

        self._numchirps = cfg["num_chirps_per_frame"]
        chirpsamples = cfg["num_samples_per_chirp"]
        
        # compute Blackman-Harris Window matrix over chirp samples(range)
        self._range_window = signal.blackmanharris(chirpsamples).reshape(1,chirpsamples)

        # compute Blackman-Harris Window matrix over number of chirps(velocity)
        self._doppler_window = signal.blackmanharris(self._numchirps).reshape(1,self._numchirps)

        self._moving_avg_alpha = 0.8
        self._mti_alpha = 1.0
        # initialize doppler averages for all antennae
        self._dopp_avg = np.zeros((chirpsamples,self._numchirps*2, num_ant),dtype=complex)

    def compute_doppler_map(self, data, i_ant):
        # Computation of doppler map for drawing
        # with middle value with speed of 0 and distance on column
        # number
        # data:     single chirp raw data for single antenna
        # i_ant:    antenna number that data is related to

        # Step 1 -  calculate fft spectrum for the frame
        fft1d = fft_spectrum(data, self._range_window)

        # prepare for doppler FFT

        # Transpose
        # Distance is now indicated on y axis
        fft1d = np.transpose(fft1d)

        # Step 2 - Windowing the Data in doppler
        fft1d = np.multiply(fft1d,self._doppler_window)

        zp2 = np.pad(fft1d,((0,0),(0,self._numchirps)),'constant')
        fft2d = np.fft.fft(zp2)/self._numchirps

        # MTI processing
        # needed to remove static objects
        # step 1 moving average
        # multiply history by (mti_alpha)
        fft2d_mti = fft2d - (self._dopp_avg[:,:,i_ant]*self._mti_alpha)

        # update moving average
        self._dopp_avg[:,:,i_ant] = (fft2d*self._moving_avg_alpha) + (self._dopp_avg[:,:,i_ant]*(1-self._moving_avg_alpha))

        # re-arrange fft result for zero speed at centre
        dopplerfft = np.fft.fftshift(fft2d_mti,(1,));

        return 20*np.log10(abs(np.fliplr(dopplerfft)))

# -------------------------------------------------
# Presentation
# -------------------------------------------------
class Draw:
    # Represents drawing for example
    #
    # Draw is done for each antenna, and each antenna is represented for
    # other subplot

    def __init__(self, max_speed_m_s, max_range_m, num_ant):
        # max_range_m:   maximum supported range
        # max_speed_m_s: maximum supported speed
        # num_ant:       Number of antennas
        self._h = []
        self._max_speed_m_s = max_speed_m_s
        self._max_range_m = max_range_m
        self._num_ant = num_ant

        self._fig, ax = plt.subplots(nrows=1, ncols=num_ant, figsize=((num_ant+1)//2,2))
        if(num_ant==1):
            self._ax = [ax]
        else:
            self._ax = ax
       
        self._fig.canvas.manager.set_window_title("Doppler")
        self._fig.set_size_inches(3*num_ant+1, 3+1/num_ant)

    def _draw_first_time(self, data_all_antennas):
        # First time draw
        #
        # It computes minimal, maximum value and draw data for all antennas
        # in same scale
        # data_all_antennas: array of raw data for each antenna
        
        minmin = min([np.min(data) for data in data_all_antennas])
        maxmax = max([np.max(data) for data in data_all_antennas])
       
        for i_ant in range(0, self._num_ant):
            data = data_all_antennas[i_ant]
            h = self._ax[i_ant].imshow(
                        data, 
                        vmin=minmin, vmax=maxmax,
                        cmap='hot',
                        extent=(    -self._max_speed_m_s,
                                    self._max_speed_m_s,
                                    0,
                                    self._max_range_m),
                        origin='lower')
            self._h.append(h)
        
            self._ax[i_ant].set_xlabel("velocity (m/s)")
            self._ax[i_ant].set_ylabel("distance (m)")
            self._ax[i_ant].set_title("antenna #"+str(i_ant))
        self._fig.subplots_adjust(right=0.8)
        cbar_ax = self._fig.add_axes([0.85, 0.0, 0.03, 1])
            
        cbar =self._fig.colorbar(self._h[0], cax=cbar_ax)
        cbar.ax.set_ylabel("magnitude (dB)")
  
    def _draw_next_time(self, data_all_antennas):
        # Update data for each antenna
        
        for i_ant in range(0, self._num_ant):
            data = data_all_antennas[i_ant]
            self._h[i_ant].set_data(data)

    def draw(self, data_all_antennas):
        # Draw data for all antenna

        first_run = len(self._h) == 0
        if first_run:
            self._draw_first_time(data_all_antennas)
        else:
            self._draw_next_time(data_all_antennas)
        plt.draw()
        plt.pause(1e-3)

# -------------------------------------------------
# Helpers
# -------------------------------------------------
def parse_attr_nframes_frate( description, def_nframes, def_frate):
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
        '''Displays range doppler map''', 
        def_nframes=50, 
        def_frate=5)

    with Device() as device:
        # activate all available antennas
        num_rx_antennas = device.get_device_information()["num_rx_antennas"]
        rx_mask = (1 << num_rx_antennas) - 1

        metric = {
            'sample_rate_Hz':           1_000_000,
            'range_resolution_m':       0.15,
            'max_range_m':              4.8,
            'max_speed_m_s':            2.45,
            'speed_resolution_m_s':     0.2,
            'frame_repetition_time_s':  1/args.frate,
            'center_frequency_Hz':      60_750_000_000,
            'rx_mask':                  rx_mask,
            'tx_mask':                  1,
            'tx_power_level':           31,
            'if_gain_dB':               33
        }
        
        cfg = device.translate_metrics_to_config(**metric)
        
        device.set_config(**cfg)
       
        frame = device.create_frame_from_device_handle()
        num_ant = frame.get_num_rx()
        
        doppler = DopplerAlgo(cfg, num_ant)
        draw = Draw( 
            metric["max_speed_m_s"], \
            metric["max_range_m"], \
            num_ant)
        
        for frame_number in range(args.nframes): # For each frame
            device.get_next_frame(frame)
            data_all_antennas = []

            for i_ant in range(0, num_ant): #For each antenna                  
                mat = frame.get_mat_from_antenna(i_ant)   
                dfft_dbfs = doppler.compute_doppler_map(mat, i_ant)
                data_all_antennas.append(dfft_dbfs)

            draw.draw(data_all_antennas)
