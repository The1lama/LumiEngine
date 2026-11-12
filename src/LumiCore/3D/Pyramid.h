#pragma once

#include <vector>
#include "../Mesh.h"

namespace LumiCore3D
{

class Pyramid : public LumiCore::Mesh
{
  public:
  Pyramid() : Mesh(GenerateMeshData(), GL_TRIANGLES) {};

  private:
  static inline LumiCore::MeshData GenerateMeshData()
  {
    LumiCore::MeshData data;

    data.vertecies = std::move(Pyramid::GenerateVertexArray());
    data.indices = std::move(Pyramid::GenerateIndicies());

    return data;
  };

  static inline std::vector<LumiCore::Vertex> GenerateVertexArray()
  {
    // clang-format off
    return std::vector<LumiCore::Vertex>{
      //                 // Position // // Normal // UV COORD //
      // Forward side
      LumiCore::Vertex( 0.f,  1.f,  0.f, 1.0f, 0.5f, 0.0f, 0.5f, 0.5f), // TOP
      LumiCore::Vertex( 1.f, -1.f, -1.f, 1.0f, 0.5f, 0.0f, 1.0f, 0.0f), // Bottom RIGHT
      LumiCore::Vertex( 1.f, -1.f,  1.f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f), // BOTTOM LEFT

      // Back side
      LumiCore::Vertex( 0.f,  1.f,  0.f, -1.0f, 0.5f, 0.0f, 0.5f, 0.5f), // TOP
      LumiCore::Vertex(-1.f, -1.f,  1.f, -1.0f, 0.5f, 0.0f, 1.0f, 0.0f), // Bottom RIGHT
      LumiCore::Vertex(-1.f, -1.f, -1.f, -1.0f, 0.5f, 0.0f, 0.0f, 0.0f), // BOTTOM LEFT

      // Right side
      LumiCore::Vertex( 0.f,  1.f,  0.f, 0.0f, 0.5f, 1.0f, 0.5f, 0.5f), // TOP
      LumiCore::Vertex( 1.f, -1.f,  1.f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f), // Bottom RIGHT
      LumiCore::Vertex(-1.f, -1.f,  1.f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f), // BOTTOM LEFT

      // Left side
      LumiCore::Vertex( 0.f,  1.f,  0.f, 0.0f, 0.5f, -1.0f, 0.5f, 0.5f), // TOP
      LumiCore::Vertex(-1.f, -1.f, -1.f, 0.0f, 0.5f, -1.0f, 1.0f, 0.0f), // Bottom RIGHT
      LumiCore::Vertex( 1.f, -1.f, -1.f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f), // BOTTOM LEFT

      // Bottom side
      LumiCore::Vertex(-1.f, -1.f,  1.f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f), // TOP LEFT
      LumiCore::Vertex( 1.f, -1.f,  1.f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f), // TOP RIGHT
      LumiCore::Vertex( 1.f, -1.f, -1.f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f), // BOTTOM RIGHT
      LumiCore::Vertex(-1.f, -1.f, -1.f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f)  // BOTTOM LEFT
    };
    // clang-format on
  }
  static inline std::vector<unsigned int> GenerateIndicies()
  {
    // clang-format off
    return std::vector<unsigned int>{
      // Front Side
      0,2,1,
      // Back Side
      3,5,4,
      // Right Side
      6,8,7,
      // Left Side
      9,11,10,
      // Bottom Side
      12,14,13,
      12,15,14
    };
    // clang-format on
  }
};

} // namespace LumiCore3D
