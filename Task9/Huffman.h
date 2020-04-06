#pragma once
#include <string>
typedef unsigned char byte;

const std::string kStr = "bfabfgdsgdsgdsfgdasgfdfdhfhdfs";


struct IInputStream {
	IInputStream(const std::string& in): str(in)
	{
		iter = str.begin();
	}
  // Возвращает false, если поток закончился
  bool Read(byte& value) {
    if (iter == str.end())
      return false;
    value = *iter;
    std::advance(iter, 1);
    return true;
  }
  const std::string& str;
  std::string::const_iterator iter;
};

struct IOutputStream {
  void Write(const byte value) {
    if (value != *iter)
      result = false;
    std::advance(iter, 1);
	output.push_back(value);
  }
  std::string output;

  std::string::const_iterator iter = kStr.begin();
  bool result = true;
};
