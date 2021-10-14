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
int32_t lastAddr = 0; //the last address
int lastInstrucSize = 0;
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

   inf.open(argv[1], std::ifstream::in);
    
   if (inf.is_open())
   {
        char x[256];
        int lineNumber = 1;

        while (inf.good())
        {
            inf.getline(x, 256, '\n');
            if (x[0] == 0)
                break;

            if (hasErrors(x))
            {
                std::cout << "Error on line " << std::dec << lineNumber
                    << ": " << x << std::endl;
                return;
            }
            lastInstrucSize = 0;
            if (x[DATABEGIN] != ' ')
                loadline(x);
            lineNumber++;
        }
        inf.close();
   }
   else
        return;
   

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

   loaded = true;
}

bool Loader::hasErrors(char *x)
{
     if (checkAddress(x))
        return true;
    else if (checkData(x))
        return true;
    else if (checkSpaces(x))
        return true;
    else if (checkLine(x))
        return true;
    else
       return false;
}

bool Loader::checkAddress(char *x)
{
    //doees address exist?
    bool exists = false;
    for (int i = 0; i < 6; i++)
    {
        if (x[i] != ' ')
            exists = true;
    }

    if (exists == false)
        return false; 
    
     //is it larger than the last?  

     bool lastAddrMore = false;
     std::string str = "";
     str += x[2];
     str += x[3];
     str += x[4];
     int32_t curAddr = std::stoul(str, NULL, 16);
      
     if (curAddr < lastAddr + lastInstrucSize)
         lastAddrMore = true;

     lastAddr = curAddr;

     if (lastAddrMore == true)
         return true;

    //formatted properly?

    if (x[0] == '0' && x[1] == 'x')
    {
        for (int i = 2; i < 5; i++)
        {
            if ((x[i] >= 48 && x[i] <= 57) || (x[i] >= 65 && x[i] <= 70) || (x[i] >= 97 && x[i] <= 102))
            {
                return checkColon(x);
            }
            else
                return true;
        }
        return checkColon(x);
    }
    else
        return true;
}

bool Loader::checkData(char *x)
{
    //does it exist?
    bool dataExists = false;
    int dataCounter = 0;
    for (int i = DATABEGIN; x[i] != '|'; i++)
    {
        if (x[i] != ' ')
        {
            dataExists = true;
            dataCounter++;
        }
    }
    
    //make sure it is even
    if (dataCounter % 2 != 0)
        return true;

    bool addExists = false;
    for (int i = 0; i < 6; i++)
    {
        if (x[i] != ' ')
            addExists = true;
    }

    if (addExists == false && dataExists == false)
        return false;

    if (addExists == false && dataExists == true)
        return true;
 

    //formatted properly?
    
    int counter = 0;
    bool dataBad = false;
    for (int i = DATABEGIN; x[i] != ' ' && i < COMMENT; i++)
    {
        //correct characters & in pairs
        if (!((x[i] >= 48 && x[i] <= 57) || (x[i] >= 65 && x[i] <= 70) || (x[i] >= 97 && x[i] <= 102)))
            dataBad = true;
        counter++;

        //columns inline (continue going after space is seen to see if there is more chars)
        if (x[i+1] == ' ')
        {
            i++;
            while (x[i] != '|' && i < COMMENT)
            {
                if (x[i] != ' ')
                    return true;
                i++;
            }

            break;
        }
    }
    
    if (dataBad == true)
        return true;

    if (counter % 2 != 0)
        return true;    

    //Memory size within range?
    std::string str = "";
    str += x[2];
    str += x[3];
    str += x[4];
    int32_t addr = std::stoul(str, NULL, 16);

    for (int i = 7; x[i] != ' '; i+=2)
    {
        addr++;
        if (addr > MEMSIZE)
            return true;
    }
    
    return false;   
}

bool Loader::checkSpaces(char *x)
{
    if (x[6] == ' ' && x[COMMENT - 1] == ' ')
        return false;
    else
        return true;
}

bool Loader::checkColon(char *x)
{
    if (x[5] != ':')
        return true;
    else
        return false;
}

bool Loader::checkLine(char *x)
{
    if (x[COMMENT] == '|')
        return false;
    else
        return true;
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
    if (inf.is_open())
        return true;
    else
        return false;
}

//Add a method that will write the data in the line to memory 
//(call that from within your loop)

void Loader::loadline(char *x)
{   
    Memory * mem = Memory::getInstance();

    uint8_t val;
    int32_t addr;
    bool err = false;
    std::string str = "";

    str += x[2];
    str += x[3];
    str += x[4];
    addr = std::stoul(str, NULL, 16);

    for (int i = 7; x[i] != ' '; i+=2) //val to int8_t
    {
        std::string str2 = "";
        str2 = x[i];
        str2 += x[i + 1];
        val = std::stoul(str2, NULL, 16);
        mem -> putByte(val, addr, err);
        lastInstrucSize++;
        addr++;
    }
}
