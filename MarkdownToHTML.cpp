#include <iostream>
#include "MarkdownToHTML.h"

const regex headingRegex("(#+) (.+)");
const regex unorderedListItemRegex("- (.+)");
const regex orderedListItemRegex("([0-9]+)\\.(.+)");
const regex codeRegex("```");

const regex boldRegex("\\*\\*(.+)\\*\\*");
const regex italicRegex("\\*(.+)\\*");
const regex linkRegex("\\[(.+)\\]\\((.+)\\)");
const regex imageRegex("!\\[(.+)\\]\\((.+)\\)");

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
    case Empty:
        processEmptyLine();
        break;
    case Other:
        processOtherLine(input);
    }
}

// Use regex to determine which kind of line it is
LineType MarkdownToHTML::determineLineType(const string& input)
{
    if(regex_match(input, headingRegex))             return Heading;
    if(regex_match(input, unorderedListItemRegex))   return UnorderedListItem;
    if(regex_match(input, orderedListItemRegex))     return OrderedListItem;
    if(regex_match(input, codeRegex))                return CodeBlock;
    if(input.empty())                                return Empty;


    return Other;
}

// Process a Line Containing a Header
void MarkdownToHTML::processHeadingLine(string& input)
{
    if(lineState != inNothing)
        throw exception();

    smatch matches;
    regex_match(input, matches, headingRegex);
    
    int hSize = matches[1].length();
    string tag = "h" + to_string(hSize);

    insertionPoint->appendChild(HTMLElement(tag.c_str(), matches[2].str()));
}

// Process a Line Containing an Unordered List Item
void MarkdownToHTML::processUnorderedListItemLine(string& input)
{
    smatch matches;
    regex_match(input, matches, unorderedListItemRegex);

    if(lineState == inNothing)
    {
        HTMLElement ul = HTMLElement{"ul"};
        insertionPoint = &(insertionPoint->appendChild(ul));
        lineState = inUnorderedList;
    }

    HTMLElement li = HTMLElement("li");

    processSubExpressions(string_view(matches[1].str()), li);

    insertionPoint->appendChild(li);
}

// Process a Line Containing an Ordered List Item
void MarkdownToHTML::processOrderedListItemLine(string& input)
{
    smatch matches;
    regex_match(input, matches, orderedListItemRegex);

    if(lineState == inNothing)
    {
        HTMLElement ol = HTMLElement{"ol"};
        insertionPoint = &(insertionPoint->appendChild(ol));
        lineState = inOrderedList;
    }

    HTMLElement li = HTMLElement("li");

    processSubExpressions(string_view(matches[2].str()), li);

    insertionPoint->appendChild(li);
}

// Process a Line Containing a Code Block Start or End
void MarkdownToHTML::processCodeBlockLine(string& input)
{

}

// Process an Empty Line
void MarkdownToHTML::processEmptyLine()
{
    if(lineState != inNothing)
    {
        lineState = inNothing;
        insertionPoint = insertionPoint->getParent();
    }
}

// Process a Line that doesnt fit any other categories
void MarkdownToHTML::processOtherLine(string& input)
{
    if(lineState == inUnorderedList || lineState == inOrderedList)
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
}

// Find Expression in String
void MarkdownToHTML::processSubExpressions(const string_view& input, HTMLElement& parent)
{
    sv_match matches;
    
    if(regex_search(input.begin(), input.end(), matches, boldRegex))
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
    else if(regex_search(input.begin(), input.end(), matches, linkRegex))
    {
        size_t view_length = matches[0].first - input.begin();
        processSubExpressions(string_view(input.begin(), view_length),  parent);

        HTMLElement b = HTMLElement("a");
        view_length = matches[1].length();
        processSubExpressions(string_view(matches[1].first, view_length), b);
        parent.appendChild(b);

        view_length = input.end() - matches[0].second;
        processSubExpressions(string_view(matches[0].second, view_length), parent);
    }
    else if(regex_search(input.begin(), input.end(), matches, linkRegex))
    {
        size_t view_length = matches[0].first - input.begin();
        processSubExpressions(string_view(input.begin(), view_length),  parent);

        HTMLElement b = HTMLElement("a");
        view_length = matches[1].length();
        processSubExpressions(string_view(matches[1].first, view_length), b);
        parent.appendChild(b);

        view_length = input.end() - matches[0].second;
        processSubExpressions(string_view(matches[0].second, view_length), parent);
    }

    else if(regex_search(input.begin(), input.end(), matches, imageRegex))
    {
        size_t view_length = matches[0].first - input.begin();
        processSubExpressions(string_view(input.begin(), view_length),  parent);

        HTMLElement b = HTMLElement("a");
        view_length = matches[1].length();
        processSubExpressions(string_view(matches[1].first, view_length), b);
        parent.appendChild(b);

        view_length = input.end() - matches[0].second;
        processSubExpressions(string_view(matches[0].second, view_length), parent);
    }
    else
    {
        cout << input << endl;
        parent.appendChild(HTMLElement("text", string(input)));
    }
    
}

// Determine what kind of expression this is
ExpressionType MarkdownToHTML::determineExpressionType(const string& input)
{
    if(regex_match(input, boldRegex))               return Bold;
    if(regex_match(input, italicRegex))             return Italic;
    if(regex_match(input, linkRegex))               return Link;
    if(regex_match(input, imageRegex))              return Image;

    return Text;
}

// Process a markdown expression (non-line)
void MarkdownToHTML::processExpression(const string& expression)
{
    ExpressionType currentExpressionType = determineExpressionType(expression);

    switch(currentExpressionType)
    {
        case Bold:
            /* processBold */
            break;
        case Italic:
            /* processItalic */
            break;
        case Link:
            /* processLink */
            break;
        case Image:
            /* processImage */
            break;
        case Text:
            /* processText */
            break;
    }
}

// Generate the HTML of the read file
string MarkdownToHTML::generate()
{
    return html.generate();
}