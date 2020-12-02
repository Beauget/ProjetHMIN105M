CLIENT = client
SERVER = server

CXX = gcc
LDFLAGS = -Wall -g -lpthread

SRC_CLIENT = client.c
SRC_SERVER = server.c


OBJ_CLIENT = $(SRC_CLIENT:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)

all: client server

client : $(OBJ_CLIENT)
	$(CXX) -o $(CLIENT) $(OBJ_CLIENT) $(LDFLAGS)

server : $(OBJ_SERVER)
	$(CXX) -o $(SERVER) $(OBJ_SERVER) $(LDFLAGS)

clean:
	rm -f $(OBJ_CLIENT) $(OBJ_SERVER) client server


