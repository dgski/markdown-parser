#include <string>
#include <regex>





using namespace std;

enum ParserState {
        Nothing
    };

class MarkdownToHTML
{
    ParserState state = Nothing;

    void processHeadingLine(smatch& matches);
    void processItalicLine(smatch& matches);


    bool tryMatch(string& input, const char* expression);

public:
    MarkdownToHTML() {};
    void processLine(string& input);
    string generate();
};

#pragma once