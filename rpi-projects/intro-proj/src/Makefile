INCLUDE_DIR=../include
SRC_DIR=../src
LIB_DIR=../lib
BIN_DIR=../bin
RPI_TOOLS_DIR=~/work/rpi/rpi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin
OBJ_DIR=../build

CC=$(RPI_TOOLS_DIR)/arm-linux-gnueabihf-gcc
CFLAGS=-I$(INCLUDE_DIR)
LIBS=
DEP=

_OBJ=hello_world.o
OBJ=$(patsubst %,$(OBJ_DIR)/%,$(_OBJ))


all: $(BIN_DIR)/hello_world 

$(BIN_DIR)/hello_world: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: %.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o *~ core $(INCLUDE_DIR)/*~


.PHONY: install
install:
	scp $(BIN_DIR)/* pi@rpi-0:~



