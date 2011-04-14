MKDIRS := $(MKDIRS) $(BUILD_TOP)/o
LIB_O :=

define BUILD_temp
  TAR :=  $(BUILD_TOP)/o/$(notdir $(basename $(1)))
  LIB_O := $(LIB_O) $$(TAR).o
  $$(TAR).o : | $(BUILD_TOP)/o
  -include $$(TAR).d
  $$(TAR).o : $(1)
	$(CC) -c -o $$@ $(CFLAGS) -MMD $$<
endef

$(foreach s,$(LIBSRCS),$(eval $(call BUILD_temp,$(s))))



