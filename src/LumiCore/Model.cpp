#include "Model.h"

#include "3D/GeneratedMesh.h"
#include "Mesh.h"
#include "Shader.h"
#include "Utility.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "logger/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <format>
#include <vector>

namespace LumiCore
{

Model::Model(const char* path) { LoadModel(path); }
Model::Model(const std::vector<Vertex>& vertecies, const std::vector<unsigned int>& indices,
             const std::vector<Texture>& textures)
{
  SaveInputedMesh(vertecies, indices, textures);
}

void Model::Draw(const Shader& shader)
{
  for (auto& mesh : m_Meshes)
  {
    mesh.Render(shader);
  }
}


void Model::SaveInputedMesh(const std::vector<Vertex>& vertecies, const std::vector<unsigned int>& indices,
                            const std::vector<Texture>& textures)
{
  // Returns a finished created mesh for the model
  m_Meshes.push_back(LumiCore3D::GenMesh(vertecies, indices, textures));
}
void Model::LoadModel(const std::string& path)
{
  Assimp::Importer importer;

  // This generates and fixes up the model that we want to import, we can extract out the data it generated to and move
  // the data to fit our formatting
  const aiScene* scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcessPreset_TargetRealtime_Fast);

  // check if error occurred
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    LOG_CRITICAL(std::format("ASSIMP could not generate model: {}", importer.GetErrorString()));
    return;
  }

  m_Directory = path.substr(0, path.find_last_of('/'));

  m_Meshes.reserve(scene->mNumMeshes);

  ProccessNode(scene->mRootNode, scene);
}


void Model::ProccessNode(aiNode* node, const aiScene* scene)
{

  // Process all the different meshes the model has, and adds them to a list that we can use for rendering out the
  // meshes later, but we need to process the mesh from Assimp to fit our structure
  for (unsigned int i = 0; i < node->mNumMeshes; ++i)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    m_Meshes.push_back(ProcessMesh(mesh, scene));
  }

  // Then we check if this node has any children and process them also

  for (unsigned int i = 0; i < node->mNumChildren; ++i)
  {
    ProccessNode(node->mChildren[i], scene);
  }
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
  std::vector<Vertex> vertecies;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  // Generate Vertexes
  for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
  {
    Vertex vertex;
    vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

    // Check if the mesh has UV coordinates
    if (mesh->mTextureCoords[0])
      vertex.uvCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    else
      vertex.uvCoord = glm::vec2(0.0f, 0.0f);

    vertecies.push_back(vertex);
  }

  // Process Indices
  // it irate through all the faces on the mesh
  for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
  {
    // Then we add each vertex index to the indices list that would combine to one face, that is only three vertexes, as
    // we set the option for `aiProcess_Triangulate` to generate faces with three vertexes
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; ++j)
    {
      indices.push_back(face.mIndices[j]);
    }
  }

  // Process textures
  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());


    std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  // Returns a finished created mesh for the model
  return LumiCore3D::GenMesh(vertecies, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
  {
    aiString str;
    mat->GetTexture(type, i, &str);

    Texture texture;
    texture.id = LumiCore::Utility::GenerateTexture(str.C_Str(), m_Directory); // Generate texture
    texture.type = typeName;
    texture.path = str.C_Str();

    textures.push_back(texture);
  }

  return textures;
}

} // namespace LumiCore
