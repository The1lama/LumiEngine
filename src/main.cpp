#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "LumiCore/2D/Quad.h"
#include "LumiCore/2D/Triangle.h"
#include "LumiCore/Mesh.h"
#include "LumiCore/Shader.h"
#include "glm/ext/vector_float2.hpp"
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

void DrawTriangle(Shader &shader, Mesh &mesh, const float runningTime) {
  mesh.SetTime(runningTime);
  mesh.Render(shader);
}
void DrawQuad(Shader &shader, Mesh &mesh, const float runningTime) {
  mesh.SetTime(runningTime);
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

int main() {

  glfwSetErrorCallback(glfwErrorCallback);

  /* Initialize the library */
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW.\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  EditorSettings settings;

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

  ///////////////

  /// Colors ///
  glm::vec3 redColor = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 blueColor = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 blackColor = glm::vec3(0.0f, 0.0f, 0.0f);

  // Roof
  Triangle roofTriangle;
  roofTriangle.SetOffset(glm::vec2(0, .42f));
  roofTriangle.SetScale(glm::vec2(1.18f, .6f));
  roofTriangle.SetAngle(0);

  // Facade
  Quad facadeQuad;
  facadeQuad.SetOffset(glm::vec2(.00f, -.15f));
  facadeQuad.SetScale(glm::vec2(1.08f, .57f));
  facadeQuad.SetAngle(0);
  facadeQuad.SetColor(blackColor);

  // Door
  Quad doorQuad;
  doorQuad.SetOffset(glm::vec2(.00f, -.285f));
  doorQuad.SetScale(glm::vec2(.17f, .30f));
  doorQuad.SetAngle(0);
  doorQuad.SetColor(redColor);

  // Window Left
  Quad leftWindowQuad;
  leftWindowQuad.SetOffset(glm::vec2(-.33, -.08f));
  leftWindowQuad.SetScale(glm::vec2(.20f, .20f));
  leftWindowQuad.SetAngle(-2);
  leftWindowQuad.SetColor(blueColor);

  // Window Right
  Quad rightWindowQuad;
  rightWindowQuad.SetOffset(glm::vec2(.33, -.15f));
  rightWindowQuad.SetScale(glm::vec2(.10f, .20f));
  rightWindowQuad.SetAngle(12);
  rightWindowQuad.SetColor(blueColor);

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
    DrawQuad(basicShader, facadeQuad, runningTime);

    // Roof
    roofTriangle.SetColor(bToggleRoofColor ? redColor : blueColor);
    DrawTriangle(basicShader, roofTriangle, runningTime + .3f);

    // Door
    DrawQuad(basicShader, doorQuad, 1);

    // Window Left
    DrawQuad(basicShader, leftWindowQuad, 1);

    // Window Right
    DrawQuad(basicShader, rightWindowQuad, runningTime + .4f);

    glfwSwapBuffers(window);
    /* Poll for and process events */
    glfwPollEvents();
  }

  basicShader.DeleteShader();
  roofTriangle.Release();
  doorQuad.Release();
  leftWindowQuad.Release();
  rightWindowQuad.Release();

  glfwTerminate();
  return 0;
}
