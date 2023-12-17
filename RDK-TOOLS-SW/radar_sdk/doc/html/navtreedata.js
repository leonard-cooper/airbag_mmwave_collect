/*
@licstart  The following is the entire license notice for the
JavaScript code in this file.

Copyright (C) 1997-2019 by Dimitri van Heesch

This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

@licend  The above is the entire license notice
for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Radar SDK Documentation", "index.html", [
    [ "About this Document", "index.html", [
      [ "Scope and Purpose", "index.html#about-scope-purpose", null ],
      [ "Installation", "index.html#about-installation", null ],
      [ "Intended Audience", "index.html#about-intended-audience", null ]
    ] ],
    [ "Introduction to Radar", "pg_radarsdk_introduction.html", [
      [ "Radar System", "pg_radarsdk_introduction.html#sct_radarsdk_introduction_radarsystem", null ],
      [ "Classification of Radar Systems", "pg_radarsdk_introduction.html#sct_radarsdk_introduction_radarsystemtypes", null ],
      [ "Range Resolution", "pg_radarsdk_introduction.html#sct_radarsdk_introduction_rangeres", null ],
      [ "Range Doppler", "pg_radarsdk_introduction.html#sct_radarsdk_introduction_rangedopp", null ]
    ] ],
    [ "Introduction to Radar SDK", "pg_radarsdk_introduction_radar_sdk.html", [
      [ "Overview", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_radar_sdk_overview", null ],
      [ "The C library", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_radar_sdk_c", [
        [ "Modules", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_radar_sdk_architecture", null ],
        [ "Conventions", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_radar_sdk_update_conventions", null ],
        [ "Object Oriented Programming and Memory Management", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_radar_sdk_oop", null ],
        [ "Error Handling", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_radar_sdk_error_handling", null ],
        [ "Thread safety", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_radar_sdk_threads", null ],
        [ "Logging", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_radar_sdk_logging", null ]
      ] ],
      [ "Exporting a Device Configuration for a Firmware project", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_radar_sdk_registers", null ],
      [ "Device Access", "pg_radarsdk_introduction_radar_sdk.html#sct_radarsdk_introduction_device_access", [
        [ "libAvian", "pg_radarsdk_introduction_radar_sdk.html#ssct_radarsdk_libavian", null ],
        [ "Strata/Stratula", "pg_radarsdk_introduction_radar_sdk.html#ssct_radarsdk_strata", null ],
        [ "DeviceControl", "pg_radarsdk_introduction_radar_sdk.html#ssct_radarsdk_devicecontrol", null ],
        [ "Interaction between DeviceControl, Strata/Stratula, and libAvian", "pg_radarsdk_introduction_radar_sdk.html#ssct_radarsdk_interaction", null ],
        [ "Combining DeviceControl and libAvian", "pg_radarsdk_introduction_radar_sdk.html#ssct_radarsdk_devicecontrol_libavian", null ]
      ] ]
    ] ],
    [ "Setup Build environment guidelines", "pg_radarsdk_setup_build_environment.html", [
      [ "Radar SDK on Windows", "pg_radarsdk_setup_build_environment.html#ssct_radarsdk_setup_build_environmenton_windows", [
        [ "MinGW", "pg_radarsdk_setup_build_environment.html#windows_mingw", null ],
        [ "Visual Studio", "pg_radarsdk_setup_build_environment.html#windows_vs", null ],
        [ "WSL (Windows Subsystem for Linux)", "pg_radarsdk_setup_build_environment.html#windows_wsl", null ]
      ] ],
      [ "Radar SDK on Linux", "pg_radarsdk_setup_build_environment.html#ssct_radarsdk_setup_build_environmenton_linux", [
        [ "Policy and Guidelines", "pg_radarsdk_setup_build_environment.html#ABI", null ]
      ] ]
    ] ],
    [ "Device configuration guidelines", "pg_radarsdk_device_config_guide.html", [
      [ "Radar device configuration parameters", "pg_radarsdk_device_config_guide.html#ssct_radarsdk_device_config_guide_params", null ],
      [ "Measurement Frame", "pg_radarsdk_device_config_guide.html#sct_radarsdk_device_config_guide_measurement_frame", null ],
      [ "Code examples", "pg_radarsdk_device_config_guide.html#sct_radarsdk_device_config_guide_code_examples", null ]
    ] ],
    [ "JSON Configuration", "pg_radarsdk_json.html", [
      [ "Configuration structure", "pg_radarsdk_json.html#json_config_structure", [
        [ "Schema of device", "pg_radarsdk_json.html#json_schema_device", null ]
      ] ],
      [ "Schema of device_config", "pg_radarsdk_json.html#json_device_config", [
        [ "Schema of fmcw_scene", "pg_radarsdk_json.html#json_schema_fmcw_scene", null ],
        [ "Schema of fmcw_single_shape", "pg_radarsdk_json.html#json_schema_fmcw_single_shape", null ],
        [ "Schema of fmcw_shape_set", "pg_radarsdk_json.html#json_schema_fmcw_shape_set", null ]
      ] ],
      [ "Segmentation configuration", "pg_radarsdk_json.html#json_schema_segmentation", null ],
      [ "Presence Sensing", "pg_radarsdk_json.html#json_schema_presence_sensing", null ],
      [ "Example configurations", "pg_radarsdk_json.html#json_schema_examples", [
        [ "Using fmcw_scene", "pg_radarsdk_json.html#json_schema_examples_scene", null ],
        [ "Using fmcw_single_shape", "pg_radarsdk_json.html#json_schema_examples_single_shape", null ],
        [ "Using fmcw_shape_set", "pg_radarsdk_json.html#json_schema_examples_shape_set", null ],
        [ "Segmentation configuration", "pg_radarsdk_json.html#json_schema_examples_with_segmentation", null ],
        [ "Configuration for multiple devices", "pg_radarsdk_json.html#json_schema_examples_multiple_devices", null ]
      ] ]
    ] ],
    [ "Algorithms supported by Radar SDK", "pg_radarsdk_algorithms.html", [
      [ "Moving Target Indicator", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_mti", null ],
      [ "2D Moving Target Indicator", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_2dmti", null ],
      [ "Range-Doppler Map", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_rdm", null ],
      [ "Range-Angle Image", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_rai", null ],
      [ "Digital Beam Former (DBF)", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_dbf", null ],
      [ "Ordered Statistics Constant False Alarm Rate (OS-CFAR)", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_detect_oscfar", null ],
      [ "Density-Based Spatial Clustering of Applications with Noise (DBSCAN)", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_detect_dbscan", null ],
      [ "Parameters Estimation", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_detect_paramestimation", null ],
      [ "Capon", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_detect_capon", null ],
      [ "Segmentation", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_segmentation", null ],
      [ "Presence Sensing", "pg_radarsdk_algorithms.html#ssct_radarsdk_algorithms_presencesensing", null ]
    ] ],
    [ "Example applications", "pg_radarsdk_applications.html", [
      [ "Presence Detection APP", "pg_radarsdk_applications.html#ssct_radarsdk_applications_presence", [
        [ "Installation procedure", "pg_radarsdk_applications.html#sssct_radarsdk_applications_presence_detection_ip", null ],
        [ "Operation of FMCW based system", "pg_radarsdk_applications.html#ssct_app_presence_fmcw", null ],
        [ "State Machine", "pg_radarsdk_applications.html#ssct_app_presence_state", null ],
        [ "Absence State Processing", "pg_radarsdk_applications.html#ssct_app_presence_absencestate", null ],
        [ "Presence State Processing", "pg_radarsdk_applications.html#ssct_app_presence_presencestate", null ],
        [ "Radar device configuration", "pg_radarsdk_applications.html#ssct_app_presence_devcnfg_param", null ],
        [ "Presence sensing algorithm configuration", "pg_radarsdk_applications.html#ssct_app_presence_psalgocnfg_param", null ]
      ] ],
      [ "Segmentation APP", "pg_radarsdk_applications.html#ssct_radarsdk_applications_segmentation", [
        [ "Installation procedure", "pg_radarsdk_applications.html#sssct_radarsdk_applications_segmentation_ip", null ],
        [ "Radar device configuration", "pg_radarsdk_applications.html#ssct_app_segmentation_devcnfg_param", null ],
        [ "Segmentation antenna configuration", "pg_radarsdk_applications.html#ssct_app_segmentation_psantencnfg_param", null ],
        [ "Segmentation algorithm configuration", "pg_radarsdk_applications.html#ssct_app_segmentation_psalgocnfg_param", null ]
      ] ],
      [ "Raw Data APP", "pg_radarsdk_applications.html#ssct_radarsdk_applications_raw_data", [
        [ "Installation procedure", "pg_radarsdk_applications.html#sssct_radarsdk_applications_raw_data_ip", null ]
      ] ],
      [ "Recorder APP", "pg_radarsdk_applications.html#ssct_radarsdk_applications_recorder", [
        [ "Installation procedure", "pg_radarsdk_applications.html#sssct_radarsdk_applications_recorder_ip", null ]
      ] ]
    ] ],
    [ "References", "pg_radarsdk__ref.html", [
      [ "List of references", "pg_radarsdk__ref.html#ref_list", null ],
      [ "List of terms and abbreviations", "pg_radarsdk__ref.html#terms_list", null ]
    ] ],
    [ "Python wrapper usage", "pg_rdk_python.html", [
      [ "1    Quick start guide", "pg_rdk_python.html#sct_rdk_python_quick", [
        [ "1.1 Requirements", "pg_rdk_python.html#sct_rdk_python_requirements", null ],
        [ "1.2 Running the example", "pg_rdk_python.html#sct_rdk_python_running_the_example", null ],
        [ "1.3   Help function", "pg_rdk_python.html#sct_rdk_python_help_function", null ],
        [ "1.4    Running the example applications", "pg_rdk_python.html#sct_rdk_python_running_applications", null ]
      ] ],
      [ "2 Writing a Radar application using Python", "pg_rdk_python.html#sct_rdk_python_algo", [
        [ "2.1 Importing the Python wrapper", "pg_rdk_python.html#sct_rdk_python_algo_import", null ],
        [ "2.2 Connecting to the Radar sensor", "pg_rdk_python.html#sct_rdk_python_algo_connect", null ],
        [ "2.3 Setting a Radar configuration", "pg_rdk_python.html#sct_rdk_python_algo_config", null ],
        [ "2.4 Fetching Radar data", "pg_rdk_python.html#sct_rdk_python_algo_data", null ],
        [ "2.5 A presence sensing application", "pg_rdk_python.html#sct_rdk_python_running_the_algoexample", [
          [ "2.5.1 Configuring the Radar and algorithm", "pg_rdk_python.html#sct_rdk_python_presence_config", null ],
          [ "2.5.2 Computing Radar distance data", "pg_rdk_python.html#sct_rdk_python_fft_spectrum", null ],
          [ "2.5.3 Presence sensing algorithm", "pg_rdk_python.html#sct_rdk_python_presence_algo", null ]
        ] ],
        [ "2.6 Extension into an anti-peeking application", "pg_rdk_python.html#sct_rdk_python_algoext", null ]
      ] ]
    ] ],
    [ "MATLAB wrapper usage", "pg_rdk_matlab.html", [
      [ "1    Quick start guide", "pg_rdk_matlab.html#sct_rdk_matlab_quick", [
        [ "1.1 Requirements", "pg_rdk_matlab.html#sct_rdk_matlab_requirements", null ],
        [ "1.2  Location of wrapper in release package", "pg_rdk_matlab.html#sct_rdk_matlab_extracting_the_package", null ],
        [ "1.3   Start MATLAB", "pg_rdk_matlab.html#sct_rdk_matlab_start", null ],
        [ "1.4 Run the example", "pg_rdk_matlab.html#sct_rdk_matlab_run_the_example", null ],
        [ "1.5   Help function", "pg_rdk_matlab.html#sct_rdk_matlab_help_function", null ]
      ] ],
      [ "2  Writing a Radar application using MATLAB", "pg_rdk_matlab.html#sct_rdk_matlab_algo", [
        [ "2.1 Accessing the MATLAB wrapper", "pg_rdk_matlab.html#sct_rdk_matlab_algo_import", null ],
        [ "2.2 Setting a Radar configuration", "pg_rdk_matlab.html#sct_rdk_matlab_algo_config", null ],
        [ "2.3 Connecting to the Radar sensor", "pg_rdk_matlab.html#sct_rdk_matlab_algo_connect", null ],
        [ "2.4 Fetching Radar data", "pg_rdk_matlab.html#sct_rdk_matlab_algo_data", null ],
        [ "2.5 A presence sensing application", "pg_rdk_matlab.html#sct_rdk_matlab_running_the_algoexample", [
          [ "2.5.1 Configuring the Radar and algorithm", "pg_rdk_matlab.html#sct_rdk_matlab_presence_config", null ],
          [ "2.5.2 Computing Radar distance data", "pg_rdk_matlab.html#sct_rdk_matlab_fft_spectrum", null ],
          [ "2.5.3 Presence sensing algorithm", "pg_rdk_matlab.html#sct_rdk_matlab_presence_algo", null ]
        ] ],
        [ "2.6 Extension into anti-peeking application", "pg_rdk_matlab.html#sct_rdk_matlab_algoext", null ]
      ] ]
    ] ],
    [ "Modules", "modules.html", "modules" ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Variables", "functions_vars.html", "functions_vars" ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"2_d_m_t_i_8h.html",
"_error_8h.html#af23114ffd89a12826ab672311501381c",
"group__gr__anglemonopulse.html#ga7b96a0b714918d08d8cc02c6b2a40991",
"group__gr__math.html#ga27af3de900d150d2a36e90799e9fc3a3",
"group__gr__ppfft.html#ga10c8ca62729c329d5e032f5a9cede78c",
"group__gr__types.html#gaa24c2c1cc332d296371b6b2cfea0843f",
"pg_rdk_matlab.html#sct_rdk_matlab_start",
"structifx___presence_sensing___config__t.html#ad77c0022999d8fa9617e9fbdbd1c6984"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';