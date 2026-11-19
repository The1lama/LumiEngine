#include "Model.h"

#include "3D/GeneratedMesh.h"
#include "EngineSettings.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Utility.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "logger/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <format>
#include <string>
#include <vector>

namespace LumiCore
{

  Model::Model(const std::string& path, Resources::AssetManagers& resources)
  {
    LoadModel(path, resources);
  }

  void Model::Draw(const Shader& shader)
  {
    for (auto& mesh : m_meshes)
    {
      mesh.Render(shader);
    }
  }
  bool Model::IsValid() const
  {
    return !m_meshes.empty();
  }


  void Model::SaveInputedMesh(const std::vector<Vertex>& vertecies, const std::vector<unsigned int>& indices,
                              const std::vector<Texture>& textures)
  {
  }

  void Model::LoadModel(const std::string& path, Resources::AssetManagers& resources)
  {
    if (!LumiCore::Utility::IsValidFile(path, LumiEngine::ValidObjectFileExtentsions))
    {
      LOG_WARNING(std::format("File {} is not valid", path));
      return;
    };

    LOG_INFO(std::format("Generating model: {}", path));

    if (LumiEngine::useCustomObjectLoader) CustomLoader(path, resources);
    else AssLoader(path, resources);
  }


  void Model::ProccessNode(aiNode* node, const aiScene* scene, Resources::AssetManagers& resources)
  {

    // Process all the different meshes the model has, and adds them to a list that we can use for rendering out the
    // meshes later, but we need to process the mesh from Assimp to fit our structure
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      m_meshes.push_back(ProcessMesh(mesh, scene, resources));
    }

    // Then we check if this node has any children and process them also

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
      ProccessNode(node->mChildren[i], scene, resources);
    }
  }

  Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, Resources::AssetManagers& resources)
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
      else vertex.uvCoord = glm::vec2(0.0f, 0.0f);

      vertecies.push_back(vertex);
    }

    // Process Indices
    // it irate through all the faces on the mesh
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
      // Then we add each vertex index to the indices list that would combine to one face, that is only three vertexes,
      // as we set the option for `aiProcess_Triangulate` to generate faces with three vertexes
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

      std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", resources);
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

      std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular", resources);
      textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

      std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, "normal", resources);
      textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }

    // Returns a finished created mesh for the model
    return LumiCore3D::GenMesh(vertecies, indices, textures);
  }

  void Model::AssLoader(const std::string& path, Resources::AssetManagers& resources)
  {
    Assimp::Importer importer;

    // This generates and fixes up the model that we want to import, we can extract out the data it generated to and
    // move the data to fit our formatting
    const aiScene* scene = importer.ReadFile(
        path, aiProcessPreset_TargetRealtime_Fast | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    // check if error occurred
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
      LOG_CRITICAL(std::format("ASSIMP could not generate model: {}", importer.GetErrorString()));
      return;
    }

    m_directory = path.substr(0, path.find_last_of('/'));

    m_meshes.reserve(scene->mNumMeshes);

    ProccessNode(scene->mRootNode, scene, resources);
  }

  void Model::CustomLoader(const std::string& path, Resources::AssetManagers& resources)
  {
    std::vector<glm::vec3> tempPositions;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempUvs;

    std::vector<Vertex> vertecies;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    std::ifstream file(path);
    if (!file.is_open())
    {
      LOG_CRITICAL(std::format("Failed to open OBJ file: {}", path));
      return;
    }

    // this will read the line in the file, line by line
    std::string line;
    while (std::getline(file, line))
    {

      // we get the current line and save it to the ss variable
      std::stringstream ss(line);
      std::string prefix;
      // We skip leading spaces and will read directly into a string until we hit
      // whitespaces on the row. We then extract said string with the >> to it is
      // gone and save the string to the prefix variable.
      ss >> prefix;

      if (prefix == std::string("v"))
      {
        std::string x;
        std::string y;
        std::string z;
        ss >> x;
        ss >> y;
        ss >> z;

        tempPositions.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
      }
      else if (prefix == std::string("vn"))
      {
        std::string x;
        std::string y;
        std::string z;
        ss >> x;
        ss >> y;
        ss >> z;

        tempNormals.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
      }

      else if (prefix == std::string("vt"))
      {
        std::string x;
        std::string y;
        ss >> x;
        ss >> y;

        tempUvs.push_back(glm::vec2(std::stof(x), std::stof(y)));
      }
      // Create vertexes and indices
      else if (prefix == std::string("f"))
      {

        std::vector<std::string> faces;
        std::string faceLine;
        while (ss >> faceLine)
        {
          faces.push_back(faceLine);
        }

        // only proccess if we have more than 3 faces
        if (faces.size() < 3 || faces.size() > 4)
        {
          continue;
        }
        // assemble the faces

        // 1. Loop through the faces vector
        // 2. Split up the string by /
        // 3. Each value goes from vertex position/uv/normal index: remember that
        // .OBJ starts from 1 so we need to do -1
        // 4. Create the indexes for the face

        // assemble vertex data
        for (int i = 0; i < faces.size(); ++i)
        {
          std::vector<std::string> values = LumiCore::Utility::SplitString(faces[i], "/");

          const int posIndex = std::stoi(values[0]) - 1;
          const int uvIndex = std::stoi(values[1]) - 1;
          const int normIndex = std::stoi(values[2]) - 1;

          const LumiCore::Vertex vertex{tempPositions[posIndex], tempNormals[normIndex], tempUvs[uvIndex]};
          vertecies.push_back(vertex);
        }

        // assemble indecies order
        if (faces.size() == 3)
        {
          const int first = vertecies.size() - 3;
          const int second = vertecies.size() - 2;
          const int third = vertecies.size() - 1;
          indices.push_back(first);
          indices.push_back(second);
          indices.push_back(third);
        }
        if (faces.size() == 4)
        {
          const int first = vertecies.size() - 4;
          const int second = vertecies.size() - 3;
          const int third = vertecies.size() - 2;
          const int fourth = vertecies.size() - 1;

          // first triangle
          indices.push_back(first);
          indices.push_back(second);
          indices.push_back(third);

          // second triangle
          indices.push_back(fourth);
          indices.push_back(first);
          indices.push_back(third);
        }
      }
      else if (prefix == std::string("o"))
      {
        //
        std::string name;
        ss >> name;
        LOG_INFO(std::format("Object name: {}", name));
      }

    } // while

    // LOG_INFO(std::format("Position   amount: {}", temp_positions.size()));
    // LOG_INFO(std::format("Normal     amount: {}", temp_normals.size()));
    // LOG_INFO(std::format("UVs        amount: {}", temp_uvs.size()));

    // LOG_INFO(std::format("Vertexes   amount: {}", vertices.size()));
    // LOG_INFO(std::format("Indices    amount: {}", indices.size()));

    // Returns a finished created mesh for the model
    m_meshes.push_back(LumiCore3D::GenMesh(vertecies, indices, textures));
  }

  std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName,
                                                   Resources::AssetManagers& resources)
  {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
    {
      aiString str;
      mat->GetTexture(type, i, &str);

      // checck if we already have generated the texture once before
      std::string texturePath = str.C_Str() + m_directory;
      if (!resources.Textures.Contains(texturePath))
      {
        Texture texture;
        texture.id = LumiCore::Utility::GenerateTexture(str.C_Str(), m_directory); // Generate texture
        texture.type = typeName;
        texture.path = str.C_Str();
        resources.Textures.Create(texturePath, texture);
      }

      textures.push_back(resources.Textures.Get(texturePath, resources));
    }

    return textures;
  }

} // namespace LumiCore
