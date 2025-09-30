#!/usr/bin/make -f
# Makefile for Studio Reverb

# --------------------------------------------------------------
# Project name, used for binaries

NAME = StudioReverb

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	Plugin.cpp \
	DSP.cpp \
	common/freeverb/revbase.cpp \
	common/freeverb/earlyref.cpp \
	common/freeverb/progenitor.cpp \
	common/freeverb/progenitor2.cpp \
	common/freeverb/slot.cpp \
	common/freeverb/delay.cpp \
	common/freeverb/comb.cpp \
	common/freeverb/allpass.cpp \
	common/freeverb/biquad.cpp \
	common/freeverb/efilter.cpp \
	common/freeverb/delayline.cpp \
	common/freeverb/utils.cpp \
	common/freeverb/nrevb.cpp

FILES_UI = \
	UI.cpp

# --------------------------------------------------------------
# Do some magic

include dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all plugin types

ifeq ($(HAVE_OPENGL),true)
TARGETS += lv2 vst2 vst3
else
TARGETS += lv2 vst2 vst3
endif

all: $(TARGETS)

# --------------------------------------------------------------
# Additional flags

BUILD_CXX_FLAGS += -I./common
BUILD_CXX_FLAGS += -std=c++11
BUILD_CXX_FLAGS += -DLIBFV3_FLOAT
BUILD_CXX_FLAGS += -fvisibility=hidden
BUILD_CXX_FLAGS += -fdata-sections -ffunction-sections

ifeq ($(HAVE_OPENGL),true)
BUILD_CXX_FLAGS += -DHAVE_OPENGL
endif

# Optimize for size in release builds
ifeq ($(DEBUG),true)
BUILD_CXX_FLAGS += -O0 -g
else
BUILD_CXX_FLAGS += -O3 -ffast-math -fno-finite-math-only
BUILD_LDFLAGS += -Wl,--strip-all -Wl,--gc-sections
endif

# --------------------------------------------------------------
# Installation paths

PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib

# User installation
install-user: all
	install -d $(HOME)/.lv2
	install -d $(HOME)/.vst
	install -d $(HOME)/.vst3
	cp -r bin/$(NAME).lv2 $(HOME)/.lv2/
	-cp bin/$(NAME)-vst.so $(HOME)/.vst/
	-cp -r bin/$(NAME).vst3 $(HOME)/.vst3/

# System installation
install: all
	install -d $(DESTDIR)$(LIBDIR)/lv2
	install -d $(DESTDIR)$(LIBDIR)/vst
	install -d $(DESTDIR)$(LIBDIR)/vst3
	cp -r bin/$(NAME).lv2 $(DESTDIR)$(LIBDIR)/lv2/
	-cp bin/$(NAME)-vst.so $(DESTDIR)$(LIBDIR)/vst/
	-cp -r bin/$(NAME).vst3 $(DESTDIR)$(LIBDIR)/vst3/

# --------------------------------------------------------------