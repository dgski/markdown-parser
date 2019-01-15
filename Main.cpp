#include <iostream>
#include <fstream>
#include <string>

#include "MarkdownToHTML.h"

#define LOG_STRING "markdown-parser -> "

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        cout << LOG_STRING "usage: ./markdown-parser <filename>" << endl;
        return -1;
    }

    fstream file;
    file.open(argv[1]);

    if(!file.is_open())
    {
        cout << LOG_STRING "error: could not open file: '" << argv[1] << "'" << endl;
        return -2;
    }

    MarkdownToHTML parser;
    string line;

    while(getline(file,line))
    {
        parser.processLine(line);
    }

    cout << parser.generate() << endl;

    return 0;
}