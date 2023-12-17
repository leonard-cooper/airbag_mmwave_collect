cmake_minimum_required(VERSION 3.0)

set(KISS_FFT_DIR ${RDK_3RD_PARTY_DIR}/libs/kissfft)

set(KISS_FFT_HEADERS
    kfc.h _kiss_fft_guts.h
    kiss_fft.h kiss_fft_log.h
    kiss_fftnd.h kiss_fftndr.h
    kiss_fftr.h kiss_fftr.h)

set(KISS_FFT_SOURCES 
    kfc.c kiss_fft.c kiss_fftnd.c
    kiss_fftndr.c kiss_fftr.c)

if(ENABLE_DOUBLE)
    # message(STATUS "Kiss FFT variant: double")
    set(KISS_FFT_DEFINES kiss_fft_scalar=double)
else()
    # message(STATUS "Kiss FFT variant: float")
    set(KISS_FFT_DEFINES kiss_fft_scalar=float)
endif()

add_library(kissfft STATIC ${KISS_FFT_HEADERS} ${KISS_FFT_SOURCES})
target_include_directories(kissfft PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(kissfft PUBLIC ${KISS_FFT_DEFINES})
