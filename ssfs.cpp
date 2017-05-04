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
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>
#include <sys/types.h>
#include "disk.h"
#include <stdlib.h>
#include <stdio.h>
#include "operations.h"


#define SIZE 4096



void * disk_op(void * data){
	std::string * str = static_cast<std::string*>(data);
	//Wait for commands from the user from command line.
	while(true){
		std::cout << "Thread" << std::endl;
		exit(0);
		std::string input;
		std::cin >> input;

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
	}
}

bool all_disk_op_valid(std::string * disk_ops, int disk_op_array_size){
	for(int i = 0; i < disk_op_array_size; i++){
		std::ifstream temp_stream;
		temp_stream.open(disk_ops[i].c_str());
		if(!temp_stream.is_open())
			return false;
	}
	return true;
}

std::vector<std::string> split_string_by_space(std::string str){
	std::istringstream buf(str);
	std::istream_iterator<std::string> beg(buf), end;
	std::vector<std::string> tokens(beg, end);

	return tokens;
}


void *handler_thread(std::string file_name){

	std::ifstream file(file_name.c_str());

	if(file.is_open())
		std::cout << "File " << file_name << " opened successfully" << std::endl;
	else
		std::cout << "File " << file_name << " failed to open"<< std::endl;
	file.close();

}


/*void create_threads(){
	int response;
	for(int i = 0; i < num_disk_op_extra; i++){
		response = pthread_create(&threads[i], NULL, handler_thread, NULL);
		if(response)
			std::cout << "Failed to create thread " << response << "." << std::endl;
	}
}*/


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
	if(!all_disk_op_valid(disk_op_threads, num_disk_op_extra)){
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

	//disk manager thread waiting for disk op threads to post disk accesses
	while(1){
				
	}

	return 0;
}
