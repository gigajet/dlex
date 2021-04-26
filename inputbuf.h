#ifndef __INPUT_BUFFER__H__
#define __INPUT_BUFFER__H__

#include <string>
#include <istream>
#include <sstream>
class InputBuffer {
  public:
    void GetChar(char&);
    char UngetChar(char);
    std::string UngetString(std::string);
    bool EndOfInput();
    std::istream* stream;
  private:
    std::vector<char> input_buffer;
};

#endif  //__INPUT_BUFFER__H__
