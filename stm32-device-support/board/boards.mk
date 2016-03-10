
## default board dir is in the appleseed DEVICE_SUPPORT_DIR location
# override BOARDDIR in custom board configuration board.mk
BOARDDIR ?= $(DEVICE_SUPPORT_DIR)/board

## default supported BOARDS
# add to BOARDS in board configuration makefile, board.mk
BOARDS = 

## Select DEVICE and FAMILY, by BOARD

## configure BOARD
include $(BOARDDIR)/$(BOARD).bsp/board.mk

## test BOARD againt BOARDS
ifeq ($(filter $(BOARD),$(BOARDS)), )
$(error board '$(BOARD)' not supported. specify BSP name without .bsp, supported boards: $(shell cd $(BOARDDIR); ls -d */))
endif

## if no network drivers have been enabled then set PHY selection to none
USE_DRIVER_ENC28J60_PHY ?= 0
USE_DRIVER_MII_RMII_PHY ?= 0

## configure BOARD SOURCE and CFLAGS
CFLAGS += -I$(DEVICE_SUPPORT_DIR)/board
CFLAGS += -I$(BOARDDIR)/$(BOARD).bsp
SOURCE += $(BOARDDIR)/$(BOARD).bsp/board_config.c


