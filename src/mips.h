#ifndef INSTRUCTION_H
#define INSTRUCTION_H    
#include <iostream>
using namespace std;

enum e_opCode {
    ADD = 0, ADDI = 1, SUB = 2, SUBI = 3, MUL = 4, MULI = 5,
    
    OR = 6, ORI = 7, AND = 8, ANDI = 9, XOR = 10, XORI = 11,
 
    LDW = 12, STW = 13,

    BZ = 14, BEQ = 15, JR = 16, HALT = 17,

    NOP = 100,
};

enum e_stages {
    IF = 0, ID = 1, EX = 2, ALU1 = 5, MEM = 3, ALU2 = 5, WB = 4
    // we may have a RF stage that include instruction decoding and source register reads
    // then the order as follow:
    // IF = 0, RF = 1, ALU1 = 2, MEM = 3, ALU2 = 4, WB = 5
};

class Instruction{

    public:
    Instruction(unsigned int a);
    
    string stringify ();
    void print();
    void decode(signed int a);

    private:
        e_opCode operation;
        signed int rs;
        signed int rt;
        signed int rd;
        signed int immediate;
        bool r_instruction;
        bool i_instruction;
        bool halt_flag;
        signed int address;

        friend class Pipeline;

};

class Pipeline{

    public:

        Pipeline(){
            string inst_array[5] = 0;
            string stage_in[5] = 0;
            string stringstage_out[5] = 0;
        }


        void clock();

        void IF_stage();
        void ID_stage();
        void EX_stage();
        void MEM_stage();
        void WB_stage();

        void visualization();

    private:

        Instruction inst_array[5];
        signed int stage_out[5]={0};  //will this be larger than 32?
        signed int stage_in[5]={0};
        signed int fetched_instruction={0};

};

#endif //INSTRUCTION_H