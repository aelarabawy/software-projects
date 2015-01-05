#######################################
## Makefile
## Author: Ahmed ElArabawy
## Created on : 16 Nov. 2014
#######################################

# ToolChain
TOOL_CHAIN_DIR=/usr/bin
CC=$(TOOL_CHAIN_DIR)/gcc -std=gnu99
AR=$(TOOL_CHAIN_DIR)/ar
CUR_DIR=.

# Include Directory
INCLUDE_DIR=../../include
PUB_INC_DIR=$(INCLUDE_DIR)/logger

## Add any more include folders here
CFLAGS= -I$(CUR_DIR) -I$(INCLUDE_DIR) -Wall 

SRC_DIR=$(CUR_DIR)
LIB_DIR=../../lib
BIN_DIR=../../bin

OBJ_DIR=../../build
LIBS=
DEP=

BIN_NAME=liblogger.a
SRC=logger.c
PUB_INC=$(SRC_DIR)/logger.h
     

# No Customization below this line
#######################################
OBJ=$(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

all: $(CUR_DIR)/$(BIN_NAME)

$(CUR_DIR)/$(BIN_NAME): $(OBJ)
	$(AR) -cvq $@ $^

$(OBJ_DIR)/%.o: %.c $(DEP)
	mkdir -p $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o *~ core $(INCLUDE_DIR)/*~ $(CUR_DIR)/$(BIN_NAME)


.PHONY: install
install:
	mkdir -p $(PUB_INC_DIR)
	cp $(PUB_INC)  $(PUB_INC_DIR)
	mkdir -p $(LIB_DIR)
	cp $(CUR_DIR)/$(BIN_NAME) $(LIB_DIR)




