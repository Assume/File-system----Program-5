CC=g++

all: ssfs_mkdsk.o
	g++ -o ssfs_mkdsk ssfs_mkdsk.o

%.o:%.cpp
	$(CC) -c -g $^ 

.PHONY clean:
	rm -rf ssfs_mkdsk *.o
