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

#include <queue>

#include "disk.h"
#include "operations.h"

#define SIZE 4096



pthread_mutex_t buf_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buf_wait = PTHREAD_COND_INITIALIZER;

bool add_buffer(message & ms);

bool is_buffer_full();

std::queue<message> queue;

int num_done = 0;


void * disk_op(void * data){
	std::string * str = static_cast<std::string*>(data);
	std::ifstream p_file((*str).c_str()); 
	std::string line;  

	//CREATE A BOUNDED BUFFER

	if (p_file.is_open()){
		while (!p_file.eof()){
			getline(p_file, line);
			message ms;
			std::vector<std::string> vec = split_string_by_space(line);
			std::string input = vec[0];
			ms.cmd = input;
			if(input.compare("CREATE") == 0){
			  std::string file_name = vec[1];
			  ms.fname = file_name.c_str();
			} else if(input.compare("IMPORT") == 0){
				std::string file_name = vec[1];
				std::string linux_file_name = vec[2];
				ms.fname = file_name;
				ms.l_fname = linux_file_name;
			} else if(input.compare("CAT") == 0){
				std::string file_name = vec[1];
				ms.fname = file_name;
			} else if(input.compare("WRITE") == 0){
				std::string file_name = vec[1];
				char char_to_write = vec[2].c_str()[0];
				int start_byte = std::atoi(vec[3].c_str());
				int num_bytes = std::atoi(vec[4].c_str());
				ms.fname = file_name;
				ms.letter = char_to_write;
				ms.start = start_byte;
				ms.bytes = num_bytes;
			} else if(input.compare("DELETE") == 0){
				std::string file_name = vec[1];
				ms.fname = file_name;
			} else if(input.compare("READ") == 0){
				std::string file_name = vec[1];
				int start_byte = std::atoi(vec[2].c_str());
				int num_bytes = std::atoi(vec[3].c_str());
				ms.fname = file_name;
				ms.start = start_byte;
				ms.bytes = num_bytes;
			} else if(input.compare("LIST") == 0){

			} else if(input.compare("SHUTDOWN") == 0){

			}

			pthread_mutex_lock(&buf_lock);
			if(is_buffer_full()){
			  pthread_cond_wait(&buf_wait, &buf_lock);
			  add_buffer(ms);
			}else
			  add_buffer(ms);
			pthread_mutex_unlock(&buf_lock);

		}
		p_file.close();
		num_done++;
	} else {
		perror("thread text file is not available");
	}
}


bool add_buffer(message & ms){
	queue.push(ms);
	return true;
}

bool is_buffer_full(){
	return queue.size() == 10;
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
	/*std::ofstream disk_file;

	  disk_file.open(disk_file_name.c_str());

	//Verify disk file exists
	if(!disk_file.is_open()){
	perror("Disk file does not exist. Exiting.");
	return -1;
	}
	 */

	file_data_holder file_holder;

	strcpy(file_holder.disk_name, disk_file_name.c_str());
	
	int num_disk_op_extra = argc - 2;
	pthread_t threads[num_disk_op_extra];
	std::string disk_op_threads[num_disk_op_extra];

	for(int i = 0; i < num_disk_op_extra; i++){
		disk_op_threads[i] = argv[i + 2];
	}
	/*
	//Verify that all passed disk op names are valid
	if(!all_disk_op_valid(disk_op_threads, num_disk_op_extra)){
	perror("One or more disk op file names are invalid. Exiting.");
	return -1;
	}
	 */

	read_in_super_block(disk_file_name, file_holder);
	read_in_inode_bitmap(disk_file_name, file_holder);
	read_in_data_bitmap(disk_file_name, file_holder);
	read_in_all_inodes(disk_file_name, file_holder);


	int rc;

	// Creating multipe threads
	for(int i = 0; i < num_disk_op_extra; i++){
		rc = pthread_create(&threads[i], NULL, disk_op, &disk_op_threads[i]); 	
		assert(rc == 0);
	}

	while(1){
	  if(num_done == num_disk_op_extra && queue.empty())
			break;
		if(!is_buffer_full())
			pthread_cond_signal(&buf_wait);
		if(!queue.empty()){
		message t_ms = queue.front();
		queue.pop();
		if(t_ms.cmd.compare("CREATE") == 0){
			create(file_holder, t_ms);
		} else if(t_ms.cmd.compare("IMPORT") == 0){
			import(file_holder, t_ms);
		} else if(t_ms.cmd.compare("CAT") == 0){
			cat(file_holder, t_ms);
		} else if(t_ms.cmd.compare("WRITE") == 0){
			write(file_holder, t_ms);
		} else if(t_ms.cmd.compare(0, 6, "DELETE") == 0){
			delete_file(file_holder, t_ms);
		} else if(t_ms.cmd.compare(0, 4, "READ") == 0){
			read(file_holder, t_ms);
		} else if(t_ms.cmd.compare(0, 4, "LIST") == 0){
			list_files(file_holder);
		} else if(t_ms.cmd.compare(0, 8, "SHUTDOWN") == 0){
			shutdown(file_holder);
			return 0;
		}
	     }
	}
	shutdown(file_holder);
	
	return 0;
}
