#
# Rules for building project
#

ROOT_PATH := ..
ABS_ROOT_PATH := $(shell pwd)

#GEN_BINS= app.bin
GEN_TARGETS= app.elf

DIRS ?= $(patsubst %/,%,$(dir $(wildcard */Makefile)))

CCFLAGS += -Ofast -Wno-pointer-to-int-cast

COMPONENTS_app = \
	mango_test/libmango_test.a	\
	fcmd/libfcmd.a		\
	user/libuser.a

LINKFLAGS_app += -L$(ROOT_PATH)/lib \
	-Wl,-Bdynamic -lpthread -lssl -lcrypto -lreadline -lncursesw \
	-Wl,--whole-archive \
	-lmango \
	-Wl,--no-whole-archive

DEFINES +=

INCLUDES += -I $(ROOT_PATH)/app/user
INCLUDES += -I $(ROOT_PATH)/app/fcmd
INCLUDES += -I $(ROOT_PATH)/components/mango

sinclude $(ROOT_PATH)/Makefile