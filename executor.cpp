#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

void read_file(std::ifstream &file, std::vector<unsigned char> &buffer){
    std::streamsize size = file.tellg();
    file.seekg(0,std::ios::beg);
    buffer.resize(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
}

void print_memory(std::vector<unsigned char> memory){
    for(auto& byte : memory)
            std::cout << std::hex          
                << std::setw(2)      
                << std::setfill('0') 
                << static_cast<int>(byte) 
                << " ";
}

class Instruction{
    public:
        unsigned char opcode;
        int arg;
};

void decode_execute(Instruction inst, std::vector<unsigned char> &mem,int &acc, int &pc, bool &n, bool &z){
    switch(inst.opcode){
        case 0x80:
            pc = (int)inst.arg;
            break;
        case 0x10:
            mem[inst.arg] = acc;
            break;
        case 0x20:
            acc = mem[inst.arg];
            break;
        case 0x30:
            acc += mem[inst.arg];
            break;
        case 0x50:
            acc = acc | mem[inst.arg];
            break;
        case 0x60:
            acc = acc & mem[inst.arg];
            break;
        case 0x70:
            acc = ~acc;
            n != n;
            break;
        case 0x90:
            if(n) pc=inst.arg;
            break;
        case 0xA0:
            if(z) pc=inst.arg;
            break;
    }
    z = acc == 0;
}

int main(int argc, char* argv[]){
    /* FASE DE CARREGAMENTO */
    std::ifstream program(argv[1], std::ios::binary | std::ios::ate);
    std::vector<unsigned char> simulated_mem;
    int acc = 0, pc = 0;
    bool n = false, z=true;

    std::vector<Instruction> instructions;
    read_file(program,simulated_mem);
    
    /* FASE DE BUSCA */
    int index = 0;
    Instruction inst;
    for(auto& byte : simulated_mem){
        if((index++)%2 == 0){
            if(byte == 0xF0) break;
            inst.opcode = byte;
        }else{
            inst.arg = (int)byte;
            instructions.push_back(inst);
        } 
    }

    /* FASE DECODE E EXECUTE */
    // std::cout << "============ ANTES =============" << std::endl;
    // print_memory(simulated_mem);

    std::cout << std::hex          
                << std::setw(2)      
                << std::setfill('0') 
                << static_cast<int>(instructions[19].arg)  
                << std::endl;

    // while(pc < simulated_mem.size()){
    //     if(instructions[pc].opcode == 0xF0) break;
    //     std::cout << std::dec << pc << std::endl;
    //     decode_execute(instructions[pc], simulated_mem, acc, pc, n, z);
    //     pc++;
    // }

    // std::cout << "============ DEPOIS =============" << std::endl;
    // print_memory(simulated_mem);

    return EXIT_SUCCESS;
}