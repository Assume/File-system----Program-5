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


bool does_file_exist(file_data_holder fh, std::string f_name){

	for(int i = 0; i < 256; i++) {
		if(strcmp(fh.all_inodes[i].file_name, f_name.c_str()) == 0){
			return true;
		}
	}
	return false;
}

bool write(file_data_holder & holder, message & ms){
	/*
	int index = get_inode_for_file_name(holder, ms.fname);

	if(index == -1){
		perror("File requested does not exist");
		return false;
	}

	if(ms.start > holder.all_inodes[index].file_size){
		perror("Start byte passed in write is greater than total bytes in file");
		return false;
	}

	if(((holder.all_inodes[index].get_unused_data_block() * (holder.s_block -> block_size)) + (ms.start + ms.bytes)) > (12 * holder.s_block -> block_size)){
		perror("Not enough blocks free to complete write");
		return false;
	}

	int start_block = holder.all_inodes[index].file_size / holder.s_block -> block_size;

	*/
}

bool read(file_data_holder & holder, message & ms){

}

bool import(file_data_holder & holder, std::string str){

}

/*bool cat(file_data_holder & holder, message & ms){

	int index = get_inode_for_file_name(holder, ms.fname);

	if(index == -1){
		perror("File requested does not exist");
		return false;
	}

	int total_blocks_to_read = holder.all_inodes[index].get_unused_data_block();
	int total_file_size = holder.all_inodes[index].file_size;

	int total_read = 0;

	char * data_read_in = (char *) malloc(total_file_size);

	for(int i = 0; i < total_blocks_to_read; i++){
		char * r_block = (char *) malloc(holder.s_block -> block_size);
		FILE * t_file;
		t_file = fopen(holder.disk_name, "rb");
		fseek(t_file, get_starting_offset(holder) + ((holder.all_inodes[i].db[i] - 1) * holder.s_block -> block_size), SEEK_SET);

		if(i + 1 == total_blocks_to_read){
			fread(r_block, 1, total_file_size - total_read, t_file);
		} else {
			fread(r_block, 1, holder.s_block -> block_size, t_file);
		}
		for(int j = 0; j < i + 1 == total_blocks_to_read ? total_file_size - total_read : holder.s_block -> block_size; j++){
			data_read_in[(i * holder.s_block -> block_size) + j] = r_block[j];
		}
		total_read += holder.s_block -> block_size;


		fclose(t_file);
		free(r_block);
	}
	std::cout << data_read_in << std::endl;
	return true;
}*/

void unlink(file_data_holder & holder, inode &in){
	int i = 0;
	while(in.db_ptr[i] != -1){
		holder.data_bitmap[in.db_ptr[i]] = 0;
		i++;
	}
	
	std::cout << "unlinking file: " << in.file_name << std::endl;
	memset(in.file_name, -1, sizeof(in.file_name));
	memset(in.db_ptr, -1, sizeof(in.db_ptr));
	in.file_size = -1;
	in.ib_ptr = -1;
	in.dib_ptr = -1;;
}

bool delete_file(file_data_holder & holder, std::string file_name){
	if(!does_file_exist(holder, file_name))
		return false;

	for(int i = 0; i < 256; i++){
		if(!file_name.compare(holder.all_inodes[i].file_name)){
			unlink(holder, holder.all_inodes[i]);
			holder.inode_bitmap[i] = 0;
			return true;
		}
	}
	return true;

}

void list_files(file_data_holder & holder){

	for(int i = 0; i < 256; i++)
		if(holder.inode_bitmap[i] == 1)
			std::cout << holder.all_inodes[i].file_name << ":" << holder.all_inodes[i].file_size << std::endl;

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


int get_free_data_block(file_data_holder fh){
	for(int i = 0; i < fh.s_block -> db_blocks; i++)
		if(fh.data_bitmap[i] == 0)
			return i;
	return -1;
}


int get_inode_for_file_name(file_data_holder fh, char * f_name){

	for(int i = 0; i < 256; i++)
		if(strcmp(fh.all_inodes[i].file_name, f_name) == 0)
			return i;
	return -1;
}

bool create(file_data_holder &fh, std::string f_name){

	if(does_file_exist(fh, f_name))
		return false;

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


void shutdown(file_data_holder & holder){
  FILE * t_file;
  t_file = fopen (holder.disk_name, "rb+");

  inode real_inodes[256];

  int inode_bitmap[256];

  int data_bitmap[holder.s_block -> db_blocks];

  super_block sb(holder.s_block -> block_size, holder.s_block -> num_blocks, holder.s_block -> db_ptr, holder.s_block -> inode_ptr, holder.s_block -> db_blocks);

  for(int i = 0; i < 256; i++){
    inode_bitmap[i] = holder.inode_bitmap[i];
    real_inodes[i] = holder.all_inodes[i];
  }

  for(int i = 0; i < holder.s_block -> db_blocks; i++)
    data_bitmap[i] = holder.data_bitmap[i];


  if (t_file != NULL){
    //write superblock byte by byte
    fwrite(&sb, 1, sizeof(sb), t_file);


    //seek to next block after super block
    fseek(t_file, sb.block_size, SEEK_SET);

    //write inode bitmap to second block
    fwrite(&inode_bitmap, 1, sizeof(inode_bitmap), t_file);

    //seek to next block -- possibly unnessecary due to write seeking num bytes written
    //but depends on the size of what was written
    fseek(t_file, sb.block_size + sizeof(inode_bitmap), SEEK_SET);

    //write data bitmap
    fwrite(&data_bitmap, 1, sizeof(data_bitmap), t_file);

    fseek(t_file, sb.block_size + sizeof(inode_bitmap) + sizeof(data_bitmap), SEEK_SET);

    fwrite(&real_inodes, 1, sizeof(real_inodes), t_file);

    fclose (t_file);
  }
}


/*
int add_shared_to(message & mes, void * ptr){

	int m_mes = 0;
	while(m_mes < 4){
		m_mes++;
		std::cout << "valid: "<<  (int)((message *)ptr)->valid << std::endl;
		std::cout << "ptr: "<<  *(int *)ptr << std::endl;
		if(((message *)ptr)->valid == 0 || !strcmp("EMPTY", (*((message *)ptr)).cmd)){
			*((message *)ptr) = mes;
			break;
		} else {
			std::cout << "command: "<< (char *)((message *)ptr)->cmd << std::endl;
			(int *)ptr += sizeof(message);
		}
	}
	return m_mes;
}
*/
