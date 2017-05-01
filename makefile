CC = g++
OBJS = ssfs_mkdsk.o disk.o
EXEC = ssfs_mkdsk

all: $(OBJS)
	g++ -o $(EXEC) $(OBJS)

%.o:%.cpp
	$(CC) -c -g $^ 

.PHONY clean:
	rm -rf $(EXEC) *.o
