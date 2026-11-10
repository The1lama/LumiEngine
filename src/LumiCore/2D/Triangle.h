#pragma once

#include "../Mesh.h"

namespace LumiCore2D {

class Triangle : public LumiCore::Mesh {
public:
  Triangle() : Mesh(GenerateMeshData(), GL_TRIANGLES) {};

private:
  static inline LumiCore::MeshData GenerateMeshData() {
    LumiCore::MeshData data;

    data.vertecies = std::move(Triangle::GenerateVertexArray());
    data.indices = std::move(Triangle::GenerateIndicies());

    return data;
  };

  static inline std::vector<LumiCore::Vertex> GenerateVertexArray() {
    // clang-format off
    return std::vector<LumiCore::Vertex>{
                          // Position //                      // Normal //
      LumiCore::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
      LumiCore::Vertex(glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
      LumiCore::Vertex(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    };
    // clang-format on
  }
  static inline std::vector<unsigned int> GenerateIndicies() {
    return std::vector<unsigned int>{0, 1, 2};
  }
};

} // namespace LumiCore2D
