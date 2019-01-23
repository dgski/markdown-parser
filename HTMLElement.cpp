#include "HTMLElement.h"

using namespace std;

string HTMLElement::generateContents() const
{
    if(textContent.length())
        return textContent;

    string res;

    for(const HTMLElement& e : contents)
        res += e.generate();

    return res;
}

string HTMLElement::generateAttributes() const
{
    string result;

    if(!attributes.empty())
        result += " ";

    for(const auto& [key,value] : attributes)
    {
        result += key + string("='") + value + string("' ");
    }

    return result;
}

HTMLElement::HTMLElement(string _tag, string _textContent)
{
    tag = _tag;
    textContent = _textContent;
}

HTMLElement& HTMLElement::setAttribute(const char*  attr, const char* val)
{
    attributes[attr] = val;
    return *this;
}

void HTMLElement::setSingle(bool _single)
{
    single = _single;
}


HTMLElement& HTMLElement::appendChild(const HTMLElement& child)
{
    contents.push_back(child);

    HTMLElement& justAdded = contents.back();
    justAdded.setParent(this);

    return justAdded;
}

string HTMLElement::generate() const
{   
    // For Text only nodes, maybe temp
    if(tag == "text" || tag == "blank")
        return generateContents();

    string res = string("<") + string(tag) + generateAttributes() + string(">");

    if(!single)
        res += generateContents() + string("</") + string(tag) + string(">") + string("\n");

    return res;
}

HTMLElement* HTMLElement::getParent() const
{
    return parent;
}

void HTMLElement::setParent(HTMLElement* _parent)
{
    parent = _parent;
}