MKDIRS := $(sort $(MKDIRS))

define SAFE_MKDIR
  CHILD := $(firstword $(filter $(1)/%,$(MKDIRS)))
  ifeq ($$(strip $$(CHILD)),)
    $(1) :
	$(MKDIR) $$@
  else
    $(1) : | $$(CHILD)
  endif
endef

$(foreach dir,$(MKDIRS),$(eval $(call SAFE_MKDIR,$(dir))))
