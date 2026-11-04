#pragma once

#include "../Mesh.h"
#include <vector>

namespace LumiCore3D {

class Cube : public LumiCore::Mesh {
public:
  Cube()
      : Mesh(Cube::GenerateVertexArray(), Cube::GenerateIndicies(),
             GL_TRIANGLES) {};

private:
  static inline std::vector<LumiCore::Vertex> GenerateVertexArray() {
    // clang-format off
    return std::vector<LumiCore::Vertex>{
      // Forward side   // Position //
      LumiCore::Vertex( 1.f,  1.f,  1.f), 
      LumiCore::Vertex( 1.f,  1.f, -1.f), 
      LumiCore::Vertex( 1.f, -1.f, -1.f), 
      LumiCore::Vertex( 1.f, -1.f,  1.f), 

      // Back side
      LumiCore::Vertex(-1.f,  1.f, -1.f),
      LumiCore::Vertex(-1.f,  1.f,  1.f),
      LumiCore::Vertex(-1.f, -1.f,  1.f),
      LumiCore::Vertex(-1.f, -1.f, -1.f),

      // Right side
      LumiCore::Vertex(-1.f,  1.f,  1.f),
      LumiCore::Vertex( 1.f,  1.f,  1.f),
      LumiCore::Vertex( 1.f, -1.f,  1.f),
      LumiCore::Vertex(-1.f, -1.f,  1.f),

      // Left side
      LumiCore::Vertex( 1.f,  1.f, -1.f), 
      LumiCore::Vertex(-1.f,  1.f, -1.f),
      LumiCore::Vertex(-1.f, -1.f, -1.f),
      LumiCore::Vertex( 1.f, -1.f, -1.f),

      // Top side
      LumiCore::Vertex(-1.f, 1.f, -1.f),
      LumiCore::Vertex( 1.f, 1.f, -1.f),
      LumiCore::Vertex( 1.f, 1.f,  1.f),
      LumiCore::Vertex(-1.f, 1.f,  1.f),

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
      0,1,2, 
      0,3,2,
      // Back Side
      4,5,6,
      4,7,6,
      //Right Side
      8,9,10,
      8,11,10,
      //Left Side
      12,13,14,
      12,15,14,
      // Top Side
      16,17,18,
      16,19,18,
      //Bottom Side
      20,21,22,
      20,23,22
    };
    // clang-format on
  }
};

} // namespace LumiCore3D
