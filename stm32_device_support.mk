
## DEVICE_SUPPORT_DIR
# the root directory of this project (where stm32_device_support.mk is) 
# must be defined in the variable DEVICE_SUPPORT_DIR, relative to the 
# directory where thw project make file is.


CFLAGS += -I$(DEVICE_SUPPORT_DIR)
include $(DEVICE_SUPPORT_DIR)/board/board.mk
include $(DEVICE_SUPPORT_DIR)/device/device.mk

###############################################################
# 
###############################################################
SYSTEMDIR = $(DEVICE_SUPPORT_DIR)/system

CFLAGS += -I$(SYSTEMDIR)

SOURCE += $(SYSTEMDIR)/system.c
SOURCE += $(SYSTEMDIR)/asserts.c
SOURCE += $(SYSTEMDIR)/hardware_exception.c
SOURCE += $(SYSTEMDIR)/stackoverflow.c

###############################################################
# Drivers defined here must build for all targets
###############################################################

DRIVERSDIR = $(DEVICE_SUPPORT_DIR)/drivers

CFLAGS += -I$(DRIVERSDIR)

## LEDS
CFLAGS += -DUSE_DRIVER_LEDS=$(USE_DRIVER_LEDS)
ifeq ($(USE_DRIVER_LEDS), 1)
CFLAGS += -I$(DRIVERSDIR)/leds
SOURCE += $(DRIVERSDIR)/leds/leds.c
endif

## USART
CFLAGS += -DUSE_DRIVER_USART=$(USE_DRIVER_USART)
ifeq ($(USE_DRIVER_USART), 1)
CFLAGS += -I$(DRIVERSDIR)/usart
SOURCE += $(DRIVERSDIR)/usart/usart.c
SOURCE += $(DRIVERSDIR)/usart/usart_it.c
endif

## SD Card, SDIO
CFLAGS += -DUSE_DRIVER_SDCARD_SDIO=$(USE_DRIVER_SDCARD_SDIO)
ifeq ($(USE_DRIVER_SDCARD_SDIO), 1)
SOURCE += $(DRIVERSDIR)/sdcard/sdcard.c
SOURCE += $(DRIVERSDIR)/sdcard/sdcard_sdio.c
SOURCE += $(DRIVERSDIR)/sdcard/sdfs.c
CFLAGS += -I$(DRIVERSDIR)/sdcard
endif

## SD Card, SPI
CFLAGS += -DUSE_DRIVER_SDCARD_SPI=$(USE_DRIVER_SDCARD_SPI)
ifeq ($(USE_DRIVER_SDCARD_SPI), 1)
SOURCE += $(DRIVERSDIR)/sdcard/sdcard.c
SOURCE += $(DRIVERSDIR)/sdcard/sdcard_spi.c
SOURCE += $(DRIVERSDIR)/sdcard/sdfs.c
CFLAGS += -I$(DRIVERSDIR)/sdcard
endif

## Networking

CFLAGS += -DUSE_DRIVER_LWIP_NET=$(USE_DRIVER_LWIP_NET)
ifeq ($(USE_DRIVER_LWIP_NET), 1)
CFLAGS += -I$(DRIVERSDIR)/net_lwip
CFLAGS += -I$(DRIVERSDIR)/net_lwip/netif

SOURCE += $(DRIVERSDIR)/net_lwip/net.c
SOURCE += $(DRIVERSDIR)/net_lwip/netconf.c
SOURCE += $(DRIVERSDIR)/net_lwip/netif/ethernetif.c
SOURCE += $(DRIVERSDIR)/net_lwip/arch/sys_arch.c

endif

## Ethernet PHY, ENC28J60
CFLAGS += -DUSE_DRIVER_ENC28J60_PHY=$(USE_DRIVER_ENC28J60_PHY)
ifeq ($(USE_DRIVER_ENC28J60_PHY), 1)
SOURCE += $(DRIVERSDIR)/ethernet/enc28j60.c
CFLAGS += -I$(DRIVERSDIR)/ethernet
CFLAGS += -DCHECKSUM_BY_HARDWARE=0
endif

## Ethernet PHY, BCM5241
CFLAGS += -DUSE_DRIVER_MII_RMII_PHY=$(USE_DRIVER_MII_RMII_PHY)
ifeq ($(USE_DRIVER_MII_RMII_PHY), 1)
SOURCE += $(DRIVERSDIR)/ethernet/MII_RMII.c
CFLAGS += -I$(DRIVERSDIR)/ethernet
CFLAGS += -DCHECKSUM_BY_HARDWARE=1
endif

###############################################################
# Include drivers for specific targets
###############################################################

## Configure drivers for FAMILY

ifeq ($(FAMILY), STM32F1)

endif

ifeq ($(FAMILY), STM32F4)

endif

