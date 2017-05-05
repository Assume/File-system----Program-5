CC = g++ -std=c++11
OBJS = ssfs_mkdsk.o disk.o operations.o
OBJS1 = ssfs.o disk.o operations.o
TEST = disk.o test_for_ssfs.o operations.o
EXEC = ssfs_mkdsk
EXEC1 = ssfs
EXECT = test
LIBS = -pthread -lrt

all: $(OBJS1)
	$(CC) -o $(EXEC1) $(OBJS1) $(LIBS)

ssfs: $(OBJS1)
	$(CC) -o $(EXEC1) $(OBJS1) $(LIBS)

disk: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS)

test: $(TEST)
	$(CC) -o $(EXECT) $(TEST)

%.o:%.cpp
	$(CC) -c -g -lstdc++ $^ 

.PHONY clean:
	rm -rf $(EXEC) $(EXEC1) $(EXECT) *.o 
