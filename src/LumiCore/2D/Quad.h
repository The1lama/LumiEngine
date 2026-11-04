#pragma once

#include "../Mesh.h"

namespace LumiCore2D {

class Quad : public LumiCore::Mesh {
public:
  Quad() : Mesh(GenerateMeshData(), GL_TRIANGLES) {};

private:
  static inline LumiCore::MeshData GenerateMeshData() {
    LumiCore::MeshData data;

    data.vertecies = std::move(Quad::GenerateVertexArray());
    data.indices = std::move(Quad::GenerateIndicies());

    return data;
  };

  static inline std::vector<LumiCore::Vertex> GenerateVertexArray() {
    // clang-format off
    return std::vector<LumiCore::Vertex>{
      glm::vec3(-0.5f, -0.5f, 0.0f),
      glm::vec3(-0.5f,  0.5f, 0.0f),
      glm::vec3( 0.5f,  0.5f, 0.0f),
      glm::vec3( 0.5f, -0.5f, 0.0f)
    };
    // clang-format on
  }
  static inline std::vector<unsigned int> GenerateIndicies() {
    return std::vector<unsigned int>{0, 1, 2, 0, 3, 2};
  }
};

} // namespace LumiCore2D
