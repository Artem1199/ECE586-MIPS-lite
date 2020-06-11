#include <iostream>
#include <string>
#include <fstream>
#include "mips.h"

using namespace std;

array <signed int, MEMORY_SIZE> mem_array = {0};

int main()

{
    // clear terminal
    system("clear");
    Instruction a;
    signed long N_inst;
    short loop_input = 0;
    string line;
    ifstream myfile ("image.txt");
    if (myfile.is_open())
    {
        while (! myfile.eof()){
        
            getline (myfile,line);

                // for (loop_input = 0; loop_input < image_size; loop_input++) {              
                     
                                          
                        stringstream ss;
                        ss << hex << line;
                        ss >> N_inst;

                     mem_array[loop_input*4] = N_inst;

                     loop_input++;
                    

               //      memory_image.push_back(N_inst);
              //       a.decode(N_inst);
             //  cout << i << " "<< a.stringify() << "\n";

                // };  
        }
        
        // print every instruction in memory
       for (int i = 50; i >= 0; i--) 
            {
                a.decode(mem_array[i*4]);
                cout << i*4 << " " << a.stringify() << "\n";
            }

        myfile.close(); // closing the file
    }
        else cout << "Unable to open file";
                system("PAUSE");

    Pipeline pip; 

    pip.run(mem_array);


                return 0; 

}