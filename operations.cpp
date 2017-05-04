#include "operations.h"
#include <string>
#include <vector>





void operations::write(std::vector<std::string> vec){
  std::string command = vec[0];
  std::string file_name = vec[1];
  char char_to_write = vec[2].c_str()[0];
  char start_byte = vec[3].c_str()[0];
  int num__bytes = std::atoi(vec[4].c_str());

}



void operations::read(std::vector<std::string> vec){
  std::string ssfs_file_name = vec[1];
  char start_byte = vec[2].c_str()[0];
  int num_bytes = std::atoi(vec[3].c_str());

}


void operations::create(std::string file_name){


}


void operations::import(std::vector<std::string> vec){
  std::string ssfs_file_name = vec[0];
  std::string unix_file_name = vec[1];



}

void operations::cat(std::string file_name){


}

void operations::f_delete(std::string file_name){


}

void operations::list_files(){


}


void operations::read_in_super_block(std::string file_name, &file_data_holder holder){

  FILE * temp_file;

  temp_file = fopen(file_name.c_str(), "rb");

  char * super_b;

  fread(super_b, 1, sizeof(super_block), temp_file);

  holder.s_block = (super_block *) super_b;
  fclose(temp_file);
}


void operations::read_in_inode_bitmap(std::string file_name, &file_data_holder holder){

  FILE * temp_file;

  temp_file = fopen(file_name.c_str(), "rb");

  fseek(temp_file, s_block -> block_size, SEEK_SET);

  char * c_inode_bitmap;

  fread(c_inode_bitmap, 1, sizeof(inode), temp_file);

  holder.inode_bitmap = (inode *) c_inode_bitmap;
  fclose(temp_file);

}

void operations::read_in_data_bitmap(std::string file_name, &file_data_holder holder){

  FILE * temp_file;

  temp_file = fopen(file_name.c_str(), "rb");

  fseek(temp_file, s_block -> block_size * 2, SEEK_SET);

  char * c_data_bitmap;

  fread(c_data_bitmap, 1, sizeof(inode), temp_file);

  holder.data_bitmap = (int *) c_data_bitmap;
  fclose(temp_file);

}


void operations::read_in_all_inodes(std::string file_name, &file_data_holder holder){
  FILE * temp_file;

  temp_file = fopen(file_name.c_str(), "rb");
  fseek(temp_file, s_block -> block_size * 3, SEEK_SET);

  char * c_inode;

  for(int i = 0; i < 256; i++){
    fread(c_inode, 1, sizeof(inode), temp_file);

    holder.all_inodes[i] = (inode *) c_inode;

    fseek(temp_file, s_block -> block_size * 3 + (sizeof(inode) * i + 1), SEEK_SET);
  }

  fclose(temp_file);
}








