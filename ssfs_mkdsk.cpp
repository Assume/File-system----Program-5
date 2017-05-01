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
	int inode_start = 2 + ((sizeof(inode) * 256)/block_size) + 1;
	int data_block_start = inode_start + 256;
	int num_data_blocks = num_blocks - data_block_start;


	//dummy values to write to disk
	int inodes[256] = {0};
	int db[num_data_blocks] = {0};
	inode in;

	//CREATE FILE
	FILE * pFile;
	pFile = fopen (disk_name.c_str(),"w");
	if (pFile!=NULL)
	{
		//seek to the end of the file and writes a character so the file is the correct size
		fseek(pFile, num_blocks * block_size - 1, 0);
		char test_char = '\0';
		fwrite(&test_char, 1, sizeof(test_char), pFile);
		
		//creating superblock for file system
		super_block sb(block_size, num_blocks, data_block_start, inode_start, num_data_blocks);

		//seeks back to beginning of the file
		fseek(pFile, 0, num_blocks * block_size - 1);
		
		//write superblock byte by byte
		fwrite(&sb, 1, sizeof(sb), pFile);

		//seek to next block
		fseek(pFile, block_size, 0);

		//write inode bitmap to second block
		fwrite(&inodes, 1, sizeof(inodes), pFile);

		//seek to next block
		fseek(pFile, block_size * 2, block_size);

		//write data bitmap
		fwrite(&db, 1, sizeof(db), pFile);

		fseek(pFile, block_size * 3, block_size * 2);
		for(int i = 0; i < 256; i++){
			fwrite(&in, 1, sizeof(in), pFile);
			//seek to the next inode spot not the next block
			fseek(pFile, block_size * 3 + (sizeof(inode) * i + 1) , block_size * 3 + (sizeof(inode) * i));
		}

		fclose (pFile);
	} else {
		perror("Unable to open file\n");
	}
	return 0;
}
