AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS=fwupgrade fwupgrade_percent fullupgrade

fwupgrade_SOURCES= main.c

fwupgrade_percent_SOURCES= shmset.c

fullupgrade_SOURCES= fullupgrade.c

AM_CFLAGS=-O2 -D_GNU_SOURCE
AM_CPPFLAGS = -I.

LIBS += -lpthread -lm

if BUILD_MODEL_SET
AM_CPPFLAGS += -I$(SDK_PATH)/PlatformBSP/$(CHIP_PROVIDER)/$(BUILD_MODEL)/al
endif
               
