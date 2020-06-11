#include "mips.h"

using namespace std;

unsigned int PC = 0;
signed int Reg[32] = {0};
array<signed int, MEMORY_SIZE> memory = {0};

#define IF 0
#define ID 1
#define EX 2
#define MEM 3
#define WB 4




Instruction::Instruction(){
    operation = NOP;
    rs = 0;
    rt = 0;
    immediate = 0;
    r_instruction = true;
 //   halt_flag = false;
}


void Instruction::decode (signed int hexin){

    signed int x = hexin;
    stringstream ss; 
    ss << hex << hexin; // convert hex to signed int
    ss >> x;

    operation = static_cast<e_opCode>((x >> 26) &  0b111111);  // shift right and remove values

    rs = (x >> 21) & 0b11111;  // shift values to the right and & to remove values to the left
    rt = (x >> 16) & 0b11111;
    rd = (x >> 11) & 0b11111;
    immediate = (x << 16)>>16;  // keeps two's complement for values
    // four conditon, i type instruction, i type instruction, or HALT
    if ((operation == ADD) or (operation == SUB) or (operation == MUL) or (operation == OR) or (operation == AND) or (operation == XOR)) {
        r_instruction = true;
    } else if ((operation == ADDI) or (operation == SUBI) or (operation == MULI) or (operation == ORI) or (operation == ANDI) or (operation == XORI) or (operation == LDW) or (operation == STW) or (operation == BZ) or  (operation == BEQ) or (operation == JR) or (operation == HALT)) {
        r_instruction = false;

        if ((operation != STW) && (operation != BEQ)){
        rd = rt; // Set rd to rt to make RAW detection logic simpler
        }

    } else {
        cout << "Error: not an r or i instruction, ";
        cout << operation << "\n";
        cout << "Instruction: " << hexin << "\n";
       // cout << "PC: " << PC << "\n";
       // halt_flag = 1;
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
    case STW:inst_string = "STW";break;
    case JR:inst_string = "JR";break; 
    case BEQ:inst_string = "BEQ";break;
    case BZ:inst_string = "BZ";break;
    case NOP:inst_string = "NOP";break;
    case HALT:inst_string = "HALT"; break;
    default: inst_string = "ERR";
        break;
    }

    if (r_instruction == true){
        inst_string += " Rd";
        inst_string += to_string(rd);
        inst_string += " Rs";
        inst_string += to_string(rs);
        inst_string += " Rt";
        inst_string += to_string(rt);

    } else {
        inst_string += " Rt";
        inst_string += to_string(rt);
        inst_string += " #";
        inst_string += to_string(immediate);
        inst_string += "(Rs";
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
    array<Instruction,5> inst_array;
    array<signed int, 5> stage_out = {0};  //will this be larger than 32?
    array<signed int,5> stage_in = {0};
    Pipeline_counter pip_count;
};

void Pipeline::run(array<signed int, MEMORY_SIZE> mem_array){
    
    memory = mem_array;

    //Init the pipeline.
    // NO RAW HAZARD ACCOUNTABILITY HERE, FIX ME
   /* IF_stage();
    clock();
    IF_stage();
    ID_stage();;
    clock();
    IF_stage();
    ID_stage();
    EX_stage();
    clock();
    IF_stage();
    ID_stage();
    EX_stage();
    MEM_stage();
    clock();*/


    // loop through pipeline until a halt is reached in the WB stage
    clock();
    while(!halt_flag) {

        if (raw_flag){
            // If there is a raw_flag, then place a NOP in EX stage and run ID, EX, MEM, WB
            ID_stage();
            EX_stage();
            MEM_stage();
            WB_stage();
        } else {
            // If there is no raw_flag then run all stages
            IF_stage();
            ID_stage();
            EX_stage();
            MEM_stage();
            WB_stage();
        }
        pip_count.count(inst_array[WB]);
        clock();
     // visualization();
      //  cout << "PC: " << PC << "\n";
     //sleep_for(20ms);
     // pip_count.print();
    }

    if (halt_flag){
        cout << "HALT REACHED" << "\n";
    } else {
        cout << "Error: Reached end of program without reaching HALT \n";
    }
    pip_count.print();

}

void Pipeline::clock(){

    if (raw_flag) {
        inst_array[WB] = inst_array[MEM];
        inst_array[MEM] = inst_array[EX];
        inst_array[EX] = inst_nop; // NOP
      //  inst_array[ID] = inst_array[ID];
      //  inst_array[IF] = inst_array[IF];

      //  stage_in[ID] = stage_out[IF];
        stage_in[EX] = 0;
        stage_in[MEM] = stage_out[EX];
        stage_in[WB] = stage_out[MEM];

    } else {
        inst_array[WB] = inst_array[MEM];
        inst_array[MEM] = inst_array[EX];
        inst_array[EX] = inst_array[ID];
        inst_array[ID] = inst_array[IF];

        // Simulate data moving to next stage after clock cycle
        stage_in[ID] = stage_out[IF];
        stage_in[EX] = stage_out[ID];
        stage_in[MEM] = stage_out[EX];
        stage_in[WB] = stage_out[MEM];
    }

    if (flush_flag){
        inst_array[IF] = inst_nop;
        inst_array[ID] = inst_nop;
        inst_array[EX] = inst_nop;
        stage_in[IF] = 0;
        stage_in[ID] = 0;
       // stage_in[EX] = 0;
        flush_flag = false;
    }

}

void Pipeline::IF_stage(){
    //Rule: Add, load, and store => Fetch the instruction and increment the program counter

    // Condition 1: check if PC count is out side mem range and provide a halt
    // Condition 2: contine if PC count is within Memory range
    // Task: bitwise shit to get instruction from memory 
    //inst_array[ID] = Instruction(PC);

    stage_out[IF] = memory[PC];

    PC += PC_OFFSET;
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
    
    /* RAW HAZARD Detection */
    // Check to see if the either source register in ID matches the destionation registers in EX or MEM
    // Also check to make sure the destionation registers aren't just R0
    // If true, then print out the hazard, and trigger the raw flag indicating the hazard
    if ((inst_array[ID].rs == inst_array[EX].rd) && (inst_array[EX].rd != 0)) {
       // cout << "RAW Hazard, ID Rs: " << inst_array[ID].rs
       //         << " EX Rd: " << inst_array[EX].rd << "\n"
       raw_flag = true;
        
        //enable forwarding
        //if (inst_array[EX].operation == LDW){
      //      raw_flag = true; 
      //  } else {
      //      raw_flag = false;
       // }

    } else if ((inst_array[ID].rs == inst_array[MEM].rd) && (inst_array[MEM].rd != 0)){
       // cout << "RAW Hazard, ID Rs: " << inst_array[ID].rs 
        //        << " MEM Rd: " << inst_array[MEM].rd << "\n";
        //        raw_flag = false;
        raw_flag = true;

    } else if ((inst_array[ID].rt == inst_array[EX].rd) && (inst_array[EX].rd != 0)){
       // cout << "RAW Hazard, ID Rt: " << inst_array[ID].rt 
        //        << " EX Rd: " << inst_array[EX].rd << "\n";
        
        //enable forwarding
      //  if (inst_array[EX].operation == LDW){
      //      raw_flag = true; 
      //  } else {
      //      raw_flag = false;
      //  }
      raw_flag = true;


    } else if ((inst_array[ID].rt == inst_array[MEM].rd) && (inst_array[MEM].rd != 0)){
        // cout << "RAW Hazard, ID Rt: " << inst_array[ID].rt 
       //         << " MEM Rd: " << inst_array[MEM].rd << "\n";
       
       //enable forwarding
       //         raw_flag = false ;
       raw_flag = true;
    } else {
        raw_flag = false;
    }

}

void Pipeline::EX_stage(){
int PC_temp = PC; // hold PC value for branch prediction check

//EX. ADD: ex: ADD R3, R4, R5; 
    // Compute the sum [R4] + [R5]

//EX. LOAD: Load R5, X(R7) R5 <- [[R7]+ x
    // Add the immediate value X to the contents of R7

//Ex. STORE: Store R6, X(R8): X+[R8] <- [R6]
    // Compute the effective address X + [R8]
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

      //  cout << "ANDI output: " << stage_out[EX] << "\n";
        break;
    case XOR:
        stage_out[EX] = Reg[inst_array[EX].rs] ^ Reg[inst_array[EX].rt];
        break;
    case XORI:
        stage_out[EX] = Reg[inst_array[EX].rs] ^ inst_array[EX].immediate;
        break;
    case BZ:
      //  cout << "BZ input value" << Reg[inst_array[EX].rs] << "\n";
        if (Reg[inst_array[EX].rs] == 0){
          //  cout << "input is equal to 0 \n";
            stage_out[EX] = inst_array[EX].immediate;
            // calculate next PC location, remove 2 OFFSET for EX delay
            // the “x”th instruction from the current instruction
            PC += inst_array[EX].immediate * PC_OFFSET - 3*PC_OFFSET;

            if ((PC_temp - PC_OFFSET*2) != PC){
                flush_flag = true;
            } else {
                flush_flag = false;
            };
        } else {
            stage_out[EX] = 0;
        }
        // Add check for PC
        break;
    case BEQ:
        if (Reg[inst_array[EX].rs] == Reg[inst_array[EX].rt])  // contents of the rs register == contents of rt regsiter
        {
            
            stage_out[EX] = inst_array[EX].immediate;
            // calculate next PC location, remove 2 OFFSET for EX delay
            PC += inst_array[EX].immediate * PC_OFFSET - 3*PC_OFFSET;
            // check to see if previous instruction was predicted
            if ((PC_temp - PC_OFFSET) != PC){
                flush_flag = true;
            } else {
                flush_flag = false;
            };

        } else {
            stage_out[EX] = 0;
        }
        // Add check for PC 
        break;
    case JR:
        stage_out[EX] = Reg[inst_array[EX].rs];

        PC = Reg[inst_array[EX].rs];  // set PC to contents of rs
        if ((PC_temp - PC_OFFSET) != PC){
                flush_flag = true;
            } else {
                flush_flag = false;
            };
        break;
    case HALT:
        stage_out[EX] = 0;
        break;
    case NOP:
        // cout << "NOP in EX stage \n";
        stage_out[EX] = 0;
        break;

    default:
        stage_out[EX] = 0;
    
    cout << "Error: EX stage did not have a proper OpCode. Halting." << "\n";
    cout << "Instruction: " << inst_array[EX].stringify() << "\n";
        // if not on of opcodes then HALT is equal to true '1'
        halt_flag = true;
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
        // check to see that the array pointer isn't outside memory
        if ((stage_in[MEM] < 0)||(stage_in[MEM] > MEMORY_SIZE - PC_OFFSET)){
            cout << "Error: outside memory bounds" << "\n";
            halt_flag = true;
        };
       // cout << "Loading: " << memory[stage_in[MEM]] << " from memory location: " << stage_in[MEM] << "\n";
        // TO DO: fix reading values from memory should be hex value 
        // Actually may not be necessary since everything in memory is already an int.
        stage_out[MEM] = memory[stage_in[MEM]];

        break;
    case STW:
        if ((stage_in[MEM] < 0)||(stage_in[MEM] > MEMORY_SIZE - PC_OFFSET)){
            cout << "Error: outside memory bounds" << "\n";
            halt_flag = true;
        };
        // TO DO: fix writing values back to memory, this should be a hex value
        // Actually may not be necessary since everything in memory is already an int.
        memory[stage_in[MEM]] = Reg[inst_array[MEM].rt];
       // cout << "Storing: " << Reg[inst_array[MEM].rt] << " at MEM loction: " << stage_in[MEM] << "\n";
        break;
    
    default:
    // Pass data calculated in EX stage if not a load / store instruction
        stage_out[MEM] = stage_in[MEM];
        break;
    }

   // cout << inst_array[MEM].stringify();
    
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
      //  PC = stage_in[WB];
        break; 

    case BEQ:
    case BZ:
     //   PC += stage_in[WB] * PC_OFFSET - 4 * PC_OFFSET; // commented out becaause branching occurs in EX

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

    if (raw_flag){
        cout << "RAW ON" << "\n";
    } else {

    }
    //cout << "\033[1;31mbold red text\033[0m\n";
    sleep_for(2000ms);
    system("clear");

}



Pipeline_counter::Pipeline_counter(){

    arith_inst = 0;
    logic_inst = 0;
    mem_inst = 0;
    cont_inst = 0;
    debug = 0;

    array<short, 32> accessed_reg = {0};
    vector<short> accessed_mem = {0};

}

void Pipeline_counter::count(Instruction inst){

switch (inst.operation)
    {
    case ADD: if ((inst.rs == 0) && (inst.rt == 0) && (inst.rd == 0) ){
        arith_inst += 0;
    } else {
        arith_inst += 1;
    } break;
    case SUB:
    case MUL: arith_inst += 1;break;
    case OR:
    case AND:
    case XOR:logic_inst += 1;break;
    case ADDI:
    case SUBI:
    case MULI:arith_inst += 1;break;
    case ORI:
    case ANDI:
    case XORI:logic_inst += 1;break;
    case LDW: mem_inst += 1; break;
    case STW:        
        accessed_mem.push_back(inst.rs + inst.immediate);
        mem_inst += 1; break;
    case JR:
    case BEQ:
    case BZ:
    case HALT:cont_inst += 1; break;
    case NOP: break;
    default: debug += 1;
        break;
    }
    //marks that the register was used in this instruction
    accessed_reg[inst.rs] = 1;
    accessed_reg[inst.rt] = 1;
    accessed_reg[inst.rd] = 1;

    if (arith_inst > 800){
        sleep_for(10000ms);
    }

}

void Pipeline_counter::print(){

    cout << "\n";

    cout << "*Instruction counts* " << "\n";

    cout << "Total number of instructions: " << cont_inst + mem_inst + logic_inst + arith_inst << "\n";
    cout << "Arithmetic instructions: " << arith_inst << "\n";
    cout << "Logical instructions: " << logic_inst << "\n";
    cout << "Memory access instructions: " << mem_inst << "\n";
    cout << "Control transfer instructions: " << cont_inst << "\n";
    cout << "DEBUG: " << debug << "\n" ;
    cout << "\n";

    cout << "*Final register state*" << "\n";
    cout << "Program couter: " << PC - 4 * PC_OFFSET << "\n";
    for (int i = 1; i < 32; i++){
        if (accessed_reg[i]){ 
            cout << "R" << i << ": " << Reg[i] << "\n";
        }
    }

    cout << "\n";
    cout << "*Final memory state*" << "\n";

    while(!accessed_mem.empty()){
        
        cout << "Address: " << accessed_mem.back() << ", Contents: "<< memory[accessed_mem.back()] << "\n";
        accessed_mem.pop_back();

    };

}