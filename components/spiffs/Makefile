#
# Rules for building component
#

ROOT_PATH := ../..

LIBS = libspiffs.a

DIRS_IGNORE := 
DIRS_ALL := $(patsubst ./%,%,$(shell find . -type d))
DIRS  := $(filter-out $(DIRS_IGNORE),$(DIRS_ALL))
CSRCS := $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.[csS]) $(wildcard $(dir)/*.cpp)))
CSRCS := $(patsubst ./%,%,$(CSRCS))
OBJS  := $(addsuffix .o,$(CSRCS))


CFLAGS += 
INCLUDES += 

sinclude $(ROOT_PATH)/Makefile


#############################################################
# make spiffs tools
#
.PHONY: fs

FSSOURCE ?= ./fs/
SPIFFSFILES ?= $(patsubst $(FSSOURCE)%,%,$(shell find $(FSSOURCE) -name '*' '!' -name .gitignore ))

mkspiffs: flash_device.c ./spiffs_cache.c ./spiffs_check.c ./spiffs_gc.c ./spiffs_hydrogen.c ./spiffs_nucleus.c
	$(CC) $(CFLAGS) -DENABLE_SPIFFS_TOOLS -I. --include ./spiffs_config.h $^ $(LDFLAGS) -o $@

fs:
	@echo $(SPIFFSFILES)
	@rm -f ./fs.lst
	@echo "" >> ./fs.lst
	$(foreach f, $(SPIFFSFILES), echo "import $(FSSOURCE)$(f) $(f)" >> ./fs.lst ;)
