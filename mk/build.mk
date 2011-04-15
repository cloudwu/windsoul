.PHONY : all

all :

LIBSRCS := \
  src/core/memory.c \
  src/core/atom.c \
  src/core/map.c

include mk/build_o.mk
include mk/build_init.mk

LIBTAR := libwindsoul.a

include mk/build_a.mk

	