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

bool write(file_data_holder & fh, message ms){

  //	std::cout << "write()" << std::endl;
	int index = get_inode_for_file_name(fh, ms.fname);

	if(index == -1){
		perror("File requested does not exist");
		return false;
	}

	if(ms.start > fh.all_inodes[index].file_size){
		perror("Start byte passed in write is greater than total bytes in file");
		return false;
	}

	int current_ublocks = get_unused_data_block(fh);
	int current_blocks = get_unused_data_block(fh);
	int new_blocks = (ms.start + ms.bytes - fh.all_inodes[index].file_size) / fh.s_block -> block_size;
	int new_total = current_blocks + new_blocks;

	if(current_blocks < 13){
		if(new_total > 12){
			new_total++;
			if(new_total > 12 + (fh.s_block->block_size / 4)){
				new_total++;
			}	
		}
	} else if(current_blocks < 12 + (fh.s_block->block_size / 4) + 1){
			if(new_total > 12 + (fh.s_block->block_size / 4)){
				new_total++;
			}	
	}

	//std::cout << "new_blocks: " << new_blocks << " current_ublocks: " << current_ublocks <<std::endl;
	if(new_blocks > current_ublocks){
		perror("Not enough blocks free to complete write");
		return false;
	}

	if((ms.start + ms.bytes) > fh.all_inodes[index].file_size){
		append(fh, index, ms);
	}

	write_data(fh, index, ms);

}

void append(file_data_holder & fh, int index, message m){

	int new_size = m.start + m.bytes;
	int current_block = fh.all_inodes[index].file_size / fh.s_block -> block_size;
	int new_blocks = new_size / fh.s_block -> block_size;

	std::cout << "current_blocks " << current_block << std::endl;
	std::cout << "new_blocks " << new_blocks << std::endl;

	int free_dblk_ind = 0;

	if(fh.all_inodes[index].file_size == 0){
		free_dblk_ind = get_free_data_block(fh);
		add_block(current_block, index, free_dblk_ind, fh);
		fh.data_bitmap[free_dblk_ind] = 1;
		current_block++;
	}

	while(current_block <= new_blocks){
		free_dblk_ind = get_free_data_block(fh);
		add_block(current_block, index, free_dblk_ind, fh);
		fh.data_bitmap[free_dblk_ind] = 1;
		current_block++;
	}
}

void add_block(int c_blk, int index, int new_block, file_data_holder & fh){	

	if(c_blk > (12 + 32 - 1)){//hardcoded -> replace 32 with (fh.s_block->block_size / 4)
		if(fh.all_inodes[index].dib_ptr == -1){
			fh.all_inodes[index].dib_ptr = get_free_data_block(fh);
			fh.data_bitmap[fh.all_inodes[index].dib_ptr] = 1;
		}
		add_did_blk(fh, index, c_blk - 13, new_block);
	}else if(c_blk > 11){
		if(fh.all_inodes[index].ib_ptr == -1){
			fh.all_inodes[index].ib_ptr = get_free_data_block(fh);
			fh.data_bitmap[fh.all_inodes[index].ib_ptr] = 1;
		}
		add_id_blk(fh, index, c_blk - 12, new_block);
	} else {
		fh.all_inodes[index].db_ptr[c_blk] = new_block;			
	}
}

void add_did_blk(file_data_holder & fh, int index, int current_block, int dblk){
	
	int dindirect_block = fh.all_inodes[index].dib_ptr + (current_block / (fh.s_block->block_size / 4)) * 4; 
	int indirect_base = (read_disk_int(fh, dindirect_block) * (fh.s_block->block_size / 4) );
	int indirect_off = current_block % (fh.s_block->block_size / 4) * 4;
	int indirect_block = indirect_base + indirect_off;
	write_disk_int(fh, indirect_block, dblk);

}

void add_id_blk(file_data_holder & fh, int index, int current_block, int dblk){
	
	std::cout << "index: " << index << " cblk: " << current_block << std::endl;
	int start = get_starting_offset(fh) + (fh.all_inodes[index].ib_ptr * fh.s_block->block_size)+ (current_block * 4);
	std::cout << "indirect  block: writing " << dblk << " to byte " << start << std::endl;
	write_disk_int(fh, start, dblk);

}

void write_data(file_data_holder & fh, int index, message m){

  //	std::cout << "write_data()" << std::endl;
	int current_block = m.start / fh.s_block -> block_size;
	int blk_st = 0;
	int blk_end = 0;
	if(m.start + m.bytes > fh.all_inodes[index].file_size){
		fh.all_inodes[index].file_size += m.start + m.bytes - fh.all_inodes[index].file_size;
	}

	//write to first data block
	if(m.start % fh.s_block -> block_size != 0){
		if(m.start <= (current_block + 1) * fh.s_block -> block_size){
			blk_st = m.start % fh.s_block -> block_size;
			if(m.start + m.bytes > (current_block + 1)* fh.s_block -> block_size){
				blk_end = fh.s_block -> block_size;
			} else {
				blk_end = (m.start + m.bytes) % fh.s_block -> block_size;
			}
			write_file(fh, index, current_block, blk_st, blk_end, m.letter);
		}
		current_block++;
		m.bytes -= blk_end - blk_st;
	}	

	//write file in block chunks
	while(m.bytes > fh.s_block -> block_size){
		write_file(fh, index, current_block, 0, fh.s_block -> block_size, m.letter);
		current_block++;
		m.bytes -= fh.s_block -> block_size;
	}

	//write left over data to last block
	if(m.bytes > 0){
		write_file(fh, index, current_block, 0, m.bytes, m.letter);
	}
}

void write_file(file_data_holder & fh, int current_file, int cur_block, int byte_start, int byte_end, char data){

	int global_block = 0; 
	if(cur_block > 43){
		global_block = get_starting_offset(fh) +  get_did_blk(fh, current_file, cur_block - 44) * fh.s_block -> block_size; 
	}
	else if(cur_block > 11){
		global_block = get_starting_offset(fh) +  get_id_blk(fh, current_file, cur_block - 12) * fh.s_block -> block_size; 
	} else {
		global_block = get_starting_offset(fh) +  fh.all_inodes[current_file].db_ptr[cur_block] * fh.s_block -> block_size; 
	}
	int start = global_block + byte_start;
	int end = global_block + byte_end;
	int offset = end - start;
	std::cout << "global: "<< global_block << " start: " << start << " offset " << offset << std::endl;
	write_disk_char(fh, start, offset, data);
}

int get_did_blk(file_data_holder & fh, int index, int c_blk){
		
	int i_bsize = (fh.s_block->block_size / 4);
	int start = fh.all_inodes[index].dib_ptr + (c_blk / i_bsize) * 4;
	int i_block = get_starting_offset(fh) + read_disk_int(fh, start) * fh.s_block->block_size;
	int d_block = i_block + (c_blk % i_bsize) * 4;
	return read_disk_int(fh, d_block);

}

int get_id_blk(file_data_holder & fh, int index, int c_blk){
		
	std::cout << "index: " << index << " cblk: " << c_blk << std::endl;
	int start = get_starting_offset(fh) + (fh.all_inodes[index].ib_ptr * fh.s_block->block_size)+ (c_blk * 4);
	std::cout << "indirect (cb: " << c_blk<< ") block: retrieving at byte " << start << std::endl;
	return read_disk_int(fh, start);

}

void write_disk_int(file_data_holder & fh, int start, int data){

	FILE * t_file;
	t_file = fopen (fh.disk_name, "rb+");
	//std::cout << "offset " << start << std::endl;

	if (t_file != NULL){
		fseek(t_file, start, SEEK_SET);
		fprintf(t_file, "%d", data);
	}
	fclose(t_file);
}

int read_disk_int(file_data_holder & fh, int start){

	FILE * t_file;
	t_file = fopen (fh.disk_name, "rb+");
	int val = 0;
	//std::cout << "offset " << start << std::endl;

	if (t_file != NULL){
		fseek(t_file, start, SEEK_SET);
		fscanf (t_file, "%d", &val);
	}
	fclose(t_file);
	return val;
}



void write_disk_char(file_data_holder & fh, int start, int offset, char data){

	std::cout << "write_disk_char() " << start << std::endl;
	FILE * t_file;
	t_file = fopen (fh.disk_name, "rb+");

	if (t_file != NULL){
		fseek(t_file, start, SEEK_SET);
		for(int i = 0; i < offset; i++){
			fputc(data, t_file);
			//std::cout << "writing" << std::endl;
		}
	}
	fclose(t_file);
}

bool read(file_data_holder & fh, message ms){

	int index = get_inode_for_file_name(fh, ms.fname);

	if(index == -1){
		perror("File requested does not exist");
		return false;
	}

	if(ms.start > fh.all_inodes[index].file_size){
		perror("Start byte passed in read is greater than total bytes in file");
		return false;
	}

	if(ms.bytes +  ms.start > fh.all_inodes[index].file_size){
		perror("End byte passed in read is greater than total bytes in file");
		return false;
	}

	read_data(fh, index, ms);

}

void read_data(file_data_holder & fh, int index, message m){

	int current_block = m.start / fh.s_block -> block_size;
	int blk_st = 0;
	int blk_end = 0;
	if(m.start + m.bytes > fh.all_inodes[index].file_size){
		fh.all_inodes[index].file_size += m.start + m.bytes - fh.all_inodes[index].file_size;
	}

	std::cout << "m.start: " << m.start << " m.bytes: " << m.bytes << std::endl;

	//read to first data block
	if(m.start % fh.s_block -> block_size != 0){
		if(m.start <= (current_block + 1) * fh.s_block -> block_size){
			blk_st = m.start % fh.s_block -> block_size;
			if(m.start + m.bytes > (current_block + 1)* fh.s_block -> block_size){
				blk_end = fh.s_block -> block_size;
			} else {
				blk_end = (m.start + m.bytes) % (fh.s_block -> block_size);
			}
			read_file(fh, index, current_block, blk_st, blk_end);
		}
		current_block++;
		m.bytes -= blk_end - blk_st;
	}	

	//read file in block chunks
	while(m.bytes > fh.s_block -> block_size){
		read_file(fh, index, current_block, 0, fh.s_block -> block_size);
		current_block++;
		m.bytes -= fh.s_block -> block_size;
	}
	//read left over data to last block
	if(m.bytes > 0){
		read_file(fh, index, current_block, 0, m.bytes);
	}
	
}

void read_file(file_data_holder & fh, int current_file, int cur_block, int byte_start, int byte_end){
	
	std::cout << "b_end: " << byte_end << " byte start: " << byte_start << " currentblock: " << cur_block << std::endl;
	
	int global_block = 0; 
	if(cur_block > 43){
		global_block = get_starting_offset(fh) +  get_did_blk(fh, current_file, cur_block - 44) * fh.s_block -> block_size; 
	} else if(cur_block > 11){
		global_block = get_starting_offset(fh) +  get_id_blk(fh, current_file, cur_block - 12) * fh.s_block -> block_size; 
	} else {
		global_block = get_starting_offset(fh) +  fh.all_inodes[current_file].db_ptr[cur_block] * fh.s_block -> block_size; 
		std::cout << " < 11 " << global_block << std::endl;
	}
	int start = global_block + byte_start;
	int end = global_block + byte_end;
	int offset = end - start;
	std::cout << "read_file: "<< start << " : " << offset << std::endl;
	read_disk_char(fh, start, offset);

}

void read_disk_char(file_data_holder & fh, int start, int offset){

	FILE * t_file;
	t_file = fopen (fh.disk_name, "rb+");
	std::cout << "read_disk_char: " << start << std::endl;
	char c;

	if (t_file != NULL){
		fseek(t_file, start, SEEK_SET);
		for(int i = 0; i < offset; i++){
			c = fgetc(t_file);
			std::cout << c << std::endl;
		}
	}
	fclose(t_file);
}

bool import(file_data_holder & fh, message ms){
	
	int sz = 0;
	FILE * fp;
	fp = fopen(ms.l_fname.c_str(), "r");
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
	ms.bytes = sz;

	int index = get_inode_for_file_name(fh, ms.fname);

	if(index == -1){
		create(fh, ms);
		index = get_inode_for_file_name(fh, ms.fname);
	} else {
		delete_file(fh, ms);
		create(fh, ms);
		index = get_inode_for_file_name(fh, ms.fname);
	}

	if(ms.start > fh.all_inodes[index].file_size){
		perror("Start byte passed in write is greater than total bytes in file");
		return false;
	}

	int current_ublocks = get_unused_data_block(fh);
	int current_blocks = get_unused_data_block(fh);
	int new_blocks = (ms.start + ms.bytes - fh.all_inodes[index].file_size) / fh.s_block -> block_size;
	int new_total = current_blocks + new_blocks;

	if(current_blocks < 13){
		if(new_total > 12){
			new_total++;
			if(new_total > 12 + (fh.s_block->block_size / 4)){
				new_total++;
			}	
		}
	} else if(current_blocks < 12 + (fh.s_block->block_size / 4) + 1){
			if(new_total > 12 + (fh.s_block->block_size / 4)){
				new_total++;
			}	
	}

	//std::cout << "new_blocks: " << new_blocks << " current_ublocks: " << current_ublocks <<std::endl;
	if(new_blocks > current_ublocks){
		perror("Not enough blocks free to complete write");
		return false;
	}

	std::cout << "ms:start " << ms.start << " ms. bytes " << ms.bytes << " filesize " << fh.all_inodes[index].file_size << std::endl;
	if((ms.start + ms.bytes) > fh.all_inodes[index].file_size){
		append(fh, index, ms);
	}

	import_data(fh, index, ms);

}

void import_data(file_data_holder & fh, int index, message m){

	int current_block = m.start / fh.s_block -> block_size;
	int blk_st = 0;
	int blk_end = 0;
	int ifile_start = 0;
	fh.all_inodes[index].file_size += m.bytes;

	//write to first data block
	if(m.start % fh.s_block -> block_size != 0){
		if(m.start + m.bytes <= (current_block + 1) * fh.s_block -> block_size){
			blk_st = m.start % fh.s_block -> block_size;
			blk_end = m.start + m.bytes - (current_block * fh.s_block -> block_size);
			import_file(fh, index, current_block, blk_st, blk_end, m.l_fname, ifile_start);
		}
		current_block++;
		m.bytes -= blk_end - blk_st;
		ifile_start += blk_end - blk_st;
	}	

	//write file in block chunks
	while(m.bytes > fh.s_block -> block_size){
		import_file(fh, index, current_block, 0, fh.s_block -> block_size, m.l_fname, ifile_start);
		current_block++;
		m.bytes -= fh.s_block -> block_size;
		ifile_start += blk_end - blk_st;
	}
	//write left over data to last block
	if(m.bytes > 0){
		import_file(fh, index, current_block, 0, m.bytes, m.l_fname, ifile_start);
		ifile_start += blk_end - blk_st;
	}
}

void import_file(file_data_holder & fh, int current_file, int cur_block, int byte_start, int byte_end, std::string iname, int ifile_off){

	int global_block = 0; 
	if(cur_block > 43){
		global_block = get_starting_offset(fh) +  get_did_blk(fh, current_file, cur_block - 44) * fh.s_block -> block_size; 
	}
	else if(cur_block > 11){
		global_block = get_starting_offset(fh) +  get_id_blk(fh, current_file, cur_block - 12) * fh.s_block -> block_size; 
	} else {
		global_block = get_starting_offset(fh) +  fh.all_inodes[current_file].db_ptr[cur_block] * fh.s_block -> block_size; 
	}
	int start = global_block + byte_start;
	int end = global_block + byte_end;
	int offset = end - start;
	std::cout << "global: "<< global_block << " start: " << start << " offset " << offset << std::endl;
	import_disk_char(fh, start, offset, iname, ifile_off);

}

void import_disk_char(file_data_holder & fh, int start, int offset, std::string ifile, int ifile_off){

	FILE * t_file;
	t_file = fopen (fh.disk_name, "rb+");
	FILE * i_file;
	i_file = fopen (ifile.c_str(), "r");

	if (t_file != NULL){
		fseek(t_file, start, SEEK_SET);
		fseek(i_file, ifile_off, SEEK_SET);
		char data;
		for(int i = 0; i < offset; i++){
			data = fgetc(i_file);
			fputc(data, t_file);
		}
	}
	fclose(t_file);
	fclose(i_file);
}

void write_disk(file_data_holder & fh, int offset, int size, void * ptr, char op){

	FILE * t_file;
	t_file = fopen (fh.disk_name, "rb+");

	if (t_file != NULL){
		fseek(t_file, offset, SEEK_SET);
		if(op == 'I'){
			fwrite((int *)ptr, 1, size, t_file);
		} else {
			for(int i = 0; i < size; i++){
				fputc(*(char*)ptr, t_file);
			}
		}
	}
	fclose(t_file);
}




bool cat(file_data_holder & fh, message ms){
	
	int index = get_inode_for_file_name(fh, ms.fname);
	ms.start = 0;
	ms.bytes = fh.all_inodes[index].file_size;
	read(fh, ms);

	return true;
}

void unlink(file_data_holder & fh, inode &in){
	int i = 0;
	while(in.db_ptr[i] != -1){
		fh.data_bitmap[in.db_ptr[i]] = 0;
		i++;
	}

	memset(in.file_name, -1, sizeof(in.file_name));
	memset(in.db_ptr, -1, sizeof(in.db_ptr));
	in.file_size = -1;
	in.ib_ptr = -1;
	in.dib_ptr = -1;;
}

bool delete_file(file_data_holder & fh, message ms){
	if(!does_file_exist(fh, ms.fname))
		return false;

	for(int i = 0; i < 256; i++){
		if(!ms.fname.compare(fh.all_inodes[i].file_name)){
			unlink(fh, fh.all_inodes[i]);
			fh.inode_bitmap[i] = 0;
			return true;
		}
	}
	return true;

}

void list_files(file_data_holder & fh){

	for(int i = 0; i < 256; i++){
		if(fh.inode_bitmap[i] == 1){
			std::cout << fh.all_inodes[i].file_name << ":" << fh.all_inodes[i].file_size << std::endl;
		}
	}

}

void read_in_super_block(std::string file_name, file_data_holder & fh){

	FILE * temp_file;
	temp_file = fopen(file_name.c_str(), "rb");
	if(temp_file != NULL){
	char * super_b = (char *) malloc(sizeof(super_block));
	fread(super_b, 1, sizeof(super_block), temp_file);
	super_block * t_sb = (super_block *) super_b;
	fh.s_block = t_sb;
	fclose(temp_file);
	}
}

void read_in_inode_bitmap(std::string file_name, file_data_holder & fh){
	FILE * temp_file;
	temp_file = fopen(file_name.c_str(), "rb");
	if(temp_file != NULL){
	fseek(temp_file, fh.s_block -> block_size, SEEK_SET);
	char * c_inode_bitmap = (char *) malloc(sizeof(int) * 256);
	fread(c_inode_bitmap, 1, sizeof(int) * 256, temp_file);
	fh.inode_bitmap = (int *) c_inode_bitmap;
	fclose(temp_file);
  }
}

void read_in_data_bitmap(std::string file_name, file_data_holder & fh){
  
	FILE * temp_file;
	temp_file = fopen(file_name.c_str(), "rb");
	if(temp_file != NULL){
	fseek(temp_file, fh.s_block -> block_size + (sizeof(int) * 256), SEEK_SET);
	char * c_data_bitmap = (char *) malloc(sizeof(int) * fh.s_block -> db_blocks);
	fread(c_data_bitmap, 1, sizeof(int) * fh.s_block -> db_blocks, temp_file);
	fh.data_bitmap = (int *) c_data_bitmap;
	fclose(temp_file);
  }
}

void read_in_all_inodes(std::string file_name, file_data_holder & fh){
  
	FILE * temp_file;
	temp_file = fopen(file_name.c_str(), "rb");
	if(temp_file != NULL){
	fseek(temp_file, fh.s_block -> block_size + (sizeof(int) * 256) + (sizeof(int) * fh.s_block -> db_blocks), SEEK_SET);
	char * c_inode = (char *) malloc(sizeof(inode) * 256);
	fread(c_inode, 1, sizeof(inode) * 256 , temp_file);
	fh.all_inodes = (inode *) c_inode;
	fclose(temp_file);
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

int get_starting_offset(file_data_holder & fh){

	return fh.s_block -> block_size + (sizeof(int) * 256) + (sizeof(int) * fh.s_block -> db_blocks) + (sizeof(inode) * 256);

}

int get_free_inode(file_data_holder & fh){

	for(int i = 0; i < 256; i++){
		if(fh.inode_bitmap[i] == 0) {
			return i;
		}
	}
	return -1;
}

int get_free_data_block(file_data_holder & fh){
	for(int i = 0; i < fh.s_block -> db_blocks; i++)
		if(fh.data_bitmap[i] == 0)
			return i;
	return -1;
}

int get_inode_for_file_name(file_data_holder & fh, std::string f_name){

	for(int i = 0; i < 256; i++)
		if(strcmp(fh.all_inodes[i].file_name, f_name.c_str()) == 0){
			return i;
		}
	return -1;
}

bool create(file_data_holder &fh, message mes){

	if(does_file_exist(fh, mes.fname))
		return false;

	int index = get_free_inode(fh);
	if(index == -1){
		perror("no inode available");
		return false;
	}

	strcpy(fh.all_inodes[index].file_name, mes.fname.c_str());
	fh.all_inodes[index].file_size = 0;
	memset(fh.all_inodes[index].db_ptr, -1 , 12);
	fh.all_inodes[index].ib_ptr = -1;;
	fh.all_inodes[index].dib_ptr = -1;
	fh.inode_bitmap[index] = 1;
	return true;

}

std::vector<std::string> split_string_by_space(std::string str){

	std::istringstream buf(str);
	std::istream_iterator<std::string> beg(buf), end;
	std::vector<std::string> tokens(beg, end);

	return tokens;
}



void shutdown(file_data_holder & fh){

	FILE * t_file;
	t_file = fopen(fh.disk_name, "rb+");

	inode real_inodes[256];
	int inode_bitmap[256];
	int data_bitmap[fh.s_block -> db_blocks];
	super_block sb(fh.s_block -> block_size, fh.s_block -> num_blocks, fh.s_block -> db_ptr, fh.s_block -> inode_ptr, fh.s_block -> db_blocks);

	for(int i = 0; i < 256; i++){
		inode_bitmap[i] = fh.inode_bitmap[i];
		real_inodes[i] = fh.all_inodes[i];
	}

	for(int i = 0; i < fh.s_block -> db_blocks; i++)
		data_bitmap[i] = fh.data_bitmap[i];


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

		fclose(t_file);
	}
}

int get_unused_data_block(file_data_holder & fh){

	int open_dblocks = 0;
	for(int i = 0; i < fh.s_block -> db_blocks; i++){
		if(fh.data_bitmap[i] == 0){
			open_dblocks++;
		}
	}
	return open_dblocks;
}

int get_used_data_block(file_data_holder & fh){

	int full_dblocks = 0;
	for(int i = 0; i < fh.s_block -> db_blocks; i++){
		if(fh.data_bitmap[i] == 1){
			full_dblocks++;
		}
	}
	return full_dblocks;
}
