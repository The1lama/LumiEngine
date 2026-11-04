#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "LumiCore/2D/Quad.h"
#include "LumiCore/2D/Triangle.h"
#include "LumiCore/3D/Cube.h"
#include "LumiCore/3D/Pyramid.h"
#include "LumiCore/3D/Sphere.h"
#include "LumiCore/Shader.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
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
  glm::vec3 greenColor = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 blueColor = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 blackColor = glm::vec3(0.0f, 0.0f, 0.0f);

  LumiCore2D::Quad quadMesh;
  if (!quadMesh.IsValid()) {
    std::cout << "Mesh VAO could not do its thing" << '\n';
    return -10;
  }

  // Facade
  MeshData facadeData(glm::vec2(.00f, -.15f), glm::vec2(1.08f, .57f), .0f,
                      blackColor, 1);

  LumiCore3D::Cube cubeObject;
  if (!cubeObject.IsValid()) {
    std::cout << "Mesh VAO could not do its thing" << '\n';
    return -20;
  }

  LumiCore3D::Pyramid pyramidObject;
  if (!pyramidObject.IsValid()) {
    std::cout << "Mesh VAO could not do its thing" << '\n';
    return -22;
  }

  LumiCore3D::Sphere sphereObject;
  if (!sphereObject.IsValid()) {
    std::cout << "Mesh VAO could not do its thing" << '\n';
    return -23;
  }

  /////////////// PROJECTION ///////////////

  glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
  glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));

  glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

  glm::mat4 viewMatix(1.0f);
  viewMatix = glm::lookAt(cameraPos, cameraTarget, worldUp);

  const auto fov = glm::radians(45.0f);
  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

  const auto aspectRatio = (float)frameWidth / (float)frameHeight;
  const float nearPlane = 0.1f;
  const float farPlane = 100.0f;
  glm::mat4 projectionMatrix =
      glm::perspective(fov, aspectRatio, nearPlane, farPlane);

  ///////////////

  Shader basic2DShader =
      Shader("assets/shaders/2dShader.vert", "assets/shaders/2dShader.frag");
  if (!basic2DShader.IsValid()) {
    std::cout << "2D Shader could not compile" << '\n';
    return -3;
  }

  Shader basic3DShader = Shader("assets/shaders/3dBasicShader.vert",
                                "assets/shaders/3dBasicShader.frag");
  if (!basic3DShader.IsValid()) {
    std::cout << "3D Shader could not compile" << '\n';
    return -2;
  }

  ///////////////

  std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";

  bool bToggleRoofColor = false;
  float lastInputTime;
  float inputDeadTime = 0.4f;

  glClearColor(.1f, .09f, .12f, 1.0f);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // draws a wireframe of the
  // model
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnable(GL_DEPTH_TEST);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    float runningTime = glfwGetTime();

    if (runningTime - lastInputTime > inputDeadTime) {
      if (processInput(window, bToggleRoofColor))
        lastInputTime = runningTime;
    }

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    basic2DShader.Use();

    //// Facade
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model =
        glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));

    basic2DShader.SetMatrix4("u_Model", model);
    basic2DShader.SetVec3("uColor", blueColor);
    basic2DShader.SetMatrix4("u_Projection", projectionMatrix);
    basic2DShader.SetMatrix4("u_View", viewMatix);
    quadMesh.Render(basic2DShader);

    basic3DShader.Use();

    glm::mat4 cubeModel = glm::mat4(1.0f);

    cubeModel = glm::translate(cubeModel, glm::vec3(0.4f, 0.0f, 0.0f));

    cubeModel = glm::rotate(cubeModel, glm::radians(-55.0f),
                            glm::vec3(1.0f, 0.0f, 0.0f));
    cubeModel = glm::rotate(cubeModel, glm::radians(-55.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));

    cubeModel = glm::scale(cubeModel, glm::vec3(0.5, 0.5, 0.5));

    basic2DShader.SetVec3("uColor", redColor);
    basic3DShader.SetMatrix4("u_Projection", projectionMatrix);
    basic3DShader.SetMatrix4("u_View", viewMatix);
    basic3DShader.SetMatrix4("u_Model", cubeModel);
    cubeObject.Render(basic3DShader);

    glm::mat4 pyramidModel = glm::mat4(1.0f);

    pyramidModel = glm::translate(pyramidModel, glm::vec3(-0.8f, 1.1f, 0.0f));

    pyramidModel = glm::rotate(pyramidModel, glm::radians(-73.0f),
                               glm::vec3(1.0f, 0.0f, 0.0f));
    pyramidModel = glm::rotate(pyramidModel, glm::radians(-35.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f));

    pyramidModel = glm::scale(pyramidModel, glm::vec3(0.3, 0.3, 0.3));

    basic3DShader.SetMatrix4("u_Model", pyramidModel);
    basic2DShader.SetVec3("uColor", blueColor);
    pyramidObject.Render(basic3DShader);

    glm::mat4 sphereModel = glm::mat4(1.0f);

    sphereModel = glm::translate(sphereModel, glm::vec3(-0.8f, 1.1f, 0.0f));

    sphereModel = glm::scale(sphereModel, glm::vec3(0.3, 0.3, 0.3));

    basic3DShader.SetMatrix4("u_Model", sphereModel);
    basic2DShader.SetVec3("uColor", greenColor);
    sphereObject.Render(basic3DShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  basic2DShader.DeleteShader();
  quadMesh.Release();

  glfwTerminate();
  return 0;
}
