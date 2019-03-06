# Markdown To HTML

A Simple Markdown to HTML Parser and Generator using the C++ Standard Library (string_view and regex components). Uses a state-machine-like approach and is fed one line at a time. Use works as follows:

``` c++
// Create Instance - parameter specifies full page generation
MarkdownToHTML parser(false); 

// Feed line to parser
string line { "# This is a Heading!" };
parser.processLine(line);

// Generate output
output << parser; // -> "<h1>This is a Heading!</h1>"
```

