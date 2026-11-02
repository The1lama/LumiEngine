#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "LumiCore/2D/Quad.h"
#include "LumiCore/2D/Triangle.h"
#include "LumiCore/Shader.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <iostream>

void glfwErrorCallback(int error, const char *description) {
  std::cout << "GLFW error (" << error << "): " << description << "\n";
}

void framebufferSizeCallback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

struct EditorSettings {
  int windowWidth = 640;
  int windowHeight = 480;
  const char *windowTitle = "LumiEngine";
};

struct MeshData {
  glm::vec2 offset;
  glm::vec2 scale;
  glm::vec3 color;
  float angle;
  float time;

  MeshData(glm::vec2 Offset, glm::vec2 Scale, float Angle, glm::vec3 Color,
           float Time)
      : offset(Offset), scale(Scale), color(Color), angle(Angle), time(Time) {};
};

void DrawTriangle(Shader &shader, LumiCore::Mesh &mesh,
                  const float runningTime) {
  mesh.SetTime(runningTime);
  mesh.Render(shader);
}
void DrawQuad(Shader &shader, LumiCore::Mesh &mesh,
              const MeshData uniformData) {
  mesh.SetOffset(uniformData.offset);
  mesh.SetScale(uniformData.scale);
  mesh.SetAngle(uniformData.angle);
  mesh.SetColor(uniformData.color);
  mesh.SetTime(uniformData.time);
  mesh.Render(shader);
}

bool processInput(GLFWwindow *window, bool &bToggleColor) {
  bool hasPressed = false;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    bToggleColor = !bToggleColor;
    hasPressed = true;
  }
  return hasPressed;
}

void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                   GLsizei lenght, const char *message, const void *userParam) {
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  std::cout << "---------------" << '\n';
  std::cout << "Debug Message (" << id << "): " << message << '\n';

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    std::cout << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cout << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cout << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cout << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cout << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cout << "Source: Other";
    break;
  }
  std::cout << '\n';

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "Type: Behavior";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cout << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cout << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cout << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "Type: Other";
    break;
  }
  std::cout << '\n';

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "Severity: Medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "Severity: Low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "Severity: Notification";
    break;
  }
  std::cout << '\n';
  std::cout << '\n';
}

int main() {

  /* Initialize the library */
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW.\n";
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
    std::cout << "Something on window initialize went wrong" << "\n";
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSwapInterval(1);

  const int loadedVersion = gladLoadGL();
  if (loadedVersion == 0) {
    std::cout << "Failed to load OpenGL functions with GLAD.\n";
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    std::cout << "initialize debug output" << '\n';
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }
  std::cout << "output" << '\n';

  ///////////////

  /// Colors ///
  glm::vec3 redColor = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 blueColor = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 blackColor = glm::vec3(0.0f, 0.0f, 0.0f);

  // Roof
  LumiCore2D::Triangle roofTriangle;
  roofTriangle.SetOffset(glm::vec2(0, .42f));
  roofTriangle.SetScale(glm::vec2(1.18f, .6f));
  roofTriangle.SetAngle(0);

  LumiCore2D::Quad quadMesh;

  // Facade
  MeshData facadeData(glm::vec2(.00f, -.15f), glm::vec2(1.08f, .57f), .0f,
                      blackColor, 1);

  // Quad facadeQuad;
  // facadeQuad.SetOffset(glm::vec2(.00f, -.15f));
  // facadeQuad.SetScale(glm::vec2(1.08f, .57f));
  // facadeQuad.SetAngle(0);
  // facadeQuad.SetColor(blackColor);

  // Door
  // Quad doorQuad;
  MeshData doorData(glm::vec2(.00f, -.285f), glm::vec2(.17f, .30f), .0f,
                    redColor, 1);

  // doorQuad.SetOffset(glm::vec2(.00f, -.285f));
  // doorQuad.SetScale(glm::vec2(.17f, .30f));
  // doorQuad.SetAngle(0);
  // doorQuad.SetColor(redColor);

  // Window Left
  MeshData leftWindowData(glm::vec2(-.33f, -.08f), glm::vec2(.20f, .20f), -2.0f,
                          blueColor, 1);
  // Quad leftWindowQuad;
  // leftWindowQuad.SetOffset(glm::vec2(-.33, -.08f));
  // leftWindowQuad.SetScale(glm::vec2(.20f, .20f));
  // leftWindowQuad.SetAngle(-2);
  // leftWindowQuad.SetColor(blueColor);

  // Window Right
  MeshData rightWindowData(glm::vec2(.33f, -.15f), glm::vec2(.10f, .20f), .12f,
                           blueColor, 1);
  // Quad rightWindowQuad;
  // rightWindowQuad.SetOffset(glm::vec2(.33, -.15f));
  // rightWindowQuad.SetScale(glm::vec2(.10f, .20f));
  // rightWindowQuad.SetAngle(12);
  // rightWindowQuad.SetColor(blueColor);

  ///////////////

  Shader basicShader =
      Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

  ///////////////

  std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";

  bool bToggleRoofColor = false;
  float lastInputTime;
  float inputDeadTime = 0.4f;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    float runningTime = glfwGetTime();

    if (runningTime - lastInputTime > inputDeadTime) {
      if (processInput(window, bToggleRoofColor))
        lastInputTime = runningTime;
    }

    /* Render here */
    glClearColor(.1f, .09f, .12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // draws a wireframe of the
    // model
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    basicShader.Use();

    // Facade
    facadeData.time = runningTime;
    DrawQuad(basicShader, quadMesh, facadeData);

    // Roof
    roofTriangle.SetColor(bToggleRoofColor ? redColor : blueColor);
    DrawTriangle(basicShader, roofTriangle, runningTime + .3f);

    // Door
    DrawQuad(basicShader, quadMesh, doorData);

    // Window Left
    DrawQuad(basicShader, quadMesh, leftWindowData);

    // Window Righ
    rightWindowData.time = runningTime + .4f;
    DrawQuad(basicShader, quadMesh, rightWindowData);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  basicShader.DeleteShader();
  roofTriangle.Release();
  quadMesh.Release();

  glfwTerminate();
  return 0;
}
