#pragma once

#include "../Mesh.h"
#include <iostream>
#include <vector>

namespace LumiCore3D {

class Cube : public LumiCore::Mesh {
public:
  Cube()
      : Mesh(Cube::GenerateVertexArray(), Cube::GenerateIndicies(),
             GL_TRIANGLES) {};

private:
  inline std::vector<float> GenerateVertexArray() {
    return std::vector<float>{-1.f, -1.f, -1.f, 1.0f, 0.0f, 0.0f, -1.f, 1.f,
                              -1.f, 0.0f, 1.0f, 0.0f, 1.f,  1.f,  -1.f, 0.0f,
                              1.0f, 0.0f, 1.f,  -1.f, -1.f, 0.0f, 0.0f, 1.0f};
  }
  inline std::vector<unsigned int> GenerateIndicies() {
    return std::vector<unsigned int>{0, 1, 2, 0, 3, 2};
  }
};

} // namespace LumiCore3D
