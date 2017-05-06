#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>     /* atoi */

#include "disk.h"
#include "operations.h"

int main(int argc, char * argv[]){

	std::string file_name = "DISK";

	file_data_holder holder;

	read_in_super_block(file_name, holder);
	read_in_inode_bitmap(file_name, holder);
	read_in_data_bitmap(file_name, holder);
	read_in_all_inodes(file_name, holder);

	memcpy(holder.disk_name, file_name.c_str(), sizeof(file_name));

	/*
	//LIST
	std::cout << "Expected output: NO FILES " << std::endl; 
	list_files(holder);


	//CREATE
	create(holder, "test1");
	create(holder, "test2");
	create(holder, "test3");
	std::cout << "Expected output: THREE FILE OF SIZE 0 " << std::endl; 
	std::cout << "Expected output: test1: 0 " << std::endl; 
	std::cout << "Expected output: test2: 0 " << std::endl; 
	std::cout << "Expected output: test3: 0 " << std::endl; 
	list_files(holder);

	//DELETE
	delete_file(holder, "test1");
	delete_file(holder, "test2");
	delete_file(holder, "test3");
	dstd::cout << "Expected output: NO FILES " << std::endl; 
	list_files(holder);
	*/

	message a1;
	a1.cmd = "CREATE";
	//strcpy(a1.fname.c_str(), "test1");
	//message a2;
	//strcpy(a2.cmd.c_str(), "CREATE");
	//strcpy(a2.fname.c_str(), "test2");
	//message a3;
	//strcpy(a3.cmd.c_str(), "CREATE");
	//strcpy(a3.fname.c_str(), "test3");


	//WRITE
	create(holder, a1);
	create(holder, a2);
	create(holder, a3);

	//creating messages
	message m1;
	strcpy(m1.cmd, "WRITE");
	strcpy(m1.fname, "test1");
	m1.letter = 'a';
	m1.start = 50;
	m1.bytes = 19;
	message m2;
	strcpy(m2.cmd, "READ");
	strcpy(m2.fname, "test1");
	m2.start = 25;
	m2.bytes = 50;

	//small write
	write(holder, m1);
	read(holder, m2);

	list_files(holder);
	
	/*
	//creating messages
	m1.start = 50;
	m1.bytes = 2 * holder.s_block.block_size;
	m1.letter = 'b';
	m2.start = 0;
	m2.bytes = 50 + 2 * holder.s_block.block_size;

	//large write
	write(holder, m1);
	read(holder, m2);

	//READ
	strcpy(m1.cmd, "READ");
	m1.letter = -1;
	m1.start = 40;
	m1.bytes = 39;
	strcpy(m2.fname, "test2");
	m2.start = 75;
	m2.bytes = holder.s_block.block_size;
	//small read
	read(holder, m1);
	//medium read
	read(holder, m2);

	//IMPORT
	//small file
	import(holder, "import_s")
	//medium file
	import(holder, "import_m")
	//large file
	import(holder, "import_l")
	//XL file
	import(holder, "import_xl")

	shutdown(holder);
	*/



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
