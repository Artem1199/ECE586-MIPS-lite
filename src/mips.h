enum e_opCode {
    ADD = 0, ADDI = 1, SUB = 2, SUBI = 3, MUL = 4, MULI = 5,
    
    OR = 6, ORI = 7, AND = 8, ANDI = 9, XOR = 10, XORI = 11,
 
    LDW = 12, STW = 13,

    BZ = 14, BEQ = 15, JR = 16, HALT = 17,

    NOP = 100,
};

enum e_stages {
    IF = 0, ID = 1, EX = 2, MEM = 3, WB = 4
};

class Instruction{

    public:
    Instruction();
    Instruction (signed int a);

    void print_Instruction();

    private:
        e_opCode operation;
        signed int rs;
        signed int rt;
        signed int rd;
        signed int immediate;
        bool r_instruction;
        friend class Pipeline;

};

class Pipeline{

    public:

        Pipeline();

        void clock();

        void IF_stage();
        void ID_stage();
        void EX_stage();
        void MEM_stage();
        void WB_stage();

        void visualization();

    private:

        Instruction inst_array[5];
        signed int stage_out[5];  //will this be larger than 32?
        signed int stage_in[5];
        signed int fetched_instruction;

};