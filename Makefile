.SUFFIXES:

BUILD		:= build
UNAME		:= $(shell uname)

ifneq ($(BUILD),$(notdir $(CURDIR)))

.PHONY: $(BUILD) clean

export QMAKE		:= $(CURDIR)/qt/qtbase/bin/qmake
export PROJECTFILE	:= $(CURDIR)/webkitJNI.pro
export OUTPUT		:= $(CURDIR)/lib$(notdir $(basename $(PROJECTFILE)))
export EXTENSION	:= $(notdir $(basename $(PROJECTFILE)))

ifeq ($(UNAME), Darwin)
	export OSEXTENSION		:= dylib
	export FINALEXTENSION	:= jnilib
else
	export OSEXTENSION		:= so
	export FINALEXTENSION	:= so
endif

$(BUILD):	
	[ -d $@ ] || mkdir -p $@
	$(MAKE) -C $(BUILD) -f $(CURDIR)/Makefile 

all: $(BUILD)

clean:
		@echo clean ...
		@rm -fr $(BUILD) $(OUTPUT).$(FINALEXTENSION)

else

$(OUTPUT).lib: Makefile.$(EXTENSION)
		@echo $(notdir $<)
		$(MAKE) -f $<
		cp $(CURDIR)/$(notdir $(basename $(OUTPUT))).$(OSEXTENSION) $(OUTPUT).$(FINALEXTENSION)
		
Makefile.$(EXTENSION): $(PROJECTFILE)
		@echo $(notdir $<)
		$(QMAKE) -o $@ $< 
		
endif
