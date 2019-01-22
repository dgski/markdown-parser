#include <iostream>
#include <fstream>
#include <string>

#include "MarkdownToHTML.h"

#define LOG_STRING "markdown-parser -> "

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        cout << LOG_STRING "usage: ./markdown-parser <filename>" << endl;
        return -1;
    }

    ifstream file(argv[1]);
    if(!file.is_open())
    {
        cout << LOG_STRING "error: could not open file: '" << argv[1] << "'" << endl;
        return -2;
    }

    MarkdownToHTML parser(false);
    string line;

    while(getline(file,line))
    {
        parser.processLine(line);
    }

    ofstream out_file(argv[2]);
    if(!out_file.is_open())
    {
        cout << LOG_STRING "error: could not write to file: '" << argv[2] << "'" << endl;
        return -3;
    }

    out_file << parser.generate();



    return 0;
}