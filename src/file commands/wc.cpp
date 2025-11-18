#include <iostream>
#include <fstream>
#include <string>   
#include <cctype>

void countLine(unsigned int& nLines) { nLines++; }
void countWords(unsigned int& nWords, std::string& line)
{
    bool inWord = false;
        for(char c : line) {
            if(std::isspace(static_cast<unsigned int>(c))){
                inWord = false;
            } else if(!inWord) {
                nWords++;
                inWord = true;
            }
        }
}
int main(int argc, char** argv){

    bool          flagword  = false;
    bool          flagline  = false;
    unsigned int  nLine     = 0;
    unsigned int  nWords    = 0;
    std::string   fileName;
    std::string   line;
    std::string   arg;

    if (argc == 2) {
        flagline = flagword = true;
    } else {
        for (int i = 1; i < argc - 1; i++) {
            arg = argv[i];
            
            if (arg == "-w" || arg == "--word")
            {
                flagword = true;
            } else if (arg == "-l" || arg == "--line") {
                flagline = true;
            } else {
                std::cout << "wrong use of the command\n";
                return -1;
            }
            
        }
    }

    fileName = argv[argc-1];
    if (fileName.find(".txt") == std::string::npos)
    {
        std::cout << "this no txt file u dum fuck" << std::endl;
        return -1;
    }
    
    std::ifstream userFile(fileName);
    if(!userFile.is_open()) return -1;
    while (std::getline(userFile, line))
    {
        countLine(nLine);
        countWords(nWords,line);
    }
    

    if (flagline) std::cout << nLine  << " ";
    if (flagword) std::cout << nWords << " ";
                  std::cout << fileName << " ";

    return 0;
}