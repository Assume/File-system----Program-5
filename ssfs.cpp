#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <sstream>
#include <cstdlib>

#include <pthread.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#include "disk.h"
#include "operations.h"

#define SIZE 4096

void * disk_op(void * data){

	std::string * str = static_cast<std::string*>(data);
	FILE * pFile;
	char input[255];

	//Wait for commands from the user from command line.
	while(fscanf(fp, "%s", input)){
		
		pFile = fopen((*str).c_str(),"r");

		std::cout << input << std::endl;

		if(input.compare(0, 7, "CREATE ") == 0 && input.size() > 7){
			std::cout << "C" <<  std::endl;
//			create(input.substr(7, input.size()));
		} else if(input.compare(0, 7, "IMPORT ") == 0 && input.size() > 7){
			std::cout << "I" <<  std::endl;
//			import(split_string_by_space(input));
		} else if(input.compare(0, 4, "CAT ") == 0 && input.size() > 4){
			std::cout << "C" <<  std::endl;
//			cat(input.substr(4, input.size()));
		} else if(input.compare(0, 6, "WRITE ") == 0 && input.size() > 6){
			std::cout << "W" <<  std::endl;
//			write(split_string_by_space(input));
		} else if(input.compare(0, 7, "DELETE ") == 0 && input.size() > 7){
			std::cout << "D" <<  std::endl;
//			f_delete(input.substr(7, input.size()));
		} else if(input.compare(0, 5, "READ ") == 0 && input.size() > 5){
			std::cout << "R" <<  std::endl;
//			read(split_string_by_space(input));
		} else if(input.compare(0, 4, "LIST") == 0){
			std::cout << "L" <<  std::endl;
//			list_files();
		} else if(input.compare(0, 8, "SHUTDOWN") == 0){
			std::cout << "S" <<  std::endl;
//			shutdown();
		}

		/* WAIT FOR A CONDITION VARIABLE */
	}
}

int main(int argc, char * argv[]){

	// SANITIZE INPUT 1,2,3,4 text files and at least write seudocode for creating threads
	if(argc == 1){
		perror("No disk file name passed. Exiting.");
		return -1;
	}

	if(argc > 6){
		perror("Invalid number of arguments passed. Exiting.");
		return -1;
	}

	std::string disk_file_name = argv[1];
	std::ofstream disk_file;

	disk_file.open(disk_file_name.c_str());

	//Verify disk file exists
	if(!disk_file.is_open()){
		perror("Disk file does not exist. Exiting.");
		return -1;
	}

	int num_disk_op_extra = argc - 2;
	pthread_t threads[num_disk_op_extra];
	std::string disk_op_threads[num_disk_op_extra];

	for(int i = 0; i < num_disk_op_extra; i++){
		disk_op_threads[i] = argv[i + 2];
	}

	//Verify that all passed disk op names are valid
	if(!operations::all_disk_op_valid(disk_op_threads, num_disk_op_extra)){
		perror("One or more disk op file names are invalid. Exiting.");
		return -1;
	}

	int shm_fd = 0;
	void * shm_ptr;
	int rc = 0;

	// Create shared memory
	shm_fd = shm_open("access", O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, SIZE);
	shm_ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0); 

	for(int i = 0; i < num_disk_op_extra; i++){
		rc = pthread_create(&threads[i], NULL, disk_op, &disk_op_threads[i]); 	
		assert(rc == 0);
	}

	//if we read three "finished commands we will exit"
	int finished = 0;

	//disk manager thread waiting for disk op threads to post disk accesses
	for(int i = 0; i < 3; i++){
		/*
		if("FINISHED".compare((char *)(shm_ptr))){
			finished++;
		} else {
			disk_op()
		}*/
		std::cout << *((char *)shm_ptr) << std::endl;
		shm_ptr += sizeof((char *)shm_ptr);
	}

	return 0;
}
