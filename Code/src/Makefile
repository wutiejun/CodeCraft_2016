# 
# Make file for code craft
# wu.tiejun

OBJS=lib/file.o  lib/hash.o lib/linklist.o  lib/memory.o lib/memtypes.o  lib/ntlavll.o  lib/pqueue.o  lib/vector.o \
	 line_reader.o main.o spf.o
APP=code_craft

CFLAGS=-I./lib -I./ -I./src 
LDFLAGS=-L./ 

all: $(APP)
	echo "Make all ..."

$(APP): build
	echo "Make $(APP) ..."

build:$(OBJS)
	echo "Build all objs ..."
	gcc $(LDFLAGS) -o $(APP) $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f $(APP)

%.o : %.c
	 cc -c $(CFLAGS) $< -o $@
	 
