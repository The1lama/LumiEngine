#pragma once

#include <vector>
#include "../Mesh.h"

namespace LumiCore3D
{

class Cube : public LumiCore::Mesh
{
  public:
  Cube(const std::vector<LumiCore::Texture> textures) : LumiCore::Mesh(GenerateMeshData(textures), GL_TRIANGLES) {};

  private:
  static inline LumiCore::MeshData GenerateMeshData(const std::vector<LumiCore::Texture> textures)
  {
    LumiCore::MeshData data;

    data.vertecies = std::move(Cube::GenerateVertexArray());
    data.indices = std::move(Cube::GenerateIndicies());
    data.textures = std::move(textures);

    return data;
  };

  static inline std::vector<LumiCore::Vertex> GenerateVertexArray()
  {
    return std::vector<LumiCore::Vertex>{
        // Forward side // Position // // Normal // UV coordinate //
        LumiCore::Vertex(1.f, 1.f, 1.f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f), // TOP LEFT
        LumiCore::Vertex(1.f, 1.f, -1.f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f), // TOP RIGHT
        LumiCore::Vertex(1.f, -1.f, -1.f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f), // BOTTOM RIGHT
        LumiCore::Vertex(1.f, -1.f, 1.f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f), // BOTTOM LEFT

        // Back side
        LumiCore::Vertex(-1.f, 1.f, -1.f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f), // TOP LEFT
        LumiCore::Vertex(-1.f, 1.f, 1.f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f), // TOP RIGHT
        LumiCore::Vertex(-1.f, -1.f, 1.f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f), // BOTTOM RIGHT
        LumiCore::Vertex(-1.f, -1.f, -1.f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f), // BOTTOM LEFT

        // Right side
        LumiCore::Vertex(-1.f, 1.f, 1.f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f), // TOP LEFT
        LumiCore::Vertex(1.f, 1.f, 1.f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f), // TOP RIGHT
        LumiCore::Vertex(1.f, -1.f, 1.f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f), // BOTTOM RIGHT
        LumiCore::Vertex(-1.f, -1.f, 1.f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f), // BOTTOM LEFT

        // Left side
        LumiCore::Vertex(1.f, 1.f, -1.f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // TOP LEFT
        LumiCore::Vertex(-1.f, 1.f, -1.f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f), // TOP RIGHT
        LumiCore::Vertex(-1.f, -1.f, -1.f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f), // BOTTOM RIGHT
        LumiCore::Vertex(1.f, -1.f, -1.f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f), // BOTTOM LEFT

        // Top side
        LumiCore::Vertex(-1.f, 1.f, -1.f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f), // TOP LEFT
        LumiCore::Vertex(1.f, 1.f, -1.f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f), // TOP RIGHT
        LumiCore::Vertex(1.f, 1.f, 1.f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f), // BOTTOM RIGHT
        LumiCore::Vertex(-1.f, 1.f, 1.f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f), // BOTTOM LEFT

        // Bottom side
        LumiCore::Vertex(-1.f, -1.f, 1.f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f), // TOP LEFT
        LumiCore::Vertex(1.f, -1.f, 1.f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f), // TOP RIGHT
        LumiCore::Vertex(1.f, -1.f, -1.f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f), // BOTTOM RIGHT
        LumiCore::Vertex(-1.f, -1.f, -1.f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f) // BOTTOM LEFT
    };
  }
  static inline std::vector<unsigned int> GenerateIndicies()
  {
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
