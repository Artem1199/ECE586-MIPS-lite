#include <iostream>
#include <string>
#include <fstream>
#include "mips.h"

using namespace std;

int image_size = 1030;

int main()

{
    Instruction a;

    vector<string> memory_image;
    short loop_input;
    int i = 1;
    string line;
    ifstream myfile ("image.txt");
    if (myfile.is_open())
    {
        while (! myfile.eof()){
        
            getline (myfile,line);

                // for (loop_input = 0; loop_input < image_size; loop_input++) {              
                     memory_image.push_back(line);
                // };  
        }
        myfile.close(); // closing the file
    }
        else cout << "Unable to open file";
                system("PAUSE");

    Pipeline pip; 

    pip.run(memory_image);


                return 0; 

}