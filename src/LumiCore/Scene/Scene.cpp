#include "Scene.h"

#include "../Utility.h"
#include "../logger/Logger.h"

namespace LumiCore::Scene
{

  bool Scene::Initialize(const std::string& newScene)
  {
    // check if file is correct
    auto fsPath = std::filesystem::path(newScene);
    if (fsPath.extension() != ".lscene") return false; // This have to be done before calling initialize but is here now

    m_filePath = newScene;

    // read file
    std::ifstream file(newScene);
    if (!file.is_open())
    {
      LOG_CRITICAL("Failed to open scene file: {}", newScene);
      return false;
    }

    ObjectType objectType = ObjectType::None;

    // This will read the line in the file, line by line
    std::string line;
    unsigned int row = -1;
    while (std::getline(file, line))
    {
      std::stringstream ss(line);
      std::string prefix;
      ss >> prefix;
      ++row;
      if (prefix == std::string("o")) // file name
      {
        std::string str;
        while (ss >> str || IsNotAComment(str))
        {
          m_sceneName = std::format("{} {}", m_sceneName, str);
        }
        LOG_DEBUG("Scene Loaded: {}", m_sceneName);
      }
      else if (prefix == std::string("sp")) // spawn point, is in a vector as there could be more SpawnPoints in scene
      {
        Objects::Transform transfrom;
        std::string stringTransform;

        // Get Position
        ss >> stringTransform;
        std::vector<std::string> positions = LumiCore::Utility::SplitString(stringTransform, ",");
        if (!LumiCore::Utility::VectorIsGreaterOrEqualTo(positions, 3))
        {
          LOG_WARNING("Could not load in {} row. As poition is wrongly formated", row);
          transfrom.Position = glm::vec3(0);
        }
        else transfrom.Position = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

        // Get Rotation
        ss >> stringTransform;
        positions = LumiCore::Utility::SplitString(stringTransform, ",");
        if (!LumiCore::Utility::VectorIsGreaterOrEqualTo(positions, 3))
        {
          LOG_WARNING("Could not load in row {}. As poition is wrongly formated", row);
          transfrom.Rotation = glm::vec3(0);
        }
        else transfrom.Rotation = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

        m_spawnPoints.emplace_back(transfrom);
      }
      else if (prefix == std::string("gp")) // Goal position
      {
        ss >> m_goalPosition.x >> m_goalPosition.y >> m_goalPosition.z;
      }
      else if (prefix == std::string("END")) // Set the object type to None
      {
        objectType = ObjectType::None;
      }
      else if (prefix == std::string("STATIC")) // Set the object type to Static
      {
        objectType = ObjectType::Static;
      }
      else if (prefix == std::string("DYNAMIC")) // Set the object type to Dynamic
      {
        objectType = ObjectType::Dynamic;
      }
      else if (prefix == std::string("e")) // Create Object/ model
      {
        Objects::GameObject object;

        // get model path
        std::string filePath;
        ss >> filePath;
        object.ModelPath = "assets/objects/" + filePath;

        // Get texture if there is a specified
        ss >> filePath;
        if (filePath != "NONE") object.TexturePath = "assets/textures/" + filePath;

        Objects::Transform transform;
        std::string posi;

        ss >> posi;
        std::vector<std::string> positions = LumiCore::Utility::SplitString(posi, ",");
        if (!LumiCore::Utility::VectorIsGreaterOrEqualTo(positions, 3))
        {
          LOG_WARNING("Could not load in {} row. As poition is wrongly formated", row);
          continue;
        }
        transform.Position = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

        // Get Rotation
        ss >> posi;
        positions = LumiCore::Utility::SplitString(posi, ",");
        if (!LumiCore::Utility::VectorIsGreaterOrEqualTo(positions, 3))
        {
          LOG_WARNING("Could not load in {} row. As Rotation is wrongly formated", row);
          continue;
        }
        transform.Rotation = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

        // Get Scale
        ss >> posi;
        positions = LumiCore::Utility::SplitString(posi, ",");
        if (!LumiCore::Utility::VectorIsGreaterOrEqualTo(positions, 3))
        {
          LOG_WARNING("Could not load in {} row. As Scale is wrongly formated", row);
          continue;
        }
        transform.Scale = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

        object.Transform = transform;
        AddToScene(object, objectType);
      }
    } // while
      // parse through it
    return true;
  }
  bool Scene::IsNotAComment(std::string str)
  {
    return str == "#";
  }
  void Scene::AddToScene(Objects::GameObject& object, const ObjectType type)
  {
    // TODO: Check if object is valid
    switch (type)
    {
    case ObjectType::Static:
      m_worldStatic.emplace_back(std::move(object));
      break;
    case ObjectType::Dynamic:
      m_worldDynamic.emplace_back(std::move(object));
      break;
    case ObjectType::None:
      break;
    }
  }

  Objects::Transform Scene::GetSpawnTransformByIndex(const int index)
  {
    return m_spawnPoints[index % m_spawnPoints.size()];
  }

  std::vector<Objects::GameObject>& Scene::GetObjectsInWorldByType(const ObjectType type)
  {
    switch (type)
    {
    case ObjectType::Static:
      return m_worldStatic;
    case ObjectType::Dynamic:
      return m_worldDynamic;
    case ObjectType::None:
    default:
      break;
    }
  }


} // namespace LumiCore::Scene
