#include <string>

void decodeMMLStr(std::string& source, int* macro, unsigned char& macroLen,
                  unsigned char& macroLoop, int macroMin, int macroMax,
                  unsigned char& macroRel, bool bit30);
void encodeMMLStr(std::string& target, int* macro, int macroLen, int macroLoop,
                  int macroRel, bool hex, bool bit30);