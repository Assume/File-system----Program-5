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

void * shm_ptr = nullptr;;

void * disk_op(void * data){

	std::string * str = static_cast<std::string*>(data);
	std::ifstream p_file((*str).c_str());
	std::string line;
	std::string token;

	std::cout << "filename: " << *str << std::endl;

	if(!shm_ptr){
		perror("shared memory not initialized");
	}

	if (p_file.is_open())
	{
		while (!p_file.eof() )
		{
			getline(p_file, line);
			std::cout << "line read" << std::endl;

			message ms;
			token = line.substr(0, line.find(" "));
			ms.cmd = new char[sizeof(token)];
			strcpy(ms.cmd, token.c_str());
			ms.fname = nullptr;
			ms.start = -1;;
			ms.bytes = -1;
			ms.letter = -1;

			//LOCK
			add_shared_to(ms, shm_ptr);
			//UNLOCK
		}
		p_file.close();
	}

		/*
		   if(p_file.is_open()){

		//Wait for commands from the user from command line.
		while(getline(p_file, input)){

		std::cout << input << std::endl;

		if(strcmp(input.substr(0, 7).c_str(), "CREATE ") == 0 && input.size() > 7){
		std::cout << "C" <<  std::endl;
		//			create(input.substr(7, input.size()));
		} else if(strcmp(input.substr(0, 7).c_str(), "IMPORT ") == 0 && input.size() > 7){
		std::cout << "I" <<  std::endl;
		//			import(split_string_by_space(input));
		} else if(strcmp(input.substr(0, 4).c_str(), "CAT ") == 0 && input.size() > 4){
		std::cout << "C" <<  std::endl;
		//			cat(input.substr(4, input.size()));
		} else if(strcmp(input.substr(0, 6).c_str(), "WRITE ") == 0 && input.size() > 6){
		std::cout << "W" <<  std::endl;
		//			write(split_string_by_space(input));
		} else if(strcmp(input.substr(0, 7).c_str(), "DELETE ") == 0 && input.size() > 7){
		std::cout << "D" <<  std::endl;
		//			f_delete(input.substr(7, input.size()));
		} else if(strcmp(input.substr(0, 5).c_str(), "READ ") == 0 && input.size() > 5){
		std::cout << "R" <<  std::endl;
		//			read(split_string_by_space(input));
		} else if(strcmp(input.substr(0, 4).c_str(), "LIST") == 0){
		std::cout << "L" <<  std::endl;
		//			list_files();
		} else if(strcmp(input.substr(0, 8).c_str(), "SHUTDOWN") == 0){
		std::cout << "S" <<  std::endl;
		//			shutdown();
		}

		//WAIT FOR A CONDITION VARIABLE
		}
		} else {
		perror("thread text file is not available");
		}
		 */
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

		int shm_fd = 0;
		int rc = 0;

		// Create shared memory
		shm_fd = shm_open("access", O_CREAT | O_RDWR, 0666);
		ftruncate(shm_fd, SIZE);
		shm_ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0); 

		/*
		for(int i = 0; i < num_disk_op_extra; i++){
			rc = pthread_create(&threads[i], NULL, disk_op, &disk_op_threads[i]); 	
			assert(rc == 0);
		}
		*/
		//rc = pthread_create(&threads[0], NULL, disk_op, &disk_op_threads[0]); 	
		std::string s = "1";
		std::string * p = &s;

		disk_op((void *)(p));

		//if we read three "finished commands we will exit"
		int finished = 0;

		//disk manager thread waiting for disk op threads to post disk accesses
		/*
		while(1){
			   if("FINISHED".compare((char *)(shm_ptr))){
			   finished++;
			   } else {
			   disk_op()
			   }
			std::cout << "before empty" << std::endl;
			if((*((message *)shm_ptr)).cmd == 0 || strcmp("EMPTY", (*((message *)shm_ptr)).cmd)){
				std::cout << "1" << std::endl;
				std::cout << sizeof((*(message *)shm_ptr)) << std::endl;
				std::cout << "2" << std::endl;
				std::cout << ((char *)(*((message *)shm_ptr)).cmd) << std::endl;
				std::cout << "3" << std::endl;
				shm_ptr = ((message *) shm_ptr) + 1;
				std::cout << *((int *)shm_ptr) << std::endl;
			}
		}
		*/
		//pthread_join(threads[0], NULL);

		return 0;
	}
