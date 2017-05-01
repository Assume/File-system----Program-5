#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <pthread.h>


pthread_t  * threads;
int num_disk_op_extra = 0;
std::string * disk_op_threads;

int * shared_mem_ptr;
int shm_fd;

int main(int argc, char * argv[]){

	// SANITIZE INPUT 1,2,3,4 text files and at least write seudocode for creating threads
	if(argc == 1){
		std::cout << "No disk file name passed. Exiting." << std::endl;
		return -1;
	}

	if(argc > 6){
		std::cout  << "Invalid number of arguments passed. Exiting." std::endl;
		return -1;
	}

	std::string disk_file_name = argv[0];
	std::ofstream disk_file;

	disk_file.open(disk_file_name);

	//Verify disk file exists
	if(!disk_file.is_open()){
		std::cout << "Disk file does not exist. Exiting." << std::endl;
		return -1;
	}

	num_disk_op_extra = argc - 1;

	threads = new p_thread[num_disk_op_extra];

	disk_op_threads = new std::string[num_disk_op_extra];

	for(int i = 0; i < num_disk_op_extra; i++)
		disk_op_threads[i] = argv[i+2];

	//Verify that all passed disk op names are valid
	if(!all_disk_op_valid(disk_op_threads, num_disk_op_extra)){
		std::cout << "One or more disk op file names are invalid. Exiting." << std::endl;
		return -1;
	}

	int pid_temp = getpid();

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, num_disk_op_extra * sizeof(int));
	shared_mem_ptr = mmap(0, num_disk_op_extra * sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);




	//Wait for commands from the user from command line.
	while(true){
		std::string input;
		cin >> input;

		if(input.compare(0, 7, "CREATE ") == 0 && input.size() > 7){
			create(input.substr(7, input.size()));
		} else if(input.compare(0, 7, "IMPORT ") == 0 && input.size() > 7){
			import(split_string_by_space(input));
		} else if(input.compare(0, 4, "CAT ") == 0 && input.size() > 4){
			cat(input.substr(4, input.size()));
		} else if(input.compare(0, 6, "WRITE ") == 0 && input.size() > 6){
			write(split_string_by_space(input));
		} else if(input.compare(0, 7, "DELETE ") == 0 && input.size() > 7){
			f_delete(input.substr(7, input.size()));
		} else if(input.compare(0, 5, "READ ") == 0 && input.size() > 5){
			read(split_string_by_space(input));
		} else if(input.compare(0, 4, "LIST") == 0){
			list_files();
		} else if(input.compare(0, 8, "SHUTDOWN") == 0){
			shutdown();
		}
	}

	return 0;
}

bool all_disk_op_valid(std::string * disk_ops, int disk_op_array_size){
	for(int i = 0; i < disk_op_array_size; i++){
		std::ifstream temp_stream;
		temp_stream.open(disk_ops[i]);
		if(!temp_stream.is_open())
			return false;
	}
	return true;
}

std::vetor<std::string> split_string_by_space(std::string str){
	std::istringstream buf(str);
	std::istream_iterator<std::string> beg(buf), end;
	std::vector<std::string> tokens(beg, end);

	return tokens;
}


void write(std::vector<std::string> vec){
	std::string command = vec[0];
	std::string file_name = vec[1];
	char char_to_write = vec[2].c_str()[0];
	byte start_byte = vec[3].c_str()[0];
	int num__bytes = std::atoi(vec[4]);

}


void read(std::vector<std::string> vec){
	std::string ssfs_file_name = vec[1];
	byte start_byte = vec[2].c_str()[0];
	int num_bytes = std::atoi(vec[3]);

}


void create(std::string file_name){


}


void import(std::vector<std::string> vec){
	std::string ssfs_file_name = vec[0];
	std::string unix_file_name = vec[1];



}

void cat(std::string file_name){


}

void f_delete(std::string file_name){


}

void list_files(){


}


void create_threads(){
	int response;
	for(int i = 0; i < num_disk_op_extra; i++){
		response = pthread_create(&thread[i], NULL, handler_thread, NULL);
		if(response)
			std::cout << "Failed to create thread " << response << "." << std::endl;
	}
}


void *handler_thread(std::string file_name){

	ifstream file (file_name);

	if(file.is_open())
		std::cout << "File " << file_name << " opened successfully" << std::endl;
	else
		std::cout << "File " << file_name << " failed to open"<< std::endl;
	file.close();

}
