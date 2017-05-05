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

  std::cout << "Expected output: NO FILES " << std::endl; 
  list_files(holder);
  
  create(holder, "test1");
  create(holder, "test2");
  create(holder, "test3");
  
  std::cout << "Expected output: THREE FILE OF SIZE 0 " << std::endl; 
  std::cout << "Expected output: test1: 0 " << std::endl; 
  std::cout << "Expected output: test2: 0 " << std::endl; 
  std::cout << "Expected output: test3: 0 " << std::endl; 
  list_files(holder);

  delete_file(holder, "test1");
  delete_file(holder, "test2");
  delete_file(holder, "test3");
  list_files(holder);
  std::cout << "Expected output: NO FILES " << std::endl; 

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
