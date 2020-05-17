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

#define PC_OFFSET 1;

unsigned int memory_image[5]{
    0x3142FFFF,
    0x00853000,
    0x00853000,
    0x00853000,
    0x00853000,
};

Instruction::Instruction (unsigned int a){
    address = a;
    operation = NOP;
    rs = 0;
    rt = 0;
    rd = 0;
    immediate = 0;
};

void Instruction::decode (signed int hexin){

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

string Instruction::stringify(){

    switch (operation)
    {
    case ADD:
        "ADD";
        break;
    case SUB:
        "SUB";
        break;
    case MUL:
        "MUL";
        break;
    case OR:
        "OR";
        break;
    case AND:
    case XOR:
        Reg[inst_array[WB].rd] = stage_in[WB];
        break;

    case ADDI:
    case SUBI:
    case MULI:
    case ORI:
    case ANDI:
    case XORI:
    case LDW:
        Reg[inst_array[WB].rt] = stage_in[WB];
        break;

    case JR:
        PC = stage_in[WB];
        break; 

    case BEQ:
    case BZ:
        PC += stage_in[WB] * PC_OFFSET - 4 * PC_OFFSET;
        break;
        
    default:
        break;
    }
    }

void Instruction::print(){

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
};

Pipeline::Pipeline(){
    inst_array[5];
    stage_in[5] = 0;
    stage_out[5] = 0;
};

void Pipeline::clock(){

    // Simulate instruction moving to next stage after clock cycle
    inst_array[IF] = inst_array[WB];
    inst_array[ID] = inst_array[IF];
    inst_array[EX] = inst_array[ID];
    inst_array[MEM] = inst_array[EX];
    inst_array[WB] = inst_array[MEM];

    // Simulate data moving to next stage after clock cycle
    stage_in[IF] = stage_out[WB];
    stage_in[ID] = stage_out[IF];
    stage_in[EX] = stage_out[ID];
    stage_in[MEM] = stage_out[EX];
    stage_in[WB] = stage_out[MEM];
}

void Pipeline::IF_stage(){

    inst_array[ID] = Instruction(PC);

    stage_out[IF] = memory_image[PC];
    PC + PC_OFFSET;

    return;
}

void Pipeline::ID_stage(){

    inst_array[ID].decode(stage_in[ID]);

}

void Pipeline::EX_stage(){

    switch (inst_array[EX].operation)
    {
    case ADD:
        stage_out[EX] =  Reg[inst_array[EX].rs] + Reg[inst_array[EX].rt];
        break;
    case ADDI:
    case LDW:
    case STW:
        stage_out[EX] =  Reg[inst_array[EX].rs] + inst_array[EX].immediate;
        break;
    case SUB:
        stage_out[EX] =  Reg[inst_array[EX].rs] - Reg[inst_array[EX].rt];
        break;
    case SUBI:
        stage_out[EX] =  Reg[inst_array[EX].rs] - inst_array[EX].immediate;
        break;
    case MUL:
        stage_out[EX] =  Reg[inst_array[EX].rs] * Reg[inst_array[EX].rt];
        break;
    case MULI:
        stage_out[EX] =  Reg[inst_array[EX].rs] * inst_array[EX].immediate;
        break;
    
    case OR:
        stage_out[EX] = Reg[inst_array[EX].rs] | Reg[inst_array[EX].rt];
        break;
    case ORI:
        stage_out[EX] = Reg[inst_array[EX].rs] | inst_array[EX].immediate;
        break;
    case AND:
        stage_out[EX] = Reg[inst_array[EX].rs] & Reg[inst_array[EX].rt];
        break;
    case ANDI:
        stage_out[EX] = Reg[inst_array[EX].rs] & inst_array[EX].immediate;
        break;
    case XOR:
        stage_out[EX] = Reg[inst_array[EX].rs] ^ Reg[inst_array[EX].rt];
        break;
    case XORI:
        stage_out[EX] = Reg[inst_array[EX].rs] ^ inst_array[EX].immediate;
        break;

    case BZ:
        if (Reg[inst_array[EX].rs] == 0){
            stage_out[EX] = inst_array[EX].immediate;
        } else {
            stage_out[EX] = 0;
        }     
        break;
    case BEQ:
        if (Reg[inst_array[EX].rs] == Reg[inst_array[EX].rt])
        {
            stage_out[EX] = inst_array[EX].immediate;
        } else {
            stage_out[EX] = 0;
        }
        break;
    case JR:
        stage_out[EX] = Reg[inst_array[EX].rs];
        break;

    default:
        break;
    }
}

void Pipeline::MEM_stage(){

    switch (inst_array[MEM].operation)
    {
    case LDW:
        stage_out[MEM] = memory_image[stage_in[MEM]];
        break;
    case STW:
        memory_image[stage_in[MEM]] = Reg[inst_array[MEM].rt];
        break;
    
    default:
    // Pass data calculated in EX stage if not a load / store instruction
        stage_out[MEM] = stage_in[MEM];
        break;
    }
    
}

void Pipeline::WB_stage(){
    
    switch (inst_array[WB].operation)
    {
    case ADD:
    case SUB:
    case MUL:
    case OR:
    case AND:
    case XOR:
        Reg[inst_array[WB].rd] = stage_in[WB];
        break;

    case ADDI:
    case SUBI:
    case MULI:
    case ORI:
    case ANDI:
    case XORI:
    case LDW:
        Reg[inst_array[WB].rt] = stage_in[WB];
        break;

    case JR:
        PC = stage_in[WB];
        break; 

    case BEQ:
    case BZ:
        PC += stage_in[WB] * PC_OFFSET - 4 * PC_OFFSET;
        break;
        
    default:
        break;
    }
    
}

void Pipeline::visualization(){


    cout << "|------IF------|------ID------|------EX------|-----MEM------|------WB------|" << "\n";
    cout << "|--------------|--------------|--------------|--------------|--------------|" << "\n";
    cout << "| " << inst_array[IF].operation << " " << inst_array[IF].rs << " " << inst_array[IF].rt << " " << inst_array[IF].immediate;
    cout << "| ";

};


int main()

{

    Instruction a(0x3142FFFF);  // operation: 12 Rs: 10 Rt: 2 Immediate: -1
     a = Instruction(0xFF853000);
    

    Pipeline mips();

    mips.ID_stage;

    a.print();
            return 0;

}