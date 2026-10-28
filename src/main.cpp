#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "LumiCore/Shader.h"
#include "glm/ext/vector_float2.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <vector>

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

float triangleVertexes[] = {// Position // Color
                            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
                            0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
                            -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f};
std::vector<unsigned int> triangleIndicies = {0, 1, 2};

float quadVertexes[] = {// Position // Color
                        -0.5f, -0.5f, 0.0f, 1.0f,  0.0f, 0.0f, -0.5f, 0.5f,
                        0.0f,  0.0f,  1.0f, 0.0f,  0.5f, 0.5f, 0.0f,  1.0f,
                        1.0f,  1.0f,  0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  1.0f};
std::vector<unsigned int> quadIndicies = {0, 1, 2, 0, 3, 2};

unsigned int quadVBO, quadVAO, quadEBO;
unsigned int triangleVBO, triangleVAO, triangleEBO;

void DrawTriangle(Shader &shader, const glm::vec2 xyOffsets,
                  const glm::vec2 xyScale, const glm::vec3 color,
                  const float runningTime, const float rotationAngle) {
  glBindVertexArray(triangleVAO);

  shader.SetFloat("uTime", runningTime);

  shader.SetFloat("xPosOffset", xyOffsets.x);
  shader.SetFloat("yPosOffset", xyOffsets.y);
  shader.SetFloat("xScale", xyScale.x);
  shader.SetFloat("yScale", xyScale.y);
  shader.SetVec3("uColor", color);
  shader.SetFloat("uRotationAngle", rotationAngle);

  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(triangleIndicies.size()),
                 GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}
void DrawQuad(Shader &shader, const glm::vec2 xyOffsets,
              const glm::vec2 xyScale, const glm::vec3 color,
              const float runningTime, const float rotationAngle) {
  glBindVertexArray(quadVAO);

  shader.SetFloat("uTime", runningTime);

  shader.SetFloat("xPosOffset", xyOffsets.x);
  shader.SetFloat("yPosOffset", xyOffsets.y);
  shader.SetFloat("xScale", xyScale.x);
  shader.SetFloat("yScale", xyScale.y);
  shader.SetVec3("uColor", color);
  shader.SetFloat("uRotationAngle", rotationAngle);

  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(quadIndicies.size()),
                 GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
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

  ////////////// Triangle thing //////////////
  glGenBuffers(1, &triangleVBO);
  glGenVertexArrays(1, &triangleVAO);
  glBindVertexArray(triangleVAO);
  // Create VBO
  glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertexes), triangleVertexes,
               GL_STATIC_DRAW);
  // Create EBO
  glGenBuffers(1, &triangleEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               triangleIndicies.size() * sizeof(unsigned int),
               triangleIndicies.data(), GL_STATIC_DRAW);

  constexpr GLsizei stride = 6 * sizeof(float);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        static_cast<void *>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  ////////////// Triangle thing //////////////

  ////////////// Quad thing //////////////
  glGenBuffers(1, &quadVBO);
  glGenVertexArrays(1, &quadVAO);
  glBindVertexArray(quadVAO);
  // Create VBO
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexes), quadVertexes,
               GL_STATIC_DRAW);
  // Create EBO
  glGenBuffers(1, &quadEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               quadIndicies.size() * sizeof(unsigned int), quadIndicies.data(),
               GL_STATIC_DRAW);

  constexpr GLsizei quadStride = 6 * sizeof(float);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, quadStride,
                        static_cast<void *>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, quadStride,
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  ////////////// Triangle thing //////////////

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

    glm::vec3 redColor = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 blueColor = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 blackColor = glm::vec3(0.0f, 0.0f, 0.0f);

    // Facade
    DrawQuad(basicShader, glm::vec2(0, -.15), glm::vec2(1.08, .57), blackColor,
             runningTime, 0);
    // Roof
    DrawTriangle(basicShader, glm::vec2(0, .42), glm::vec2(1.18, 0.6),
                 bToggleRoofColor ? redColor : blueColor, runningTime + .3, 0);
    // Door
    DrawQuad(basicShader, glm::vec2(0, -.285), glm::vec2(.17, .3), blueColor, 1,
             0);
    // Window Left
    DrawQuad(basicShader, glm::vec2(-.33, -.08), glm::vec2(.20, .20), redColor,
             1, -2);
    // Window Right
    DrawQuad(basicShader, glm::vec2(.33, -.15), glm::vec2(.10, .20), redColor,
             runningTime + 0.4, 12);

    glfwSwapBuffers(window);
    /* Poll for and process events */
    glfwPollEvents();
  }

  basicShader.DeleteShader();
  glDeleteBuffers(1, &triangleVBO);
  glDeleteVertexArrays(1, &triangleVAO);
  glDeleteBuffers(1, &triangleEBO);

  glDeleteBuffers(1, &quadVBO);
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadEBO);

  glfwTerminate();
  return 0;
}
