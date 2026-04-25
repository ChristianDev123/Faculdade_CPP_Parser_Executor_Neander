#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void read_file(std::ifstream &file, std::vector<std::string> &buffer){
    std::string line;
    while(std::getline(file, line))
        buffer.push_back(line);
}


int main(){
    std::vector<std::string> buffer_code;
    std::ifstream source_code("code.asm");
    if(!source_code) return EXIT_FAILURE;
    read_file(source_code, buffer_code);
    source_code.close();

    for(auto& line : buffer_code)
        std::cout << line << std::endl;
    
    return EXIT_SUCCESS;
}