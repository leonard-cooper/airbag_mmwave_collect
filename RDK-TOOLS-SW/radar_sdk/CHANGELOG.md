# Release 3.1

## Main new Features

* The radar sensor BGT60UTR11 now works out of the box with the presence
  sensing application.
* Support for the Avian radar sensor BGT60UTR13D has been added.
* The new tool bgt60-configurator-cli has been added which allows to convert a
  json configuration to register values.

## API Changes

Changes:
* The type of the second parameter of `ifx_ppfft_set_mean_removal_flag`
  has been changed from `uint8_t` to `bool`.
* The declaration of the function `ifx_presence_sensing_get_config_defaults`
  has changed. There is a new input parameter of type `ifx_Device_Radar_Type_t`
  which specifies the device type for which the configuration should be given.
  The parameter of type `ifx_Device_Config_t` is now an output parameter. The
  implementation of the function now gives in this output parameter the default
  device configuration for presence sensing.
* The declaration of the function `ifx_segmentation_config_get_defaults`
  has changed. There is a new input parameter of type `ifx_Device_Radar_Type_t`
  which specifies the device type for which the configuration should be given.
  The first parameter of type `ifx_Device_Config_t` is now an output parameter.
  The implementation of the function now gives in this output parameter the
  default device configuration for segmentation.
* The members of the enum `ifx_Device_Shield_Type_t` have been renamed.
* The functions `ifx_vect_median_range_r` and `ifx_vect_median_r` have been
  renamed to `ifx_vec_median_range_r` and `ifx_vec_median_r`, respectively.
* The function for creating a Butterworth filter object has been renamed
  from `ifx_signal_filter_butterwort_create_r` to
  `ifx_signal_filter_butterworth_create_r`.
* The structure `ifx_Device_Info_t` has a new member `shield_type`. The field
  is also available via the Python and Matlab wrappers.
* The function `ifx_signal_correlate_r` has one more parameter to specify the
  mode (either full or same).
* Python radar wrapper: The method `get_list` now expects the shield type to be
* of type `ShieldType`
  instead of a string.

Functions added:
* The function `ifx_device_config_get_limits` has been added.
* The function `ifx_device_get_device_type` has been added.
* The functions `ifx_vec_dot_r` and `ifx_vec_dot2_r` to compute the dot product
  between two vectors have been added.
* The function `ifx_device_cw_destroy` has been added.
* The functions `ifx_mat_clone_r`, `ifx_mat_clone_c` to clone matrices have
  been added.
* The functions `ifx_device_cw_get_sampling_rate_limits` and
  `ifx_device_cw_get_sampling_rate` have been added to the constant wave (CW)
  mode.

## Bug fixes

* Fixed a bug in `ifx_device_set_config` which caused that the
  chirp-to-chirp-time and the frame time were not set correctly in some cases.
* Fixed a bug in functions `ifx_vec_set_range_r` and `ifx_vec_set_range_c`.
  The behavior has changed and now they fill the vector from offset value place
  for a range value of places, meaning they fill the vector from offset to
  offset+range.
* Fix the bug that the functions `ifx_mat_set_row_r` and
  `ifx_mat_set_row_c` set an error if the number of columns of the matrix
  equals `count`.
* Missing checks of parameters for `ifx_rdm_run_rc` and `ifx_rdm_run_r` have
  been added.
* A check has been added to avoid a crash when connecting to a radar
  shield with a broken EEPROM.

## Other changes

* Reading the temperature from an Avian radar sensor is now cached. When
  calling `ifx_device_get_temperature` twice within a short amount of time the
  function will return the same temperature value. This change prevents
  performance degredations and stability issues when fetching time-domain data
  from the radar sensor and reading the temperature very often.
* The format of the return value of `ifx_sdk_get_version_string` has changed:
  It now returns a string with major number, minor number, and patch level
  separated by dots.

# Release 3.0

* The function `ifx_cube_slice_abs_r` has been removed.
* Functions to calculate range, speed and frequency axis for Range Spectum and Range Doppler Map can be used from `SpectrumAxis.h` module.
* The functions `ifx_rs_calc_range_axis` and `ifx_rs_calc_freq_axis`, have been removed from `RangeSpectrum.h` module.
* The functions `ifx_rdm_calc_range_axis` and `ifx_rdm_calc_speed_axis`, have been removed from `RangeDopplerMap.h` module.
* FFTW3 as a backend for FFT computations has been removed. KissFFT is now the only supported backend.
* The function `ifx_math_abs_c` has been removed. One can use the function `ifx_complex_abs` instead.
* The option for memory tracking has been removed. The function `ifx_mem_get_total_alloc_size` has been removed as well.
* The functions `ifx_vec_setrng_r` and `ifx_vec_setrng_c` have been renamed to `ifx_vec_set_range_r` and `ifx_vec_set_range_c`, respectively
* The function `ifx_device_get_center_frequency` has been renamed to `ifx_devconf_get_center_frequency`.
* The function `ifx_device_get_bandwidth` has been renamed to `ifx_devconf_get_bandwidth`.
* The function `ifx_device_get_chirp_repetition_time` has been renamed to `ifx_devconf_get_chirp_repetition_time`.
* The function `ifx_device_count_rx_antennas` has been renamed to `ifx_devconf_count_rx_antennas`.
* The enum `ifx_Device_Type_t` has been renamed to `ifx_Device_Shield_Type_t`.

# Release 2.7

* The error code `IFX_ERROR_DEVICE_NOT_SUPPORTED` has been added to
  ifxBase/Error.h.
* The obsolete enum `ifx_Device_TX_Mode` has been removed from
  ifxDevice/DeviceConfig.h.

# Release 2.6

* The argument `msg` of the function `ifx_log` is now const, the prototype has
  been changed from
  `void ifx_log(FILE* f, ifx_Log_Severity_t severity, char* msg, ...)`
  to `void ifx_log(FILE* f, ifx_Log_Severity_t severity, const char* msg, ...)`.
* First argument of `ifx_ultrasonic_set_config` is no longer const. The
  prototype changed from `void ifx_ultrasonic_set_config(const
  ifx_Ultrasonic_Device_t device, const ifx_Ultrasonic_Config_t* config)` to
  `void ifx_ultrasonic_set_config(ifx_Ultrasonic_Device_t device, const
  ifx_Ultrasonic_Config_t* config)`.
* Radar device specific functions (DeviceControl) have been moved from the
  ifxRadar to ifxDevice.
* The forward header in ifxBase has been removed. Every module has one
  convenience header that includes all headers of the module. For example,
  ifxBase/Base.h includes all headers of the ifxBase module and similar for
  ifxAlgo/Algo.h, ifxDevice/Device.h, ifxRadar/Radar.h...
* The functions to retrieve the SDK version
  (`ifx_radar_sdk_get_version_string`, `ifx_radar_sdk_get_version_string_full`,
  `ifx_radar_sdk_get_version_hash`) have been renamed to
  `ifx_sdk_get_version_string`, `ifx_sdk_get_version_string_full`,
  `ifx_sdk_get_version_hash`, respectively. The functions are now part of
  ifxBase.

# Release 2.5

## Splitting up of the SDK

The library has been split up in several paths. If you only include
ifxRadar/SDK.h in your project, no further action is required.

Following header files have been moved:
* Complex.h, Cube.h, Error.h, Forward.h, LA.h, Log.h, Math.h, Matrix.h, Mem.h,
  Types.h, Vector.h have been moved from to ifxBase/.
* 2DMTI.h, DBSCAN.h, FFT.h, MTI.h, OSCFAR.h, PreprocessedFFT.h, Signal.h,
  Window.h haven been moved to ifxAlgo/.

## DeviceConfig and DeviceControl

* `ifx_Device_Metrics_t`: The type of `sample_rate_Hz` is now `uint32_t`
  (previously `ifx_Float_t`).
* `ifx_Device_Config_t`:
   - `adc_samplerate_Hz` has been renamed to `sample_rate_Hz`.
   - The type of `tx_mask` has been changed to `uint32_t` (previously `uint8_t`).
   - `tx_mode` has been replaced by `tx_mask` and `mimo_mode`.
   - The type of `tx_mask` has been changed to `uint32_t` (previously `uint8_t`)
   - `bgt_tx_power` has been renamed to `tx_power_level`. The type has been
     changed to `uint32_t` (previously `uint8_t`).
   - The type of `if_gain_dB` has been changed to `uint32_t` (previosly
     `uint8_t`).
   - `frame_period_us` has been replaced by `frame_repetition_time_s`.
   - `cirp_to_chirp_time_100ps` has been replaced by `chirp_repetition_time_s`.
* `ifx_Device_Info_t`: The members `major_version_hw` and `minor_version_hw`
  have been removed.
* The function `ifx_device_get_pulse_repetition_time` has been renamed to
  `ifx_device_get_chirp_repetition_time`.
* The function `ifx_device_get_chirp_time` has been added.
* The function `ifx_device_get_device_information` has been added.
* The function `ifx_device_get_firmware_information` has been added.
* The function `ifx_device_get_next_frame_timeout` has been added.
* The function `ifx_device_get_list` now returns a list of uuids instead of a
  string with COM ports. See the documentation for more details.
* The functions `ifx_device_create_by_uuid` and `ifx_device_get_shield_uuid`
  expect/return the uuid as a C string instead of an `uint8_t` array.

## List

A simple dynamical list has been added (ifxBase/List.h).

## PreprocessedFFT

* `ifx_ppfft_calc_freq_axis`: The second parameter `sampling_freq` is now
  given in units of Hz (in contrast to kHz which was previously used).

## RangeSpectrum

* `ifx_rs_calc_range_axis`: The second parameter `chirp_bandwidth` is now given
  in units of Hz (in contrast to kHz which was previously used). The type of
  the variable has been changed from `uint32_t` to `ifx_Float_t`.

## RangeDopplerMap

* `ifx_rdm_set_doppler_window`: The second argument `chirp_bandwidth` is now
  given in units of Hz (in contrast to kHz which was previously used).
* `ifx_rdm_calc_range_axis`: The second argument `center_frequency` is now
  given in units of Hz (in contrast to kHz which was previously used).

## Presence Sensing

* Presence Sensing is now a module (previously it was just an application).

## Segmentation

* `ifx_Segmentation_Config_t`:
    - `samples_per_chirp` has been renamed to `num_samples_per_chirp`.
    - `chirps_per_frame` has been renamed to `num_chirps_per_frame`.
    - `prt_s` has been renamed to `chirp_repetition_time_s`.
    - `fc_Hz` has been renamed to `center_frequency_Hz`.
    - `max_range_m` has been renamed to `max_detection_range_m`.
    - `max_fov_degrees` has been removed.
    - `num_segments` has been removed.
    - `segment_width_degrees` has been renamed to `segment_degrees`.
    - `static_target_cancelation` has been renamed to `static_target_removal`.
    - `hysteresis_enabled` has been renamed to `enable_hysteresis`.
    - The type of `max_num_tracks` has been changed to `uint32_t` (previously
      `uint8_t`).
* The function `ifx_segmentation_config_get_defaults` that initializes a
  `ifx_Segmentation_Config_t` structure with default values has been added.

## Applications

* The JSON configuration schema has been changed. The new format is described
  in the RDK documentation.

## Python Wrapper

* The name of the parameters of `set_config` have been renamed according to
  the changes to DeviceConfig (see above).
* The default values of the parameters in `set_config` have been changed.

## Matlab Wrapper

* The name of the parameters have been renamed according to the changes to
  DeviceConfig (see above).
