#pragma once

#include "logger/Logger.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <glad/glad.h>
#include <string>
#include <vector>

#include <stb_image.h>

namespace LumiCore::Utility
{

  /**
   * Check is a file exist in a given path
   *
   * @ const std::string& path: The path to the file
   *
   * @ return bool: If the file exist on said path
   */
  inline bool IsValidFile(const std::string& path)
  {
    std::ifstream file(path);
    return file.good();
  }

  /**
   * Check is a file exist in a given path and if the given file extentions is also included in said file
   *
   * @ const std::string& filePath: The file path to the file
   * @ const std::vector<std::string>& validExtentions: Is a vector of all the valid file extentions the files can have
   *
   * @ return bool: if both of the statements are true
   */
  inline bool IsValidFile(const std::string& filePath, const std::vector<std::string>& validExtentions)
  {
    const std::filesystem::path path(filePath);
    const bool isValidExtention = std::find(std::begin(validExtentions), std::end(validExtentions),
                                            path.extension().string()) != std::end(validExtentions);
    const bool fileExist = IsValidFile(filePath);

    return fileExist && isValidExtention;
  }

  /**
   * This will split a string by a delimiter
   *
   * @ std::string s: The string to split
   * @ const std::string& delimiter: The character(s) that the string should be split with
   *
   * @ return std::vector<std::string>: A vector of the strings that is divided up by the delimiter
   */
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

  /**
   * If a vector is greater or equal to a value
   *
   * @ std::vector<T> vector: The vector we want to get the size of
   * @ std::unsigned int value: The amount we want to check from
   *
   * @ return bool: if the vector size is greater or equal to value
   */
  template <typename T>
  inline bool VectorIsGreaterOrEqualTo(std::vector<T> vector, unsigned int value)
  {
    return vector.size() >= value;
  }

  /**
   * Checks if an item is contained in a vector
   *
   * @ cosnt std::vector<T>& vector: The vector we want to search
   * @ const T& item: The item we want to look up
   *
   * @ return bool: If the item is contained in vectector
   */
  template <typename T>
  inline bool ContainsInVector(const std::vector<T>& vector, const T& item)
  {
    return std::find(vector.begin(), vector.end(), item) != vector.end();
  }

  /**
   * Generates a texture from a filepath that is given
   *
   * @ const std::string& filePath: The file path where the texture is located at
   *
   * @ return unsigned int: The texture Id that was genereated
   */
  inline unsigned int GenerateTexture(const std::string& filePath)
  {
    stbi_set_flip_vertically_on_load(true);

    unsigned int id;
    int width, height, channels;
    // generate texture object
    glGenTextures(1, &id);

    // load the texture data
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
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

      // set texture optionsmesh
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else LOG_WARNING("Could not load in texture: {}", filePath);

    // Once we have called `glTexImage2D`, we have bound the texture object and
    // attached an image to it. We can generate a Mipmap with `glGenerateMipmap`,
    // and after that has been done it is good practice to free the image memory
    stbi_image_free(data);

    return id;
  }

  /**
   * Generates a texture from a filepath that is given
   *
   * @ const std::string& filename: the file name of the texture
   * @ const std::string& directory: the directory where the texture is located at
   *
   * @ return unsigned int: The texture Id that was genereated
   */
  inline unsigned int GenerateTexture(const std::string& filename, const std::string& directory)
  {
    const std::string path = directory + '/' + filename;
    return GenerateTexture(path);
  }

  /**
   * Replaces all characters in a string
   *
   * @ std::string str: The string we want to modify
   * @ const std::string& from: The character(s) we want to replace
   * @ const std::string& to: The character(s) we want to replace with
   *
   * @ return std::string: The new string with the changed character(s)
   */
  inline std::string ReplaceAllChar(std::string str, const std::string& from, const std::string& to)
  {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
      str.replace(start_pos, from.length(), to);
      start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
  }


} // namespace LumiCore::Utility
