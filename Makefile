# Project Name
TARGET = ParametricChorus

# Library Locations
LIBDAISY_DIR = ../../libdaisy
DAISYSP_DIR = ../../DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

# Sources
CPP_SOURCES = table_osc.cpp iir_bp_fb.cpp iir_lp_nf.cpp hilbert_pdn.cpp ParametricChorus.cpp
C_SOURCES = $(LIBDAISY_DIR)/Drivers/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c

GCC_PATH = /data/nucleo/gcc-arm-none-eabi-10-2020-q4-major/bin/

#OPT = -Ofast
#DEBUG = 1
include $(SYSTEM_FILES_DIR)/Makefile
