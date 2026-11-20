
#include "LumiCore/EngineSettings.h"
#include <filesystem>
#include <fstream>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include "LumiCore/InputSystem.h"
#include "LumiCore/Model.h"
#include "LumiCore/ResourceManager.h"
#include "glm/ext/matrix_float3x3.hpp"
#include "glm/matrix.hpp"
#include <GLFW/glfw3.h>
#include <format>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

#include "LumiCore/Camera.h"
#include "LumiCore/Mesh.h"
#include "LumiCore/Shader.h"
#include "LumiCore/Utility.h"
#include "LumiCore/logger/Logger.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

namespace WindowOpenGL
{
  void GlfwErrorCallback(int error, const char* description)
  {
    LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("GLFW error ({}): {}", error, description));
  }

  void FramebufferSizeCallback(GLFWwindow*, int width, int height)
  {
    glViewport(0, 0, width, height);
  }

  static int renderingModeIndex;
  void SwitchRenderingMode()
  {
    renderingModeIndex = renderingModeIndex + 1 < 3 ? renderingModeIndex + 1 : 0;

    if (renderingModeIndex == 0) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // fill in the triangles
    else if (renderingModeIndex == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draws in wire frame mode
    else glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // draws vertex points
  }

  void GlDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei lenght, const char* message,
                     const void* userParam)
  {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 131139) return;

    LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("---------------"));
    LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Debug Message ({}): {}", id, message));

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Source: API"));
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Source: Window System"));
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Source: Shader Compiler"));
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Source: Third Party"));
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Source: Application"));
      break;
    case GL_DEBUG_SOURCE_OTHER:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Source: Other"));
      break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Type: Error"));
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Type: Deprecated Behavior"));
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Type: Undefined Behavior"));
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Type: Portability"));
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Type: Performance"));
      break;
    case GL_DEBUG_TYPE_MARKER:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Type: Marker"));
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Type: Push Group"));
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Type: Pop Group"));
      break;
    case GL_DEBUG_TYPE_OTHER:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Type: Other"));
      break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Serverity: High"));
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Serverity: Medium"));
      break;
    case GL_DEBUG_SEVERITY_LOW:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Serverity: Low"));
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("Serverity: Notification"));
      break;
    }
  }

  GLFWwindow* InitializeWindow()
  {
    /* Initialize the library */
    if (!glfwInit())
    {
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Failed to initialize GLFW");
      return nullptr;
    }

    glfwSetErrorCallback(GlfwErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window =
        glfwCreateWindow(LumiEngine::windowWidth, LumiEngine::windowHeight, LumiEngine::windowTitle, nullptr, nullptr);

    if (!window)
    {
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Something on window initialize went wrong");
      glfwTerminate();
      return nullptr;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSwapInterval(1);

    const int loadedVersion = gladLoadGL();
    if (loadedVersion == 0)
    {
      LOG_ERROR("Failed to load OpenGL functions with GLAD");
      glfwDestroyWindow(window);
      glfwTerminate();
      return nullptr;
    }

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
      LOG_DEBUG("initialize debug output");
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(GlDebugOutput, nullptr);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    return window;
  };

} // namespace WindowOpenGL

namespace Objects
{
  struct Transform
  {
    glm::vec3 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale = glm::vec3(1.0f);
  };

  struct GameObject
  {
    std::string ModelPath;
    std::string TexturePath;
    Objects::Transform Transform;
  };

  struct StaticObject
  {
    std::string Model;
    std::string TextureName;
    Objects::Transform Transform;
  };

} // namespace Objects

namespace SceneStuff
{
  enum class ObjectType
  {
    None,
    Static,
    Dynamic,
  };

  struct Scene
  {
    // Scene Info //
    std::string SceneName;
    std::string FilePath;
    int LevelId;

    std::vector<Objects::Transform> SpawnPoints;
    glm::vec3 GoalPosition;

    // Scene Objects //
    std::vector<Objects::GameObject> WorldStatic;
    std::vector<Objects::GameObject> WorldDynamic;

    bool Initialize(const std::string& newScene)
    {
      // check if file is correct
      auto fsPath = std::filesystem::path(newScene);
      if (fsPath.extension() != ".lscene")
        return false; // This have to be done before calling initialize but is here now

      FilePath = newScene;

      // read file
      std::ifstream file(newScene);
      if (!file.is_open())
      {
        LOG_CRITICAL(std::format("Failed to open scene file: {}", newScene));
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
            SceneName = std::format("{} {}", SceneName, str);
          }
          LOG_DEBUG(std::format("Scene Loaded: {}", SceneName));
        }
        else if (prefix == std::string("sp")) // spawn point, is in a vector as there could be more SpawnPoints in scene
        {
          LOG_INFO("SpawnPoint has been found");
          Objects::Transform transfrom;
          std::string stringTransform;

          // Get Position
          ss >> stringTransform;
          std::vector<std::string> positions = LumiCore::Utility::SplitString(stringTransform, ",");
          if (!LumiCore::Utility::VectorIsGreaterOrEqualTo(positions, 3))
          {
            LOG_WARNING(std::format("Could not load in {} row. As poition is wrongly formated", row));
            transfrom.Position = glm::vec3(0);
          }
          else
            transfrom.Position = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

          // Get Rotation
          ss >> stringTransform;
          positions = LumiCore::Utility::SplitString(stringTransform, ",");
          if (!LumiCore::Utility::VectorIsGreaterOrEqualTo(positions, 3))
          {
            LOG_WARNING(std::format("Could not load in {} row. As poition is wrongly formated", row));
            transfrom.Rotation = glm::vec3(0);
          }
          else
            transfrom.Rotation = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

          SpawnPoints.emplace_back(transfrom);
        }
        else if (prefix == std::string("gp")) // Goal position
        {
          ss >> GoalPosition.x >> GoalPosition.y >> GoalPosition.z;
          LOG_INFO("Goal Position has been found");
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
            LOG_WARNING(std::format("Could not load in {} row. As poition is wrongly formated", row));
            continue;
          }
          transform.Position = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

          // Get Rotation
          ss >> posi;
          positions = LumiCore::Utility::SplitString(posi, ",");
          if (!LumiCore::Utility::VectorIsGreaterOrEqualTo(positions, 3))
          {
            LOG_WARNING(std::format("Could not load in {} row. As Rotation is wrongly formated", row));
            continue;
          }
          transform.Rotation = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

          // Get Scale
          ss >> posi;
          positions = LumiCore::Utility::SplitString(posi, ",");
          if (!LumiCore::Utility::VectorIsGreaterOrEqualTo(positions, 3))
          {
            LOG_WARNING(std::format("Could not load in {} row. As Scale is wrongly formated", row));
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

    bool IsNotAComment(std::string str)
    {
      return str == "#";
    }

    void AddToScene(Objects::GameObject& object, const ObjectType type)
    {
      // TODO: Check if object is valid
      switch (type)
      {
      case ObjectType::Static:
        WorldStatic.emplace_back(std::move(object));
        break;
      case ObjectType::Dynamic:
        WorldDynamic.emplace_back(std::move(object));
        break;
      case ObjectType::None:
        break;
      }
    }

    // TODO: Remove Object from scene

    std::vector<Objects::GameObject>& GetObjectsInWorldByType(const ObjectType type)
    {
      switch (type)
      {
      case ObjectType::Static:
        return WorldStatic;
      case ObjectType::Dynamic:
        return WorldDynamic;
      case ObjectType::None:
      default:
        break;
      }
    }

    void Cleanup()
    {
      WorldStatic.clear();
      WorldDynamic.clear();
    }
  };

  struct SceneManager
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

    LOG_INFO(std::format("Scene file generated: {}", filePathName));
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
        LOG_INFO(std::format("Scene file found: {}", path.filename().string()));
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
      LOG_CRITICAL(std::format("Could not find scene {}   Loading in default scene...", sceneName));

      sceneName = "Default_Level.lscene";
      if (!LumiCore::Utility::ContainsInVector(m_sceneNames, sceneName)) GenerateNewScene();
    }

    if (m_currentScene) m_currentScene->Cleanup();
    m_currentScene.reset();

    m_currentScene = std::make_unique<Scene>();
    m_currentScene->Initialize(SceneDirectory + sceneName);

    return m_currentScene.get();
  }


} // namespace SceneStuff

void CallbackKeyPressFunction()
{
  LOG_DEBUG("Keypress callback has beedn called");
}

namespace PlayerStuff
{

  struct playerActions
  {
    float moveX = 0;
    float moveZ = 0;

    float yaw = 0;
    float pitch = 0;
  };


  struct Player
  {
    LumiCore::Camera Camera;
    std::string ModelPath;
    glm::vec3 Position, Rotation;
    playerActions Action;

    Player(glm::vec3 startPos, glm::vec3 startRotation, std::string model)
    {
      Position = startPos;
      Rotation = startRotation;

      Camera.SetLocation(startPos);
      Camera.SetYaw(startRotation.x);
      Camera.SetPitch(startRotation.y);

      ModelPath = model;
    }
  };

  // handle player input
} // namespace PlayerStuff

namespace Pipeline
{
  using namespace PlayerStuff;

  void GetPlayerInput(GLFWwindow* window, int activePlayers, std::vector<Player>& players)
  {
    if (players.size() > 2) return;

    if (activePlayers >= 1)
    {
      playerActions action;
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      {
        action.moveX = 1;
      }
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      {
        action.moveX = -1;
      }
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      {
        action.yaw = -1;
      }
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      {
        action.yaw = 1;
      }
      if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
      {
        action.pitch = 1;
      }
      if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
      {
        action.pitch = -1;
      }
      players[0].Action = action;
    }

    if (players.size() >= 2)
    {
      playerActions action;
      if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      {
        action.moveX = 1;
      }
      if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      {
        action.moveX = -1;
      }
      if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      {
        action.yaw = -1;
      }
      if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      {
        action.yaw = 1;
      }
      if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
      {
        action.pitch = 1;
      }
      if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
      {
        action.pitch = -1;
      }
      players[1].Action = action;
    }
  };

  // This is where we are moving the player and modifying the player struct position
  void GameLogic(const int activePlayers, std::vector<Player>& players)
  {
    constexpr float speed = 0.4f;

    for (Player& current : players)
    {
      playerActions& action = current.Action;
      LumiCore::Camera& camera = current.Camera;
      if (action.moveX != 0)
      {
        glm::vec3 location = camera.GetLocation();
        location += camera.GetForwardDirection() * speed * action.moveX;
        camera.SetLocation(location);
        current.Position = location;
      }
      if (action.moveZ != 0)
      {
        glm::vec3 location = camera.GetLocation();
        location += camera.GetRightDirection() * speed * action.moveZ;
        camera.SetLocation(location);
        current.Position = location;
      }
      if (action.yaw != 0)
      {
        float yaw = camera.GetYaw();
        yaw += speed * action.yaw;
        camera.SetYaw(yaw);
        current.Rotation.x = yaw;
      }
      if (action.pitch != 0)
      {
        float pitch = camera.GetPitch();
        pitch += speed * action.pitch;
        camera.SetPitch(pitch);
        current.Rotation.y = pitch;
      }
    }
  };

  // return the aspect ratio
  float UpdateViewPort(GLFWwindow* window, const int playerIndex, const int activePlayers)
  {
    // DO MATH for dividing up the window into a split screen
    int windowWidth;
    int windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    if (windowHeight <= 0 || windowWidth <= 0) return 1.0f;

    const bool isPlayerOne = playerIndex == 0;
    const int halftWidth = windowWidth / 2;

    // BOTTOM X
    const int bottomX = (activePlayers < 2) ? 0 : (isPlayerOne ? 0 : halftWidth);

    // TOP X
    const int topX = (activePlayers < 2) ? windowWidth : (isPlayerOne ? halftWidth : windowWidth);
    const int viewportWidth = topX - bottomX;

    // Change the viewport for the screen
    glViewport(bottomX, 0, viewportWidth, windowHeight);
    glEnable(GL_SCISSOR_TEST);
    glScissor(bottomX, 0, viewportWidth, windowHeight); // The area we should divide the screen up, same as the viewport

    if (isPlayerOne) glClearColor(0.3f, .0f, .0f, 1.0f);
    else glClearColor(0.0f, .0f, .3f, 1.0f);

    /// DO THE RENDERING HERE ///
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // return aspect ratio
    return (float)viewportWidth / (float)windowHeight;
  }

  // Render the screen
  void RenderPlay(GLFWwindow* window, Shader& shader, const int activePlayers,
                  std::vector<PlayerStuff::Player>& players, SceneStuff::Scene& scene,
                  LumiCore::Resources::AssetManagers& resources)
  {

    glClearColor(.1f, .09f, .12f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    constexpr glm::vec3 forward(1.0f, 0.0f, 0.0f);
    constexpr glm::vec3 up(0.0f, 1.0f, 0.0f);
    constexpr glm::vec3 right(0.0f, 0.0f, 1.0f);

    shader.Use();
    for (int i = 0; i < activePlayers; ++i)
    {
      Player& player = players[i];

      // Maybe change view port, then we also need to change the aspect ratio
      player.Camera.SetAspectRatio(UpdateViewPort(window, i, activePlayers));

      shader.SetMatrix4("u_View", player.Camera.GetViewMatrix());
      shader.SetMatrix4("u_Projection", player.Camera.GetProjectionMatrix());

      for (auto& object : scene.GetObjectsInWorldByType(SceneStuff::ObjectType::Static))
      {
        LumiCore::Model& mesh = resources.Models.Get(object.ModelPath, resources);
        if (!mesh.IsValid()) continue;
        glActiveTexture(GL_TEXTURE0);

        std::string& texPath = object.TexturePath;
        if (!texPath.empty())
        {
          auto texId = resources.Textures.Get(texPath, resources).id;
          shader.SetInt("u_Texture_diffuse1", texId);
          glBindTexture(GL_TEXTURE_2D, texId);
        }

        //   glActiveTexture(GL_TEXTURE0);
        //   shader.SetInt(("u_Texture_diffuse0"), 6);
        //   glBindTexture(GL_TEXTURE_2D, 6);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, object.Transform.Position);
        model = glm::rotate(model, glm::radians(object.Transform.Rotation.x), forward);
        model = glm::rotate(model, glm::radians(object.Transform.Rotation.y), up);
        model = glm::rotate(model, glm::radians(object.Transform.Rotation.z), right);

        model = glm::scale(model, object.Transform.Scale);

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(glm::mat3(model))));
        shader.SetMatrix3("u_NormalMatrix", normalMatrix);
        shader.SetMatrix4("u_Model", model);

        mesh.Draw(shader);
      }

      // if there are more players render them out
      if (activePlayers > 1)
      {
        Player& other = players[(i + 1) % activePlayers];

        LumiCore::Model& mesh = resources.Models.Get(other.ModelPath, resources);
        if (!mesh.IsValid()) continue;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, other.Position);

        model = glm::rotate(model, glm::radians(other.Rotation.x), up);
        model = glm::rotate(model, glm::radians(other.Rotation.y), right);
        model = glm::rotate(model, glm::radians(other.Rotation.z), forward);

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(glm::mat3(model))));
        shader.SetMatrix3("u_NormalMatrix", normalMatrix);
        shader.SetMatrix4("u_Model", model);

        mesh.Draw(shader);
      }
    }
    glDisable(GL_SCISSOR_TEST);
  };

} // namespace Pipeline


int main()
{
  LOG(LumiCore::Logger::ELogVerbose::INFO, "Debugger initialized");

  GLFWwindow* window = WindowOpenGL::InitializeWindow();
  if (!window)
  {
    LOG_ERROR("Could not Initialize OpenGL or Window");
    return -1;
  }

  SceneStuff::SceneManager SceneManager;
  {
    const std::vector<std::string> allscenes = SceneManager.GetAllScenes();
    std::string level = std::string("main.lscene");
    if (const SceneStuff::Scene* scene = SceneManager.SwitchScene(level); scene == nullptr)
    {
      glfwTerminate();
      return -1;
    } // main.lscene // guy.lscene // cats.lscene
  }


  LumiCore::Resources::AssetManagers resources;

  ///////////////

  Shader shader = Shader("assets/shaders/Shader.vert", "assets/shaders/Shader.frag");
  if (!shader.IsValid())
  {
    LOG_ERROR("3D shader could not compile");
    return -2;
  }

  ///////////////

  LOG_INFO(std::format("OpenGL: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION))));
  LOG_INFO(std::format("Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER))));

  /// TEST WITH INPUTMANAGER ///
  // LumiCore::InputSystem::InputSystem inputManager(window);
  // LumiCore::InputSystem::InputKeyBind keybindW{LumiCore::InputSystem::EKeyCode::Keycode_W, 0};
  // LumiCore::InputSystem::InputKeyBind keybindA{LumiCore::InputSystem::EKeyCode::Keycode_A, 0};
  // LumiCore::InputSystem::InputKeyBind keybind1{LumiCore::InputSystem::EKeyCode::Keycode_Key1, 0};
  // LumiCore::InputSystem::InputKeyBind keybind2{LumiCore::InputSystem::EKeyCode::Keycode_Key2, 0};
  // inputManager.AddKeybind(keybindW);
  // inputManager.BindKeybindPress(keybindW, CallbackKeyPressFunction);
  // inputManager.AddKeybind(keybindA);
  // inputManager.BindKeybindPress(keybindA, CallbackKeyPressFunction);
  // inputManager.AddKeybind(keybind1);
  // inputManager.BindKeybindPress(keybind1, WindowOpenGL::SwitchRenderingMode);
  // inputManager.AddKeybind(keybind2);
  // inputManager.BindKeybindPress(keybind2,
  //                               [&camera, &shader]() { SwitchProjectionMatrix(camera, basic3DShader); });
  /// TEST WITH INPUTMANAGER ///

  std::vector<PlayerStuff::Player> players;

  int activePlayers = 2;

  for (int i = 0; i < activePlayers; i++)
  {
    Objects::Transform transform = SceneManager.GetCurrentScene().SpawnPoints[i];
    players.emplace_back(PlayerStuff::Player{transform.Position, transform.Rotation, "assets/objects/cat/Maxwell.obj"});
  }

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    float runningTime = (float)glfwGetTime();

    //////////////////////
    //// Handle input ////
    //////////////////////

    std::vector<PlayerStuff::playerActions> inputs;
    inputs.resize(activePlayers);
    Pipeline::GetPlayerInput(window, activePlayers, players);

    //////////////////////
    //// Handle Logic ////
    //////////////////////

    Pipeline::GameLogic(activePlayers, players);

    //////////////////////////
    //// Handle Rendering ////
    //////////////////////////

    /* Render here */
    Pipeline::RenderPlay(window, shader, activePlayers, players, SceneManager.GetCurrentScene(), resources);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
