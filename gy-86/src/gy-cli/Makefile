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

## Add any more include folders here
CFLAGS= -I$(CUR_DIR) -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/logger -I$(INCLUDE_DIR)/mpu60x0 \
                                      -I$(INCLUDE_DIR)/ms5611 -I$(INCLUDE_DIR)/hmc5883 \
                                      -I$(INCLUDE_DIR)/gy86 -Wall 

SRC_DIR=$(CUR_DIR)
LIB_DIR=../../lib
BIN_DIR=../../bin

OBJ_DIR=../../build
LIBS= -lgy86 -lms5611 -lhmc5883 -lmpu60x0 -llogger -lm

DEP=

BIN_NAME=gy86-cli

SRC= main.c mpu-config.c ms-config.c hmc-config.c

# No Customization below this line
#######################################
OBJ=$(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

all: $(CUR_DIR)/$(BIN_NAME)

$(CUR_DIR)/$(BIN_NAME): _logger _mpu60x0 _ms5611 _hmc5883 _gy86 $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS) $(LIBS) -L$(LIB_DIR)

$(OBJ_DIR)/%.o: %.c $(DEP)
	mkdir -p $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: _logger
_logger: force
	echo "Building Logger++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	cd ../logger ; make ; make install ; make clean
	echo "Finished Building Logger-------------------------------------------------------"

.PHONY: _mpu60x0
_mpu60x0: force
	echo "Building MPU60X0+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	cd ../mpu60x0 ; make ; make install ; make clean
	echo "Finished Building MPU60X0------------------------------------------------------"
	

.PHONY: _ms5611
_ms5611: force
	echo "Building MS5611++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	cd ../ms5611 ; make ; make install ; make clean
	echo "Finished Building MS5611-------------------------------------------------------"

.PHONY: _hmc5883
_hmc5883: force
	echo "Building HMC5883++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	cd ../hmc5883 ; make ; make install ; make clean
	echo "Finished Building HMC5883-----------------------------------------------------"

.PHONY: _gy86
_gy86: force
	echo "Building GY86+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	cd ../gy86 ; make ; make install ; make clean
	echo "Finished Building GY86--------------------------------------------------------"

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o *~ core $(INCLUDE_DIR)/*~ $(CUR_DIR)/$(BIN_NAME)
	cd ../logger  ; make clean
	cd ../mpu60x0 ; make clean
	cd ../hmc5883 ; make clean
	cd ../ms5611  ; make clean
	cd ../gy86    ; make clean 

.PHONY: install
install:
	mkdir -p $(BIN_DIR)
	cp $(CUR_DIR)/$(BIN_NAME) $(BIN_DIR)

.PHONY: force
force:
	true