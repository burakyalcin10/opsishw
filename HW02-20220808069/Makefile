# Makefile for RPC Task Processing System

CC = gcc
CFLAGS = -Wall -pthread -I/usr/include/tirpc
LDFLAGS = -ltirpc
RPCGEN = rpcgen

# RPC generated files
RPC_SOURCES = task_clnt.c task_svc.c task_xdr.c
RPC_HEADERS = task.h

# Source files
SERVER_SOURCES = server.c task_queue.c result_store.c task_processor.c
CLIENT_SOURCES = client.c

# Object files
SERVER_OBJS = task_svc.o server.o task_queue.o result_store.o task_processor.o task_xdr.o
CLIENT_OBJS = client.o task_clnt.o task_xdr.o

# Executables
SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)

# Generate RPC stubs from task.x
$(RPC_SOURCES) $(RPC_HEADERS): task.x
	$(RPCGEN) task.x

# Build server
$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJS) $(LDFLAGS)

# Build client
$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJS) $(LDFLAGS)

# Compile object files
server.o: server.c $(RPC_HEADERS) task_queue.h result_store.h task_processor.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c $(RPC_HEADERS)
	$(CC) $(CFLAGS) -c client.c

task_queue.o: task_queue.c task_queue.h $(RPC_HEADERS)
	$(CC) $(CFLAGS) -c task_queue.c

result_store.o: result_store.c result_store.h $(RPC_HEADERS)
	$(CC) $(CFLAGS) -c result_store.c

task_processor.o: task_processor.c task_processor.h $(RPC_HEADERS)
	$(CC) $(CFLAGS) -c task_processor.c

task_svc.o: task_svc.c $(RPC_HEADERS)
	$(CC) $(CFLAGS) -c task_svc.c

task_clnt.o: task_clnt.c $(RPC_HEADERS)
	$(CC) $(CFLAGS) -c task_clnt.c

task_xdr.o: task_xdr.c $(RPC_HEADERS)
	$(CC) $(CFLAGS) -Wno-unused-variable -c task_xdr.c

# Clean build artifacts
clean:
	rm -f $(SERVER) $(CLIENT) *.o $(RPC_SOURCES) $(RPC_HEADERS)

.PHONY: all clean
