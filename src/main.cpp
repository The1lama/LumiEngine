#include "LumiCore/3D/GeneratedMesh.h"
#include "LumiCore/Mesh.h"
#include <GLFW/glfw3.h>
#include <corecrt_math.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <glad/glad.h>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "LumiCore/2D/Quad.h"
#include "LumiCore/3D/Cube.h"
#include "LumiCore/3D/Pyramid.h"
#include "LumiCore/3D/Sphere.h"
#include "LumiCore/Shader.h"
#include "LumiCore/Texture.h"
#include "LumiCore/Utility.h"
#include "LumiCore/logger/Logger.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

void glfwErrorCallback(int error, const char *description) {
  LOG(LumiCoreLogger::LOGLEVEL::ERROR,
      std::format("GLFW error ({}): {}", error, description));
}

void framebufferSizeCallback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

struct EditorSettings {
  int windowWidth = 640;
  int windowHeight = 480;
  const char *windowTitle = "LumiEngine";
};

void SwitchRenderingMode();

bool processInput(GLFWwindow *window, bool &bToggleColor, bool &bSwitchDrawMode,
                  bool &bSwitchProjectionMatrix) {
  bool hasPressed = false;

  // Change color on object //
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    hasPressed = true;
    bToggleColor = false;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
    bToggleColor = true;
  }

  // Draw mode switch //
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
    hasPressed = true;
    if (bSwitchDrawMode == false)
      SwitchRenderingMode();
    bSwitchDrawMode = true;
  }
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
    bSwitchDrawMode = false;
  }

  // Projection matrix switch //
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
    hasPressed = true;
    bSwitchProjectionMatrix = true;
  }
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
    bSwitchProjectionMatrix = false;
  }

  return hasPressed;
}

void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                   GLsizei lenght, const char *message, const void *userParam) {
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("---------------"));
  LOG(LumiCoreLogger::LOGLEVEL::ERROR,
      std::format("Debug Message ({}): {}", id, message));

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Source: API"));
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Source: Window System"));
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR,
        std::format("Source: Shader Compiler"));
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Source: Third Party"));
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Source: Application"));
    break;
  case GL_DEBUG_SOURCE_OTHER:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Source: Other"));
    break;
  }

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Type: Error"));
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR,
        std::format("Type: Deprecated Behavior"));
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR,
        std::format("Type: Undefined Behavior"));
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Type: Portability"));
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Type: Performance"));
    break;
  case GL_DEBUG_TYPE_MARKER:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Type: Marker"));
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Type: Push Group"));
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Type: Pop Group"));
    break;
  case GL_DEBUG_TYPE_OTHER:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Type: Other"));
    break;
  }

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Serverity: High"));
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Serverity: Medium"));
    break;
  case GL_DEBUG_SEVERITY_LOW:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, std::format("Serverity: Low"));
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    LOG(LumiCoreLogger::LOGLEVEL::ERROR,
        std::format("Serverity: Notification"));
    break;
  }
}

static int renderingModeIndex;
void SwitchRenderingMode() {
  renderingModeIndex = renderingModeIndex + 1 < 3 ? renderingModeIndex + 1 : 0;

  if (renderingModeIndex == 0)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // fill in the triangles
  else if (renderingModeIndex == 1)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draws in wireframe mode
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // draws vertex points
};

glm::mat4 CreateOrthographicProjection(const float left, const float right,
                                       const float top, const float bottom,
                                       const float nearPlane,
                                       const float farPlane) {
  glm::mat4 matrix(1.0f);

  // Translation
  // Why we are moving our projection matrix is because we need to
  // center the viwing box so that the middle of the viwing box is
  // between out left and right coordiantes

  matrix[3][0] = -((right + left) / (right / left));                 // Moving X
  matrix[3][1] = -((top + bottom) / (top - bottom));                 // Moving Y
  matrix[3][2] = -((farPlane + nearPlane) / (farPlane - nearPlane)); // Moving Z

  // Scaling
  ////////////////
  // and this we are doing only on the XY axies, and not the Z. This is because
  // we are drawing the targets only on the XY axies and will ignore the Z space
  // as we are not looking for depth in this projection matrix
  ////////////////

  // After translation we need to scale the boundaries to fill the
  // Canonical View Volume (CVV)
  // This is so we can map the target to values between -1,+1. on the screen,
  //
  // CVV
  // Also called normalized viewing volume. It is where the viewing frustums or
  // orthographic boxes are transformed, i.e. changes/warps the projection of
  // the target to either orthographic or perspectives projection.

  // Why we are doing 2 / original size is because ...
  matrix[0][0] = 2 / (right - left); // Scaling X
  matrix[1][1] = 2 / (top - bottom); // Scaling Y

  // Z fliping
  // Why we are Z fliping is because of how the camera has been setup together
  // with how OpenGL handels NDC.
  //
  // The camera has been set up with a Right Handed system, where the positive Z
  // is pointing outwards from the screen. i.e. the +Z is pointing towards the
  // camera.
  //
  // This differs to how OpenGL expecs NDC values to be the left handed system
  // for depth testing, so that the values goes from -1 for objects close to
  // screen and +1 for objectes furthure away from screen.
  //
  // This is why we are filpping the Z axis from the right hand system to left
  // hand.

  matrix[2][2] = -(2 / (farPlane - nearPlane)); // Scaling Y

  return matrix;
}

void GenerateProjectionMatrixValues(GLFWwindow *window,
                                    glm::mat4 &perspectiveMat,
                                    glm::mat4 &orthMat) {

  const auto fov = glm::radians(45.0f);
  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

  const auto aspectRatio = (float)frameWidth / (float)frameHeight;
  const float nearPlane = 0.1f;
  const float farPlane = 100.0f;

  // Perspective Matrix //
  perspectiveMat = glm::perspective(fov, aspectRatio, nearPlane, farPlane);

  // Orthographic Matrix //
  // const float orthSize = 2 * aspectRatio;
  //
  // this is an own distance to how far a way an object would be for it to be
  // the same size in both perspective and orthographic projection
  const float targDis = 8.0f;
  //
  // the two is to scale the tan*0.5 back to full heigh of the orth
  const float orthHeight = 2.0f * targDis * glm::tan(fov * 0.5f);
  const float orthWidth = orthHeight * aspectRatio;

  const float halfW = orthWidth * 0.5f;
  const float halfH = orthHeight * 0.5f;

  orthMat = CreateOrthographicProjection(-halfW, halfW, halfH, -halfH,
                                         nearPlane, farPlane);
}

void SwitchProjectionMatrix(Shader &inShader, glm::mat4 newProjectionMatrix) {
  inShader.Use();
  inShader.SetMatrix4("u_Projection", newProjectionMatrix);
  inShader.UnUse();
}

bool IsValidFile(const std::filesystem::path &filePath) {
  const std::string validExtentions[] = {".obj"};
  const bool isValidExtention =
      std::find(std::begin(validExtentions), std::end(validExtentions),
                filePath.extension().string()) != std::end(validExtentions);
  const bool fileExist = std::filesystem::exists(filePath);

  return fileExist && isValidExtention;
}

void CustomObjectLoader(const std::filesystem::path &objectPath,
                        std::vector<LumiCore::Vertex> &vertices,
                        std::vector<unsigned int> &indices) {
  std::vector<glm::vec3> temp_positions;
  std::vector<glm::vec3> temp_normals;
  std::vector<glm::vec2> temp_uvs;

  std::ifstream file(objectPath);
  if (!file.is_open()) {
    LOG_CRITICAL(
        std::format("Failed to open OBJ file: {}", objectPath.string()));
    return;
  }

  // this will read the line in the file, line by line
  std::string line;
  while (std::getline(file, line)) {

    // we get the current line and save it to the ss variable
    std::stringstream ss(line);
    std::string prefix;
    // we skip leading spaces and will read directly into a string until we hit
    // white spaces on the row. we then extrat said string with the >> to it is
    // gone and save the string to the prefix variable.
    ss >> prefix;

    if (prefix == std::string("v")) {
      std::string x;
      std::string y;
      std::string z;
      ss >> x;
      ss >> y;
      ss >> z;

      temp_positions.push_back(
          glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
    } else if (prefix == std::string("vn")) {
      std::string x;
      std::string y;
      std::string z;
      ss >> x;
      ss >> y;
      ss >> z;

      temp_normals.push_back(
          glm::vec3(std::stof(x), std::stof(y), std::stof(z)));
    }

    else if (prefix == std::string("vt")) {
      std::string x;
      std::string y;
      ss >> x;
      ss >> y;

      temp_uvs.push_back(glm::vec2(std::stof(x), std::stof(y)));
    }
    // Create vertexes and indicies
    else if (prefix == std::string("f")) {

      std::vector<std::string> faces;
      std::string faceLine;
      while (ss >> faceLine) {
        faces.push_back(faceLine);
      }

      // only proccess if we have more than 3 faces
      if (faces.size() < 3 || faces.size() > 4) {
        continue;
      }
      // assemble the faces

      // 1. Loop through the faces vector
      // 2. Split up the string by /
      // 3. each value goes from vertex position/uv/normal index: remeber that
      // .OBJ starts from 1 so we need to do -1
      // 4. create the indecies for the face

      // assemble vertex data
      for (int i = 0; i < faces.size(); ++i) {
        std::vector<std::string> values = Utility::SplitString(faces[i], "/");

        const int posIndex = std::stoi(values[0]) - 1;
        const int uvIndex = std::stoi(values[1]) - 1;
        const int normIndex = std::stoi(values[2]) - 1;

        const LumiCore::Vertex vertex{temp_positions[posIndex],
                                      temp_normals[normIndex],
                                      temp_uvs[uvIndex]};
        vertices.push_back(vertex);
      }

      // assemble indecies order
      if (faces.size() == 3) {
        const int first = vertices.size() - 3;
        const int second = vertices.size() - 2;
        const int third = vertices.size() - 1;
        indices.push_back(first);
        indices.push_back(second);
        indices.push_back(third);
      }
      if (faces.size() == 4) {
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
    } else if (prefix == std::string("o")) {
      //
      std::string name;
      ss >> name;
      LOG_INFO(std::format("Object name: {}", name));
    }

  } // while
  LOG_INFO(std::format("Position   amount: {}", temp_positions.size()));
  LOG_INFO(std::format("Normal     amount: {}", temp_normals.size()));
  LOG_INFO(std::format("UVs        amount: {}", temp_uvs.size()));
}

std::unique_ptr<LumiCore::Mesh>
GenerateObject(const std::filesystem::path &objectPath) {
  constexpr bool useCustomObjectLoader = true;
  //
  // Validate check if the file extention is correct and if file exist
  if (!IsValidFile(objectPath)) {
    LOG_WARNING(
        std::format("File {} is not valid", objectPath.filename().string()));
  };

  std::vector<LumiCore::Vertex> vertices;
  std::vector<unsigned int> indices;

  if (useCustomObjectLoader) {
    CustomObjectLoader(objectPath, vertices, indices);
  } else {
    (objectPath, vertices, indices);
  }

  LOG_INFO(std::format("Vertexes   amount: {}", vertices.size()));
  LOG_INFO(std::format("Indices    amount: {}", indices.size()));

  return std::make_unique<LumiCore3D::GenMesh>(vertices, indices);
}

int main() {
  LOG(LumiCoreLogger::LOGLEVEL::INFO, "Debugger initialized");

#pragma region Init
  /* Initialize the library */
  if (!glfwInit()) {
    LOG(LumiCoreLogger::LOGLEVEL::ERROR, "Failed to initialize GLFW");
    return -1;
  }

  glfwSetErrorCallback(glfwErrorCallback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  EditorSettings settings;

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  /* Create a windowed mode window and its OpenGL context */
  GLFWwindow *window =
      glfwCreateWindow(settings.windowWidth, settings.windowHeight,
                       settings.windowTitle, nullptr, nullptr);

  if (!window) {
    LOG(LumiCoreLogger::LOGLEVEL::ERROR,
        "Something on window initialize went wrong");
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
  if (loadedVersion == 0) {
    LOG_ERROR("Failed to load OpenGL functions with GLAD");
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    LOG_DEBUG("initialize debug output");
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }

  ///////////////
#pragma endregion Init

  /// Colors ///
  glm::vec3 redColor = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 greenColor = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 blueColor = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 blackColor = glm::vec3(0.0f, 0.0f, 0.0f);

#pragma region MeshObjects
  LumiCore2D::Quad quadMesh;
  if (!quadMesh.IsValid()) {
    LOG_WARNING("Mesh VAO coud not be initialized for quad mesh");
    return -10;
  }

  LumiCore3D::Cube cubeObject;
  if (!cubeObject.IsValid()) {
    LOG_WARNING("Mesh VAO coud not be initialized for cube mesh");
    return -20;
  }

  LumiCore3D::Pyramid pyramidObject;
  if (!pyramidObject.IsValid()) {
    LOG_WARNING("Mesh VAO coud not be initialized for pyramid mesh");
    return -22;
  }

  LumiCore3D::Sphere sphereObject;
  if (!sphereObject.IsValid()) {
    LOG_WARNING("Mesh VAO coud not be initialized for sphere mesh");
    return -23;
  }

  std::unique_ptr<LumiCore::Mesh> ape =
      GenerateObject("assets/objects/monkeyHat.obj");
  // GenerateObject("assets/objects/monkey.obx", 0);
  // GenerateObject("assets/objects/thing.obj", 0);

#pragma endregion MeshObjects

  /////////////// PROJECTION ///////////////

  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
  glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));

  glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

  glm::mat4 viewMatix(1.0f);
  viewMatix = glm::lookAt(cameraPos, cameraTarget, worldUp);

  // Projection Matrix //
  glm::mat4 perspectiveMatrix;
  glm::mat4 orthographicMatrix;
  GenerateProjectionMatrixValues(window, perspectiveMatrix, orthographicMatrix);

  ///////////////

  LumiCore::Texture uvTexture = LumiCore::Texture("assets/textures/uvTest.png");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, uvTexture.GetId());

  ///////////////

  Shader basic3DShader = Shader("assets/shaders/3dBasicShader.vert",
                                "assets/shaders/3dBasicShader.frag");
  if (!basic3DShader.IsValid()) {
    LOG_ERROR("3D shader could not compile");
    return -2;
  }
  basic3DShader.Use();
  // these we only need to set one time as they are the same for now
  basic3DShader.SetMatrix4("u_View", viewMatix);
  basic3DShader.UnUse();

  SwitchProjectionMatrix(basic3DShader, perspectiveMatrix);

  ///////////////

  LOG_INFO(std::format(
      "OpenGL: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION))));
  LOG_INFO(std::format("Renderer: {}", reinterpret_cast<const char *>(
                                           glGetString(GL_RENDERER))));

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
  while (!glfwWindowShouldClose(window)) {
    float runningTime = glfwGetTime();

    int currentWindowWidth;
    int currentWindowHeight;
    glfwGetFramebufferSize(window, &currentWindowWidth, &currentWindowHeight);
    if (cacheWindowWitdh != currentWindowWidth ||
        cacheWindowHeigh != currentWindowHeight) {
      GenerateProjectionMatrixValues(window, perspectiveMatrix,
                                     orthographicMatrix);
      cacheWindowHeigh = currentWindowHeight;
      cacheWindowWitdh = currentWindowWidth;
    }

    bool bSwitchProjectionMatrix = false;
    processInput(window, bToggleRoofColor, bSwitchDrawingMode,
                 bSwitchProjectionMatrix);

    if (bSwitchProjectionMatrix && !bHasSwitchedProjectionMatrix) {
      projectionIndex = ++projectionIndex % 2;
      SwitchProjectionMatrix(basic3DShader, projectionIndex == 0
                                                ? perspectiveMatrix
                                                : orthographicMatrix);
    }

    bHasSwitchedProjectionMatrix = bSwitchProjectionMatrix;

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    basic3DShader.Use();

    const glm::vec3 forward(1.0f, 0.0f, 0.0f);
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    const glm::vec3 right(0.0f, 0.0f, 1.0f);

    /*
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
    */
    { // ape
      glm::mat4 apeModel = glm::mat4(1.0f);

      apeModel = glm::translate(apeModel, glm::vec3(0.0, -0.2f, 3.0f));
      apeModel *=
          glm::rotate(glm::mat4(1.0f), glm::radians(runningTime * 50), up);
      apeModel = glm::scale(apeModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("u_Color", greenColor);
      basic3DShader.SetMatrix4("u_Model", apeModel);
      ape->Render(basic3DShader);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  quadMesh.Release();
  ape.reset();

  glfwTerminate();
  return 0;
}
