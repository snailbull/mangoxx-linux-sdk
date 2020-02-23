#
# Rules for root path
#
# ld -verbose    系统默认的连接脚本
#
# ldconfig -p | grep libcrypto   查找库文件路径
#  $^=all depend files   $@=target   $<=first depend files
#

CROSS_COMPILE = 
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
NM = $(CROSS_COMPILE)nm
CXX = $(CROSS_COMPILE)g++
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
STRIP = $(CROSS_COMPILE)strip

LD_FILE := $(ROOT_PATH)/ld/app.ld

SUBDIRS ?= $(patsubst %/,%,$(dir $(wildcard */Makefile)))

CFLAGS += -Ofast -g -Wpointer-arith -Wundef

INCLUDES += -I $(ROOT_PATH)/components

CFLAGS += $(INCLUDES) $(EXTRA_CFLAGS)
LDFLAGS += -L$(ROOT_PATH)/lib $(LD_FLAGS) -Wl,--as-needed


#############################################################
# Top targets
#
.PHONY: all clean install debug
all: .subdirs $(OBJS) $(LIBS) $(TARGETS)

clean:
	$(foreach d,$(SUBDIRS),$(MAKE) -C $(d) clean;)
	$(if $(SUBDIRS),, find ./ -name "*.[od]" | xargs rm -f;rm -f $(LIBS);)

install: 
	$(foreach d, $(SUBDIR), $(MAKE) -C $(d) install;)
	$(if $(SUBDIRS),, cp -t $(ROOT_PATH)/lib $(LIBS);$(STRIP) --strip-unneeded $(ROOT_PATH)/lib/$(LIBS);)

.subdirs:
	@set -e; $(foreach d, $(SUBDIRS), $(MAKE) -C $(d);)

debug:
	@echo "SUBDIRS:" $(SUBDIRS)
	@echo "DIRS:" $(DIRS)
	@echo "OBJS:" $(OBJS)
	@echo "CSRCS:" $(CSRCS)
	@echo "LIBS:" $(LIBS)
	@echo "INCLUDES:" $(INCLUDES)
	@echo "TARGETS:" $(TARGETS)
	@echo "CFLAGS:" $(CFLAGS)
	@echo "LDFLAGS:" $(LDFLAGS)
	@echo "PROJECT_NAME:" $(PROJECT_NAME)
	@echo "EXTRA_CFLAGS:" $(EXTRA_CFLAGS)

#############################################################
# src file targets
# 
%.o: %.c
	@echo CC $<
	@$(CC) $(CFLAGS) -o $@ -c $<

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
%.o: %.cpp
	@echo CXX $<
	@$(CXX) $(CFLAGS) -o $@ -c $<

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o: %.s
	@echo AS $<
	@$(CC) $(CFLAGS) -o $@ -c $<

%.d: %.s
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o: %.S
	@echo AS $<
	@$(CC) $(CFLAGS) -D__ASSEMBLER__ -o $@ -c $<

%.d: %.S
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


#############################################################
# Functions
#
define MakeLibrary
$(1).a: $$(OBJS)
	@echo AR $(1).a
	@$$(AR) ru $$@ $$^
endef

define MakeTarget
$(1).elf: $$(OBJS)
	@echo LD $$@
	$$(CC) $$(LDFLAGS) $$(OBJS) -o $$@
endef

$(foreach lib,$(LIBS),$(eval $(call MakeLibrary,$(basename $(lib)))))
$(foreach target,$(TARGETS),$(eval $(call MakeTarget,$(basename $(target)))))

DEPS := $(OBJS:.o=.d)
sinclude $(DEPS)
