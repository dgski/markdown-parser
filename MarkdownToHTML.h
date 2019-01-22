#include <string>
#include <regex>
#include <memory>
#include "HTMLElement.h"

using namespace std;

// Regular Expressions for line elements
const regex headingRegex("\(#+) (.+)");
const regex unorderedListItemRegex("- (.+)");
const regex orderedListItemRegex("([0-9]+)\\.(.+)");
const regex codeRegex("```");

// Regular Expressions for in-line elements
const regex boldRegex("\\*\\*(.+)\\*\\*");
const regex italicRegex("\\*(.+)\\*");
const regex linkRegex("\\[(.+)\\]\\((.+)\\)");
const regex imageRegex("!\\[(.+)\\]\\((.+)\\)");

// Represents the current state of the parser - needed for elements spaning multiple lines
enum ParserLineState {
        inNothing,
        inParagraph,
        inUnorderedList,
        inOrderedList,
        inCodeBlock
    };

// Represents the regular expression matching the whole line
enum LineType {
    Heading,
    UnorderedListItem,
    OrderedListItem,
    CodeBlock,
    Empty,
    Other
};

// Specialized for string_view
typedef match_results<std::string_view::const_iterator> sv_match;


class MarkdownToHTML
{
    HTMLElement rootNode;
    HTMLElement* insertionPoint = nullptr;

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

public:
    MarkdownToHTML(bool generateFullPage = true)
    : rootNode((generateFullPage) ? "html" : "blank")
    {
        if(generateFullPage)
            insertionPoint = &(rootNode.appendChild(HTMLElement("body")));
        else
            insertionPoint = &rootNode;
    };
    void processLine(string& input);
    string generate();
};

#pragma once