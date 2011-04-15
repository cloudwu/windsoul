BUILD_TOP := $(WINDSOUL)/build
LIB_DIR := $(WINDSOUL)/lib
BUILD_TOOLS := $(WINDSOUL)/build_tools

CC := gcc
AR := ar rc
RM := rm
RMDIR := rm -r -f
MKDIR := mkdir -p
DEBUG ?= -g -O0
TOUCH := touch

ifeq ($(filter -g,$(DEBUG)),-g)
  LDFLAGS := -g -Wall
else
  LDFLAGS := -s -Wall
endif

CFLAGS = $(DEBUG) -Werror -Wall
