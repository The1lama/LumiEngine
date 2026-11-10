#pragma once

#include "../Mesh.h"
#include <vector>

namespace LumiCore3D {

class Cube : public LumiCore::Mesh {
public:
  Cube() : LumiCore::Mesh(GenerateMeshData(), GL_TRIANGLES) {};

private:
  static inline LumiCore::MeshData GenerateMeshData() {
    LumiCore::MeshData data;

    data.vertecies = std::move(Cube::GenerateVertexArray());
    data.indices = std::move(Cube::GenerateIndicies());

    return data;
  };

  static inline std::vector<LumiCore::Vertex> GenerateVertexArray() {
    // clang-format off
    return std::vector<LumiCore::Vertex>{
      // Forward side   // Position //  // Normal //
      LumiCore::Vertex( 1.f,  1.f,  1.f, 1.0f, 0.0f, 0.0f), 
      LumiCore::Vertex( 1.f,  1.f, -1.f, 1.0f, 0.0f, 0.0f), 
      LumiCore::Vertex( 1.f, -1.f, -1.f, 1.0f, 0.0f, 0.0f), 
      LumiCore::Vertex( 1.f, -1.f,  1.f, 1.0f, 0.0f, 0.0f), 

      // Back side
      LumiCore::Vertex(-1.f,  1.f, -1.f, -1.0f, 0.0f, 0.0f),
      LumiCore::Vertex(-1.f,  1.f,  1.f, -1.0f, 0.0f, 0.0f),
      LumiCore::Vertex(-1.f, -1.f,  1.f, -1.0f, 0.0f, 0.0f),
      LumiCore::Vertex(-1.f, -1.f, -1.f, -1.0f, 0.0f, 0.0f),

      // Right side
      LumiCore::Vertex(-1.f,  1.f,  1.f, 0.0f, 0.0f, 1.0f),
      LumiCore::Vertex( 1.f,  1.f,  1.f, 0.0f, 0.0f, 1.0f),
      LumiCore::Vertex( 1.f, -1.f,  1.f, 0.0f, 0.0f, 1.0f),
      LumiCore::Vertex(-1.f, -1.f,  1.f, 0.0f, 0.0f, 1.0f),

      // Left side
      LumiCore::Vertex( 1.f,  1.f, -1.f, 0.0f, 0.0f, -1.0f), 
      LumiCore::Vertex(-1.f,  1.f, -1.f, 0.0f, 0.0f, -1.0f),
      LumiCore::Vertex(-1.f, -1.f, -1.f, 0.0f, 0.0f, -1.0f),
      LumiCore::Vertex( 1.f, -1.f, -1.f, 0.0f, 0.0f, -1.0f),

      // Top side
      LumiCore::Vertex(-1.f, 1.f, -1.f, 0.0f, 1.0f, 0.0f),
      LumiCore::Vertex( 1.f, 1.f, -1.f, 0.0f, 1.0f, 0.0f),
      LumiCore::Vertex( 1.f, 1.f,  1.f, 0.0f, 1.0f, 0.0f),
      LumiCore::Vertex(-1.f, 1.f,  1.f, 0.0f, 1.0f, 0.0f),

      // Bottom side
      LumiCore::Vertex(-1.f, -1.f,  1.f, 0.0f, -1.0f, 0.0f),
      LumiCore::Vertex( 1.f, -1.f,  1.f, 0.0f, -1.0f, 0.0f),
      LumiCore::Vertex( 1.f, -1.f, -1.f, 0.0f, -1.0f, 0.0f),
      LumiCore::Vertex(-1.f, -1.f, -1.f, 0.0f, -1.0f, 0.0f)
    };
    // clang-format on
  }
  static inline std::vector<unsigned int> GenerateIndicies() {
    // clang-format off
    return std::vector<unsigned int>{
      // Front Side
      0,2,1, 
      0,3,2,
      // Back Side
      4,6,5,
      4,7,6,
      //Right Side
      8,10,9,
      8,11,10,
      //Left Side
      12,14,13,
      12,15,14,
      // Top Side
      16,18,17,
      16,19,18,
      //Bottom Side
      20,22,21,
      20,23,22
    };
    // clang-format on
  }
};

} // namespace LumiCore3D
