#ifndef DISK_H
#define DISK_H

#include <iostream>

struct message {

	char * cmd;
	char * name;
	int start;
	int bytes;
	char letter;
};

class inode {

	  std::string file_name;
	  int file_size;
	  int db_ptr[12];
	  int ib_ptr;
	  int dib_ptr;

};

class super_block{

 	public:
		int block_size;
		int num_blocks;
		int db_ptr;
		int inode_ptr;
		int db_blocks;
		super_block(int, int, int, int, int);
};

struct file_data_holder {

  super_block * s_block;
  inode * inode_bitmap;
  int * data_bitmap;
  inode ** all_inodes;

};

#endif
