#include <iostream>
#include <string>
#include <fstream>
#include "mips.h"

using namespace std;

int image_size = 1026;

int main()

{

    string array[image_size];
    short loop_input;
    string line;
    ifstream myfile ("image.txt");
    if (myfile.is_open())
    {
        while (! myfile.eof()){
            getline (myfile,line);
            cout << line << endl;
                for (loop_input = 0; loop_input < image_size; loop_input++) {
                    array[loop_input]=line;
                };  
        }
        myfile.close(); // closing the file
    }
        else cout << "Unable to open file";
                system("PAUSE");

    Instruction a;
    a.decode(0x3142FFFF);
    cout << a.stringify();

                return 0; 

}