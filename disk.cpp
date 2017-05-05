#include "disk.h"

super_block::super_block(int blk_size, int num_blks, int db_start, int in_start, int db_blks){

		block_size = blk_size;
		num_blocks = num_blks;
		db_ptr = db_start;
		inode_ptr = in_start;
		db_blocks = db_blks;
}


int inode::get_unused_data_block(){

  for(int i = 0; i < 12; i++)
    if(db_ptr[i] == -1)
      return i;

  return -1;

}
