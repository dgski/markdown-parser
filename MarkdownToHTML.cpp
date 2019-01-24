#include <iostream>
#include "MarkdownToHTML.h"

MarkdownToHTML::MarkdownToHTML(bool generateFullPage)
: rootNode((generateFullPage) ? "html" : "blank")
{
    if(generateFullPage)
        insertionPoint = &(rootNode.appendChild(HTMLElement("body")));
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
    if(regex_match(input.begin(), input.end(), matches, headingRegex))
        return Heading;
    if(regex_match(input.begin(), input.end(), matches, unorderedListItemRegex))
        return UnorderedListItem;
    if(regex_match(input.begin(), input.end(), matches, orderedListItemRegex))
        return OrderedListItem;
    if(regex_match(input.begin(), input.end(), matches, codeRegex))
        return CodeBlock;
    if(regex_match(input.begin(), input.end(), matches, tableRegex))
        return Table;
    if(input.empty())
        return Empty;
    
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
        insertionPoint->appendChild(HTMLElement("text",regex_replace(input,regex("\\s"),"&nbsp")));

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

ExpressionType MarkdownToHTML::determineExpressionType(const string_view& input, sv_match& matches)
{
    if(regex_search(input.begin(), input.end(), matches, boldRegex))
        return Bold;
    else if(regex_search(input.begin(), input.end(), matches, italicRegex))
        return Italic;
    else if(regex_search(input.begin(), input.end(), matches, imageRegex))
        return Image;
    else if(regex_search(input.begin(), input.end(), matches, linkRegex))
        return Link;


    return Text;
}

void MarkdownToHTML::processBoldExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    size_t view_length = matches[0].first - input.begin();
    processSubExpressions(string_view(input.begin(), view_length),  parent);
    
    HTMLElement b = HTMLElement("b");
    view_length = matches[1].length();
    processSubExpressions(string_view(matches[1].first, view_length), b);
    parent.appendChild(b);

    view_length = input.end() - matches[0].second;
    processSubExpressions(string_view(matches[0].second, view_length), parent);
}

void MarkdownToHTML::processItalicExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    size_t view_length = matches[0].first - input.begin();
    processSubExpressions(string_view(input.begin(), view_length),  parent);

    HTMLElement b = HTMLElement("i");
    view_length = matches[1].length();
    processSubExpressions(string_view(matches[1].first, view_length), b);
    parent.appendChild(b);

    view_length = input.end() - matches[0].second;
    processSubExpressions(string_view(matches[0].second, view_length), parent);
}

void MarkdownToHTML::processImageExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    size_t view_length = matches[0].first - input.begin();
    processSubExpressions(string_view(input.begin(), view_length),  parent);

    HTMLElement img = HTMLElement("img");
    view_length = matches[1].length();
    img.setAttribute("alt", matches[1].str().c_str());
    img.setAttribute("src", matches[2].str().c_str());
    parent.appendChild(img);

    view_length = input.end() - matches[0].second;
    processSubExpressions(string_view(matches[0].second, view_length), parent);
}

void MarkdownToHTML::processLinkExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    size_t view_length = matches[0].first - input.begin();
    processSubExpressions(string_view(input.begin(), view_length),  parent);

    HTMLElement a = HTMLElement("a");
    view_length = matches[1].length();
    processSubExpressions(string_view(matches[1].first, view_length), a);
    a.setAttribute("href", matches[2].str().c_str());
    parent.appendChild(a);

    view_length = input.end() - matches[0].second;
    processSubExpressions(string_view(matches[0].second, view_length), parent);
}

void MarkdownToHTML::processTextExpression(const string_view& input, const sv_match& matches, HTMLElement& parent)
{
    parent.appendChild(HTMLElement("text", string(input)));
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