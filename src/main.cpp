#define STB_IMAGE_IMPLEMENTATION

#include "LumiCore/Camera.h"
#include "LumiCore/ComponentTypes.h"
#include "LumiCore/ECS/ECSTypes.h"
#include "LumiCore/ECS/EcsFacade.h"
#include "LumiCore/ECS/Systems/RenderSystem.h"
#include "LumiCore/EngineSettings.h"
#include "LumiCore/InputSystem.h"
#include "LumiCore/Mesh.h"
#include "LumiCore/Model.h"
#include "LumiCore/ResourceManager.h"
#include "LumiCore/Scene/Objects.h"
#include "LumiCore/Scene/Scene.h"
#include "LumiCore/Scene/SceneManager.h"
#include "LumiCore/Shader.h"
#include "LumiCore/Utility.h"
#include "LumiCore/logger/Logger.h"
#include "LumiCore/math.h"


#include <GLFW/glfw3.h>
#include <format>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <string>
#include <vector>

#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace WindowOpenGL
{
  void GlfwErrorCallback(int error, const char* description)
  {
    LOG(LumiCore::Logger::ELogVerbose::ERROR, "GLFW error ({}): {}", error, description);
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

    LOG(LumiCore::Logger::ELogVerbose::ERROR, "---------------");
    LOG(LumiCore::Logger::ELogVerbose::ERROR, "Debug Message ({}): {}", id, message);

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Source: API");
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Source: Window System");
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Source: Shader Compiler");
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Source: Third Party");
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Source: Application");
      break;
    case GL_DEBUG_SOURCE_OTHER:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Source: Other");
      break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Type: Error");
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Type: Deprecated Behavior");
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Type: Undefined Behavior");
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Type: Portability");
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Type: Performance");
      break;
    case GL_DEBUG_TYPE_MARKER:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Type: Marker");
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Type: Push Group");
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Type: Pop Group");
      break;
    case GL_DEBUG_TYPE_OTHER:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Type: Other");
      break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Serverity: High");
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Serverity: Medium");
      break;
    case GL_DEBUG_SEVERITY_LOW:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Serverity: Low");
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      LOG(LumiCore::Logger::ELogVerbose::ERROR, "Serverity: Notification");
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

void CallbackKeyPressFunction()
{
  LOG_DEBUG("Keypress callback has beedn called");
}


namespace Pipeline
{
  using namespace PlayerStuff;

  // This is where we are moving the player and modifying the player struct position
  void GameLogic(const int activePlayers, std::vector<std::unique_ptr<Player>>& players)
  {
    constexpr float speed = 0.4f;

    for (int i = 0; i < players.size(); ++i)
    {
      Player* player = players[i].get();

      playerActions& action = player->Action;
      LumiCore::Camera& camera = player->Camera;
      auto& pos = player->Position;
      auto& rot = player->Rotation;

      if (action.yaw != 0)
      {
        rot.x += speed * action.yaw;
      }
      if (action.pitch != 0)
      {
        rot.y += speed * action.pitch;
      }
      if (action.moveX != 0)
      {
        // Calculate the new camera position after moving player
        const glm::vec3 forwardDir = LumiCore::Math::CalculateForwardDirectionEuler(rot.x, rot.y);
        pos += forwardDir * speed * action.moveX;
      }
      if (action.moveZ != 0)
      {
        const glm::vec3 forwardDir = LumiCore::Math::CalculateForwardDirectionEuler(rot.x, rot.y);
        pos += LumiCore::Math::CalculateRightDirection(forwardDir, glm::vec3(0, 1, 0) * speed * action.moveZ);
      }

      action.Reset();
      camera.SetLocation(pos);
      camera.SetYaw(rot.x);
      camera.SetPitch(rot.y);
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
                  std::vector<std::unique_ptr<PlayerStuff::Player>>& players, LumiCore::Scene::Scene& scene,
                  LumiCore::Resources::AssetManagers& resources,
                  std::shared_ptr<LumiCore::ECS::Systems::RenderSystem> renderSystem)
  {

    glClearColor(.1f, .09f, .12f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    for (int i = 0; i < activePlayers; ++i)
    {
      Player* player = players[i].get();

      // Maybe change view port, then we also need to change the aspect ratio
      player->Camera.SetAspectRatio(UpdateViewPort(window, i, activePlayers));

      shader.SetMatrix4("u_View", player->Camera.GetViewMatrix());
      shader.SetMatrix4("u_Projection", player->Camera.GetProjectionMatrix());

      /*
      for (auto& object : scene.GetObjectsInWorldByType(LumiCore::Scene::ObjectType::Static))
      {
        LumiCore::Model& mesh = resources.Models.Get(object.ModelPath, resources);
        if (!mesh.IsValid()) continue;
        glActiveTexture(GL_TEXTURE0);

        std::string& texPath = object.TexturePath;
        if (!texPath.empty())
        {
          auto texId = resources.Textures.Get(texPath, resources).Id;
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
      */
      renderSystem->Render(shader, resources);

      // if there are more players render them out
      // if (activePlayers > 1)
      //{
      //  Player* other = players[(i + 1) % activePlayers].get();

      //  LumiCore::Model& mesh = resources.Models.Get(other->ModelPath, resources);
      //  if (!mesh.IsValid()) continue;

      //  glm::mat4 model = glm::mat4(1.0f);
      //  model = glm::translate(model, other->Position);

      //  model = glm::rotate(model, glm::radians(other->Rotation.x), up);
      //  model = glm::rotate(model, glm::radians(other->Rotation.y), right);
      //  model = glm::rotate(model, glm::radians(other->Rotation.z), forward);

      //  glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(glm::mat3(model))));
      //  shader.SetMatrix3("u_NormalMatrix", normalMatrix);
      //  shader.SetMatrix4("u_Model", model);

      //  mesh.Draw(shader);
      //}
    }
    glDisable(GL_SCISSOR_TEST);
  };

} // namespace Pipeline

void StartImGUIFrame(bool& showAnotherWindow, LumiCore::Scene::SceneManager& sceneManager, ImGuiIO& io);

int main()
{
  LOG(LumiCore::Logger::ELogVerbose::INFO, "Debugger initialized");

  GLFWwindow* window = WindowOpenGL::InitializeWindow();
  if (!window)
  {
    LOG_ERROR("Could not Initialize OpenGL or Window");
    return -1;
  }

  LOG_INFO("OpenGL: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  LOG_INFO("Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));


  //// ImGui Initialization ////
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard control
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad control
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking Branch

  // The second parameter will install GLFW callbacks and chain them to ImGui
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  LumiCore::Scene::SceneManager SceneManager;
  {
    const std::vector<std::string> allscenes = SceneManager.GetAllScenes();
    std::string level = std::string("cats.lscene"); // main.lscene // guy.lscene // cats.lscene
    if (const LumiCore::Scene::Scene* scene = SceneManager.SwitchScene(level); scene == nullptr)
    {
      glfwTerminate();
      return -1;
    }
  }

  LumiCore::Resources::AssetManagers resources;

  ///////////////

  Shader shader = Shader("assets/shaders/Shader.vert", "assets/shaders/Shader.frag");
  if (!shader.IsValid())
  {
    LOG_ERROR("3D shader could not compile");
    return -2;
  }


  auto* ecs = LumiCore::ECS::ECSFacade::GetInstance();

  auto renderSystem = ecs->RegisterSystem<LumiCore::ECS::Systems::RenderSystem>();

  LumiCore::ECS::Signature renderSignature;
  renderSignature.set(ecs->GetComponentType<LumiCore::Transform>());
  renderSignature.set(ecs->GetComponentType<LumiCore::ModelComponent>());
  ecs->SetSystemSignature<LumiCore::ECS::Systems::RenderSystem>(renderSignature);

  ///////////////

  bool showAnotherWindow{false};

#pragma region Input System
  // Initializes Input system so it is a singleton should only need to do once
  auto& inputManager = LumiCore::InputSystem::InputSystem::GetInstance()->Initialize(window);
#pragma endregion // Input System

  SceneManager.CreateNewPlayer();
  SceneManager.CreateNewPlayer();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    float runningTime = (float)glfwGetTime();

    StartImGUIFrame(showAnotherWindow, SceneManager, io);

    //////////////////////
    //// Handle input ////
    //////////////////////

    glfwPollEvents();
    // need to call update so that we can send out the event for the different key states
    inputManager.Update();

    //////////////////////
    //// Handle Logic ////
    //////////////////////

    Pipeline::GameLogic((int)SceneManager.GetAllPlayerCount(), SceneManager.GetAllPlayers());

    //////////////////////////
    //// Handle Rendering ////
    //////////////////////////

    /* Render here */
    Pipeline::RenderPlay(window, shader, (int)SceneManager.GetAllPlayerCount(), SceneManager.GetAllPlayers(),
                         SceneManager.GetCurrentScene(), resources, renderSystem);

    // End ImGui frame and draws the ImGui frames
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Shutdown for ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // Clean up all of the keybinds we have added
  LumiCore::InputSystem::InputSystem::GetInstance()->Reset();

  glfwTerminate();
  return 0;
}

void StartImGUIFrame(bool& showAnotherWindow, LumiCore::Scene::SceneManager& sceneManager, ImGuiIO& io)
{
  // Start ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  { // Draw scene switcher
    ImGui::Begin("LumiEngine");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::Checkbox("Check Scene List", &showAnotherWindow);

    // Show active players positions
    for (int i = 0; i < sceneManager.GetAllPlayerCount(); i++)
    {
      PlayerStuff::Player& p = sceneManager.GetPlayerByIndex(i);
      std::string playerInfoText = std::format("Player {}: pos {},{},{}: rot {},{},{}", i + 1, p.Position.x,
                                               p.Position.y, p.Position.z, p.Rotation.x, p.Rotation.y, p.Rotation.z);
      ImGui::Text("%s", playerInfoText.c_str());
    }

    // switch between split screen or not
    ImGui::Text("Switch between split screen");
    ImGui::SameLine();
    if (ImGui::Button("Toggle"))
    {
      sceneManager.GetAllPlayerCount() == 2 ? sceneManager.RemovePlayer(1) : sceneManager.CreateNewPlayer();
    }

    ImGui::End();
  }

  if (showAnotherWindow)
  {
    std::vector<std::string> sceneList = sceneManager.GetAllScenes();

    ImGui::Begin("Scene Switcher", &showAnotherWindow);

    ImGui::Text("Switch Scene by Pressing on the button");
    for (int i = 0; i < sceneList.size(); ++i)
    {
      std::string name = sceneList[i];
      if (ImGui::Button(name.c_str()))
      {
        // Switch the scene
        sceneManager.SwitchScene(name);

        // Move the players to spawn location
        for (int i = 0; i < sceneManager.GetAllPlayerCount(); i++)
        {
          LumiCore::Transform transform = sceneManager.GetCurrentScene().GetSpawnTransformByIndex(i);
          PlayerStuff::Player& player = sceneManager.GetPlayerByIndex(i);
          player.Position = transform.Position;
          player.Rotation = transform.Rotation;
        }
      }
    }
    if (ImGui::Button("Close"))
    {
      showAnotherWindow = false;
    }
    ImGui::End();
  }
}


