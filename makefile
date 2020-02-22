# common linux makefile
# 2018-9-4
#
# ld -verbose    系统默认的连接脚本
#
# LFLAGS+= --eh-frame-hdr -m elf_i386 --hash-style=gnu -dynamic-linker /lib/ld-linux.so.2 
# LFLAGS+= /usr/lib/gcc/i386-redhat-linux/4.1.1/../../../crt1.o  /usr/lib/gcc/i386-redhat-linux/4.1.1/../../../crti.o  /usr/lib/gcc/i386-redhat-linux/4.1.1/crtbegin.o
# LFLAGS+= --no-as-needed /usr/lib/gcc/i386-redhat-linux/4.1.1/crtend.o /usr/lib/gcc/i386-redhat-linux/4.1.1/../../../crtn.o
# gcc -c hello.c -o hello.o
# ld $(LFLAGS) -o hello hello.o
#
# -Wl,-T my.lds   或者使用自己写的ld脚本
#
# ldconfig -p | grep libcrypto   查找库文件路径

CROSS_COMPILE = 
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
NM = $(CROSS_COMPILE)nm
CPP = $(CROSS_COMPILE)g++
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

__CONFIG_DEBUG = debug

ifeq ($(__CONFIG_DEBUG),debug)
OUTPUT := .output/debug
else
OUTPUT := .output/release
endif

LD_FILE := $(ROOT_PATH)/ld/app.ld
BIN_FILE := $(ROOT_PATH)/bin/app.bin

DIRS  ?= $(patsubst %/,%,$(dir $(wildcard */Makefile)))
CSRCS ?= $(basename $(wildcard *.c)) $(basename $(wildcard *.cpp)) $(basename $(wildcard *.[sS]))
OBJS := $(CSRCS:%=$(OUTPUT)/%.o)
DEPS := $(CSRCS:%=$(OUTPUT)/%.d)

LIBS := $(GEN_LIBS:%=$(OUTPUT)/%)
BINS := $(GEN_BINS:%=$(OUTPUT)/%)

GEN_BUILTS := $(foreach f,$(DIRS),$(f)/$(OUTPUT)/built-in.o)

CCFLAGS += -g -Wpointer-arith -Wundef

CFLAGS = $(CCFLAGS) $(DEFINES) $(INCLUDES)


#############################################################
# Functions
#

define ShortcutRule
$(1): .subdirs $(2)/$(1)
endef

define MakeLibrary
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(OUTPUT)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OUTPUT)/$$(notdir $$(obj)))
$$(OUTPUT)/$(1).a: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1))
	@mkdir -p $$(OUTPUT)
	@$$(if $$(filter %.a,$$?),mkdir -p $$(EXTRACT_DIR)_$(1))
	@$$(if $$(filter %.a,$$?),cd $$(EXTRACT_DIR)_$(1); $$(foreach lib,$$(filter %.a,$$?),$$(AR) xo $$(UP_EXTRACT_DIR)/$$(lib);))
	@echo AR $(1).a
	@$$(AR) ru $$@ $$(filter %.o,$$?) $$(if $$(filter %.a,$$?),$$(EXTRACT_DIR)_$(1)/*.o)
	@echo LD built-in.o
	$$(LD) -r -o $$(OUTPUT)/built-in.o $$(OBJS) $$(foreach f,$$(DIRS),$$(f)/$$(OUTPUT)/built-in.o)
	@$$(if $$(filter %.a,$$?),$$(RM) -r $$(EXTRACT_DIR)_$(1))
endef

define MakeTarget
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(OUTPUT)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OUTPUT)/$$(notdir $$(obj)))
$(1).elf: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1))
	@echo CC $$@
	$$(CC) $$(LDFLAGS) $$(if $$(LINKFLAGS_$(1)),$$(LINKFLAGS_$(1)),$$(LINKFLAGS_DEFAULT) $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1))) $$(GEN_BUILTS) -o $$@
endef

#############################################################
# Rules base
# Should be done in top-level makefile only
#
.PHONY: all clean distclean
all: .subdirs $(OBJS) $(LIBS) $(BINS) $(GEN_TARGETS)

clean:
	@$(foreach d, $(DIRS), $(MAKE) -C $(d) clean;)
	$(RM) -r $(OUTPUT)

distclean:
	@$(foreach d, $(DIRS), $(MAKE) -C $(d) distclean;)
	$(RM) -r $(OUTPUT)

.subdirs:
	@set -e; $(foreach d, $(DIRS), $(MAKE) -C $(d);)

debug:
	@echo "INCLUDES:" $(INCLUDES)
	@echo "DEFINES:" $(DEFINES)
	@echo "DIRS:" $(DIRS)
	@echo "OBJS:" $(OBJS)
	@echo "CSRCS:" $(CSRCS)
	@echo "LIBS:" $(LIBS)
	@echo "BINS:" $(BINS)
	@echo "OUTPUT:" $(OUTPUT)
	@echo "GEN_TARGETS:" $(GEN_TARGETS)
	@echo "GEN_LIBS:" $(GEN_LIBS)
	@echo "GEN_BINS:" $(GEN_BINS)
	@echo "ABS_ROOT_PATH:" $(ABS_ROOT_PATH)
	@echo "GEN_BUILTS:" $(GEN_BUILTS)
	@echo "COMPONENTS_app:" $(COMPONENTS_app)
	@echo "LINKFLAGS_app:" $(LINKFLAGS_app)


ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifdef DEPS
sinclude $(DEPS)
endif
endif
endif

$(OUTPUT)/%.o: %.c
	@echo CC $<
	@mkdir -p $(OUTPUT);
	@$(CC) $(CFLAGS) $(COPTS_$(*F)) -o $@ -c $<

$(OUTPUT)/%.d: %.c
	@mkdir -p $(OUTPUT);
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OUTPUT)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
$(OUTPUT)/%.o: %.cpp
	@echo CPP $<
	@mkdir -p $(OUTPUT);
	@$(CPP) $(CFLAGS) $(COPTS_$(*F)) -o $@ -c $<

$(OUTPUT)/%.d: %.cpp
	@mkdir -p $(OUTPUT);
	@set -e; rm -f $@; \
	$(CPP) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OUTPUT)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OUTPUT)/%.o: %.s
	@mkdir -p $(OUTPUT);
	@echo AS $<
	@$(CC) $(CFLAGS) -o $@ -c $<

$(OUTPUT)/%.d: %.s
	@mkdir -p $(OUTPUT); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OUTPUT)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OUTPUT)/%.o: %.S
	@mkdir -p $(OUTPUT);
	@echo AS $<
	@$(CC) $(CFLAGS) -D__ASSEMBLER__ -o $@ -c $<

$(OUTPUT)/%.d: %.S
	@mkdir -p $(OUTPUT); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OUTPUT)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(foreach lib,$(GEN_LIBS),$(eval $(call ShortcutRule,$(lib),$(OUTPUT))))

$(foreach bin,$(GEN_BINS),$(eval $(call ShortcutRule,$(bin),$(OUTPUT))))

$(foreach lib,$(GEN_LIBS),$(eval $(call MakeLibrary,$(basename $(lib)))))

$(foreach target,$(GEN_TARGETS),$(eval $(call MakeTarget,$(basename $(target)))))


INCLUDES += -I $(ROOT_PATH)/include
INCLUDES += -I $(ROOT_PATH)/components
