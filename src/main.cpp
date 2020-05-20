#include <iostream>
#include <string>
#include <fstream>
#include "mips.h"

using namespace std;

int image_size = 1030;

int main()

{
    Instruction a;

    string array[image_size];
    short loop_input;
    int i = 1;
    string line;
    ifstream myfile ("image.txt");
    if (myfile.is_open())
    {
        while (! myfile.eof()){
        
            getline (myfile,line);

            a.decode(line);
            string b = a.stringify();
            cout << i++ << " " << b << " " << line << "\n";       ;
            
           // cout << line << endl;
           // a.decode(line);
           // cout << a.stringify()  << "\n";

                 for (loop_input = 0; loop_input < image_size; loop_input++) {              
                     array[loop_input]=line;
                 };  
        }
        myfile.close(); // closing the file
    }
        else cout << "Unable to open file";
                system("PAUSE");

   // Instruction a;
   // a.decode(0x3142FFFF);
   // cout << a.stringify();

   cout << array[1] << "test";



                return 0; 

}