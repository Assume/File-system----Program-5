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


void write(std::vector<std::string> vec){
  std::string command = vec[0];
  std::string file_name = vec[1];
  char char_to_write = vec[2].c_str()[0];
  char start_byte = vec[3].c_str()[0];
  int num__bytes = std::atoi(vec[4].c_str());

  



  
}

void read(std::vector<std::string> vec){
  std::string ssfs_file_name = vec[1];
  char start_byte = vec[2].c_str()[0];
  int num_bytes = std::atoi(vec[3].c_str());

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


void read_in_super_block(std::string file_name, file_data_holder & holder){

  FILE * temp_file;

  temp_file = fopen(file_name.c_str(), "rb");

  char * super_b = (char *) malloc(sizeof(super_block));

  fread(super_b, 1, sizeof(super_block), temp_file);

  super_block * t_sb = (super_block *) super_b;

  holder.s_block = t_sb;
  
  fclose(temp_file);
}


void read_in_inode_bitmap(std::string file_name, file_data_holder & holder){

  FILE * temp_file;

  temp_file = fopen(file_name.c_str(), "rb");

  fseek(temp_file, holder.s_block -> block_size, SEEK_SET);

  char * c_inode_bitmap;

  fread(c_inode_bitmap, 1, sizeof(inode), temp_file);

  holder.inode_bitmap = (inode *) c_inode_bitmap;
  fclose(temp_file);

}

void read_in_data_bitmap(std::string file_name, file_data_holder & holder){

  FILE * temp_file;

  temp_file = fopen(file_name.c_str(), "rb");

  fseek(temp_file, holder.s_block -> block_size * 2, SEEK_SET);

  char * c_data_bitmap;

  fread(c_data_bitmap, 1, sizeof(inode), temp_file);

  holder.data_bitmap = (int *) c_data_bitmap;
  fclose(temp_file);

}


void read_in_all_inodes(std::string file_name, file_data_holder & holder){
  FILE * temp_file;

  temp_file = fopen(file_name.c_str(), "rb");
  fseek(temp_file, holder.s_block -> block_size * 3, SEEK_SET);

  char * c_inode;

  for(int i = 0; i < 256; i++){
    fread(c_inode, 1, sizeof(inode), temp_file);

    holder.all_inodes[i] = (inode *) c_inode;

    fseek(temp_file, holder.s_block -> block_size * 3 + (sizeof(inode) * i + 1), SEEK_SET);
  }

  fclose(temp_file);
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

