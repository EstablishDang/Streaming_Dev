AUTOMAKE_OPTIONS = foreign
SUBDIRS=  

bin_PROGRAMS= sig_check sig_SHA512_check

sig_check_SOURCES = sig_check.c
sig_SHA512_check_SOURCES = sig_SHA512_check.c

AM_CFLAGS    = -O2 -D_GNU_SOURCE
AM_CPPFLAGS  = -I. -I./inc \
               -I$(SDK_PATH)/3rdlib/include \
               -I$(SDK_PATH)/CfgMgr/nvconf/inc \
               -I$(SDK_PATH)/HubbleAPI/include \
               -I$(SDK_PATH)/HubbleAPI/nxcHubbleAPI \
               -I$(SDK_PATH)/HubbleAPI/nxcCommon \
               -I$(SDK_PATH)/HubbleAPI/nxcNetwork/inc

if BUILD_MODEL_SET
AM_CPPFLAGS += -I$(SDK_PATH)/PlatformBSP/$(CHIP_PROVIDER)/$(BUILD_MODEL)/bsp/include \
               -I$(SDK_PATH)/PlatformBSP/$(CHIP_PROVIDER)/$(BUILD_MODEL)/al
endif

LIBS += -L$(SDK_PATH)/CfgMgr/nvconf/.libs -lpthread \
        -L$(SDK_PATH)/3rdlib/lib -lmbedtls -lmbedx509 -lmbedcrypto