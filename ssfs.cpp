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

  std::string disk_op_threads[num_disk_op_extra];

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
    std::string input;
    cin >> input;

    if(input.compare(0, 7, "CREATE ") == 0 && input.size() > 7){
      create(input.substr(7, input.size()));
    } else if(input.compare(0, 7, "IMPORT ") == 0 && input.size() > 7){
      import(input.substr(7, input.size()));
    } else if(input.compare(0, 4, "CAT ") == 0 && input.size() > 4){
      cat(input.substr(4, input.size()));
    } else if(input.compare(0, 6, "WRITE ") == 0 && input.size() > 6){
      /* NEED MORE HERE FOR FORMATTING FOR EXTRA INPUT */
      write(input.substr(6, input.size()));
    } else if(input.compare(0, 7, "DELETE ") == 0 && input.size() > 7){
      f_delete(input.substr(7, input.size()));
    } else if(input.compare(0, 5, "READ ") == 0 && input.size() > 5){
      /* NEED MORE HERE FOR FORMATTING FOR EXTRA INPUT */
      read(input.substr(5, input.size()));
    } else if(input.compare(0, 4, "LIST") == 0){
      list_files();
    } else if(input.compare(0, 8, "SHUTDOWN") == 0){
      shutdown();
    }
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



void create(std::string file_name){
  

}


void import(std::string file_name){


}

void cat(std::string file_name){


}
