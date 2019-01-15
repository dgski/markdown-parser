#include <iostream>
#include "MarkdownToHTML.h"

void MarkdownToHTML::processLine(string& input)
{
    regex heading("(#+) (.+)");
    regex italic("\\*(.+)\\*");
    regex bold("\\*\\*(.+)\\*\\*");
    regex unorderedListItem("- (.+)");
    regex orderedlistItem("([0-9]+)\\.(.+)");
    regex link("\\[(.+)\\]\\((.+)\\)");
    regex image("!\\[(.+)\\]\\((.+)\\)");
    regex code("```");
    
    smatch partsFound;


    if(regex_match(input,partsFound,heading))
        processHeadingLine(partsFound);
    else if(regex_match(input,partsFound, italic))
        processItalicLine(partsFound);
    else if(regex_match(input, bold))
        cout << "WE FOUND BOLD!" << endl;
    else if(regex_match(input, unorderedListItem))
        cout << "WE FOUND UNORDERED LIST ITEM!" << endl;
    else if(regex_match(input, orderedlistItem))
        cout << "WE FOUND ORDERED LIST ITEM!" << endl;
    else if(regex_match(input, link))
        cout << "WE FOUND LINK!" << endl;
    else if(regex_match(input, image))
        cout << "WE FOUND IMAGE!" << endl;
    else if(regex_match(input, code))
        cout << "WE FOUND CODE START OR END" << endl;
    else
        cout << "WE FOUND NOTHING!" << endl;

}

bool MarkdownToHTML::tryMatch(string& input, const char* expression)
{
    return false;
}

void MarkdownToHTML::processHeadingLine(smatch& matches)
{
    int hSize = matches[1].length();
    cout << "<h" << hSize << ">" << matches[2] << "</h" << hSize << ">" << endl;
}

void MarkdownToHTML::processItalicLine(smatch& matches)
{
    cout << "<i>" << matches[1] << "</i>" << endl;
}


string MarkdownToHTML::generate()
{
    return "Hello!";
}