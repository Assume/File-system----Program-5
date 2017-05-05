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
#include <string.h>

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
  char * c_inode_bitmap = (char *) malloc(sizeof(int) * 256);
  fread(c_inode_bitmap, 1, sizeof(int) * 256, temp_file);
  holder.inode_bitmap = (int *) c_inode_bitmap;
  fclose(temp_file);

}

void read_in_data_bitmap(std::string file_name, file_data_holder & holder){

  FILE * temp_file;
  temp_file = fopen(file_name.c_str(), "rb");
  fseek(temp_file, holder.s_block -> block_size + (sizeof(int) * 256), SEEK_SET);
  char * c_data_bitmap = (char *) malloc(sizeof(int) * holder.s_block -> db_blocks);
  fread(c_data_bitmap, 1, sizeof(int) * holder.s_block -> db_blocks, temp_file);
  holder.data_bitmap = (int *) c_data_bitmap;
  fclose(temp_file);

}


void read_in_all_inodes(std::string file_name, file_data_holder & holder){

  FILE * temp_file;
  temp_file = fopen(file_name.c_str(), "rb");
  fseek(temp_file, holder.s_block -> block_size + (sizeof(int) * 256) + (sizeof(int) * holder.s_block -> db_blocks), SEEK_SET);
  char * c_inode = (char *) malloc(sizeof(inode) * 256);
  fread(c_inode, 1, sizeof(inode) * 256 , temp_file);
  holder.all_inodes = (inode *) c_inode;
  
  /*
  char * c_node = (char *) malloc(sizeof(inode));
  fread(c_node, 1, sizeof(inode), temp_file);

  inode * temp_i = (inode *) c_node;

  std::cout << temp_i -> file_name << std::endl;
  */
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

int get_starting_offset(file_data_holder & holder){
  
  return holder.s_block -> block_size + (sizeof(int) * 256) + (sizeof(int) * holder.s_block -> db_blocks) + (sizeof(inode) * 256);

}

int get_free_inode(file_data_holder fh){

	for(int i = 0; i < 256; i++){
		if(fh.inode_bitmap[i] == 0) {
			return i;
		}
	}
	return -1;
}

bool create(file_data_holder &fh, std::string f_name){

	int index = get_free_inode(fh);
	if(index == -1){
		perror("no inode available");
		return false;
	}

	char arr[12] = {-1};

	strcpy(fh.all_inodes[index].file_name, f_name.c_str());
	fh.all_inodes[index].file_size = 0;
	memcpy(fh.all_inodes[index].db_ptr, arr , sizeof(arr));
	fh.all_inodes[index].ib_ptr = -1;;
	fh.all_inodes[index].dib_ptr = -1;
	fh.inode_bitmap[index] = 1;
	return true;

}
