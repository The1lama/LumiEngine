#pragma once


#include "Mesh.h"

class Shader;
class Mesh;

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiMaterial;
enum aiTextureType;

namespace LumiCore
{

class Model
{

public:
  Model(const char* path);
  Model(const std::vector<Vertex>& vertecies, const std::vector<unsigned int>& indices,
        const std::vector<Texture>& textures);

  void Draw(const Shader& shader);

private:
  std::vector<Mesh> m_Meshes;
  std::string m_Directory;

  void SaveInputedMesh(const std::vector<Vertex>& vertecies, const std::vector<unsigned int>& indices,
                       const std::vector<Texture>& textures);


  void LoadModel(const std::string& path);
  void ProccessNode(aiNode* node, const aiScene* scene);
  Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

  std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

} // namespace LumiCore
