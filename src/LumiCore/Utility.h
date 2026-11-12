#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace Utility {

inline bool IsValidFile(const std::string path) {
  std::ifstream file(path);
  return file.good();
}

inline std::vector<std::string> SplitString(std::string s,
                                            const std::string &delimiter) {
  std::vector<std::string> tokens;

  size_t pos{0};
  std::string token;
  // - std::string::npos when used with len or this function .find, means "until
  // - the end of the string" .find(delimiter) searches for the first occurrence
  // in a string with the delimiter
  // - .substr() creates a new string string object from 0 to pos. pos is the
  // position until we find the delimiter
  // - .erase() we remove the newly copied string from the original string + the
  // delimiter
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}

} // namespace Utility
