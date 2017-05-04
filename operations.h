#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "disk.h"

void read_in_super_block(std::string file_name, &file_data_holder holder);
void read_in_inode_bitmap(std::string file_name, &file_data_holder holder);
void read_in_data_bitmap(std::string file_name, &file_data_holder holder);
void read_in_all_inodes(std::string file_name, &file_data_holder holder);
void write(std::vector<std::string> vec);
void read(std::vector<std::string> vec);
void create(std::string file_name);
void import(std::vector<std::string> vec);
void cat(std::string file_name);
void f_delete(std::string file_name);
void list_files();

#endif
