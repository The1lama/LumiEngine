#pragma once

#include "../Mesh.h"
#include <vector>

namespace LumiCore3D {

class Pyramid : public LumiCore::Mesh {
public:
  Pyramid() : Mesh(GenerateMeshData(), GL_TRIANGLES) {};

private:
  static inline LumiCore::MeshData GenerateMeshData() {
    LumiCore::MeshData data;

    data.vertecies = std::move(Pyramid::GenerateVertexArray());
    data.indices = std::move(Pyramid::GenerateIndicies());

    return data;
  };

  static inline std::vector<LumiCore::Vertex> GenerateVertexArray() {
    // clang-format off
    return std::vector<LumiCore::Vertex>{
      // TOP Vertex      // Position //
      LumiCore::Vertex( 0.f,  1.f,  0.f), 

      // Forward side
      LumiCore::Vertex( 1.f, -1.f, -1.f), 
      LumiCore::Vertex( 1.f, -1.f,  1.f), 

      // Back side
      LumiCore::Vertex(-1.f, -1.f,  1.f),
      LumiCore::Vertex(-1.f, -1.f, -1.f),

      // Right side
      LumiCore::Vertex( 1.f, -1.f,  1.f),
      LumiCore::Vertex(-1.f, -1.f,  1.f),

      // Left side
      LumiCore::Vertex(-1.f, -1.f, -1.f), 
      LumiCore::Vertex( 1.f, -1.f, -1.f),

      // Bottom side
      LumiCore::Vertex(-1.f, -1.f,  1.f),
      LumiCore::Vertex( 1.f, -1.f,  1.f),
      LumiCore::Vertex( 1.f, -1.f, -1.f),
      LumiCore::Vertex(-1.f, -1.f, -1.f)
    };
    // clang-format on
  }
  static inline std::vector<unsigned int> GenerateIndicies() {
    // clang-format off
    return std::vector<unsigned int>{
      // Front Side
      0,2,1,
      // Back Side
      0,4,3,
      // Right Side
      0,6,5,
      // Left Side
      0,8,7,
      // Bottom Side
      9,11,10,
      9,12,11
    };
    // clang-format on
  }
};

} // namespace LumiCore3D
