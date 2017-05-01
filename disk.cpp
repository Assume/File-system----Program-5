#include "disk.h"


/*

	Constructor for superblock to set parameters

*/
super_block::super_block(int blk_size, int num_blks, int db_start, int in_start, int db_blks){

		block_size = blk_size;
		num_blocks = num_blks;
		db_ptr = db_start;
		inode_ptr = in_start;
		db_blocks = db_blks;
}
