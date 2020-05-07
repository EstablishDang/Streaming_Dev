AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS=aes_decrypt_full

aes_decrypt_full_SOURCES= main.c encrypt.c aes_cryptodev.c aes.c aes_sw.c decrypt.c

AM_CFLAGS=-O2 -D_GNU_SOURCE -DNDEBUG

LIBS = -lpthread -lm -lrt
 

AM_CPPFLAGS= -I. -I./inc -I$(SDK_PATH)/CfgMgr/nvconf/inc -I$(SDK_PATH)/CfgMgr/nvconf/ \
             -I$(SDK_PATH)/HubbleAPI/include