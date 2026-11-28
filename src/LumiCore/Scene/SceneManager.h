#pragma once

#include <memory>
#include <vector>


namespace PlayerStuff
{
  struct Player;
} // namespace PlayerStuff


namespace LumiCore::Scene
{
  class Scene;

  class SceneManager
  {
  private:
    std::vector<std::string> m_sceneNames;
    const std::string SceneDirectory = "assets/scenes/";

    std::vector<std::unique_ptr<PlayerStuff::Player>> m_players;

    std::unique_ptr<Scene> m_currentScene;

    void GenerateNewScene();
    bool LoadInScenes();

  public:
    void CreateNewPlayer();
    void StorePlayer(PlayerStuff::Player& newPlayer);
    void RemovePlayer(const int index);

    const std::vector<std::string>& GetAllScenes();

    const Scene* SwitchScene(std::string& sceneName);

    Scene& GetCurrentScene() const
    {
      return *m_currentScene;
    };

    unsigned int GetAllPlayerCount() const
    {
      return m_players.size();
    }
    PlayerStuff::Player& Extracted();
    PlayerStuff::Player& GetPlayerByIndex(const unsigned int index);


    std::vector<std::unique_ptr<PlayerStuff::Player>>& GetAllPlayers()
    {
      return m_players;
    }
  };

} // namespace LumiCore::Scene
