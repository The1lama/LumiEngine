#pragma once

#include "../Mesh.h"

namespace LumiCore2D {

class Triangle : public LumiCore::Mesh {
public:
  Triangle()
      : Mesh(Triangle::GenerateVertexArray(), Triangle::GenerateIndicies(),
             GL_TRIANGLES) {};

private:
  inline std::vector<float> GenerateVertexArray() {
    return std::vector<float>{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
                              0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
                              -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f};
  }
  inline std::vector<unsigned int> GenerateIndicies() {
    return std::vector<unsigned int>{0, 1, 2};
  }
};

} // namespace LumiCore2D
