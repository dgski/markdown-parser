#include <string>
#include <regex>
#include <memory>
#include "html-element/HTMLElement.h"

using namespace std;

// Regular Expressions for line elements
const regex headingRegex("\(#+) (.+)");
const regex unorderedListItemRegex("- (.+)");
const regex orderedListItemRegex("([0-9]+)\\.(.+)");
const regex codeRegex("```(.+)?");
const regex tableRegex(".+(\\|.+)+");
const regex emptyRegex("");

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
    ParserLineState lineState = inNothing;

    shared_ptr<HTMLElement> rootNode = nullptr;
    HTMLElement* insertionPoint = nullptr;

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
    void processSubExpressions(const string_view& input, shared_ptr<HTMLElement> parent);
    void processSubExpressionsBetween(const char* begin, const char* end, shared_ptr<HTMLElement> parent);
    void processBoldExpression(const string_view& input, const sv_match& matches, shared_ptr<HTMLElement> parent);
    void processItalicExpression(const string_view& input, const sv_match& matches, shared_ptr<HTMLElement> parent);
    void processImageExpression(const string_view& input, const sv_match& matches, shared_ptr<HTMLElement> parent);
    void processLinkExpression(const string_view& input, const sv_match& matches, shared_ptr<HTMLElement> parent);
    void processTextExpression(const string_view& input, const sv_match& matches, shared_ptr<HTMLElement> parent);

public:
    MarkdownToHTML(bool generateFullPage = true);
    void processLine(string& input);
    const HTMLElement& getcRootNode() const;
};

ostream& operator<<(ostream& stream, const MarkdownToHTML& parser);

#pragma once