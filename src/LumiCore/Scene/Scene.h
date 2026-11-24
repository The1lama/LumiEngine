#pragma once

#include "Objects.h"
#include <glm/vec3.hpp>
#include <string>

namespace LumiCore::Scene
{
  enum class ObjectType
  {
    None,
    Static,
    Dynamic,
  };

  class Scene
  {
  private:
    // Scene Info //
    std::string m_sceneName;
    std::string m_filePath;
    int m_levelId;

    std::vector<Objects::Transform> m_spawnPoints;
    glm::vec3 m_goalPosition;

    // Scene Objects //
    std::vector<Objects::GameObject> m_worldStatic;
    std::vector<Objects::GameObject> m_worldDynamic;

    bool IsNotAComment(std::string str);

  public:
    bool Initialize(const std::string& newScene);

    void AddToScene(Objects::GameObject& object, const ObjectType type);

    void Cleanup()
    {
      m_worldStatic.clear();
      m_worldDynamic.clear();
    }

#pragma region Getters
    Objects::Transform GetSpawnTransformByIndex(const int index);

    std::string GetScenename() const
    {
      return m_sceneName;
    }
    std::string GetFilePath() const
    {
      return m_filePath;
    }
    int GetLevelId() const
    {
      return m_levelId;
    }

    std::vector<Objects::GameObject>& GetObjectsInWorldByType(const ObjectType type);
#pragma endregion // Getters
  };

} // namespace LumiCore::Scene
