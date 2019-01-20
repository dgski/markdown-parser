#include <string>
#include <list>

using namespace std;

class HTMLElement
{
    HTMLElement* parent =  nullptr;
    string tag;
    string textContent;
    list<HTMLElement> contents;
    void setParent(HTMLElement* _parent);
    string generateContents() const;
public:
    HTMLElement(string _tag, string _textContent = "");
    HTMLElement& appendChild(const HTMLElement& child);
    HTMLElement* getParent();
    string generate() const;
};