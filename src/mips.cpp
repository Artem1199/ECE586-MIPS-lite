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


Instruction::Instruction(){
    operation = NOP;
    rs = 0;
    rt = 0;
    immediate = 0;
    r_instruction = true;
    i_instruction = false;
    halt_flag = false;
}


void Instruction::decode (signed int hexin){

    signed x;
    stringstream ss;
    ss << hex << hexin;
    ss >> x;

    operation = static_cast<e_opCode>((x >> 26) &  0b111111);

    // cout << x << "\n";
    // cout << (x << 6)  <<"\n";
    // cout << ((x<<6)>>33) << "\n";


    rs = (x >> 21) & 0b11111;  //shift values to the right and & to remove values to the left
    rt = (x >> 16) & 0b11111;
    rd = (x >> 11) & 0b11111;
    immediate = (x << 16)>>16;
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
    string inst_string;
    switch (operation)
    {
    case ADD:inst_string = "ADD";break;
    case SUB:inst_string = "SUB";break;
    case MUL:inst_string = "MUL";break;
    case OR:inst_string = "OR";break;
    case AND:inst_string = "AND";break;
    case XOR:inst_string = "XOR";break;
    case ADDI:inst_string = "ADDI";break;
    case SUBI:inst_string = "SUBI";break;
    case MULI:inst_string = "MULI";break;
    case ORI:inst_string = "ORI";break;
    case ANDI:inst_string = "ANDI";break;
    case XORI:inst_string = "XORI";break;
    case LDW:inst_string = "LDW";break;
    case JR:inst_string = "JR";break; 
    case BEQ:inst_string = "BEQ";break;
    case BZ:inst_string = "BZ";break;
    case NOP:inst_string = "NOP";break;
    case HALT:inst_string = "HALT"; break;
    default: inst_string = "ERR";
        break;
    }

    if (r_instruction == true){
        inst_string += " R";
        inst_string += to_string(rd);
        inst_string += " R";
        inst_string += to_string(rs);
        inst_string += " R";
        inst_string += to_string(rt);

    } else {
        inst_string += " R";
        inst_string += to_string(rt);
        inst_string += " #";
        inst_string += to_string(immediate);
        inst_string += "(R";
        inst_string += to_string(rs);
        inst_string += ")";
    }
    return inst_string;
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
    string stage_out[5];
};

void Pipeline::run(vector<signed int> memory_image){

   
    memory = memory_image;

    int i = 0;

    while(!halt_flag) {

        IF_stage();
        ID_stage();
        EX_stage();
        MEM_stage();
        WB_stage();
        
       // visualization();
        clock();

     //   cout << i++ << " " << inst_array[WB].stringify() << "\n";
    }
       // if(halt_flag) {
      //      cout << "HALT REACHED!" << "\n";
         //   cout << inst_array[WB].stringify(); 
     //   }

}

void Pipeline::clock(){

    inst_array[WB] = inst_array[MEM];
    inst_array[MEM] = inst_array[EX];
    inst_array[EX] = inst_array[ID];
    inst_array[ID] = inst_array[IF];

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

    stage_out[IF] = memory[PC];

    PC += PC_OFFSET;

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

    inst_array[ID].decode(stage_in[ID]);

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
        stage_out[EX] = 0;
        // if not on of opcodes then HALT is equal to true '1'
       // halt_flag = 1;
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
        stage_out[MEM] = memory[stage_in[MEM]];
        break;
    case STW:
        memory[stage_in[MEM]] = Reg[inst_array[MEM].rt];
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
    case HALT:
        halt_flag = true;
        break;
        
    default:
        break;
    }
    
}

void Pipeline::visualization(){

    cout << "|- " << inst_array[IF].stringify() << " -|- " << inst_array[ID].stringify() << " -|- " 
                    << inst_array[EX].stringify() << " -|- " << inst_array[MEM].stringify() << " -|- "
                        << inst_array[WB].stringify() << " -|" << "\n";
    //cout << "\033[1;31mbold red text\033[0m\n";
    sleep_for(2000ms);
    system("clear");

};