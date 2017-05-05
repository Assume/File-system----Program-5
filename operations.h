#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "disk.h"
#include <vector>

//reading data structures from the disk
void read_in_super_block(std::string, file_data_holder &);
void read_in_inode_bitmap(std::string, file_data_holder &);
void read_in_data_bitmap(std::string, file_data_holder  &);
void read_in_all_inodes(std::string, file_data_holder &);

bool all_disk_op_valid(std::string *, int);

//commands on disk
void write(std::vector<std::string>);
void read(std::vector<std::string>);
void create(std::string file_name);
void import(std::vector<std::string>);
void cat(std::string);
void f_delete(std::string);
void list_files();

#endif
