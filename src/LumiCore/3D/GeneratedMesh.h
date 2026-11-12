#pragma once

#include "../Mesh.h"

namespace LumiCore3D {
class GenMesh : public LumiCore::Mesh {

public:
  GenMesh(std::vector<LumiCore::Vertex> vertecies,
          std::vector<unsigned int> indices)
      : Mesh(GenerateMeshData(vertecies, indices), GL_TRIANGLES) {};

private:
  static inline LumiCore::MeshData
  GenerateMeshData(std::vector<LumiCore::Vertex> vert,
                   std::vector<unsigned int> indeces) {
    LumiCore::MeshData data;

    data.vertecies = std::move(vert);
    data.indices = std::move(indeces);

    return data;
  };
};

} // namespace LumiCore3D
