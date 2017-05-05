CC = g++
OBJS = ssfs_mkdsk.o disk.o operations.o
OBJS1 = ssfs.o disk.o operations.o
EXEC = ssfs_mkdsk
EXEC1 = ssfs
LIBS = -pthread -lrt

all: $(OBJS1)
	$(CC) -o $(EXEC1) $(OBJS1) $(LIBS)

ssfs: $(OBJS1)
	$(CC) -o $(EXEC1) $(OBJS1)

ssfs_mkdsk: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS)

%.o:%.cpp
	$(CC) -c -g -lstdc++ $^ 

.PHONY clean:
	rm -rf $(EXEC) $(EXEC1) *.o DISK
