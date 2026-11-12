#include "Texture.h"
#include "Utility.h"
#include "logger/Logger.h"

#include <format>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace LumiCore
{

Texture::Texture(const std::string& path)
{
  // Check if the file is valid
  if (!Utility::IsValidFile(path))
  {
    LOG_WARNING(std::format("File path is not valid: {}", path));
    return;
  }

  Generate(path);
}

void Texture::Generate(const std::string& path)
{
  stbi_set_flip_vertically_on_load(true);
  // generate texture object
  glGenTextures(1, &m_Id);
  glBindTexture(GL_TEXTURE_2D, m_Id);

  // set texture options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // load the texture data
  unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);

  if (!data)
  {
    LOG_WARNING(std::format("Could not load in texture: {}", path));
    return;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Once we have called `glTexImage2D`, we have bound the texture object and
  // attached an image to it. We can generate a Mipmap with `glGenerateMipmap`,
  // and after that has been done it is good practice to free the image memory
  stbi_image_free(data);
}

bool Texture::IsValid() const { return m_Id != 0; }

#pragma region Getters
const unsigned int Texture::GetId() const { return m_Id; }
int Texture::GetWidth() const { return m_width; }
int Texture::GetHeight() const { return m_height; }
int Texture::GetChannels() const { return m_channels; }
#pragma endregion

} // namespace LumiCore
