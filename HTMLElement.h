#include <string>
#include <list>
#include <map>

using namespace std;

class HTMLElement
{
    HTMLElement* parent =  nullptr;
    string tag;
    string textContent;

    list<HTMLElement> contents;
    map<string,string> attributes;

    void setParent(HTMLElement* _parent);
    string generateContents() const;
    string generateAttributes() const;
public:
    HTMLElement(string _tag, string _textContent = "");
    HTMLElement& setAttribute(const char* attr, const char* val);
    HTMLElement& appendChild(const HTMLElement& child);
    HTMLElement* getParent() const;
    string generate() const;
};