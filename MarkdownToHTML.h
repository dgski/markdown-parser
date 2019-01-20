#include <string>
#include <regex>
#include <memory>
#include "HTMLElement.h"

using namespace std;

enum ParserLineState {
        inNothing,
        inParagraph,
        inUnorderedList,
        inOrderedList,
        inCodeBlock
    };

enum LineType {
    Heading,
    UnorderedListItem,
    OrderedListItem,
    CodeBlock,
    Other
};

class MarkdownToHTML
{
    HTMLElement html{"html"};
    HTMLElement* insertionPoint = &html;

    ParserLineState lineState = inNothing;

    void processHeadingLine(string& input);
    void processUnorderedListItemLine(string& input);
    void processOrderedListItemLine(string& input);
    void processCodeBlockLine(string& input);
    void processOtherLine(string& input);

    LineType determineLineType(const string& input);

public:
    MarkdownToHTML()
    {
    };
    void processLine(string& input);
    string generate();
};

#pragma once