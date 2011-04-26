.PHONY : test

test :

TESTSRCS := \
  src/test/set.c \
  src/test/map.c \
  src/test/log.c

INC := src/core
INC := $(addprefix -I,$(INC))

MKDIRS := $(MKDIRS) $(BUILD_TOP)/test $(TEST_DIR)

define TEST_temp
  TAR_O :=  $(BUILD_TOP)/test/$(notdir $(basename $(1)))
  $$(TAR_O).o : | $(BUILD_TOP)/test
  -include $$(TAR_O).d
  $$(TAR_O).o : $(1) 
	$(CC) -c -o $$@ $(CFLAGS) $(INC) -MMD $(1)
  TAR_EXE := $(TEST_DIR)/$(notdir $(basename $(1)))$(EXE)
  test : $$(TAR_EXE)
  $$(TAR_EXE) : | $(TEST_DIR)
  $$(TAR_EXE) : OBJS := $$(TAR_O).o
  $$(TAR_EXE) : $$(TAR_O).o $(LIBTAR)
	$(CC) -o $$@ $(LDFLAGS) $$(OBJS) -L$(LIB_DIR) -lwindsoul
endef

$(foreach s,$(TESTSRCS),$(eval $(call TEST_temp,$(s))))



	