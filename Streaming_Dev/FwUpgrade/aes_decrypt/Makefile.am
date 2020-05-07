AUTOMAKE_OPTIONS = foreign
SUBDIRS=  

bin_PROGRAMS= aes_decrypt

aes_decrypt_SOURCES = ./src/aes.c \
                      ./src/aes_sw.c \
                      ./src/aes_cryptodev.c \
                      ./src/decrypt.c \
                      ./src/main.c

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

LIBS += -L$(SDK_PATH)/HubbleAPI/nxcCommon/.libs -lnxcCommon \
        -L$(SDK_PATH)/HubbleAPI/nxcNetwork/.libs -lnxcNetwork\
        -L$(SDK_PATH)/HubbleAPI/nxcHubbleAPI/.libs -lnxcHubbleAPI\
        -L$(SDK_PATH)/CfgMgr/nvconf/.libs -lnv -lpthread -lm\
        -L$(SDK_PATH)/3rdlib/lib -lcurl
