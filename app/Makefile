#############################################################
# Required variables for each makefile
# Discard this section from all parent makefiles
# Expected variables (with automatic defaults):
#   CSRCS (all "C" files in the dir)
#   SUBDIRS (all subdirs with a Makefile)
#   GEN_LIBS - list of libs to be generated ()
#   GEN_IMAGES - list of object file images to be generated ()
#   GEN_BINS - list of binaries to be generated ()
#   COMPONENTS_xxx - a list of libs/objs in the form
#     subdir/lib to be extracted and rolled up into
#     a generated lib/image xxx.a ()
#
#FLAVOR = release
FLAVOR = debug

ifndef PDIR # {
#GEN_IMAGES= app.out
#GEN_BINS= app.bin
GEN_TARGETS= app.elf
SUBDIRS ?= $(patsubst %/,%,$(dir $(wildcard */Makefile)))
endif # } PDIR

CCFLAGS += -Ofast -Wno-pointer-to-int-cast

TARGET_LDFLAGS =

COMPONENTS_app = \
	mango_test/libmango_test.a	\
	fcmd/libfcmd.a		\
	mango/libmango.a	\
	user/libuser.a

LINKFLAGS_app := \
	-L$(PDIR)lib	\
	-Wl,-E			\
	-lssl			\
	-lcrypto		\
	-lreadline		\
	-lncursesw		\
	-lpthread

#DEPENDS_app := $(LD_FILE)

#############################################################
# Configuration i.e. compile options etc.
# Target specific stuff (defines etc.) goes in here!
# Generally values applying to a tree are captured in the
#   makefile at its root level - these are then overridden
#   for a subtree within the makefile rooted therein
#

#UNIVERSAL_TARGET_DEFINES =		\

# Other potential configuration flags include:
#	-DTXRX_TXBUF_DEBUG
#	-DTXRX_RXBUF_DEBUG
#	-DWLAN_CONFIG_CCX
#CONFIGURATION_DEFINES =	-DICACHE_FLASH

DEFINES +=				\
	$(UNIVERSAL_TARGET_DEFINES)	\
	$(CONFIGURATION_DEFINES)


#############################################################
# Recursion Magic - Don't touch this!!
#
# Each subtree potentially has an include directory
#   corresponding to the common APIs applicable to modules
#   rooted at that subtree. Accordingly, the INCLUDE PATH
#   of a module can only contain the include directories up
#   its parent path, and not its siblings
#
# Required for each makefile to inherit from the parent
#
INCLUDES += -I $(PDIR)include
INCLUDES += -I $(PDIR)app/mango
INCLUDES += -I $(PDIR)app/user

ifndef PDIR
PDIR := $(CURDIR)/../
else
PDIR := $(PDIR)../
endif
sinclude $(PDIR)Makefile