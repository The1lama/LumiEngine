#pragma once


#include "Mesh.h"
#include "assimp/material.h"

class Shader;
class Mesh;

struct aiNode;
struct aiMesh;
struct aiScene;

namespace LumiCore::Resources
{
  struct AssetManagers;
} // namespace LumiCore::Resources

namespace LumiCore
{

  class Model
  {
  public:
    Model(const std::string& path, Resources::AssetManagers& resources);

    void Draw(const Shader& shader);
    bool IsValid() const;

  private:
    std::vector<Mesh> m_meshes;
    std::string m_directory;

    void SaveInputedMesh(const std::vector<Vertex>& vertecies, const std::vector<unsigned int>& indices,
                         const std::vector<Texture>& textures);


    void LoadModel(const std::string& path, Resources::AssetManagers& resources);
    void ProccessNode(aiNode* node, const aiScene* scene, Resources::AssetManagers& resources);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, Resources::AssetManagers& resources);

    void AssLoader(const std::string& path, Resources::AssetManagers& resources);
    void CustomLoader(const std::string& path, Resources::AssetManagers& resources);

    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName,
                                              Resources::AssetManagers& resources);
  };

} // namespace LumiCore
