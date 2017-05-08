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
bool write(file_data_holder &, message);
void write_disk(file_data_holder &, int, int, void *, char c = 'C');
void write_disk_char(file_data_holder &, int, int, char);
void read_disk_char(file_data_holder &, int, int);
void write_data(file_data_holder &, int, message);
void write_file(file_data_holder &, int, int, int, int, char);
void write_disk_int(file_data_holder &, int , int);
void append(file_data_holder &, int, message);
void add_block(int, int, int, file_data_holder &);
void add_id_blk(file_data_holder &, int, int, int);
void add_did_blk(file_data_holder &, int, int, int);
bool read(file_data_holder &, message);
void read_data(file_data_holder &, int, message);
void read_file(file_data_holder &, int, int, int, int);
int read_disk_int(file_data_holder &, int);
bool create(file_data_holder &, message);
int create(file_data_holder &, std::string);
bool import(file_data_holder &, message);
void import_data(file_data_holder &, int, message);
void import_file(file_data_holder &, int, int, int, int, std::string, int);
void import_disk_char(file_data_holder &, int, int, std::string, int);
bool cat(file_data_holder &, message);
bool delete_file(file_data_holder &, message);
void list_files(file_data_holder &);
void shutdown(file_data_holder &);
int get_id_blk(file_data_holder &, int, int);
int get_unused_data_block(file_data_holder &);
int get_did_blk(file_data_holder &, int, int);

//reading from disk
int get_starting_offset(file_data_holder &);
int get_free_data_block(file_data_holder &);
int get_free_inode(file_data_holder &);
int get_inode_for_file_name(file_data_holder &, std::string);

std::vector<std::string> split_string_by_space(std::string);


#endif
