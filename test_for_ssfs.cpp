#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "disk.h"
#include "operations.h"

int main(int argc, char * argv[]){

	std::string file_name;
	if(argc > 1){
		file_name = argv[1];
	} else {
		file_name = "DISK";
	}

	file_data_holder holder;

	read_in_super_block(file_name, holder);
	read_in_inode_bitmap(file_name, holder);
	read_in_data_bitmap(file_name, holder);
	read_in_all_inodes(file_name, holder);

	memcpy(holder.disk_name, file_name.c_str(), sizeof(file_name) + 1);

	/*
	//LIST
	std::cout << "Expected output: NO FILES " << std::endl; 
	list_files(holder);


	//CREATE
	message c1;
	message c2;
	message c3;
	c1.cmd = "WRITE";
	c2.cmd = "WRITE";
	c3.cmd = "WRITE";
	c1.fname = "test1";
	c2.fname = "test2";
	c3.fname = "test3";
	create(holder, c1);
	create(holder, c2);
	create(holder, c3);
	std::cout << "Expected output: THREE FILE OF SIZE 0 " << std::endl; 
	std::cout << "Expected output: test1: 0 " << std::endl; 
	std::cout << "Expected output: test2: 0 " << std::endl; 
	std::cout << "Expected output: test3: 0 " << std::endl; 
	list_files(holder);

	//DELETE
	delete_file(holder, c1);
	delete_file(holder, c2);
	delete_file(holder, c3);
	std::cout << "Expected output: NO FILES " << std::endl; 
	list_files(holder);

	//WRITE and READ
	message wr1;
	message wr2;
	message wr3;
	message wr4;

	wr1.cmd = "WRITE";
	wr1.fname = "test1";
	wr1.letter = 'a';
	wr1.start = 0;
	wr1.bytes = 50;

	wr4.cmd = "READ";
	wr4.fname = "test1";
	wr4.start = 10;
	wr4.bytes = 38;

	wr2.cmd = "WRITE";
	wr2.fname = "test1";
	wr2.letter = 'b';
	wr2.start = 20;
	wr2.bytes = 19;

	wr3.cmd = "READ";
	wr3.fname = "test1";
	wr3.start = 0;
	wr3.bytes = 50;

	create(holder, wr1);
	create(holder, wr2);
	create(holder, wr3);
	create(holder, wr4);

	//small write and small read
	write(holder, wr1);
	read(holder, wr4);
	write(holder, wr2);
	read(holder, wr3);

	list_files(holder);
	//med write and read
	*/
	message wr5;
	message wr6;
	message wr7;
	message wr8;

	wr5.cmd = "WRITE";
	wr5.fname = "test1";
	wr5.letter = 'a';
	wr5.start = 0;
	wr5.bytes = 2 * holder.s_block->block_size;

	wr6.cmd = "READ";
	wr6.fname = "test1";
	wr6.start = 100;
	wr6.bytes = holder.s_block->block_size;

	wr7.cmd = "WRITE";
	wr7.fname = "test1";
	wr7.letter = 'b';
	wr7.start = 100;
	wr7.bytes = holder.s_block->block_size;

	wr8.cmd = "READ";
	wr8.fname = "test1";
	wr8.start = 0;
	wr8.bytes = 2 * holder.s_block->block_size;

	create(holder, wr5);
	create(holder, wr6);
	create(holder, wr7);
	create(holder, wr8);

	write(holder, wr5);
	//read(holder, wr6);
	write(holder, wr7);
	read(holder, wr8);
	list_files(holder);
/*
	//IMPORT
	//small import
	message i1;
	i1.cmd = "IMPORT";
	i1.fname = "import1";
	i1.l_fname = "import_s";

	//medium import
	message i2;
	i2.cmd = "IMPORT";
	i2.fname = "import2";
	i2.l_fname = "import_m";
	import(holder, i2);
	list_files(holder);

	//large import
	message i3;
	i3.cmd = "IMPORT";
	i3.fname = "import3";
	i3.l_fname = "import_m";
	import(holder, i3)
	list_files(holder);

	//XL import
	message i4;
	i4.cmd = "IMPORT";
	i4.fname = "import4";
	i4.l_fname = "import_m";
	import(holder, i4)
	list_files(holder);

	//CAT
	c1.name = "cat1";
	c1.start = 0;
	c1.bytes = 0;
	c1.letter = 'a';

	c2.name = "cat2";
	c2.start = 0;
	c2.bytes = 0;
	c2.letter = 'b';

	c3.name = "cat3";
	c3.start = 0;
	c3.bytes = 0;
	c3.letter = 'c';

	message c4;
	c4.cmd = "CREATE";
	c4.name = "cat4";
	c4.start = 0;
	c4.bytes = 0;
	c4.letter = 'd';

	//small cat
	message ca1;
	ca1.cmd = "CAT";
	ca1.fname = "cat1";
	cat(holder, ca1);

	//med cat
	message ca2;
	ca2.cmd = "CAT";
	ca2.name = "cat2";
	cat(holder, ca2);

	//large cat
	message ca3;
	ca3.cmd = "CAT";
	ca3.name = "cat3";
	cat(holder, ca3);

	//xl cat
	message ca4;
	ca4.cmd = "CAT";
	ca4.name = "cat4";
	cat(holder, ca4);
	*/
	//SHUTDOWN



	//std::cout << holder.s_block -> block_size << " :: " << holder.s_block -> num_blocks << " :: " << holder.s_block -> db_ptr << " :: " << holder.s_block -> inode_ptr << " :: " << holder.s_block -> db_blocks << std::endl;

	/*std::cout << "INODE BITMAP" << std::endl;
	  for(int i = 0; i < 256; i++)
	  std::cout << i << "inode value: " << holder.inode_bitmap[i] << std::endl;
	  std::cout << "END INODE BITMAP" << std::endl;

	  std::cout << "DATA BITMAP" << std::endl;
	  for(int i = 0; i < holder.s_block -> db_blocks; i++)
	  std::cout << i << "data bitmap value: " << holder.data_bitmap[i] << std::endl;
	  std::cout << "END DATA BITMAP" << std::endl;

	  std::cout << "ALL INODES" << std::endl;
	  for(int i = 0; i < 256; i++)
	  std::cout << "all inodes(" << i << "): " << holder.all_inodes[i].file_name << " :: " << holder.all_inodes[i].dib_ptr << std::endl; 
	 */
}
