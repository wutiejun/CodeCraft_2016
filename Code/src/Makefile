# 
# Make file for code craft
# wu.tiejun

OBJS=lib/file.o  lib/hash.o lib/linklist.o  lib/memory.o lib/memtypes.o  lib/ntlavll.o  lib/pqueue.o  lib/vector.o \
	 line_reader.o spf.o data.o main.o arithmetic_dp.o
APP=code_craft

#-Werror  -Wall 
CFLAGS=-g -I./lib -I./ -I./src
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
	 
