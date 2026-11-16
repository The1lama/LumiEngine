#include "ResourceManager.h"
#include <format>
#include <fstream>
#include <memory>
#include <sstream>

#include "Model.h"
#include "Utility.h"
#include "logger/Logger.h"

namespace LumiCore::Resources
{


std::shared_ptr<LumiCore::Model> ResourceManager::GetModel(const std::string& path,
                                                           const std::vector<LumiCore::Texture>& textures)
{
  LOG_INFO(std::format("-- Getting Model: {}", path));
  // Searches for if the model from path has been loaded before

  // If it has return the model
  if (auto model = m_modelResources.find(path); model != m_modelResources.end())
  {
    // return the model
    return model->second.lock();
  }

  // - Else create the model
  // - add it to the map
  // - return the model

  // This way we don't use the emplace as it will construct a new object, and we keep the same object but only have a
  // weak ptr reference to it in this unordered map
  std::shared_ptr<Model> model = GenerateObject(path, textures);
  m_modelResources.insert({path, model});
  return model;
}

std::shared_ptr<Model> ResourceManager::CustomObjectLoader(const std::string& path,
                                                           std::vector<LumiCore::Vertex>& vertices,
                                                           std::vector<unsigned int>& indices,
                                                           const std::vector<LumiCore::Texture>& textures)
{
  std::vector<glm::vec3> temp_positions;
  std::vector<glm::vec3> temp_normals;
  std::vector<glm::vec2> temp_uvs;

  std::ifstream file(path);
  if (!file.is_open())
  {
    LOG_CRITICAL(std::format("Failed to open OBJ file: {}", path));
    return nullptr;
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

      temp_positions.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
    }
    else if (prefix == std::string("vn"))
    {
      std::string x;
      std::string y;
      std::string z;
      ss >> x;
      ss >> y;
      ss >> z;

      temp_normals.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
    }

    else if (prefix == std::string("vt"))
    {
      std::string x;
      std::string y;
      ss >> x;
      ss >> y;

      temp_uvs.push_back(glm::vec2(std::stof(x), std::stof(y)));
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

        const LumiCore::Vertex vertex{temp_positions[posIndex], temp_normals[normIndex], temp_uvs[uvIndex]};
        vertices.push_back(vertex);
      }

      // assemble indecies order
      if (faces.size() == 3)
      {
        const int first = vertices.size() - 3;
        const int second = vertices.size() - 2;
        const int third = vertices.size() - 1;
        indices.push_back(first);
        indices.push_back(second);
        indices.push_back(third);
      }
      if (faces.size() == 4)
      {
        const int first = vertices.size() - 4;
        const int second = vertices.size() - 3;
        const int third = vertices.size() - 2;
        const int fourth = vertices.size() - 1;

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

  return std::make_shared<LumiCore::Model>(vertices, indices, textures);
}

std::shared_ptr<Model> ResourceManager::GenerateModel(const std::string& path)
{
  return std::make_shared<LumiCore::Model>(path.c_str());
}

std::shared_ptr<Model> ResourceManager::GenerateObject(const std::string& path,
                                                       const std::vector<LumiCore::Texture>& textures)
{
  constexpr bool useCustomObjectLoader = false;
  //
  // Validate check if the file extension is correct and if file exist
  const std::vector<std::string> validFileExtentsions = {".obj"};
  if (!LumiCore::Utility::IsValidFile(path, validFileExtentsions))
  {
    LOG_WARNING(std::format("File {} is not valid", path));
    return nullptr;
  };

  LOG_INFO(std::format("Generating model: {}", path));

  if (useCustomObjectLoader)
  {
    std::vector<LumiCore::Vertex> vertices;
    std::vector<unsigned int> indices;

    return CustomObjectLoader(path, vertices, indices, textures);
  }
  else
  {
    return GenerateModel(path);
  }
}

} // namespace LumiCore::Resources
