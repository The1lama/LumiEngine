#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include <memory>
#include <string>

namespace LumiCore
{
  struct Transform;

  namespace ECS
  {
    using Entity = std::uint32_t;

    class ECSFacade;
  } // namespace ECS
} // namespace LumiCore

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

    std::vector<LumiCore::Transform> m_spawnPoints;
    glm::vec3 m_goalPosition;

    // Scene Objects //
    std::vector<ECS::Entity> m_worldStatic;
    std::vector<ECS::Entity> m_worldDynamic;

    bool IsNotAComment(std::string str);

  public:
    Scene();
    ~Scene();
    bool Initialize(const std::string& newScene);

    void AddToScene(ECS::Entity entity, const ObjectType type);

    void Cleanup()
    {
      m_worldStatic.clear();
      m_worldDynamic.clear();
    }

#pragma region Getters
    LumiCore::Transform GetSpawnTransformByIndex(const int index);

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

    std::vector<ECS::Entity>& GetObjectsInWorldByType(const ObjectType type);
#pragma endregion // Getters
  };

} // namespace LumiCore::Scene
