#ifndef DISK_H
#define DISK_H

#include <iostream>
#include <string>
#include <stdlib.h>

//inode on disk
class inode {

 public:
	  char file_name[32];
	  int file_size;
	  int db_ptr[12];
	  int ib_ptr;
	  int dib_ptr;

	  int get_unused_data_block();

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
  inode * all_inodes;
  char disk_name[32];
  
};

//message passed to disk_handler
struct message {

  std::string cmd;
  std::string fname;
  std::string l_fname;
  int start;
  int bytes;
  char letter;

};

struct thread_data_holder {

  std::string file_name;

};

#endif
