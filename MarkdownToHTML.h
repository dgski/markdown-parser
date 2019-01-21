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
    Empty,
    Other
};

enum ExpressionType {
    Bold,
    Italic,
    Link,
    Image,
    Text
};


typedef match_results<std::string_view::const_iterator> sv_match;


class MarkdownToHTML
{
    HTMLElement html{"html"};
    HTMLElement* insertionPoint = &html;

    ParserLineState lineState = inNothing;

    // Line Processing
    LineType determineLineType(const string& input);

    void processHeadingLine(string& input);
    void processUnorderedListItemLine(string& input);
    void processOrderedListItemLine(string& input);
    void processCodeBlockLine(string& input);
    void processEmptyLine();
    void processOtherLine(string& input);

    // Expression Processing
    void processSubExpressions(const string_view& input, HTMLElement& parent);
    ExpressionType determineExpressionType(const string& input);
    void processExpression(const string& expression);


public:
    MarkdownToHTML()
    {
    };
    void processLine(string& input);
    string generate();
};

#pragma once