#######################################
## Makefile
## Author: Ahmed ElArabawy
## Created on : 16 Nov. 2014
#######################################

# ToolChain
TOOL_CHAIN_DIR=/usr/bin
CC=$(TOOL_CHAIN_DIR)/gcc -std=gnu99

# Include Directory
INCLUDE_DIR=../../include
## Add any more include folders here
CFLAGS=-I$(INCLUDE_DIR) -I/usr/include/glib-2.0 -I/usr/lib//x86_64-linux-gnu/glib-2.0/include 

SRC_DIR=../wpa_supplicant_client
LIB_DIR=../../lib
BIN_DIR=../../bin

OBJ_DIR=../../build
LIBS= -lgio-2.0 -lglib-2.0 -lgobject-2.0
DEP=

BIN_NAME=wpa_supplicant_client
SRC= main.c                  \
	 wpa_supplicant_client.c \
     wpa_supplicant_client_dbus_controller.c  \
     wpa_supplicant_client_proxy_introspectable.c  \
     wpa_supplicant_client_name_watcher.c  \
     wpa_supplicant_client_if_manager.c  \
     wpa_supplicant_client_if.c \
     wpa_supplicant_client_if_dbus_controller.c \
     wpa_supplicant_client_bss_manager.c \
     wpa_supplicant_client_bss.c \
     wpa_supplicant_client_network_mgr.c \
     wpa_supplicant_client_network.c \
     wpa_supplicant_client_bss_dbus_controller.c \
 	 wpa_supplicant_client_network_dbus_controller.c     

# No Customization below this line
#######################################
OBJ=$(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

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



