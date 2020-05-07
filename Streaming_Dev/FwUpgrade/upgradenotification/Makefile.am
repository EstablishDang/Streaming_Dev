AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS=upgradenotification

upgradenotification_SOURCES= src/main.c

AM_CFLAGS=-O2 -D_GNU_SOURCE
AM_CPPFLAGS = -I.

LIBS += -L$(SDK_PATH)/HubbleAPI/nxcHubbleAPI/.libs -lnxcHubbleAPI\
        -L$(SDK_PATH)/CfgMgr/nvconf/.libs -lnv

AM_CPPFLAGS += -I$(SDK_PATH)/CfgMgr/nvconf/inc \
               -I$(SDK_PATH)/HubbleAPI/include \
               -I$(SDK_PATH)/HubbleAPI/nxcHubbleAPI

if BUILD_MODEL_SET
AM_CPPFLAGS += -I$(SDK_PATH)/PlatformBSP/$(CHIP_PROVIDER)/$(BUILD_MODEL)/al
endif
               
