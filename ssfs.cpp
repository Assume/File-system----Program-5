#include <iostream>
#include <fstream>
#include <string>


int main(int argc, char * argv[]){
   // SANITIZE INPUT 1,2,3,4 text files and at least write seudocode for creating threads

  if(argc == 1){
    std::cout << "No disk file name passed. Exiting." << std::endl;
    return -1;
  }

  if(argc > 6){
    std::cout  << "Invalid number of arguments passed. Exiting." std::endl;
    return -1;
  }
  
  std::string disk_file_name = argv[0];
  std::ofstream disk_file;
  
  disk_file.open(disk_file_name);

  //Verify disk file exists
  if(!disk_file.is_open()){
    std::cout << "Disk file does not exist. Exiting." << std::endl;
    return -1;
  }
  
  int num_disk_op_extra = argc - 2;

  std::string * disk_op_threads = new std::string[num_disk_op_extra];

  for(int i = 0; i < num_disk_op_extra; i++)
    disk_op_threads[i] = argv[i+2];

  //Verify that all passed disk op names are valid
  if(!all_disk_op_valid(disk_op_threads, num_disk_op_extra)){
    std::cout << "One or more disk op file names are invalid. Exiting." << std::endl;
    return -1;
  }
    
  
  /*
    Wait for commands from the user from command line.
 */
  while(true){



  }

  return 0;
}


bool all_disk_op_valid(std::string * disk_ops, int disk_op_array_size){
  for(int i = 0; i < disk_op_array_size; i++){
    std::ifstream temp_stream;
    temp_stream.open(disk_ops[i]);
    if(!temp_stream.is_open())
      return false;
  }
  return true;

}
