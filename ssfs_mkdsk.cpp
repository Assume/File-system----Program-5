#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>     /* atoi */

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
	if(num_blocks < 128 || num_blocks > 512 || block_size < 128 || block_size > 512){
		perror("Block size has to be less than 512 bytes or greater than 128 bytes");
	}
	if(num_blocks * block_size < 1024 || num_blocks * block_size > 131072){
		perror("Block size has to be less than 1024 bytes or greater than 131072 bytes (128KB)");
	}
	if((num_blocks & (num_blocks - 1) != 0 ) || (block_size & (block_size -1) != 0)){
		perror("Block size and the number of blocks have to be a power of 2");
	}


	//Create File

	FILE * pFile;
	pFile = fopen (disk_name.c_str(),"w");
	if (pFile!=NULL)
	{
		fseek(pFile, num_blocks * block_size -1, 0);
		char test_char = '\0';
		fwrite(&test_char, 1, sizeof(test_char), pFile);
		fclose (pFile);
	} else {
		perror("Unable to open file\n");
	}
	return 0;
}
