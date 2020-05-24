ARDUINO_DIR = /usr/share/arduino
ARDMK_DIR = /usr/share/arduino
AVR_TOOLS_DIR = /usr
AVRDUDE_CONF = /etc/avrdude.conf
ARDUINO_LIBS = Rotary SSD1306Ascii Si5351Arduino PCF8574 Wire EEPROM LiquidCrystal

PROJECT_DIR = $(shell pwd)
SRCDIR = $(PROJECT_DIR)/src
LOCAL_CPP_SRCS ?= $(wildcard $(SRCDIR)/*.cpp)
LOCAL_INO_SRCS ?= $(wildcard $(SRCDIR)/*.ino)
BOARD_TAG = nano328
MONITOR_BAUDRATE = 9600
CPPFLAGS_STD = -std=c++11
CPPFLAGS += -I$(SRCDIR)
MONITOR_PORT = /dev/ttyUSB0
CURRENT_DIR = $(shell basename $(CURDIR))
OBJDIR = $(PROJECT_DIR)/bin/$(BOARD_TAG)/$(CURRENT_DIR)
include $(ARDMK_DIR)/Arduino.mk
