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
bool write(file_data_holder &, message &);
bool read(file_data_holder &, message &);
bool create(file_data_holder &, message &);
bool import(file_data_holder &, message &);
bool cat(file_data_holder &,message &);
bool delete_file(file_data_holder &, message &);
void list_files(file_data_holder &);
void shutdown(file_data_holder &);

//reading from disk
int get_starting_offset(file_data_holder &);
int get_free_data_block(file_data_holder &);
int get_free_inode(file_data_holder &);
int get_inode_for_file_name(file_data_holder &, std::string);

std::vector<std::string> split_string_by_space(std::string);

//shared memory
int add_shared_to(message &, void *);

#endif
