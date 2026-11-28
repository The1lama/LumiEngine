#include "SceneManager.h"

#include <format>
#include <memory>

#include "../ComponentTypes.h"
#include "../Utility.h"
#include "Objects.h"
#include "Scene.h"

namespace LumiCore::Scene
{

  void SceneManager::GenerateNewScene()
  {
    const std::string sceneName = "Default Level";
    const std::string fileName = std::format("{}.lscene", LumiCore::Utility::ReplaceAllChar(sceneName, " ", "_"));
    const std::string filePathName = std::string(std::format("{}{}", SceneDirectory, fileName));

    std::ofstream fs;
    fs.open(filePathName.c_str());

    fs << std::format("o {}\n", sceneName);

    fs << "# Position Rotation\n";
    fs << "sp  1,1,0  90,0,0\n";
    fs << "sp -8,1,0 -90,0,0\n";

    fs << "# Position\n";
    fs << "gp 8,1,0\n";

    fs << "STATIC\n";
    fs << "# Model_Name           Texture_Name   POS   ROTATION     SCALE\n";
    fs << "e engine/Plane.obj      noise.jpg    0,-4,0   0,0,0   20.0,20.0,20.0\n";
    fs << "e engine/Cube.obj       uvTest.png   0,0,0    0,0,0   2,2,1\n";
    fs << "e guy/MaleVillager.obj  NONE         4,1,1    0,0,0   1,1,1\n";
    fs << "END\n";

    fs.close();

    LOG_INFO("Scene file generated: {}", filePathName);
    m_sceneNames.emplace_back(fileName);
  }

  bool SceneManager::LoadInScenes()
  {
    for (const auto& entry : std::filesystem::directory_iterator(SceneDirectory))
    {
      std::filesystem::path path = entry.path();
      if (entry.path().extension().string() == ".lscene")
      {
        m_sceneNames.emplace_back(path.filename().string());
        // LOG_INFO("Scene file found: {}", path.filename().string());
      }
    }
    return !m_sceneNames.empty();
  }

  const std::vector<std::string>& SceneManager::GetAllScenes()
  {
    m_sceneNames.clear();
    if (!LoadInScenes()) GenerateNewScene();
    return m_sceneNames;
  }

  const Scene* SceneManager::SwitchScene(std::string& sceneName)
  {
    if (!LumiCore::Utility::ContainsInVector(m_sceneNames, sceneName))
    {
      LOG_CRITICAL("Could not find scene {}   Loading in default scene...", sceneName);

      sceneName = "Default_Level.lscene";
      if (!LumiCore::Utility::ContainsInVector(m_sceneNames, sceneName)) GenerateNewScene();
    }

    if (m_currentScene) m_currentScene->Cleanup();
    m_currentScene.reset();

    m_currentScene = std::make_unique<Scene>();
    m_currentScene->Initialize(SceneDirectory + sceneName);

    return m_currentScene.get();
  }
  PlayerStuff::Player& SceneManager::GetPlayerByIndex(const unsigned int index)
  {
    return *m_players[index];
  }

  void SceneManager::CreateNewPlayer()
  {
    const unsigned int playerIndex = m_players.size();
    Transform transform = GetCurrentScene().GetSpawnTransformByIndex(playerIndex);
    m_players.emplace_back(std::make_unique<PlayerStuff::Player>(transform.Position, transform.Rotation,
                                                                 "assets/objects/cat/Maxwell.obj", playerIndex));

    LOG_DEBUG("Created new player. Player amount: {}", m_players.size());
  }
  void SceneManager::StorePlayer(PlayerStuff::Player& newPlayer)
  {
    m_players.emplace_back(std::move(&newPlayer));
  }
  void SceneManager::RemovePlayer(const int index)
  {
    // Dont do anything if the index is on player 1 or the player vector is bigger than index
    if (m_players.size() < index || index == 0) return;
    m_players.erase(m_players.begin() + index);
  }

} // namespace LumiCore::Scene
