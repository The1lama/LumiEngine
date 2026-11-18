#include <GLFW/glfw3.h>
#include <format>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>
#include "LumiCore/InputSystem.h"
#include "LumiCore/Model.h"
#include "LumiCore/ResourceManager.h"
#include "glm/ext/matrix_float3x3.hpp"
#include "glm/matrix.hpp"

#define STB_IMAGE_IMPLEMENTATION
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
void glfwErrorCallback(int error, const char* description)
{
  LOG(LumiCore::Logger::ELogVerbose::ERROR, std::format("GLFW error ({}): {}", error, description));
}

void framebufferSizeCallback(GLFWwindow*, int width, int height)
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
  else if (renderingModeIndex == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draws in wireframe mode
  else glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // draws vertex points
}

bool processInput(GLFWwindow* window, bool& bToggleColor, bool& bSwitchDrawMode, bool& bSwitchProjectionMatrix)
{
  bool hasPressed = false;

  // Change color on object //
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    hasPressed = true;
    bToggleColor = false;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
  {
    bToggleColor = true;
  }

  // Draw mode switch //
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
  {
    hasPressed = true;
    if (bSwitchDrawMode == false) SwitchRenderingMode();
    bSwitchDrawMode = true;
  }
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
  {
    bSwitchDrawMode = false;
  }

  // Projection matrix switch //
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
  {
    hasPressed = true;
    bSwitchProjectionMatrix = true;
  }
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
  {
    bSwitchProjectionMatrix = false;
  }

  return hasPressed;
}

void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei lenght, const char* message,
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

  glfwSetErrorCallback(glfwErrorCallback);

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
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
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
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  }
  return window;
};

} // namespace WindowOpenGL

namespace Objects
{

enum EObjectType
{
  NONE,
  ROTATE,
};

struct Transform
{
  glm::vec3 Position;
  glm::vec3 Rotation;
  glm::vec3 Scale;
};

struct GameObject
{
  std::shared_ptr<LumiCore::Model> model;
  EObjectType type;
  Transform transform;
};

} // namespace Objects


namespace Resources
{
void InitResources() {}
} // namespace Resources


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

struct playerActions
{
  float moveX = 0;
  float moveZ = 0;

  float yaw = 0;
  float pitch = 0;
};


struct Player
{
  LumiCore::Camera camera;
  std::shared_ptr<LumiCore::Model> model;
  glm::vec3 position, rotation;
  playerActions action;

  Player(glm::vec3 startPos, glm::vec3 startRotation, std::shared_ptr<LumiCore::Model> model)
  {
    position = startPos;
    rotation = startRotation;

    camera.SetLocation(startPos);
    camera.SetYaw(startRotation.x);
    camera.SetPitch(startRotation.y);

    this->model = model;
  }
};

// handle player input
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
    players[0].action = action;
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
    players[1].action = action;
  }
};

// this is where we are moving the player and chaning the player struct position
void GameLogic(const int ActivePlayers, std::vector<Player>& players)
{
  constexpr float Speed = 1.0f;

  for (Player& current : players)
  {
    playerActions& action = current.action;
    LumiCore::Camera& camera = current.camera;
    if (action.moveX != 0)
    {
      glm::vec3 location = camera.GetLocation();
      location += camera.GetForwardDirection() * Speed * action.moveX;
      camera.SetLocation(location);
      current.position = location;
    }
    if (action.moveZ != 0)
    {
      glm::vec3 location = camera.GetLocation();
      location += camera.GetRightDirection() * Speed * action.moveZ;
      camera.SetLocation(location);
      current.position = location;
    }
    if (action.yaw != 0)
    {
      float yaw = camera.GetYaw();
      yaw += Speed * action.yaw;
      camera.SetYaw(yaw);
      current.rotation.x = yaw;
    }
    if (action.pitch != 0)
    {
      float pitch = camera.GetPitch();
      pitch += Speed * action.pitch;
      camera.SetPitch(pitch);
      current.rotation.y = pitch;
    }
  }
};

// return the aspect ratio
float updateViewPort(GLFWwindow* window, const int playerIndex, const int ActivePlayers)
{
  // DO MATH for dividing up the window into a split screen
  int windowWidth;
  int windowHeight;
  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

  const bool IsPlayerOne = playerIndex == 0;

  // BOTTOM X
  int bx = ActivePlayers < 2 ? 0 : IsPlayerOne ? 0 : windowWidth / 2;

  // TOP X
  int tx = ActivePlayers < 2 ? windowWidth : IsPlayerOne ? windowWidth / 2 : windowWidth;

  // Change the viewport for the screen
  glViewport(bx, 0, tx, windowHeight);
  glEnable(GL_SCISSOR_TEST);
  glScissor(bx, 0, tx, windowHeight); // the area we should divide the screen up, same as the view port

  if (IsPlayerOne) glClearColor(0.3f, .0f, .0f, 1.0f);
  else glClearColor(0.0f, .0f, .3f, 1.0f);

  /// DO THE RENDERING HERE ///
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  // return aspect ratio
  return ((float)tx - (float)bx) / (float)windowHeight;
}


// Render teh screen
void renderPlay(GLFWwindow* window, Shader& shader, const int ActivePlayers, std::vector<Player>& players,
                std::vector<Objects::GameObject>& renderObjects)
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

  for (int i = 0; i < ActivePlayers; ++i)
  {
    Player& player = players[i];

    // maybe change view port, then we also need to change the aspect ratio
    player.camera.SetAspectRatio(updateViewPort(window, i, ActivePlayers));

    shader.SetMatrix4("u_View", player.camera.GetViewMatrix());

    for (auto& object : renderObjects)
    {
      if (object.model == nullptr) continue;

      glm::mat4 model = glm::mat4(1.0f);

      model = glm::translate(model, object.transform.Position);

      model = glm::rotate(model, glm::radians(object.transform.Rotation.x), forward);
      model = glm::rotate(model, glm::radians(object.transform.Rotation.y), up);
      model = glm::rotate(model, glm::radians(object.transform.Rotation.z), right);

      // if (object.type == Objects::ROTATE)
      //{
      //   model *= glm::rotate(glm::mat4(1.0f), glm::radians(runningTime * 50), up);
      // }

      model = glm::scale(model, object.transform.Scale);

      glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(glm::mat3(model))));
      shader.SetMatrix3("u_NormalMatrix", normalMatrix);
      shader.SetMatrix4("u_Model", model);

      object.model->Draw(shader);
    }

    // if there are more players render them out
    if (ActivePlayers > 1)
    {
      Player& other = players[(i + 1) % ActivePlayers];


      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, other.position);

      // model = glm::rotate(model, glm::radians(other.rotation.x), forward);
      model = glm::rotate(model, glm::radians(other.rotation.x), up);
      model = glm::rotate(model, glm::radians(other.rotation.y), right);

      glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(glm::mat3(model))));
      shader.SetMatrix3("u_NormalMatrix", normalMatrix);
      shader.SetMatrix4("u_Model", model);


      other.model->Draw(shader);
    }
  }
  glDisable(GL_SCISSOR_TEST);
};

int main()
{
  LOG(LumiCore::Logger::ELogVerbose::INFO, "Debugger initialized");

  GLFWwindow* window = WindowOpenGL::InitializeWindow();
  if (!window)
  {
    LOG_ERROR("Could not Initialize OpenGL or Window");
    return -1;
  }

  /// Colors ///
  glm::vec3 redColor = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 greenColor = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 blueColor = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 blackColor = glm::vec3(0.0f, 0.0f, 0.0f);

  LumiCore::Texture uvTexture;
  uvTexture.id = LumiCore::Utility::GenerateTexture("uvTest.png", "assets/textures");
  uvTexture.type = "material";

  LumiCore::Texture noiseTexture;
  noiseTexture.id = LumiCore::Utility::GenerateTexture("noise.jpg", "assets/textures");
  noiseTexture.type = "material";

#pragma region MeshObjects
  std::vector<LumiCore::Texture> textures{uvTexture};

  LumiCore::Resources::ResourceManager RM;

  std::vector<Objects::GameObject> renderObjects;

  glm::vec3 halfScale(0.5f, 0.5f, 0.5f);

  { // Barrel
    Objects::GameObject object;
    object.model = RM.GetModel("assets/objects/barrel/Barrel.obj", textures);
    object.transform = {glm::vec3(-2.0f, 0.8f, 0.0f), glm::vec3(0, 0, 0), halfScale};
    object.type = Objects::ROTATE;
    renderObjects.push_back(object);
  }

  { // Maxwell 1
    Objects::GameObject object;
    object.model = RM.GetModel("assets/objects//cat/Maxwell.obj", textures);
    object.transform = {glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0, 0, 0), halfScale};
    object.type = Objects::ROTATE;
    renderObjects.push_back(object);
  }
  { // Maxwell 2
    Objects::GameObject object;
    object.model = RM.GetModel("assets/objects//cat/Maxwell.obj", textures);
    object.transform = {glm::vec3(2.0f, 0.8f, 0.0f), glm::vec3(0, 64, 0), halfScale};
    object.type = Objects::NONE;
    renderObjects.push_back(object);
  }
  { // Guy
    Objects::GameObject object;
    object.model = RM.GetModel("assets/objects/guy/MaleVillager.obj", textures);
    object.transform = {glm::vec3(-2.0f, -1.5f, .0f), glm::vec3(0, 45.f, 0), halfScale};
    object.type = Objects::NONE;
    renderObjects.push_back(object);
  }
  { // TallBuilding02
    Objects::GameObject object;
    object.model = RM.GetModel("assets/objects/building/TallBuilding02.obj", textures);
    object.transform = {glm::vec3(2.0f, -2.0f, 0.0f), glm::vec3(0.f), halfScale * 0.2f};
    object.type = Objects::ROTATE;
    renderObjects.push_back(object);
  }
  { // SmallBuilding02
    Objects::GameObject object;
    object.model = RM.GetModel("assets/objects/smallBuilding/SmallBuilding02.obj", textures);
    object.transform = {glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.f), halfScale * 0.4f};
    object.type = Objects::ROTATE;
    renderObjects.push_back(object);
  }
  { // FILE NOT FOUND
    Objects::GameObject object;
    object.model = RM.GetModel("assets/objects/building/TallBuilding02", textures);
    object.transform = {glm::vec3(2.0f, -0.7f, 0.0f), glm::vec3(90.f), halfScale * 0.2f};
    object.type = Objects::ROTATE;
    renderObjects.push_back(object);
  }

#pragma endregion MeshObjects

  /////////////// PROJECTION ///////////////

  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -6.0f);

  const auto fov = glm::radians(45.0f);
  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
  float aspectRatio = (float)frameWidth / (float)frameHeight;

  constexpr float nearPlane = 0.01f;
  constexpr float farPlane = 100.0f;
  LumiCore::Camera camera(cameraPos, fov, aspectRatio, nearPlane, farPlane);

  ///////////////

  ///////////////

  Shader shader = Shader("assets/shaders/3dBasicShader.vert", "assets/shaders/3dBasicShader.frag");
  if (!shader.IsValid())
  {
    LOG_ERROR("3D shader could not compile");
    return -2;
  }

  shader.Use();
  // these we only need to set one time as they are the same for now
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

  std::vector<Player> players{
      Player{glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(90, 0, 0),
             RM.GetModel("assets/objects/cat/Maxwell.obj", textures)},
      Player{glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(90, 0, 0),
             RM.GetModel("assets/objects/barrel/Barrel.obj", textures)},
  };

  int activePlayers = (int)players.size();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    float runningTime = glfwGetTime();

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

    std::vector<playerActions> inputs;
    inputs.resize(activePlayers);
    GetPlayerInput(window, activePlayers, players);

    //////////////////////
    //// Handle Logic ////
    //////////////////////

    GameLogic(activePlayers, players);

    //////////////////////////
    //// Handle Rendering ////
    //////////////////////////

    /* Render here */
    shader.Use();
    renderPlay(window, shader, activePlayers, players, renderObjects);


    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
