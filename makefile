CC = g++
OBJS = ssfs_mkdsk.o disk.o
OBJS1 = ssfs.o disk.o
EXEC = ssfs_mkdsk
EXEC1 = ssfs

all: $(OBJS1)
	$(CC) -o $(EXEC1) $(OBJS1) -pthread -lrt

ssfs: $(OBJS1)
	$(CC) -o $(EXEC1) $(OBJS1)

ssfs_mkdsk: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS)

%.o:%.cpp
	$(CC) -c -g -lstdc++ $^ 

.PHONY clean:
	rm -rf $(EXEC) $(EXEC1) *.o
