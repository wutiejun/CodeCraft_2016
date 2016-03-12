# 
# Make file for code craft
# wu.tiejun

OBJS=line_reader.o main.o
APP=code_craft

CFLAGS=-I./lib -I./
LDFLAGS=-L./

all:$(APP)
	echo "Make all ..."

$(APP):build
	echo "Make $(APP) ..."

build:$(OBJS)
	echo "Build all objs ..."
	ld $(LDFLAGS) -o $(APP) $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f $(APP)

%.o : %.c
	 cc -c $(CFLAGS) $< -o $@
	 
