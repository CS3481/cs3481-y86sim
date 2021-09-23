/**
 * Names: Eli O & Blake Lucas
 * Team: Ricky Bobby & Cal Naughton Jr.
*/
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>

#include "Loader.h"
#include "Memory.h"

//first column in file is assumed to be 0
#define ADDRBEGIN 2   //starting column of 3 digit hex address 
#define ADDREND 4     //ending column of 3 digit hex address
#define DATABEGIN 7   //starting column of data bytes
#define COMMENT 28    //location of the '|' character 

/**
 * Loader constructor
 * Opens the .yo file named in the command line arguments, reads the contents of the file
 * line by line and loads the program into memory.  If no file is given or the file doesn't
 * exist or the file doesn't end with a .yo extension or the .yo file contains errors then
 * loaded is set to false.  Otherwise loaded is set to true. And Dr. Wilkes was here.
 *
 * @param argc is the number of command line arguments passed to the main; should
 *        be 2
 * @param argv[0] is the name of the executable
 *        argv[1] is the name of the .yo file
 */
Loader::Loader(int argc, char * argv[])
{
   loaded = false;

   for (int i = 0; i < argc; i++)  //print contents of argv[]
   {
       printf("%s\n", argv[i]); 
   }

   inf.open("asumr.yo", std::ifstream::in);
    
   if (inf.is_open())   //print the asumer.yo file
   {
        char x[256];

        while (inf.good())
        {
            inf.getline(x, 256, '\n');
            //printf("%s\n", x);
            loadline(x);
        }

        inf.close();
   }
   

   //Start by writing a method that opens the file (checks whether it ends 
   //with a .yo and whether the file successfully opens; if not, return without 
   //loading)

   //The file handle is declared in Loader.h.  You should use that and
   //not declare another one in this file.
   
   //Next write a simple loop that reads the file line by line and prints it out
   
   //Next, add a method that will write the data in the line to memory 
   //(call that from within your loop)

   //Finally, add code to check for errors in the input line.
   //When your code finds an error, you need to print an error message and return.
   //Since your output has to be identical to your instructor's, use this cout to print the
   //error message.  Change the variable names if you use different ones.
   //  std::cout << "Error on line " << std::dec << lineNumber
   //       << ": " << line << std::endl;


   //If control reaches here then no error was found and the program
   //was loaded into memory.
   loaded = true;  
  
}

/**
 * isLoaded
 * returns the value of the loaded data member; loaded is set by the constructor
 *
 * @return value of loaded (true or false)
 */
bool Loader::isLoaded()
{
   return loaded;
}


//You'll need to add more helper methods to this file.  Don't put all of your code in the
//Loader constructor.  When you add a method here, add the prototype to Loader.h in the private
//section.


bool Loader::checkInputFile()
{
    inf.open("asumr.yo", std::ifstream::in);

    if (inf.is_open())
        return true;
    else
        return false;
}

//Add a method that will write the data in the line to memory 
//(call that from within your loop)

void Loader::loadline(char *x)
{
    inf.open("asumr.yo", std::ifstream::in);

    Memory * mem = Memory::getInstance();

    if (x[0] == '0' && x[DATABEGIN] != ' ')
    {
        uint8_t val;
        int32_t addr;
        bool err = false;

//Helper method to convert char to int
//Another method to turn array into 64bit val


//!!!!! UPDATE LOADER FOR THIS METHOD
        for (int i = 0; i < 5; i++)
        {
            //Bitshift?
            //addr = x[i];
        }

        for (int i = 7; x[i] != ' '; i++)
        {
            //Bitshift?
            //val = x[i];
        }

        mem.putByte(val, addr, err);
    }
}
