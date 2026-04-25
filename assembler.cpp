#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>


using namespace std;

void read_file(std::ifstream &file, std::vector<std::string> &buffer){
    std::string line;
    while(std::getline(file, line))
        buffer.push_back(line);
}

class Symbol {
    public:
        std::string name;
        int mem_add;
        int size = 1;
        int value = 0;
};

std::vector<Symbol> symbol_table(std::vector<std::string> code){
    std::vector<Symbol> response;
    int 
        data_index = 0,
        mem_add = 255;
    /* Encontrando Diretiva de Dados*/
    for(auto&line : code){
        if(line.find(".DATA") || line.find(".data")) break;
        data_index++;
    }
    
    std::regex pattern(R"((\s*)(\w+)(\s+)(\w+)(\s+)(\d+)(\s*))");
    std::smatch match;

    while(!std::regex_search(code[++data_index], std::regex(R"(\.([a-z,A-Z]*))")) && data_index < code.size()){
        /* MATCH serve como um buffer de cada encontro das diretivas do regex, match[0] string com ocorrencia do regex */
        if(!std::regex_search(code[data_index], match, pattern))
            std::cerr << "[ERRO] Falha encontrada ao verificar a estrutura de diretiva!";

        Symbol s;
        s.name = match[2];
        s.mem_add = mem_add;
        s.size = match[4].str() == "SPACE" || match[4].str() == "space" ? std::stoi(match[6].str()) : 1;
        s.value = match[4].str() == "DATA" || match[4].str() == "data" ? std::stoi(match[6].str()) : 0; 
        mem_add -= s.size;
        response.push_back(s);
    }
    return response;
}


std::unordered_map<string, int> enum_operations {
    {"NOP", 0},
    {"STA", 10},
    {"LDA", 20},
    {"ADD", 30},
    {"OR", 50},
    {"AND", 60},
    {"NOT", 70},
    {"JMP", 80},
    {"JN", 90},
    {"JZ", 0xA0},
    {"HLT", 0xF0}
};

int main(){
    std::vector<std::string> buffer_code;
    std::vector<Symbol> sym_table;
    std::ifstream source_code("code.asm");
    if(!source_code) return EXIT_FAILURE;
    read_file(source_code, buffer_code);
    source_code.close();
    
    sym_table = symbol_table(buffer_code);

    
    return EXIT_SUCCESS;
}