#######################################
## Makefile
## Author: Ahmed ElArabawy
## Created on : 16 Nov. 2014
#######################################

# ToolChain
TOOL_CHAIN_DIR=/usr/bin
CC=$(TOOL_CHAIN_DIR)/gcc -std=gnu99

# Binary Name
BIN_NAME=intro-netlink

# Include Directory
INCLUDE_DIR=../../include
## Add any more include folders here
CFLAGS=-I$(INCLUDE_DIR)  

SRC_DIR=../intro-netlink
LIB_DIR=../../lib
BIN_DIR=../../bin

OBJ_DIR=../../build
LIBS=
DEP=

# No Customization below this line
#######################################
_OBJ_tx=$(BIN_NAME)_tx.o
_OBJ_rx=$(BIN_NAME)_rx.o

OBJ_tx=$(patsubst %,$(OBJ_DIR)/%,$(_OBJ_tx))
OBJ_rx=$(patsubst %,$(OBJ_DIR)/%,$(_OBJ_rx))

all: $(BIN_DIR)/$(BIN_NAME)_tx $(BIN_DIR)/$(BIN_NAME)_rx   

tx: $(BIN_DIR)/$(BIN_NAME)_tx

rx: $(BIN_DIR)/$(BIN_NAME)_rx


$(BIN_DIR)/$(BIN_NAME)_tx: $(OBJ_tx)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(BIN_DIR)/$(BIN_NAME)_rx: $(OBJ_rx)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
		
$(OBJ_DIR)/%.o: %.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o *~ core $(INCLUDE_DIR)/*~ 


.PHONY: install
install:
	echo "Nothing to Install"



