#include <string>
#include <regex>
#include <memory>
#include "HTMLElement.h"

using namespace std;

// Regular Expressions for line elements
const regex headingRegex("\(#+) (.+)");
const regex unorderedListItemRegex("- (.+)");
const regex orderedListItemRegex("([0-9]+)\\.(.+)");
const regex codeRegex("```(.+)?");
const regex tableRegex("\\|((.+)\\|)+");

// Regular Expressions for in-line elements
const regex boldRegex("\\*\\*(\\S(.*?\\S)?)\\*\\*");
const regex italicRegex("\\*(.+)\\*");
const regex linkRegex("\\[(.+)\\]\\((.+)\\)");
const regex imageRegex("!\\[(.+)\\]\\((.+)\\)");

// Represents the current state of the parser - needed for elements spaning multiple lines
enum ParserLineState {
    inNothing,
    inParagraph,
    inUnorderedList,
    inOrderedList,
    inTable,
    inCodeBlock
};

// Represents the regular expression matching the whole line
enum LineType {
    Heading,
    UnorderedListItem,
    OrderedListItem,
    CodeBlock,
    Table,
    Empty,
    Other
};

// Represents a in-line regular expression
enum ExpressionType {
    Bold,
    Italic,
    Link,
    Image,
    Text
};

// Specialized for string_view
typedef match_results<std::string_view::const_iterator> sv_match;

class MarkdownToHTML
{
    HTMLElement rootNode;
    HTMLElement* insertionPoint = nullptr;

    ParserLineState lineState = inNothing;

    // Line Processing
    LineType determineLineType(const string_view& input, sv_match& matches);

    void processHeadingLine(const sv_match& matches);
    void processUnorderedListItemLine(const sv_match& matches);
    void processOrderedListItemLine(const sv_match& matches);
    void processTableLine(const sv_match& matches);
    void processCodeBlockLine(const sv_match& matches);
    void processEmptyLine();
    void processOtherLine(string& input);

    // Expression Processing
    ExpressionType determineExpressionType(const string_view& input, sv_match& matches);

    void processSubExpressions(const string_view& input, HTMLElement& parent);
    void processBoldExpression(const string_view& input, const sv_match& matches, HTMLElement& parent);
    void processItalicExpression(const string_view& input, const sv_match& matches, HTMLElement& parent);
    void processImageExpression(const string_view& input, const sv_match& matches, HTMLElement& parent);
    void processLinkExpression(const string_view& input, const sv_match& matches, HTMLElement& parent);
    void processTextExpression(const string_view& input, const sv_match& matches, HTMLElement& parent);

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