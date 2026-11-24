#pragma once

#include "Scene.h"
#include <memory>

namespace LumiCore::Scene
{

  class SceneManager
  {
  private:
    std::vector<std::string> m_sceneNames;
    const std::string SceneDirectory = "assets/scenes/";

    std::unique_ptr<Scene> m_currentScene;

    void GenerateNewScene();
    bool LoadInScenes();

  public:
    const std::vector<std::string>& GetAllScenes();

    const Scene* SwitchScene(std::string& sceneName);

    Scene& GetCurrentScene() const
    {
      return *m_currentScene;
    };
  };

} // namespace LumiCore::Scene
