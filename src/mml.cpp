#include "mml.hpp"

void decodeMMLStr(std::string& source, int* macro, unsigned char& macroLen,
                  unsigned char& macroLoop, int macroMin, int macroMax,
                  unsigned char& macroRel, bool bit30)
{
  int buf = 0;
  bool negaBuf = false;
  bool setBit30 = false;
  bool hasVal = false;
  macroLen = 0;
  macroLoop = 255;
  macroRel = 255;
  for (char& i : source) {
    switch (i) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      hasVal = true;
      buf *= 10;
      buf += i - '0';
      break;
    case '-':
      if (!hasVal) {
        hasVal = true;
        negaBuf = true;
      }
      break;
    case '@':
      if (bit30) {
        setBit30 = true;
      }
      break;
    case ' ':
      if (hasVal) {
        hasVal = false;
        macro[macroLen] = negaBuf ? -buf : buf;
        negaBuf = false;
        if (macro[macroLen] < macroMin) macro[macroLen] = macroMin;
        if (macro[macroLen] > macroMax) macro[macroLen] = macroMax;
        if (setBit30) macro[macroLen] ^= 0x40000000;
        setBit30 = false;
        macroLen++;
        buf = 0;
      }
      break;
    case '|':
      if (hasVal) {
        hasVal = false;
        macro[macroLen] = negaBuf ? -buf : buf;
        negaBuf = false;
        if (macro[macroLen] < macroMin) macro[macroLen] = macroMin;
        if (macro[macroLen] > macroMax) macro[macroLen] = macroMax;
        if (setBit30) macro[macroLen] ^= 0x40000000;
        setBit30 = false;
        macroLen++;
        buf = 0;
      }
      if (macroLoop == 255) {
        macroLoop = macroLen;
      }
      break;
    case '/':
      if (hasVal) {
        hasVal = false;
        macro[macroLen] = negaBuf ? -buf : buf;
        negaBuf = false;
        if (macro[macroLen] < macroMin) macro[macroLen] = macroMin;
        if (macro[macroLen] > macroMax) macro[macroLen] = macroMax;
        if (setBit30) macro[macroLen] ^= 0x40000000;
        setBit30 = false;
        macroLen++;
        buf = 0;
      }
      if (macroRel == 255) {
        macroRel = macroLen;
      }
      break;
    }
    if (macroLen >= 255) break;
  }
  if (hasVal && macroLen < 255) {
    hasVal = false;
    macro[macroLen] = negaBuf ? -buf : buf;
    negaBuf = false;
    if (macro[macroLen] < macroMin) macro[macroLen] = macroMin;
    if (macro[macroLen] > macroMax) macro[macroLen] = macroMax;
    if (setBit30) macro[macroLen] ^= 0x40000000;
    setBit30 = false;
    macroLen++;
    buf = 0;
  }
}

void encodeMMLStr(std::string& target, int* macro, int macroLen, int macroLoop,
                  int macroRel, bool hex, bool bit30)
{
  target = "";
  char buf[32];
  for (int i = 0; i < macroLen; i++) {
    if (i == macroLoop) target += "| ";
    if (i == macroRel) target += "/ ";
    if (bit30
        && ((macro[i] & 0xc0000000) == 0x40000000
            || (macro[i] & 0xc0000000) == 0x80000000))
      target += "@";
    int macroVal = macro[i];
    if (macro[i] < 0) {
      if (!(macroVal & 0x40000000)) macroVal |= 0x40000000;
    } else {
      if (macroVal & 0x40000000) macroVal &= ~0x40000000;
    }
    if (hex) {
      if (i == macroLen - 1) {
        snprintf(buf, 31, "%.2X", macroVal);
      } else {
        snprintf(buf, 31, "%.2X ", macroVal);
      }
    } else {
      if (i == macroLen - 1) {
        snprintf(buf, 31, "%d", macroVal);
      } else {
        snprintf(buf, 31, "%d ", macroVal);
      }
    }
    target += buf;
  }
}