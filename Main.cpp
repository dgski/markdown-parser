#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

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

    auto start = chrono::system_clock::now();

    MarkdownToHTML parser(false);
    string line;
    string contents;

    while(getline(file,line))
    {
        contents += line;
        parser.processLine(line);
    }

    ofstream out_file(argv[2]);
    if(!out_file.is_open())
    {
        cout << LOG_STRING "error: could not write to file: '" << argv[2] << "'" << endl;
        return -3;
    }

    out_file << parser.generate();

    auto end = chrono::system_clock::now();
    auto dur = (end - start).count() / 1000000.00;

    cout << LOG_STRING "html generation took: " << dur << "s" << endl;

    return 0;
}