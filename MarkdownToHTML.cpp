#include <iostream>
#include "MarkdownToHTML.h"

void MarkdownToHTML::processLine(string& input)
{
    LineType currentLineType = determineLineType(input);

    if(lineState == inCodeBlock && currentLineType != CodeBlock)
    {
        currentLineType = Other;
    }
    
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
    if(lineState == inCodeBlock)
    {
        insertionPoint = insertionPoint->getParent();
        lineState = inNothing;
        return;
    }


    HTMLElement div = HTMLElement("div");
    div
        .setAttribute("class", "code")
        .setAttribute("style", "padding: 10px; color: white; background-color: black");
    
    insertionPoint = &(insertionPoint->appendChild(div));
    lineState = inCodeBlock;
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

    if(lineState == inCodeBlock)
    {
        insertionPoint->appendChild(HTMLElement("text", input));
        insertionPoint->appendChild(HTMLElement("br"));
    }
}

// Find Expression in String
void MarkdownToHTML::processSubExpressions(const string_view& input, HTMLElement& parent)
{
    sv_match matches;
    
    if(regex_search(input.begin(), input.end(), matches, boldRegex))
    {
        cout << "# BOLD FOUND: " << endl;
        size_t view_length = matches[0].first - input.begin();
        cout << "FRONT: " << string_view(input.begin(), view_length) << endl;
        processSubExpressions(string_view(input.begin(), view_length),  parent);
        
        HTMLElement b = HTMLElement("b");
        view_length = matches[1].length();
        cout << "MIDDLE: " << string_view(matches[1].first, view_length) << endl;
        processSubExpressions(string_view(matches[1].first, view_length), b);
        parent.appendChild(b);

        view_length = input.end() - matches[0].second;
        cout << "END: " << string_view(matches[0].second, view_length) << endl;
        processSubExpressions(string_view(matches[0].second, view_length), parent);
    }
    else if(regex_search(input.begin(), input.end(), matches, italicRegex))
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
    else if(regex_search(input.begin(), input.end(), matches, imageRegex))
    {
        cout << "# IMAGE FOUND: " << endl;

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
    else if(regex_search(input.begin(), input.end(), matches, linkRegex))
    {
        cout << "# LINK FOUND: " << endl;


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
    else
    {
        cout << input << endl;
        parent.appendChild(HTMLElement("text", string(input)));
    }
    
}

// Generate the HTML of the read file
string MarkdownToHTML::generate()
{
    return rootNode.generate();
}