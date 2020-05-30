
FSSOURCE ?= ../fs/

#############################################################
# Get the files to pack into the spiffs image
#

SPIFFSFILES ?= $(patsubst $(FSSOURCE)%,%,$(shell find $(FSSOURCE) -name '*' '!' -name .gitignore -type f))


SRCS=main.c ../spiffs_cache.c  ../spiffs_check.c  ../spiffs_gc.c  ../spiffs_hydrogen.c  ../spiffs_nucleus.c
CFLAGS=-g -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -I. -I../ -DPLATFORM_LINUX -Ddbg_printf=printf --include ../spiffs_config.h

CC:=gcc

spiffsimg: $(SRCS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f spiffsimg

fs:
	rm -f ./fs.lst
	echo "" >> ./fs.lst
	$(foreach f, $(SPIFFSFILES), echo "import $(FSSOURCE)$(f) $(f)" >> ./fs.lst ;)
	./spiffsimg -f ./fs.bin -c -r ./fs.lst