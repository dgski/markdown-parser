#include <iostream>
#include "MarkdownToHTML.h"
#include <array>

MarkdownToHTML::MarkdownToHTML(bool generateFullPage)
: rootNode((generateFullPage) ? "html" : "blank")
{
    if(generateFullPage)
    {
        rootNode.appendChild(HTMLElement("head"));
        insertionPoint = &(rootNode.appendChild(HTMLElement("body")));
    }
    else
        insertionPoint = &rootNode;
};

void MarkdownToHTML::processLine(string& input)
{
    sv_match matches;
    string_view inputView(input);

    LineType currentLineType = determineLineType(inputView, matches);

    if(lineState == inCodeBlock && currentLineType != CodeBlock)
        currentLineType = Other;

    switch(currentLineType)
    {
    case Heading:
        processHeadingLine(matches);
        break;
    case UnorderedListItem:
        processUnorderedListItemLine(matches);
        break;
    case OrderedListItem:
        processOrderedListItemLine(matches);
        break;
    case CodeBlock:
        processCodeBlockLine(matches);
        break;
    case Table:
        processTableLine(matches);
        break;
    case Empty:
        processEmptyLine();
        break;
    case Other:
        processOtherLine(input);
    }
}

LineType MarkdownToHTML::determineLineType(const string_view& input, sv_match& matches)
{
    static const pair<const regex&, LineType> LineRegexAndTypes [] {
        {headingRegex, Heading},
        {unorderedListItemRegex, UnorderedListItem},
        {orderedListItemRegex, OrderedListItem},
        {codeRegex, CodeBlock},
        {tableRegex, Table},
        {emptyRegex, Empty},
    };

    for(const auto& [r,t] : LineRegexAndTypes)
    {
        if(regex_match(input.begin(), input.end(), matches, r))
            return t;
    }
    
    return Other;
}

void MarkdownToHTML::processHeadingLine(const sv_match& matches)
{
    if(lineState != inNothing)
        throw exception();
    
    int hSize = matches[1].length();
    string tag = "h" + to_string(hSize);

    insertionPoint->appendChild(HTMLElement(tag.c_str(), matches[2].str()));
}

void MarkdownToHTML::processUnorderedListItemLine(const sv_match& matches)
{
    if(lineState == inNothing)
    {
        HTMLElement ul = HTMLElement{"ul"};
        insertionPoint = &(insertionPoint->appendChild(ul));
        lineState = inUnorderedList;
    }

    HTMLElement li = HTMLElement("li");
    processSubExpressions(string_view(matches[1].first, matches[1].length()), li);
    insertionPoint->appendChild(li);
}

void MarkdownToHTML::processOrderedListItemLine(const sv_match& matches)
{
    if(lineState == inNothing)
    {
        HTMLElement ol = HTMLElement{"ol"};
        insertionPoint = &(insertionPoint->appendChild(ol));
        lineState = inOrderedList;
    }

    HTMLElement li = HTMLElement("li");
    processSubExpressions(string_view(matches[2].first, matches[2].length()), li);
    insertionPoint->appendChild(li);
}

void MarkdownToHTML::processCodeBlockLine(const sv_match& matches)
{
    if(lineState == inCodeBlock)
    {
        insertionPoint = insertionPoint->getParent();
        lineState = inNothing;
        return;
    }

    HTMLElement div = HTMLElement("div");
    div.setAttribute("class", "code");

    insertionPoint = &(insertionPoint->appendChild(div));
    lineState = inCodeBlock;
}

void MarkdownToHTML::processTableLine(const sv_match& matches)
{
    if(matches[0].str().find("---") != string::npos) // <th> underline
        return;

    // Decide whether td or th
    string tag_name;
    if(lineState != inTable)
    {
        insertionPoint = &(insertionPoint->appendChild(HTMLElement("table")));
        tag_name = "th";
        lineState = inTable;
    }
    else
    {
        tag_name = "td";
    }

    insertionPoint = &(insertionPoint->appendChild(HTMLElement("tr")));

    string cellValue;
    for(char c : matches[0].str())
    {
        if(c == '|')
        {
            HTMLElement cell(tag_name.c_str());
            processSubExpressions(string_view(cellValue), cell);
            insertionPoint->appendChild(cell);

            cellValue.clear();
            continue;
        }
        cellValue += c;
    }
    
    HTMLElement cell(tag_name.c_str());
    processSubExpressions(string_view(cellValue), cell);
    insertionPoint->appendChild(cell);
    insertionPoint = insertionPoint->getParent();
}

void MarkdownToHTML::processEmptyLine()
{
    if(lineState != inNothing)
    {
        lineState = inNothing;
        insertionPoint = insertionPoint->getParent();
    }
}

void MarkdownToHTML::processOtherLine(string& input)
{
    if(lineState == inUnorderedList || lineState == inOrderedList || lineState == inTable)
    {
        insertionPoint = insertionPoint->getParent();
        lineState = inNothing;
    }

    if(lineState == inNothing)
    {
        HTMLElement p = HTMLElement("p");
        processSubExpressions(string_view(input), p);

        insertionPoint = &(insertionPoint->appendChild(p));
        lineState = inParagraph;
    }

    if(lineState == inCodeBlock)
    {
        insertionPoint->appendChild(TextElement(regex_replace(input,regex("\\s"),"&nbsp")));

        HTMLElement br("br");
        br.setSingle(true);
        insertionPoint->appendChild(br);
    }
}

void MarkdownToHTML::processSubExpressions(const string_view& input, HTMLElement& parent)
{
    sv_match matches;
    ExpressionType currentExpression = determineExpressionType(input, matches);

    switch(currentExpression)
    {
        case Bold:
            processBoldExpression(input, matches, parent);
            break;
        case Italic:
            processItalicExpression(input, matches, parent);
            break;
        case Image:
            processImageExpression(input, matches, parent);
            break;
        case Link:
            processLinkExpression(input, matches, parent);
            break;
        case Text:
        default:
            processTextExpression(input, matches, parent);
    }
}

void MarkdownToHTML::processSubExpressionsBetween(const char* begin, const char* end, HTMLElement& parent)
{
    size_t view_length = end - begin;
    processSubExpressions(string_view(begin, view_length), parent);
}

ExpressionType MarkdownToHTML::determineExpressionType(const string_view& input, sv_match& matches)
{
    static const pair<const regex&, ExpressionType> ExpressionRegexAndTypes [] {
        {boldRegex, Bold},
        {italicRegex, Italic},
        {imageRegex, Image},
        {linkRegex, Link},
    };

    for(const auto& [r,t] : ExpressionRegexAndTypes)
    {
        if(regex_search(input.begin(), input.end(), matches, r))
            return t;
    }

    return Text;
}

void MarkdownToHTML::processBoldExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    processSubExpressionsBetween(input.begin(), matches[0].first, parent);
    
    HTMLElement b = HTMLElement("b");
    processSubExpressionsBetween(matches[1].first, matches[1].second, b);
    parent.appendChild(b);

    processSubExpressionsBetween(matches[0].second, input.end(), parent);
}

void MarkdownToHTML::processItalicExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    processSubExpressionsBetween(input.begin(), matches[0].first, parent);

    HTMLElement i = HTMLElement("i");
    processSubExpressionsBetween(matches[1].first, matches[1].second, i);
    parent.appendChild(i);

    processSubExpressionsBetween(matches[0].second, input.end(), parent);
}

void MarkdownToHTML::processImageExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    processSubExpressionsBetween(input.begin(), matches[0].first, parent);

    HTMLElement img = HTMLElement("img");
    img.setAttribute("alt", matches[1].str().c_str());
    img.setAttribute("src", matches[2].str().c_str());
    parent.appendChild(img);

    processSubExpressionsBetween(matches[0].second, input.end(), parent);
}

void MarkdownToHTML::processLinkExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    processSubExpressionsBetween(input.begin(), matches[0].first, parent);

    HTMLElement a = HTMLElement("a");
    a.setAttribute("href", matches[2].str().c_str());
    processSubExpressionsBetween(matches[1].first, matches[1].second, a);
    parent.appendChild(a);

    processSubExpressionsBetween(matches[0].second, input.end(), parent);
}

void MarkdownToHTML::processTextExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    parent.appendChild(TextElement(string(input)));
}

const HTMLElement& MarkdownToHTML::getcRootNode() const
{
    return rootNode;
}

ostream& operator<<(ostream& stream, const MarkdownToHTML& parser)
{
    stream << parser.getcRootNode();
    return stream;
}