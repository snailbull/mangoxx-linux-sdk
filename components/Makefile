#
# Rules for building project
#

ROOT_PATH := ..

#GEN_IMAGES= app.out
#GEN_BINS= app.bin
#GEN_TARGETS= app.elf

DIRS ?= $(patsubst %/,%,$(dir $(wildcard */Makefile)))

CCFLAGS += -Ofast

TARGET_LDFLAGS =

COMPONENTS_app = \
	user/libuser.a

LINKFLAGS_app := \
	-L$(ROOT_PATH)/lib	\
	-lssl			\
	-lcrypto		\
	-lpthread

DEFINES +=

INCLUDES += -I $(ROOT_PATH)/include

sinclude $(ROOT_PATH)/Makefile