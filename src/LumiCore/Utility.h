#pragma once

#include <format>
#include <fstream>
#include <string>
#include <vector>
#include "Mesh.h"
#include "logger/Logger.h"

#include <stb_image.h>

namespace LumiCore::Utility
{

inline bool IsValidFile(const std::string path)
{
  std::ifstream file(path);
  return file.good();
}

inline std::vector<std::string> SplitString(std::string s, const std::string& delimiter)
{
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
  while ((pos = s.find(delimiter)) != std::string::npos)
  {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}

inline unsigned int GenerateTexture(const std::string& filename, const std::string& directory)
{
  const std::string path = directory + '/' + filename;

  stbi_set_flip_vertically_on_load(true);

  unsigned int id;
  int width, height, channels;
  // generate texture object
  glGenTextures(1, &id);

  // load the texture data
  unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  if (data)
  {
    GLenum format;
    if (channels == 1) // Gray sacle
      format = GL_RED;
    else if (channels == 3) // RGB values
      format = GL_RGB;
    else if (channels == 4) // RBGA values
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, id);

    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
    LOG_WARNING(std::format("Could not load in texture: {}", path));


  // Once we have called `glTexImage2D`, we have bound the texture object and
  // attached an image to it. We can generate a Mipmap with `glGenerateMipmap`,
  // and after that has been done it is good practice to free the image memory
  stbi_image_free(data);

  return id;
}


} // namespace LumiCore::Utility
