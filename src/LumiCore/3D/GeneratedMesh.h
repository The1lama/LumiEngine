#pragma once

#include "../Mesh.h"

namespace LumiCore3D
{
  class GenMesh : public LumiCore::Mesh
  {

  public:
    GenMesh(const std::vector<LumiCore::Vertex>& vertecies, const std::vector<unsigned int>& indices,
            const std::vector<LumiCore::Texture>& textures) :
        Mesh(GenerateMeshData(vertecies, indices, textures), GL_TRIANGLES) {};

  private:
    static inline LumiCore::MeshData GenerateMeshData(const std::vector<LumiCore::Vertex>& vert,
                                                      const std::vector<unsigned int>& indeces,
                                                      const std::vector<LumiCore::Texture>& textures)
    {
      LumiCore::MeshData data;

      data.Vertecies = std::move(vert);
      data.Indices = std::move(indeces);
      data.Textures = std::move(textures);

      return data;
    };
  };

} // namespace LumiCore3D
