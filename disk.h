#ifndef DISK_H
#define DISK_H

#include <iostream>

//inode on disk
class inode {

 public:
	  std::string file_name;
	  int file_size;
	  int db_ptr[12];
	  int ib_ptr;
	  int dib_ptr;

};

//superblock on disk
class super_block{

 	public:
		int block_size;
		int num_blocks;
		int db_ptr;
		int inode_ptr;
		int db_blocks;
		super_block(int, int, int, int, int);
};

//data read from disk
struct file_data_holder {

  super_block * s_block;
  int * inode_bitmap;
  int * data_bitmap;
  inode* all_inodes[256];

};

//message passed to disk_handler
struct message {

	char * cmd;
	char * name;
	int start;
	int bytes;
	char letter;
};

#endif
