#include <GLFW/glfw3.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "LumiCore/Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "LumiCore/3D/Cube.h"
#include "LumiCore/3D/GeneratedMesh.h"
#include "LumiCore/3D/Pyramid.h"
#include "LumiCore/3D/Sphere.h"
#include "LumiCore/Camera.h"
#include "LumiCore/Mesh.h"
#include "LumiCore/Shader.h"
#include "LumiCore/Utility.h"
#include "LumiCore/logger/Logger.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

void glfwErrorCallback(int error, const char* description)
{
  LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("GLFW error ({}): {}", error, description));
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); }

struct EditorSettings
{
  int windowWidth = 640;
  int windowHeight = 480;
  const char* windowTitle = "LumiEngine";
};

void SwitchRenderingMode();

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
    if (bSwitchDrawMode == false)
      SwitchRenderingMode();
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
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("---------------"));
  LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Debug Message ({}): {}", id, message));

  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Source: API"));
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Source: Window System"));
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Source: Shader Compiler"));
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Source: Third Party"));
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Source: Application"));
    break;
  case GL_DEBUG_SOURCE_OTHER:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Source: Other"));
    break;
  }

  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Type: Error"));
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Type: Deprecated Behavior"));
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Type: Undefined Behavior"));
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Type: Portability"));
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Type: Performance"));
    break;
  case GL_DEBUG_TYPE_MARKER:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Type: Marker"));
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Type: Push Group"));
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Type: Pop Group"));
    break;
  case GL_DEBUG_TYPE_OTHER:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Type: Other"));
    break;
  }

  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Serverity: High"));
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Serverity: Medium"));
    break;
  case GL_DEBUG_SEVERITY_LOW:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Serverity: Low"));
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    LOG(LumiCoreLogger::ELogVerbose::ERROR, std::format("Serverity: Notification"));
    break;
  }
}

static int renderingModeIndex;
void SwitchRenderingMode()
{
  renderingModeIndex = renderingModeIndex + 1 < 3 ? renderingModeIndex + 1 : 0;

  if (renderingModeIndex == 0)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // fill in the triangles
  else if (renderingModeIndex == 1)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draws in wireframe mode
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // draws vertex points
};

bool IsValidFile(const std::filesystem::path& filePath)
{
  const std::string validExtentions[] = {".obj"};
  const bool isValidExtention = std::find(std::begin(validExtentions), std::end(validExtentions),
                                          filePath.extension().string()) != std::end(validExtentions);
  const bool fileExist = LumiCore::Utility::IsValidFile(filePath.string());

  return fileExist && isValidExtention;
}

std::unique_ptr<LumiCore::Model> CustomObjectLoader(const std::filesystem::path& objectPath,
                                                    std::vector<LumiCore::Vertex>& vertices,
                                                    std::vector<unsigned int>& indices,
                                                    const std::vector<LumiCore::Texture> textures)
{
  std::vector<glm::vec3> temp_positions;
  std::vector<glm::vec3> temp_normals;
  std::vector<glm::vec2> temp_uvs;

  std::ifstream file(objectPath);
  if (!file.is_open())
  {
    LOG_CRITICAL(std::format("Failed to open OBJ file: {}", objectPath.string()));
    return nullptr;
  }

  // this will read the line in the file, line by line
  std::string line;
  while (std::getline(file, line))
  {

    // we get the current line and save it to the ss variable
    std::stringstream ss(line);
    std::string prefix;
    // we skip leading spaces and will read directly into a string until we hit
    // white spaces on the row. we then extrat said string with the >> to it is
    // gone and save the string to the prefix variable.
    ss >> prefix;

    if (prefix == std::string("v"))
    {
      std::string x;
      std::string y;
      std::string z;
      ss >> x;
      ss >> y;
      ss >> z;

      temp_positions.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
    }
    else if (prefix == std::string("vn"))
    {
      std::string x;
      std::string y;
      std::string z;
      ss >> x;
      ss >> y;
      ss >> z;

      temp_normals.push_back(glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
    }

    else if (prefix == std::string("vt"))
    {
      std::string x;
      std::string y;
      ss >> x;
      ss >> y;

      temp_uvs.push_back(glm::vec2(std::stof(x), std::stof(y)));
    }
    // Create vertexes and indicies
    else if (prefix == std::string("f"))
    {

      std::vector<std::string> faces;
      std::string faceLine;
      while (ss >> faceLine)
      {
        faces.push_back(faceLine);
      }

      // only proccess if we have more than 3 faces
      if (faces.size() < 3 || faces.size() > 4)
      {
        continue;
      }
      // assemble the faces

      // 1. Loop through the faces vector
      // 2. Split up the string by /
      // 3. each value goes from vertex position/uv/normal index: remeber that
      // .OBJ starts from 1 so we need to do -1
      // 4. create the indecies for the face

      // assemble vertex data
      for (int i = 0; i < faces.size(); ++i)
      {
        std::vector<std::string> values = LumiCore::Utility::SplitString(faces[i], "/");

        const int posIndex = std::stoi(values[0]) - 1;
        const int uvIndex = std::stoi(values[1]) - 1;
        const int normIndex = std::stoi(values[2]) - 1;

        const LumiCore::Vertex vertex{temp_positions[posIndex], temp_normals[normIndex], temp_uvs[uvIndex]};
        vertices.push_back(vertex);
      }

      // assemble indecies order
      if (faces.size() == 3)
      {
        const int first = vertices.size() - 3;
        const int second = vertices.size() - 2;
        const int third = vertices.size() - 1;
        indices.push_back(first);
        indices.push_back(second);
        indices.push_back(third);
      }
      if (faces.size() == 4)
      {
        const int first = vertices.size() - 4;
        const int second = vertices.size() - 3;
        const int third = vertices.size() - 2;
        const int fourth = vertices.size() - 1;

        // first triangle
        indices.push_back(first);
        indices.push_back(second);
        indices.push_back(third);

        // second triangle
        indices.push_back(fourth);
        indices.push_back(first);
        indices.push_back(third);
      }
    }
    else if (prefix == std::string("o"))
    {
      //
      std::string name;
      ss >> name;
      LOG_INFO(std::format("Object name: {}", name));
    }

  } // while
  LOG_INFO(std::format("Position   amount: {}", temp_positions.size()));
  LOG_INFO(std::format("Normal     amount: {}", temp_normals.size()));
  LOG_INFO(std::format("UVs        amount: {}", temp_uvs.size()));

  LOG_INFO(std::format("Vertexes   amount: {}", vertices.size()));
  LOG_INFO(std::format("Indices    amount: {}", indices.size()));

  return std::make_unique<LumiCore::Model>(vertices, indices, textures);
}

std::unique_ptr<LumiCore::Model> GenerateModel(const std::string& path)
{
  return std::make_unique<LumiCore::Model>(path.c_str());
}

std::unique_ptr<LumiCore::Model> GenerateObject(const std::filesystem::path& objectPath,
                                                const std::vector<LumiCore::Texture> textures)
{
  constexpr bool useCustomObjectLoader = false;
  //
  // Validate check if the file extention is correct and if file exist
  if (!IsValidFile(objectPath))
  {
    LOG_WARNING(std::format("File {} is not valid", objectPath.filename().string()));
  };


  std::unique_ptr<LumiCore::Model> returnModel;
  if (useCustomObjectLoader)
  {
    std::vector<LumiCore::Vertex> vertices;
    std::vector<unsigned int> indices;

    return CustomObjectLoader(objectPath, vertices, indices, textures);
  }
  else
  {
    return GenerateModel(objectPath.string());
  }
}


int main()
{
  LOG(LumiCoreLogger::ELogVerbose::INFO, "Debugger initialized");

#pragma region Init
  /* Initialize the library */
  if (!glfwInit())
  {
    LOG(LumiCoreLogger::ELogVerbose::ERROR, "Failed to initialize GLFW");
    return -1;
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
    LOG(LumiCoreLogger::ELogVerbose::ERROR, "Something on window initialize went wrong");
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSwapInterval(1);

  // window cache size
  int cacheWindowWitdh;
  int cacheWindowHeigh;
  glfwGetFramebufferSize(window, &cacheWindowWitdh, &cacheWindowHeigh);

  const int loadedVersion = gladLoadGL();
  if (loadedVersion == 0)
  {
    LOG_ERROR("Failed to load OpenGL functions with GLAD");
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
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

  ///////////////
#pragma endregion Init

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

  LumiCore3D::Cube cubeObject(textures);
  if (!cubeObject.IsValid())
  {
    LOG_WARNING("Mesh VAO coud not be initialized for cube mesh");
    return -20;
  }

  LumiCore3D::Pyramid pyramidObject(textures);
  if (!pyramidObject.IsValid())
  {
    LOG_WARNING("Mesh VAO coud not be initialized for pyramid mesh");
    return -22;
  }

  textures[0] = noiseTexture;

  LumiCore3D::Sphere sphereObject(textures);
  if (!sphereObject.IsValid())
  {
    LOG_WARNING("Mesh VAO coud not be initialized for sphere mesh");
    return -23;
  }

  // std::unique_ptr<LumiCore::Model> ape = GenerateObject("assets/objects/MonkeyHat.obj", textures);
  std::unique_ptr<LumiCore::Model> backpack =
      GenerateObject("assets/objects/smallBuilding/SmallBuilding02.obj", textures);
  // GenerateObject("assets/objects/Monkey.obx", 0);
  // GenerateObject("assets/objects/thing.obj", 0);

#pragma endregion MeshObjects

  /////////////// PROJECTION ///////////////

  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);

  const auto fov = glm::radians(45.0f);
  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);
  float aspectRatio = (float)frameWidth / (float)frameHeight;

  constexpr float nearPlane = 0.01f;
  constexpr float farPlane = 100.0f;
  LumiCore::Camera camera(cameraPos, fov, aspectRatio, nearPlane, farPlane);

  ///////////////

  ///////////////

  Shader basic3DShader = Shader("assets/shaders/3dBasicShader.vert", "assets/shaders/3dBasicShader.frag");
  if (!basic3DShader.IsValid())
  {
    LOG_ERROR("3D shader could not compile");
    return -2;
  }
  basic3DShader.Use();
  // these we only need to set one time as they are the same for now
  basic3DShader.SetMatrix4("u_View", camera.GetViewMatrix());
  basic3DShader.SetMatrix4("u_Projection", camera.GetProjectionMatrix());

  basic3DShader.UnUse();

  ///////////////

  LOG_INFO(std::format("OpenGL: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION))));
  LOG_INFO(std::format("Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER))));

  bool bToggleRoofColor = false;
  float lastInputTime;
  bool bSwitchDrawingMode = false;
  //
  // Switch between projection matrixes
  bool bHasSwitchedProjectionMatrix = false;
  int projectionIndex = 0;

  glClearColor(.1f, .09f, .12f, 1.0f);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

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

      basic3DShader.Use();
      basic3DShader.SetMatrix4("u_View", camera.GetViewMatrix());
      basic3DShader.UnUse();
      cacheWindowHeigh = currentWindowHeight;
      cacheWindowWitdh = currentWindowWidth;
    }

    bool bSwitchProjectionMatrix = false;
    processInput(window, bToggleRoofColor, bSwitchDrawingMode, bSwitchProjectionMatrix);

    if (bSwitchProjectionMatrix && !bHasSwitchedProjectionMatrix)
    {
      camera.SwitchProjectionMatrix();

      basic3DShader.Use();
      basic3DShader.SetMatrix4("u_Projection", camera.GetProjectionMatrix());
      basic3DShader.UnUse();
    }

    bHasSwitchedProjectionMatrix = bSwitchProjectionMatrix;

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    basic3DShader.Use();

    const glm::vec3 forward(1.0f, 0.0f, 0.0f);
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    const glm::vec3 right(0.0f, 0.0f, 1.0f);

    { // Cube 1
      glm::mat4 cubeModel = glm::mat4(1.0f);

      cubeModel = glm::translate(cubeModel, glm::vec3(-2.0f, 1.5f, 0.0f));

      cubeModel = glm::rotate(cubeModel, glm::radians(90.0f), up);
      cubeModel = glm::rotate(cubeModel, glm::radians(90.0f), forward);

      cubeModel = glm::scale(cubeModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("u_Color", redColor);
      basic3DShader.SetMatrix4("u_Model", cubeModel);
      cubeObject.Render(basic3DShader);
    }

    { // Cube 2
      glm::mat4 cubeModel = glm::mat4(1.0f);

      cubeModel = glm::translate(cubeModel, glm::vec3(-2.0f, -1.5f, 0.0f));

      cubeModel = glm::rotate(cubeModel, glm::radians(-90.0f), up);
      cubeModel = glm::rotate(cubeModel, glm::radians(-90.0f), forward);

      cubeModel = glm::scale(cubeModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("u_Color", redColor);
      basic3DShader.SetMatrix4("u_Model", cubeModel);
      cubeObject.Render(basic3DShader);
    }

    { // Pyramid 1
      glm::mat4 pyramidModel = glm::mat4(1.0f);

      pyramidModel = glm::translate(pyramidModel, glm::vec3(0.0f, 1.5f, 0.0f));

      pyramidModel = glm::rotate(pyramidModel, glm::radians(90.0f), up);

      pyramidModel = glm::scale(pyramidModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("u_Color", blueColor);
      basic3DShader.SetMatrix4("u_Model", pyramidModel);
      pyramidObject.Render(basic3DShader);
    }

    { // Pyramid 2
      glm::mat4 pyramidModel = glm::mat4(1.0f);

      pyramidModel = glm::translate(pyramidModel, glm::vec3(0.0f, -1.5f, 0.0f));

      pyramidModel = glm::rotate(pyramidModel, glm::radians(-90.0f), up);

      pyramidModel = glm::scale(pyramidModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("u_Color", blueColor);
      basic3DShader.SetMatrix4("u_Model", pyramidModel);
      pyramidObject.Render(basic3DShader);
    }

    { // Sphere 1
      glm::mat4 sphereModel = glm::mat4(1.0f);

      sphereModel = glm::translate(sphereModel, glm::vec3(2.0, 1.5f, 0.0f));

      sphereModel = glm::rotate(sphereModel, glm::radians(0.0f), up);

      sphereModel = glm::scale(sphereModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("u_Color", greenColor);
      basic3DShader.SetMatrix4("u_Model", sphereModel);
      sphereObject.Render(basic3DShader);
    }

    { // Sphere 2
      glm::mat4 sphereModel = glm::mat4(1.0f);

      sphereModel = glm::translate(sphereModel, glm::vec3(2.0, -1.5f, 0.0f));

      sphereModel = glm::rotate(sphereModel, glm::radians(-180.0f), up);
      sphereModel = glm::rotate(sphereModel, glm::radians(-90.0f), right);

      sphereModel = glm::scale(sphereModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("u_Color", greenColor);
      basic3DShader.SetMatrix4("u_Model", sphereModel);
      sphereObject.Render(basic3DShader);
    }
    { // ape
      glm::mat4 apeModel = glm::mat4(1.0f);

      apeModel = glm::translate(apeModel, glm::vec3(0.0, -0.2f, 3.0f));
      apeModel *= glm::rotate(glm::mat4(1.0f), glm::radians(runningTime * 50), up);
      apeModel = glm::scale(apeModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("u_Color", greenColor);
      basic3DShader.SetMatrix4("u_Model", apeModel);
      // ape->Draw(basic3DShader);
      backpack->Draw(basic3DShader);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // ape.reset();

  glfwTerminate();
  return 0;
}
