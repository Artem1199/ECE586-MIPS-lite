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

#define PC_OFFSET 1

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

Instruction::Instruction(){
    operation = NOP;
    rs = 0;
    rt = 0;
    immediate = 0;
    r_instruction = true;
    i_instruction = false;
    halt_flag = false;
}


void Instruction::decode (string hexin){

    signed int x;
    stringstream ss;
    ss << hex << hexin;
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

    if (r_instruction == true){
        a += " R";
        a += to_string(rd);
        a += " R";
        a += to_string(rs);
        a += " R";
        a += to_string(rt);

    } else {
        a += " R";
        a += to_string(rt);
        a += " #";
        a += to_string(immediate);
        a += "(R";
        a += to_string(rs);
        a += ")";
    }
    return a;

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
    Instruction inst_array[5];
    string stage_in[5];
    string stringstage_out[5];
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
    //Rule: Add, load, and store => Fetch the instruction and increment the program counter

    // Condition 1: check if PC count is out side mem range and provide a halt
    // Condition 2: contine if PC count is within Memory range
    // Task: bitwise shit to get instruction from memory 
    //inst_array[ID] = Instruction(PC);

    stage_out[IF] = memory_image[PC];

    PC + PC_OFFSET;

    return;
}

void Pipeline::ID_stage(){
//EX. ADD: ex: ADD R3, R4, R5; 
    // Decode the instruction in IR to determine the operation to 
    // be performed (add). Read the contents of registers R4 and R5 

//EX. LOAD: Load R5, X(R7) R5 <- [[R7]+ x
    //Decode the instruction in IR to determine the operation to 
    // be performed (load). Read the contents of register R7.

//Ex. STORE: Store R6, X(R8): X+[R8] <- [R6]
    // Decode the instruction in IR to determine the operation to
    // be performed (store). Read the contents of registers R6 and R8.

   // inst_array[ID].decode(stage_in[ID]);

}

void Pipeline::EX_stage(){
//EX. ADD: ex: ADD R3, R4, R5; 
    // Compute the sum [R4] + [R5]

//EX. LOAD: Load R5, X(R7) R5 <- [[R7]+ x
    // Add the immediate value X to the contents of R7

//Ex. STORE: Store R6, X(R8): X+[R8] <- [R6]
    // Compute the effective address X + [R8]

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

    //return halt_flag;    // need to figure this out
}

void Pipeline::MEM_stage(){
//EX. ADD: ex: ADD R3, R4, R5; 
    // No action, since there are no memory operands

//EX. LOAD: Load R5, X(R7) R5 <- [[R7]+ x
    // Use the sum X+[R7] as the effective address of the 
    // source operand, read the contents of that location from memory

//Ex. STORE: Store R6, X(R8): X+[R8] <- [R6]
    // Store the contents of register R6 into memory location X + [R8]

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
//EX. ADD: ex: ADD R3, R4, R5; 
    // Write the result into register R3

//EX. LOAD: Load R5, X(R7) R5 <- [[R7]+ x
    // Write the data received from memory into register R5

//Ex. STORE: Store R6, X(R8): X+[R8] <- [R6]
    // No action
    
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
    cout << "| ";

};