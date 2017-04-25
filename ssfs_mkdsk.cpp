#include <iostream>
#include <fstream>
#include <string>



int main(int argc, char * argv[]){
	// sanitize input => make sure input is what instructions specify
	// no more or less than three args, unless instructions et default val
	// make sure argv[1], argv[2] are numbers
	int num_blocks = 0;
	int block_size = 0;
	std::string disk_name;
	num_blocks = atoi(argv[1]);// 0 if argv is not a number
	block_size = atoi(argv[2]);// 0 if argv is not a number
	if(argc > 3){
		disk_name = argv[3];
	} else {
		disk_name = "DISK";
	}
	
	//Create File

	std::ofstream myfile;
	myfile.open ("filename.txt");               
	myfile << "Writing this to a file.\n";
	myfile << "000" << std::endl;            
	myfile.close();
	return 0;
}
