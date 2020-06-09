#ifndef INSTRUCTION_H
#define INSTRUCTION_H    
#include <iostream>
#include <sstream>
#include <bitset>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <stdint.h>

using namespace std;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;


#define PC_OFFSET 4
#define MEMORY_SIZE 1024 * PC_OFFSET

enum e_opCode {
    ADD = 0, ADDI = 1, SUB = 2, SUBI = 3, MUL = 4, MULI = 5,
    
    OR = 6, ORI = 7, AND = 8, ANDI = 9, XOR = 10, XORI = 11,
 
    LDW = 12, STW = 13,

    BZ = 14, BEQ = 15, JR = 16, HALT = 17,

    NOP = 999,
};

enum e_stages {

    IF = 0, ID = 1, EX = 2, MEM = 3, WB = 4,

};

class Instruction{

    public:
    Instruction();
    //Instruction(unsigned int a);
    
    string stringify ();
    void print();
    void decode(signed int a);

    private:
        e_opCode operation;
        int rs;
        int rt;
        int rd;
        short immediate;
        bool r_instruction;
        bool halt_flag;
        // signed int address; - I don't believe having the instruction store it's own address is proper

        friend class Pipeline;
        friend class Pipeline_counter;

};


class Pipeline_counter{

    public:
        Pipeline_counter();
        void count(Instruction inst);
        void print();

    private:
        
        int arith_inst;
        int logic_inst;
        int mem_inst;
        int cont_inst;
        int debug;

        array<short, 32> accessed_reg;
        vector<short> accessed_mem;

};

class Pipeline{

    public:


        void run(array<signed int, MEMORY_SIZE> mem_array);

        Pipeline();
        void clock();

        void IF_stage();
        void ID_stage();
        void EX_stage();
        void MEM_stage();
        void WB_stage();
        void visualization();
        void count();

        

    private:
        bool halt_flag = false;
        bool raw_flag = false; // true if raw hazard detected in ID stage
        bool flush_flag = false; // true if values in IF / ID need to be flushed from misbranch
        array<Instruction,5> inst_array;
        array<signed int, 5> stage_out;  //will this be larger than 32?
        array<signed int,5> stage_in;
        array<signed int,5> fetched_instruction;
        const Instruction inst_nop;



        Pipeline_counter pip_count;
};


#endif //INSTRUCTION_H