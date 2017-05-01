#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include "disk.h"

int main(int argc, char * argv[]){
	// sanitize input => make sure input is what instructions specify
	// no more or less than three args, unless instructions et default val
	// make sure argv[1], argv[2] are numbers
	int num_blocks = 0;
	int block_size = 0;
	std::string disk_name;
	num_blocks = atoi(argv[1]);// 0 if argv is not a number
	block_size = atoi(argv[2]);// 0 if argv is not a number
	if(argc > 3){
		disk_name = argv[3];
	} else {
		disk_name = "DISK";
	}
	if(block_size < 128 || block_size > 512){
		perror("Block size has to be less than 512 bytes or greater than 128 bytes");
	}
	if(num_blocks < 1024 || num_blocks > 131072){
		perror("Number of blocks has to be greater than 1024 bytes and less that 131072 bytes (128KB)");
	}
	if((num_blocks & (num_blocks - 1) != 0 ) || (block_size & (block_size -1) != 0)){
		perror("Block size and the number of blocks have to be a power of 2");
	}

	//CALCULATE ADDRESSES
	int inode_start = ((sizeof(inode) * 256)/block_size) + 1;

	//CREATE FILE

	FILE * pFile;
	pFile = fopen (disk_name.c_str(),"w");
	if (pFile!=NULL)
	{
		fseek(pFile, num_blocks * block_size - 1, 0);
		char test_char = '\0';
		fwrite(&test_char, 1, sizeof(test_char), pFile);
		
		super_block sb(block_size, num_blocks, 2 * block_size, 258 * block_size, num_dblks);

		fseek(pFile, 0, num_blocks * block_size - 1);
		
		printf("%d\n", sizeof(sb));



		fclose (pFile);
	} else {
		perror("Unable to open file\n");
	}
	return 0;
}
