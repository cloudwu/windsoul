.PHONY : all

all :

LIBSRCS := \
  src/core/memory.c \
  src/core/atom.c \
  src/core/map.c \
  src/core/set.c \
  src/core/log.c \
  src/core/utf8.c \
  src/core/nativefs.c \
  src/core/path.c \
  src/core/vfs.c

include mk/build_o.mk
include mk/build_init.mk

LIBTAR := libwindsoul.a

include mk/build_a.mk

	