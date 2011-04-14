ifeq ($(WINDSOUL),)
  include mk/usage.mk
else
  include mk/defines.mk

  ifeq ($(strip $(MAKECMDGOALS)),clean)
    include mk/clean.mk
  else
    include mk/build.mk
    include mk/mkdir.mk
  endif
endif

