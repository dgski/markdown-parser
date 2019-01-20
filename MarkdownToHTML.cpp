#include <iostream>
#include "MarkdownToHTML.h"

const regex heading("(#+) (.+)");
const regex italic("\\*(.+)\\*");
const regex bold("\\*\\*(.+)\\*\\*");
const regex unorderedListItem("- (.+)");
const regex orderedListItem("([0-9]+)\\.(.+)");
const regex link("\\[(.+)\\]\\((.+)\\)");
const regex image("!\\[(.+)\\]\\((.+)\\)");
const regex code("```");



void MarkdownToHTML::processLine(string& input)
{
    LineType currentLineType = determineLineType(input);

    switch(currentLineType)
    {
    case Heading:
        processHeadingLine(input);
        break;
    case UnorderedListItem:
        processUnorderedListItemLine(input);
        break;
    case OrderedListItem:
        processOrderedListItemLine(input);
        break;
    case CodeBlock:
        processCodeBlockLine(input);
        break;
    case Other:
        processOtherLine(input);
    }

}

// Use regex to determine which kind of line it is
LineType MarkdownToHTML::determineLineType(const string& input)
{
    if(regex_match(input, heading))             return Heading;
    if(regex_match(input, unorderedListItem))   return UnorderedListItem;
    if(regex_match(input, orderedListItem))     return OrderedListItem;
    if(regex_match(input, code))                return CodeBlock;

    return Other;
}

// Process a Line Containing a Header
void MarkdownToHTML::processHeadingLine(string& input)
{
    if(lineState != inNothing)
        throw exception();

    smatch matches;
    regex_match(input, matches, heading);
    
    int hSize = matches[1].length();
    string tag = "h" + to_string(hSize);

    insertionPoint->appendChild(HTMLElement(tag.c_str(), matches[2].str()));
}

// Process a Line Containing an Unordered List Item
void MarkdownToHTML::processUnorderedListItemLine(string& input)
{
    smatch matches;
    regex_match(input, matches, unorderedListItem);

    if(lineState == inNothing)
    {
        HTMLElement ul = HTMLElement{"ul"};
        insertionPoint = &(insertionPoint->appendChild(ul));
        lineState = inUnorderedList;
    }

    insertionPoint->appendChild(HTMLElement("li", matches[1]));
}

void MarkdownToHTML::processOrderedListItemLine(string& input)
{
    smatch matches;
    regex_match(input, matches, orderedListItem);

    if(lineState == inNothing)
    {
        HTMLElement ol = HTMLElement{"ol"};
        insertionPoint = &(insertionPoint->appendChild(ol));
        lineState = inOrderedList;
    }

    insertionPoint->appendChild(HTMLElement("li", matches[2]));
}

void MarkdownToHTML::processCodeBlockLine(string& input)
{

}

void MarkdownToHTML::processOtherLine(string& input)
{
    if(lineState == inUnorderedList || lineState == inOrderedList)
    {
        insertionPoint = insertionPoint->getParent();
        lineState = inNothing;
    }
}


string MarkdownToHTML::generate()
{
    return html.generate();
}