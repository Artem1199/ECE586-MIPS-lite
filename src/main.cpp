#include <iostream>
#include <string>
#include <fstream>
#include "mips.h"

using namespace std;

int main()

{
    string array[32];
    short loop_input;
    string line;
    ifstream myfile ("image.txt");
    if (myfile.is_open())
    {
        while (! myfile.eof()){
            getline (myfile,line);
            cout << line << endl;
                for (loop_input = 0; loop_input < 32; loop_input++) {
                    array[loop_input]=line;
                };  
        }
        myfile.close(); // closing the file
    }
        else cout << "Unable to open file";
                system("PAUSE");
                return 0; 

    /** Instruction a(0x3142FFFF);  // operation: 12 Rs: 10 Rt: 2 Immediate: -1
    a = Instruction(0xFF853000);
    a.print();
            return 0;
            **/

}