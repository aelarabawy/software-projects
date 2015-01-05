#######################################
## Makefile
## Author: Ahmed ElArabawy
## Created on : 28 Nov. 2014
#######################################

# ToolChain
TOOL_CHAIN_DIR=/usr/bin
CC=$(TOOL_CHAIN_DIR)/gcc -std=gnu99

# Binary Name
BIN_NAME=intro_nl80211_client

# Include Directory
INCLUDE_DIR=../../include
## Add any more include folders here
CFLAGS=-I$(INCLUDE_DIR)  

SRC_DIR=../intro_nl80211_client
LIB_DIR=../../lib
BIN_DIR=../../bin

OBJ_DIR=../../build
LIBS= 
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
	rm -f $(OBJ_DIR)/*.o *~ core $(INCLUDE_DIR)/*~ $(BIN_DIR)/$(BIN_NAME)


.PHONY: install
install:
	echo "Nothing to Install"



