#
# Rules for building component
#

ROOT_PATH := ../..

LIBS = libjpge.a

DIRS_IGNORE := 
DIRS_ALL := $(patsubst ./%,%,$(shell find . -type d))
DIRS  := $(filter-out $(DIRS_IGNORE),$(DIRS_ALL))
CSRCS := $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.[csS]) $(wildcard $(dir)/*.cpp)))
CSRCS := $(patsubst ./%,%,$(CSRCS))
OBJS  := $(addsuffix .o,$(CSRCS))


INCLUDES += 

sinclude $(ROOT_PATH)/Makefile
