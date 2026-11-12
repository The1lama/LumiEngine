#pragma once

#include <string>

namespace LumiCore {

class Texture {
private:
  unsigned int m_Id;
  int m_width, m_height, m_channels;

  void Generate(const std::string &path);

protected:
public:
  Texture(const std::string &path);
  ~Texture() = default;

  const unsigned int GetId() const;
  bool IsValid() const;

  int GetWidth() const;
  int GetHeight() const;
  int GetChannels() const;
};

} // namespace LumiCore
