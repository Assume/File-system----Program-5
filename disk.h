#ifndef DISK_H
#define DISK_H

#include <iostream>

class inode {

	private:
	  std::string file_name;
	  int file_size;
	  int db_ptr;
	  int ib_ptr;
	  int dib_tr;

};


class super_block{
	private:
		int block_size;
		int num_blocks;
		int db_ptr;
		int inode_ptr;
		int db_blocks;
	public:
		super_block(int, int, int, int, int);
};

#endif
