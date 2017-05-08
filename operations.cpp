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

	std::cout << "write()" << std::endl;
	int index = get_inode_for_file_name(fh, ms.fname);

	if(index == -1){
		perror("File requested does not exist");
		return false;
	}

	if(ms.start > fh.all_inodes[index].file_size){
		perror("Start byte passed in write is greater than total bytes in file");
		return false;
	}

	if(((fh.all_inodes[index].get_unused_data_block() * (fh.s_block -> block_size)) + (ms.start + ms.bytes)) > (12 * fh.s_block -> block_size)){
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

	int free_dblk_ind = 0;

	if(fh.all_inodes[index].file_size == 0){
		free_dblk_ind = get_free_data_block(fh);
		add_block(current_block, index, free_dblk_ind, fh);
		fh.data_bitmap[free_dblk_ind] = 1;
	}

	if(current_block < new_blocks){
		free_dblk_ind = get_free_data_block(fh);
		add_block(++current_block, index, free_dblk_ind, fh);
		fh.data_bitmap[free_dblk_ind] = 1;
	}
	
	/*
	//write the rest of the current block
	int cur_blk = (fh.s_block -> block_size) - (ms.start % fh.s_block -> block_size);
	int g_start = get_starting_offset(fh) + fh.all_inodes[index].db_ptr[i + j]  * fh.s_block -> block_size;
	write_disk(fh, ms.start, cur_blk, (void *)(&ms.letter));
	fh.all_inodes[index].file_size += cur_blk;

	int add_blocks = ((ms.start + ms.bytes) / (fh.s_block -> block_size)) - (fh.all_inodes[index].file_size/ (fh.s_block -> block_size));
	int off = (ms.start + ms.bytes) % (fh.s_block -> block_size);
	
	int i = 0;
	int blks = blocks;
	
	//find the first unused direct data block
	while(fh.all_inodes[index].db_ptr[i] > 0){
		i++;
	}

	//increasing size of file
	fh.all_inodes[index].file_size += blocks * fh.s_block -> block_size + off;
	int ib_start = 0;
	int j;

	//increments to blocks - 1 because the last block would only be partially filled so we do that separately
	for(j = 0; j < blocks - 1 && (i + j) < 12; j++, blks--){
		//get free block
		fh.all_inodes[index].db_ptr[i + j] = get_free_data_block(fh);
		//set data block as used
		fh.data_bitmap[fh.all_inodes[index].db_ptr[i + j]] = 1;
		//calculate byte on disk where writing start
		ib_start = get_starting_offset(fh) + fh.all_inodes[index].db_ptr[i + j]  * fh.s_block -> block_size;
		//write a block of data
		write_disk(fh, ib_start, fh.s_block -> block_size, (void *)&c);
	}
	
	//if blks == 1 we are still allocating direct data blocks
	if(blks == 1){
		int pos = get_starting_offset(fh) + fh.all_inodes[index].db_ptr[i + j]  * fh.s_block -> block_size;
		write_disk(fh, pos, off, (void *)(&c));
	} else {

		std::cout << "12 data blocks exceeded" << std::endl;

		//now we have to allocate a indirect block to memory
		if(blks > 0){

			int i_blocks = fh.s_block -> block_size / 4;

			if(fh.all_inodes[index].dib_ptr == -1){
				fh.all_inodes[index].dib_ptr = get_free_data_block(fh);
				fh.data_bitmap[fh.all_inodes[index].dib_ptr];
			}

			int blks2 = blks;
			int d_address = 0;
			ib_start = get_starting_offset(fh) +  fh.all_inodes[index].dib_ptr * fh.s_block -> block_size;

			for(int k = 0; blks2 > 0 && k < i_blocks; k++, blks2--){
				d_address = get_free_data_block(fh);
				fh.data_bitmap[d_address];
				d_address *= fh.s_block -> block_size;
				write_disk(fh, ib_start + k * 4, 4, (void *)(&d_address), 'I');		
			}

			if(blks2 > 0){
				//this means double indirect
			}
		}
	}
	*/
}

void add_block(int c_blk, int index, int new_block, file_data_holder & fh){	

	fh.all_inodes[index].db_ptr[c_blk] = new_block;			
}

void write_data(file_data_holder & fh, int index, message m){

	std::cout << "write_data()" << std::endl;
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
				blk_end = (current_block + 1) * fh.s_block -> block_size;
			} else {
				blk_end = m.start + m.bytes;
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
	
	/*
	int w_size = m.bytes;
	int w_start = m.start;
	int w_total = w_start +  w_size;
	int cur_blk = (fh.s_block -> block_size) - (ms.start % fh.s_block -> block_size);
	int g_blk = get_last_block(index);

	//writing to last non_full direct block
	if(w_size <= cur_blk){
		write_block(fh, index, g_blk, w_start % fh.s_block -> block_size, w_size, m.letter);
	} else {
		write_block(fh, index, g_blk, w_start % fh.s_block -> block_size, cur_blk, m.letter);
		w_start += cur_blk;
		while(w_start < w_total){
			//cur_blk = (fh.s_block -> block_size) - (w_start % fh.s_block -> block_size);	
			if(w_total - w_start < fh.s_block -> block_size){
				write_block(fh, index, get_last_block(index), w_start % (fh.s_block -> block_size), w_total - w_start, m.letter)
			w_start += fh.s_block -> block_size;
		}
	}
*/
}

void write_file(file_data_holder & fh, int current_file, int cur_block, int byte_start, int byte_end, char data){

	int global_block = get_starting_offset(fh) +  fh.all_inodes[current_file].db_ptr[cur_block] * fh.s_block -> block_size; 
	int start = global_block + byte_start;
	int end = global_block + byte_end;
	int offset = end - start;
	write_disk_char(fh, start, offset, data);
}

void write_disk_char(file_data_holder & fh, int start, int offset, char data){

	FILE * t_file;
	t_file = fopen (fh.disk_name, "rb+");
	std::cout << "offset " << offset << std::endl;

	if (t_file != NULL){
		fseek(t_file, start, SEEK_SET);
		for(int i = 0; i < offset; i++){
			fputc(data, t_file);
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

	//read to first data block
	if(m.start % fh.s_block -> block_size != 0){
		if(m.start + m.bytes <= (current_block + 1) * fh.s_block -> block_size){
			blk_st = m.start % fh.s_block -> block_size;
			blk_end = m.start + m.bytes - (current_block * fh.s_block -> block_size);
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

	int global_block = get_starting_offset(fh) +  fh.all_inodes[current_file].db_ptr[cur_block] * fh.s_block -> block_size; 
	int start = global_block + byte_start;
	int end = global_block + byte_end;
	int offset = end - start;
	read_disk_char(fh, start, offset);
}

void read_disk_char(file_data_holder & fh, int start, int offset){

	FILE * t_file;
	t_file = fopen (fh.disk_name, "rb+");
	char c;

	if (t_file != NULL){
		fseek(t_file, start, SEEK_SET);
		for(int i = 0; i < offset; i++){
			c = fgetc(t_file);
			std::cout << c ;
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
		index = create(fh, ms.fname);
	}

	if(ms.start > fh.all_inodes[index].file_size){
		perror("Start byte passed in write is greater than total bytes in file");
		return false;
	}

	if(((fh.all_inodes[index].get_unused_data_block() * (fh.s_block -> block_size)) + (ms.start + ms.bytes)) > (12 * fh.s_block -> block_size)){
		perror("Not enough blocks free to complete write");
		return false;
	}


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

	
	/*
	int w_size = m.bytes;
	int w_start = m.start;
	int w_total = w_start +  w_size;
	int cur_blk = (fh.s_block -> block_size) - (ms.start % fh.s_block -> block_size);
	int g_blk = get_last_block(index);

	//writing to last non_full direct block
	if(w_size <= cur_blk){
		write_block(fh, index, g_blk, w_start % fh.s_block -> block_size, w_size, m.letter);
	} else {
		write_block(fh, index, g_blk, w_start % fh.s_block -> block_size, cur_blk, m.letter);
		w_start += cur_blk;
		while(w_start < w_total){
			//cur_blk = (fh.s_block -> block_size) - (w_start % fh.s_block -> block_size);	
			if(w_total - w_start < fh.s_block -> block_size){
				write_block(fh, index, get_last_block(index), w_start % (fh.s_block -> block_size), w_total - w_start, m.letter)
			w_start += fh.s_block -> block_size;
		}
	}
*/
}

void import_file(file_data_holder & fh, int current_file, int cur_block, int byte_start, int byte_end, std::string iname, int ifile_off){

	int global_block = get_starting_offset(fh) +  fh.all_inodes[current_file].db_ptr[cur_block] * fh.s_block -> block_size; 
	int start = global_block + byte_start;
	int end = global_block + byte_end;
	int offset = end - start;
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

	ieturn true;
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
	char * super_b = (char *) malloc(sizeof(super_block));
	fread(super_b, 1, sizeof(super_block), temp_file);
	super_block * t_sb = (super_block *) super_b;
	fh.s_block = t_sb;
	fclose(temp_file);
}

void read_in_inode_bitmap(std::string file_name, file_data_holder & fh){

	FILE * temp_file;
	temp_file = fopen(file_name.c_str(), "rb");
	fseek(temp_file, fh.s_block -> block_size, SEEK_SET);
	char * c_inode_bitmap = (char *) malloc(sizeof(int) * 256);
	fread(c_inode_bitmap, 1, sizeof(int) * 256, temp_file);
	fh.inode_bitmap = (int *) c_inode_bitmap;
	fclose(temp_file);
}

void read_in_data_bitmap(std::string file_name, file_data_holder & fh){

	FILE * temp_file;
	temp_file = fopen(file_name.c_str(), "rb");
	fseek(temp_file, fh.s_block -> block_size + (sizeof(int) * 256), SEEK_SET);
	char * c_data_bitmap = (char *) malloc(sizeof(int) * fh.s_block -> db_blocks);
	fread(c_data_bitmap, 1, sizeof(int) * fh.s_block -> db_blocks, temp_file);
	fh.data_bitmap = (int *) c_data_bitmap;
	fclose(temp_file);

}

void read_in_all_inodes(std::string file_name, file_data_holder & fh){

	FILE * temp_file;
	temp_file = fopen(file_name.c_str(), "rb");
	fseek(temp_file, fh.s_block -> block_size + (sizeof(int) * 256) + (sizeof(int) * fh.s_block -> db_blocks), SEEK_SET);
	char * c_inode = (char *) malloc(sizeof(inode) * 256);
	fread(c_inode, 1, sizeof(inode) * 256 , temp_file);
	fh.all_inodes = (inode *) c_inode;
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

int create(file_data_holder &fh, std::string name){

	if(does_file_exist(fh, name))
		return false;

	int index = get_free_inode(fh);
	if(index == -1){
		perror("no inode available");
		return false;
	}

	strcpy(fh.all_inodes[index].file_name, name.c_str());
	fh.all_inodes[index].file_size = 0;
	memset(fh.all_inodes[index].db_ptr, -1 , 12);
	fh.all_inodes[index].ib_ptr = -1;;
	fh.all_inodes[index].dib_ptr = -1;
	fh.inode_bitmap[index] = 1;
	return index;

}

std::vector<std::string> split_string_by_space(std::string str){

	std::istringstream buf(str);
	std::istream_iterator<std::string> beg(buf), end;
	std::vector<std::string> tokens(beg, end);

	return tokens;
}



void shutdown(file_data_holder & fh){

	FILE * t_file;
	t_file = fopen (fh.disk_name, "rb+");

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

	}
	fclose (t_file);
}

/*int get_last_block(file_data_holder & fh, int index){
	
	int i = 0;
	while(fh.all_inodes[index].db_ptr[i+1] > -1){
		i++;
	}
	return i;
}*/

/*void write_block(file_data_holder & fh, int file_index, int blk_index, int start, int size, char c){

	if(start + size == fh.s_block -> block_size){
		fh.all_inodes[index].db_ptr[blk_index + 1] = 0;
	}

	int st = fh.all_inodes[index].db_ptr[blk_index] * fh.s_block -> block_size;
	write_disk_char(fh, st + start, st + start + size, c);	
}*/


