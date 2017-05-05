#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>
#include <cstdlib>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "disk.h"
#include "operations.h"

#define SIZE 4096

void * disk_op(void * data){

	std::string * str = static_cast<std::string*>(data);
	std::ifstream p_file((*str).c_str());
	std::string line;
	std::string token;

	//CREATE A BOUNDED BUFFER

	std::cout << "filename: " << *str << std::endl;

	if (p_file.is_open())
	{
		while (!p_file.eof() )
		{
			getline(p_file, line);
			std::cout << "line read" << std::endl;

			message ms;
			token = line.substr(0, line.find('\n'));
			ms.cmd = new char[sizeof(token)];
			strcpy(ms.cmd, token.c_str());
			ms.fname = nullptr;
			ms.start = -1;;
			ms.bytes = -1;
			ms.letter = -1;
			ms.valid = 1;

			//LOCK
			add_buffer(ms);// atomically write to bounded buffer
			//UNLOCK

			//WAIT FOR A CONDITION VARIABLE TO BE SEND FROM DISK MANAGER TO OP THREAD
		}
		p_file.close();
	} else {
		perror("thread text file is not available");
		return -1;
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
		if(!all_disk_op_valid(disk_op_threads, num_disk_op_extra)){
			perror("One or more disk op file names are invalid. Exiting.");
			return -1;
		}

		// Creating multipe threads
		for(int i = 0; i < num_disk_op_extra; i++){
			rc = pthread_create(&threads[i], NULL, disk_op, &disk_op_threads[i]); 	
			assert(rc == 0);
		}

		//if we read three "finished commands we will exit"
		int finished = 0;

		//disk manager thread waiting for disk op threads to post disk accesses
		while(1){
			if("FINISHED".compare(){
				finished++;
			} else {
				//call intermediate function with message from buffer that executes it
			}
		}
			//pthread_join(threads[0], NULL);

		return 0;
	}
