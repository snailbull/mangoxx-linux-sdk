#
# Rules for building project
#

ROOT_PATH := ../..

DIRS_IGNORE := fsm_simple minilzo_test tinyxml_test
DIRS_ALL := $(patsubst ./%,%,$(shell find . -type d))
DIRS  := $(filter-out $(DIRS_IGNORE),$(DIRS_ALL))

CSRCS := $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.[csS]) $(wildcard $(dir)/*.cpp)))
CSRCS := $(patsubst ./%,%,$(CSRCS))
OBJS  := $(addsuffix .o,$(CSRCS))

###############################################################################
# User define variable
#
CFLAGS += -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast

LD_FLAGS += -Wl,-Bdynamic \
	-lpthread -lssl -lcrypto -lreadline -lncursesw -lm \
	-Wl,--whole-archive \
	-lmango \
	-lcjson \
	-ljpge \
	-Wl,--no-whole-archive

TARGETS= $(PROJECT_NAME).elf

INCLUDES += -I $(ROOT_PATH)/apps/$(PROJECT_NAME)
INCLUDES += -I $(ROOT_PATH)/components/mango
INCLUDES += -I $(ROOT_PATH)/components/jpge

sinclude $(ROOT_PATH)/Makefile
