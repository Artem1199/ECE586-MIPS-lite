#include <iostream>
#include <sstream>
#include <bitset>
#include <string>
#include "mips.h"

using namespace std;

unsigned int PC = 0;
signed int Reg[32] = {0};
#define IF 0
#define ID 1
#define EX 2
#define MEM 3
#define WB 4

unsigned int memory_image[5]{
    0x3142FFFF,
    0x00853000,
    0x00853000,
    0x00853000,
    0x00853000,
};

Instruction::Instruction (signed int hexin){

    signed int x;
    std::stringstream ss;
    ss << std:: hex << hexin;
    ss >> x;

    operation = static_cast<e_opCode>(x >> 26);
    rs = (x<<6)>>27;
    rt = (x<<11)>>27;
    rd = (x<<16)>>27;
    immediate = (x<<16)>>16;

    if ((operation == ADD) or (operation == SUB) or (operation == MUL) or (operation == OR) or (operation == AND) or (operation == XOR)) {
        r_instruction = true;
    } else {
        r_instruction = false;
    };
    
};

void Instruction::print_Instruction(){


    if (this -> r_instruction){
        cout << "operation: " << this -> operation << " ";
        cout << "Rs: " << this -> rs << " ";
        cout << "Rt: " << this -> rt << " ";
        cout << "Rd: " << this -> rd << " ";
    } else {
        cout << "operation: " << this -> operation << " ";
        cout << "Rs: " << this -> rs << " ";
        cout << "Rt: " << this -> rt << " ";
        cout << "Immediate: " << this -> immediate << " ";
    };

}

void Pipeline::IF_stage(){

    this -> stage_out[IF] = memory_image[PC];
    PC + 1;

    return;
}

void Pipeline::ID_stage(){

    Instruction a = Instruction(this -> stage_out[IF]);

    this -> inst_array[ID] = a;
}

void Pipeline::EX_stage(){

    switch (inst_array[EX].operation)
    {
    case ADD:
        stage_out[EX] =  inst_array[EX].rs + inst_array[EX].rt;
        break;
    case (ADDI  or LDW or STW):
        stage_out[EX] =  inst_array[EX].rs + inst_array[EX].immediate;
        break;
    case SUB:
        stage_out[EX] =  inst_array[EX].rs - inst_array[EX].rt;
        break;
    case SUBI:
        stage_out[EX] =  inst_array[EX].rs - inst_array[EX].immediate;
        break;
    case MUL:
        stage_out[EX] =  inst_array[EX].rs * inst_array[EX].rt;
        break;
    case MULI:
        stage_out[EX] =  inst_array[EX].rs * inst_array[EX].immediate;
        break;
    
    case OR:
        stage_out[EX] = inst_array[EX].rs | inst_array[EX].rt;
        break;
    case ORI:
        stage_out[EX] = inst_array[EX].rs | inst_array[EX].immediate;
        break;
    case AND:
        stage_out[EX] = inst_array[EX].rs & inst_array[EX].rt;
        break;
    case ANDI:
        stage_out[EX] = inst_array[EX].rs & inst_array[EX].immediate;
        break;
    case XOR:
        stage_out[EX] = inst_array[EX].rs ^ inst_array[EX].rt;
        break;
    case XORI:
        stage_out[EX] = inst_array[EX].rs ^ inst_array[EX].immediate;
        break;

    default:
        break;
    }
}

void Pipeline::MEM_stage(){
    
    


}

void Pipeline::WB_stage(){
    
    


}


int main()

{

    Instruction a(0x3142FFFF);  // operation: 12 Rs: 10 Rt: 2 Immediate: -1
     a = Instruction(0xFF853000);
    

    a.print_Instruction();
            return 0;

}