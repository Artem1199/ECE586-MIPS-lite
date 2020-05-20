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

    // LDW R1 0(R2) 
    // ADD R3, R1, R5  *2 stalls

    // LDW R1 0(R2)
    // SUB R10 R11 R12
    // ADD R3, R1, R5  *1 stalls 

    
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
    // four conditon, i type instruction, i type instruction, or HALT
    if ((operation == ADD) or (operation == SUB) or (operation == MUL) or (operation == OR) or (operation == AND) or (operation == XOR)) {
        r_instruction = true;
        i_instruction = false;
    } else if ((operation == ADDI) or (operation == SUBI) or (operation == MULI) or (operation == ORI) or (operation == ANDI) or (operation == XORI) or (operation == LDW) or (operation == STW) or (operation == BZ) or  (operation == BEQ) or (operation == JR) or (operation == HALT)) {
        i_instruction = true;
        r_instruction = false;
    } else { // halt is confirmed
        i_instruction = true;
        r_instruction = false;
        halt_flag = 1;
    }
};

string Instruction::stringify(){


    string a;
    switch (operation)
    {
    case ADD:a = "ADD";break;
    case SUB:a = "SUB";break;
    case MUL:a = "MUL";break;
    case OR:a = "OR";break;
    case AND:a = "AND";break;
    case XOR:a = "OR";break;
    case ADDI:a = "ADDI";break;
    case SUBI:a = "SUBI";break;
    case MULI:a = "MULI";break;
    case ORI:a = "ORI";break;
    case ANDI:a = "ANDI";break;
    case XORI:a = "XORI";break;
    case LDW:a = "LDW";break;
    case JR:a = "JR";break; 
    case BEQ:a = "BEQ";break;
    case BZ:a = "BZ";break;
    case NOP:a = "NOP";break;
    default: a = "ERR";
        break;
    }

    a += " R"
    a += to_string()


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
    string inst_array[5];
    string stage_in[5] = 0;
    string stringstage_out[5] = 0;
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
    // Condition 1: check if PC count is out side mem range and provide a halt
    // Condition 2: contine if PC cound is within Memory range
        // Task: bitwise shit to get instruction from memory 
    inst_array[ID] = Instruction(PC);

    stage_out[IF] = memory_image[PC];

    PC + PC_OFFSET;

    return;
}

void Pipeline::ID_stage(){

    inst_array[ID].decode(stage_in[ID]);

}

void Pipeline::EX_stage(){
    int halt_flag = 0;
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
        // Add check for PC

        break;
    case BEQ:
        if (Reg[inst_array[EX].rs] == Reg[inst_array[EX].rt])
        {
            stage_out[EX] = inst_array[EX].immediate;
        } else {
            stage_out[EX] = 0;
        }
        // Add check for PC 

        break;
    case JR:
        stage_out[EX] = Reg[inst_array[EX].rs];
        break;

    default:
        // if not on of opcodes then HALT is equal to true '1'
        halt_flag = 1;
    }
    return halt_flag;    // need to figure this out
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


/** int main()

{

    Instruction a(0x3142FFFF);  // operation: 12 Rs: 10 Rt: 2 Immediate: -1
     a = Instruction(0xFF853000);
    
    // Pipeline mips();

    // mips.ID_stage;

    a.print();
            return 0;
 **/