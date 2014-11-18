#######################################
## Makefile
## Author: Ahmed ElArabawy
## Created on : 16 Nov. 2014
#######################################

# ToolChain
RPI_TOOLS_DIR=/usr/bin
CC=$(RPI_TOOLS_DIR)/gcc -std=gnu99

# Binary Name
BIN_NAME=intro-dbus

# Include Directory
INCLUDE_DIR=../include
## Add any more include folders here
CFLAGS=-I$(INCLUDE_DIR) -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include 

SRC_DIR=../src
LIB_DIR=../lib
BIN_DIR=../bin

OBJ_DIR=../build
#LIBS= -lgio-2.0
LIBS= -l:libgio-2.0.so.0.4000.0 -l:libglib-2.0.so.0.4000.0 -l:libgobject-2.0.so.0.4000.0
DEP=

# No Customization below this line
#######################################
_OBJ=$(BIN_NAME).o
OBJ=$(patsubst %,$(OBJ_DIR)/%,$(_OBJ))


all: $(BIN_DIR)/$(BIN_NAME)

$(BIN_DIR)/$(BIN_NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: %.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o *~ core $(INCLUDE_DIR)/*~


.PHONY: install
install:
	echo "Nothing to Install"



