#pragma once

#include "../Mesh.h"

class Quad : public Mesh {
public:
  Quad()
      : Mesh(Quad::GenerateVertexArray(), Quad::GenerateIndicies(),
             GL_TRIANGLES) {};

private:
  inline std::vector<float> GenerateVertexArray() {
    return std::vector<float>{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
                              -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f,
                              0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,
                              0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f};
  }
  inline std::vector<unsigned int> GenerateIndicies() {
    return std::vector<unsigned int>{0, 1, 2, 0, 3, 2};
  }
};
