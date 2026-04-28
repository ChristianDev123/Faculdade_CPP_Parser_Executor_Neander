#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <cstdint>

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

class Command {
    public:
        int opcode;
        int arg = 0;
};

std::string string_to_upper(std::string s){
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

std::vector<Symbol> symbol_table(std::vector<std::string> code){
    std::vector<Symbol> response;
    int 
        data_index = 0,
        mem_add = 255;
    /* Encontrando Diretiva de Dados*/
    for(auto&line : code){
        if(string_to_upper(line).find(".DATA") != std::string::npos) break;
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
        s.size = string_to_upper(match[4].str()) == "SPACE" ? std::stoi(match[6].str()) : 1;
        s.value = string_to_upper(match[4].str()) == "DATA" ? std::stoi(match[6].str()) : 0; 
        mem_add -= s.size;
        response.push_back(s);
    }
    return response;
}

std::unordered_map<string, int> enum_operations {
    {"NOP", 0x0},
    {"STA", 0x10},
    {"LDA", 0x20},
    {"ADD", 0x30},
    {"OR", 0x50},
    {"AND", 0x60},
    {"NOT", 0x70},
    {"JMP", 0x80},
    {"JN", 0x90},
    {"JZ", 0xA0},
    {"HLT", 0xF0}
};

int sum_addrs(std::string s){
    int response;
    char opr;
    std::smatch match;
    if(std::regex_search(s, match, std::regex(R"(((\d+)(\s*(\+|\-))?\s*)+)"))){
        response = std::stoi(match[1].str());
        for(int i = 2; i < match.size(); i++){
            if(i%2 == 0){
                opr = match[i].str()[0];
                continue;
            }
            response = opr == '+' ? 
                response+std::stoi(match[i].str()): 
                response-std::stoi(match[i].str());
        }
    }
    return response;
}

std::vector<Command> translate_text(std::vector<std::string> code, std::vector<Symbol> symbol_table){
    std::vector<Command> response;
    int data_index = 0;
    
    for(auto& line : code){
        if(string_to_upper(line).find(".TEXT") != std::string::npos) break;
        data_index++;
    }
    
    // ^\s*(\w+) -> pega o comando
    // \s+ -> obriga espaço
    // (\[(.*)\]|([\w\d]+) -> aceita como argumento uma palavra/número ou estrutura [abc...]
    // /s*$ -> obriga espaço ou nada ao fim do comando
    std::regex pattern0(R"(^\s*(\w+)(?:\s+(?:\[(.*)\]|([\w\d]+)))?\s*$)");
    std::regex pattern1(R"((^\s*)(\w+:)(\s*$))");

    std::smatch match, cmd_match;
    std::vector<std::string> cmd_arg_implicit = {"NOT","NOP","HLT"};
    
    data_index++;
    
    for(int i = data_index; i < code.size(); i++){
        if(std::regex_search(code[i], std::regex(R"(\.([a-z,A-Z]*))"))) /* valida seção de texto do código*/
            break;
        if(std::regex_replace(code[i], std::regex(R"(\s)"),"").empty()) /* valida linha vazia */
            continue;
        
        Command c;
        if(std::regex_search(code[i],pattern1)){// em caso de label aponta para o proximo endereço de memória
            c.opcode = enum_operations["NOP"];
            response.push_back(c);
            continue;
        }
        
        if(!std::regex_search(code[i], match, pattern0)){ // verifica se os commandos estão seguindo um padrão esperado
            std::cerr << "[ERRO] Falha ao tentar ler o código em .text; código: " << code[i] << " linha:  " << i << std::endl;
            continue;
        }
        
        if(string_to_upper(match[1].str()).find("ORG") != std::string::npos){ // informado o início do algoritmo
            c.opcode = enum_operations["JMP"];
            c.arg = std::stoi(match[3].str());
            response.push_back(c);
            continue;
        }
        
        c.opcode = enum_operations[string_to_upper(match[1].str())];
        if(std::find(cmd_arg_implicit.begin(), cmd_arg_implicit.end(), string_to_upper(match[1].str())) == cmd_arg_implicit.end()){ // caso de comandos com argumentos explicitos
            if(match[3].matched){// argumento sem a estrutura [abc..]
                for(auto& sym : symbol_table){
                    if(sym.name == match[3].str()){
                        c.arg = sym.mem_add;
                        break;
                    }
                }
            }else if(match[2].matched){// argumento com a estrutura [abc..]
                std::string calc = match[3];
                for(auto& sym : symbol_table){
                    size_t pos = match[3].str().find(sym.name);
                    if(pos != std::string::npos){
                        calc.replace(pos,sym.name.size(),std::to_string(sym.mem_add));
                        break;
                    }
                }                
                c.arg = sum_addrs(calc);
            }
        }
        response.push_back(c);
    }
    return response;
}

int main(int argc, char*argv[]){
    std::vector<std::string> buffer_code;
    std::vector<Symbol> sym_table;
    std::ifstream source_code("code.asm");
    std::ofstream output_exec(argv[1], std::ios::out | std::ios::binary);// as flags informam que queremos salvar os binários do arquivo e não um texto
    std::vector<Command> translated_cmd_list;
    
    unsigned char memory[256];
    for(int i = 0; i < 256; i++)
        memory[i] = 0x00;

    if(!source_code) return EXIT_FAILURE;
    read_file(source_code, buffer_code);
    source_code.close();
    
    sym_table = symbol_table(buffer_code);
    translated_cmd_list = translate_text(buffer_code, sym_table);
    
    int i = 0;
    for(auto& cmd : translated_cmd_list){
        unsigned char op = static_cast<uint8_t>(cmd.opcode);
        unsigned char arg = static_cast<uint8_t>(cmd.arg);
        memory[i++]=op;
        memory[i++]=arg;
    }
    for(auto& data : sym_table){
        memory[data.mem_add] = data.value;
    }
    
    if(output_exec.is_open()){
        output_exec << std::setfill('0');
        output_exec.write(reinterpret_cast<const char*>(memory), sizeof(memory));
        output_exec.close();
    }
    return EXIT_SUCCESS;
}