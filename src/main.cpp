
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

  struct EditorSettings
  {
    int windowWidth = 640;
    int windowHeight = 480;
    const char* windowTitle = "LumiEngine";
  };

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

    EditorSettings settings;

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window =
        glfwCreateWindow(settings.windowWidth, settings.windowHeight, settings.windowTitle, nullptr, nullptr);

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
    glm::vec3 Scale;
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

    std::vector<glm::vec3> SpawnPoints;
    glm::vec3 GoalPosition;

    // Scene Objects //
    std::vector<Objects::GameObject> WorldStatic;
    std::vector<Objects::GameObject> WorldDynamic;

    void Initialize(const std::string& newScene)
    {
      // check if file is correct
      auto fsPath = std::filesystem::path(newScene);
      if (fsPath.extension() != ".lscene") return; // This have to be done before calling initialize but is here now

      FilePath = newScene;

      // read file
      std::ifstream file(newScene);
      if (!file.is_open())
      {
        LOG_CRITICAL(std::format("Failed to open OBJ file: {}", newScene));
        return;
      }

      ObjectType objectType = ObjectType::None;

      // This will read the line in the file, line by line
      std::string line;
      while (std::getline(file, line))
      {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == std::string("o")) // file name
        {
          ss >> SceneName;
          LOG_DEBUG(std::format("Scene Loaded: {}", SceneName));
        }
        else if (prefix == std::string("sp")) // spawn point, is in a vector as there could be more SpawnPoints in scene
        {
          LOG_INFO("SpawnPoint has been found");
          glm::vec3 sp;
          ss >> sp.x >> sp.y >> sp.z;
          SpawnPoints.emplace_back(sp);
        }
        else if (prefix == std::string("gp")) // Goal position
        {
          ss >> GoalPosition.x >> GoalPosition.y >> GoalPosition.z;
          LOG_INFO("Goal Position has been found");
        }
        else if (prefix == std::string("END")) // Set the object type to None
        {
          LOG_INFO("END has been found");
          objectType = ObjectType::None;
        }
        else if (prefix == std::string("STATIC")) // Set the object type to Static
        {
          LOG_INFO("STATIC has been found");
          objectType = ObjectType::Static;
        }
        else if (prefix == std::string("DYNAMIC")) // Set the object type to Dynamic
        {
          LOG_INFO("DYNAMIC has been found");
          objectType = ObjectType::Dynamic;
        }
        else if (prefix == std::string("e")) // Create Object/ model
        {
          LOG_INFO("E has been found");

          Objects::GameObject object;

          // get model path
          std::string filePath;
          ss >> filePath;
          object.ModelPath = "assets/objects/" + filePath;

          // Get texture if there is a specified
          ss >> filePath;
          if (filePath != "NONE") object.TexturePath = "assets/textures/" + filePath;

          Objects::Transform transform;
          // get Position
          // glm::vec3 pos;

          std::string posi;

          ss >> posi;
          std::vector<std::string> positions = LumiCore::Utility::SplitString(posi, ",");
          LOG_DEBUG(std::format("Position: x: {} y: {} z: {}", positions[0], positions[1], positions[2]));
          transform.Position = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

          // Get Rotation
          ss >> posi;
          positions = LumiCore::Utility::SplitString(posi, ",");
          LOG_DEBUG(std::format("Rotation: x: {} y: {} z: {}", positions[0], positions[1], positions[2]));
          transform.Rotation = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

          // Get Scale
          ss >> posi;
          positions = LumiCore::Utility::SplitString(posi, ",");
          LOG_DEBUG(std::format("Scale: x: {} y: {} z: {}", positions[0], positions[1], positions[2]));
          transform.Scale = glm::vec3(std::stof(positions[0]), std::stof(positions[1]), std::stof(positions[2]));

          object.Transform = transform;
          AddToScene(object, objectType);
        }
      } // while
        // parse through it
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
      }
    }

    void Cleanup()
    {
      WorldStatic.clear();
      WorldDynamic.clear();
    }
  };


} // namespace SceneStuff

void CallbackKeyPressFunction()
{
  LOG_DEBUG("Keypress callback has beedn called");
}

void SwitchProjectionMatrix(LumiCore::Camera& camera, Shader& shader)
{
  camera.SwitchProjectionMatrix();

  shader.Use();
  shader.SetMatrix4("u_Projection", camera.GetProjectionMatrix());
  shader.UnUse();
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

    const bool isPlayerOne = playerIndex == 0;

    // BOTTOM X
    int bx = activePlayers < 2 ? 0 : isPlayerOne ? 0 : windowWidth / 2;

    // TOP X
    int tx = activePlayers < 2 ? windowWidth : isPlayerOne ? windowWidth / 2 : windowWidth;

    // Change the viewport for the screen
    glViewport(bx, 0, tx, windowHeight);
    glEnable(GL_SCISSOR_TEST);
    glScissor(bx, 0, tx, windowHeight); // The area we should divide the screen up, same as the viewport

    if (isPlayerOne) glClearColor(0.3f, .0f, .0f, 1.0f);
    else glClearColor(0.0f, .0f, .3f, 1.0f);

    /// DO THE RENDERING HERE ///
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // return aspect ratio
    return ((float)tx - (float)bx) / (float)windowHeight;
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

      for (auto& object : scene.GetObjectsInWorldByType(SceneStuff::ObjectType::Static))
      {
        LumiCore::Model& mesh = resources.Models.Get(object.ModelPath, resources);
        if (!mesh.IsValid()) continue;
        glActiveTexture(GL_TEXTURE0);

        shader.SetInt("u_Texture_diffuse1", 0);
        glBindTexture(GL_TEXTURE_2D, resources.Textures.Get(object.TexturePath, resources).id);

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

        // model = glm::rotate(model, glm::radians(other.rotation.x), forward);
        model = glm::rotate(model, glm::radians(other.Rotation.x), up);
        model = glm::rotate(model, glm::radians(other.Rotation.y), right);

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

  SceneStuff::Scene currentScene;
  currentScene.Initialize("assets/scenes/main.lscene");
  LumiCore::Resources::AssetManagers resources;

#pragma region MeshObjects

  glm::vec3 halfScale(0.5f, 0.5f, 0.5f);

  { // Barrel
    Objects::GameObject object;
    object.ModelPath = "assets/objects/barrel/Barrel.obj";
    object.TexturePath = "assets/textures/uvTest.png";
    object.Transform = {glm::vec3(-2.0f, 0.8f, 0.0f), glm::vec3(0, 0, 0), halfScale};
    currentScene.AddToScene(object, SceneStuff::ObjectType::Static);
  }
  { // Maxwell 1
    Objects::GameObject object;
    object.ModelPath = "assets/objects/cat/Maxwell.obj";
    object.Transform = {glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0, 0, 0), halfScale};
    object.TexturePath = "assets/textures/uvTest.png";
    currentScene.AddToScene(object, SceneStuff::ObjectType::Static);
  }
  { // Maxwell 2
    Objects::GameObject object;
    object.ModelPath = "assets/objects/cat/Maxwell.obj";
    object.Transform = {glm::vec3(2.0f, 0.8f, 0.0f), glm::vec3(0, 64, 0), halfScale};
    object.TexturePath = "assets/textures/uvTest.png";
    currentScene.AddToScene(object, SceneStuff::ObjectType::Static);
  }
  { // Guy
    Objects::GameObject object;
    object.ModelPath = "assets/objects/guy/MaleVillager.obj";
    object.Transform = {glm::vec3(-2.0f, -1.5f, .0f), glm::vec3(0, 45.f, 0), halfScale};
    object.TexturePath = "assets/textures/uvTest.png";
    currentScene.AddToScene(object, SceneStuff::ObjectType::Static);
  }
  { // TallBuilding02
    Objects::GameObject object;
    object.ModelPath = "assets/objects/building/TallBuilding02.obj";
    object.Transform = {glm::vec3(2.0f, -2.0f, 0.0f), glm::vec3(0.f), halfScale * 0.2f};
    object.TexturePath = "assets/textures/uvTest.png";
    currentScene.AddToScene(object, SceneStuff::ObjectType::Static);
  }
  { // SmallBuilding02
    Objects::GameObject object;
    object.ModelPath = "assets/objects/smallBuilding/SmallBuilding02.obj";
    object.Transform = {glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.f), halfScale * 0.4f};
    object.TexturePath = "assets/textures/uvTest.png";
    currentScene.AddToScene(object, SceneStuff::ObjectType::Static);
  }
  { // FILE NOT FOUND
    Objects::GameObject object;
    object.ModelPath = "assets/objects/building/TallBuilding02";
    object.Transform = {glm::vec3(2.0f, -0.7f, 0.0f), glm::vec3(90.f), halfScale * 0.2f};
    object.TexturePath = "assets/textures/uvTest.png";
    currentScene.AddToScene(object, SceneStuff::ObjectType::Static);
  }

#pragma endregion MeshObjects

  /////////////// PROJECTION ///////////////

  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -6.0f);

  const auto fov = glm::radians(45.0f);
  int frameWidth;
  int frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
  float aspectRatio = (float)frameWidth / (float)frameHeight;

  constexpr float nearPlane = 0.01f;
  constexpr float farPlane = 100.0f;
  LumiCore::Camera camera(cameraPos, fov, aspectRatio, nearPlane, farPlane);

  ///////////////

  ///////////////

  Shader shader = Shader("assets/shaders/Shader.vert", "assets/shaders/Shader.frag");
  if (!shader.IsValid())
  {
    LOG_ERROR("3D shader could not compile");
    return -2;
  }

  shader.Use();
  // These we only need to set one time as they are the same for now
  shader.SetMatrix4("u_Projection", camera.GetProjectionMatrix());
  shader.SetVec3("u_ViewPosition", camera.GetLocation()); // used for specular light

  shader.UnUse();

  ///////////////

  LOG_INFO(std::format("OpenGL: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION))));
  LOG_INFO(std::format("Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER))));

  float lastInputTime;
  bool bSwitchDrawingMode = false;

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

  // window cache size
  int cacheWindowWitdh;
  int cacheWindowHeigh;
  glfwGetFramebufferSize(window, &cacheWindowWitdh, &cacheWindowHeigh);

  std::vector<PlayerStuff::Player> players{
      PlayerStuff::Player{glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(90, 0, 0), "assets/objects/cat/Maxwell.obj"},
      PlayerStuff::Player{glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(90, 0, 0), "assets/objects/barrel/Barrel.obj"},
  };


  int activePlayers = (int)players.size();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    float runningTime = (float)glfwGetTime();

    int currentWindowWidth;
    int currentWindowHeight;
    glfwGetFramebufferSize(window, &currentWindowWidth, &currentWindowHeight);

    if (cacheWindowWitdh != currentWindowWidth || cacheWindowHeigh != currentWindowHeight)
    {

      camera.SetAspectRatio((float)currentWindowWidth / (float)currentWindowHeight);

      shader.Use();
      shader.SetMatrix4("u_View", camera.GetViewMatrix());
      shader.UnUse();
      cacheWindowHeigh = currentWindowHeight;
      cacheWindowWitdh = currentWindowWidth;
    }

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
    Pipeline::RenderPlay(window, shader, activePlayers, players, currentScene, resources);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
