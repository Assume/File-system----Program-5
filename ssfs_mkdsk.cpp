#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include "disk.h"
#include <fstream>
#include <math.h>
#include <time.h>
#include "operations.h"

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
		perror("Block size has to be less than or equal to 512 bytes or greater than or equal to 128 bytes");
		return -1;
	}
	if(num_blocks < 1024 || num_blocks > 131072){
		perror("Number of blocks has to be greater than or equal to 1024 bytes and less than or equal to 131072 bytes (128KB)");
		return -1;
	}
	if((num_blocks & (num_blocks - 1) != 0 ) || (block_size & (block_size -1) != 0)){
		perror("Block size and the number of blocks have to be a power of 2");
		return -1;
	}

	//CALCULATE ADDRESSES
	int inode_start = 2 + ((sizeof(inode) * 256)/block_size) + 1;
	int data_block_start = inode_start + 256;
	int num_data_blocks = num_blocks - data_block_start;


	//dummy values to write to disk
	
	int inodes[256];
	srand(time(NULL));
	for(int i = 0; i < 256; i++)
	  inodes[i] = rand() % 2;
	int db[num_data_blocks];

	for(int i = 0; i < num_data_blocks; i++)
	  db[i] = rand() % 2;
	
	inode in;


	in.file_name[0] = 't';
	in.file_name[1] = 'e';
	in.file_name[2] = 's';
	in.file_name[3] = 't';
	in.file_name[4] = '\0';
	
	in.file_name = t_char;
	in.dib_ptr = 20;

	inode real_inodes[256];

	for(int i = 0; i < 256; i++)
	  real_inodes[i] = in;

	
	//CREATE FILE
	FILE * pFile;
	pFile = fopen (disk_name.c_str(),"wb");
	if (pFile != NULL)
	{
		//seek to the end of the file and writes a character so the file is the correct size
		fseek(pFile, num_blocks * block_size - 1, 0);
		char test_char = '\0';
		fwrite(&test_char, 1, sizeof(test_char), pFile);
		
		//creating superblock for file system
		super_block sb(block_size, num_blocks, data_block_start, inode_start, num_data_blocks);


		char * sb_char = (char *) & sb;
		
		//seeks back to beginning of the file
		fseek(pFile, (-1 * num_blocks * block_size), SEEK_END);
		
		//write superblock byte by byte
		fwrite(&sb, 1, sizeof(sb), pFile);
		
		
		//seek to next block after super block
		fseek(pFile, block_size, SEEK_SET);

		//write inode bitmap to second block
		fwrite(&inodes, 1, sizeof(inodes), pFile);

		//seek to next block -- possibly unnessecary due to write seeking num bytes written
		//but depends on the size of what was written
		fseek(pFile, block_size + sizeof(inodes), SEEK_SET);

		//write data bitmap
		fwrite(&db, 1, sizeof(db), pFile);
		
		fseek(pFile, block_size + sizeof(inodes) + sizeof(db), SEEK_SET);

		fwrite(&real_inodes, 1, sizeof(real_inodes), pFile);
		/*for(int i = 0; i < 256; i++){
			fwrite(&in, 1, sizeof(in), pFile);
			//seek to the next inode spot not the next block
			fseek(pFile, block_size + sizeof(inodes) + sizeof(db) + (sizeof(inode) * i + 1), SEEK_SET);
		}*/
		
		fclose (pFile);
	       
	} else {
		perror("Unable to open file\n");
	}
	return 0;
}
