MKDIRS := $(MKDIRS) $(BUILD_TOOLS)

GENINIT := $(BUILD_TOOLS)/geninit$(EXE)

$(GENINIT) : | $(BUILD_TOOLS)
$(GENINIT) : build_tools/geninit.c
	$(CC) $(CFLAGS) -o $@ $<

LIB_INIT_O := $(LIB_O:.o=.init.o)
LIB_INIT_C := $(LIB_O:.o=.init.c)
LIB_NAMES := $(sort $(basename $(notdir $(LIB_O))))

$(LIB_INIT_O) : $(BUILD_TOP)/o/%.o : $(BUILD_TOP)/o/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIB_INIT_C) : $(BUILD_TOP)/o/%.init.c : $(BUILD_TOP)/o/%.o $(GENINIT)
	objdump -t $< | $(GENINIT) $@ $(LIB_NAMES)
