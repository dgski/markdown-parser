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

HTMLElement::HTMLElement(string _tag, string _textContent)
{
    tag = _tag;
    textContent = _textContent;
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
    if(tag == "text")
        return generateContents();

    return string("<") + string(tag) + string(">") + generateContents() + string("</") + string(tag) + string(">") + string("\n");
}

HTMLElement* HTMLElement::getParent()
{
    return parent;
}

void HTMLElement::setParent(HTMLElement* _parent)
{
    parent = _parent;
}