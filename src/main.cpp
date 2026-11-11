#include <GLFW/glfw3.h>
#include <corecrt_math.h>
#include <format>
#include <glad/glad.h>

#include "LumiCore/2D/Quad.h"
#include "LumiCore/3D/Cube.h"
#include "LumiCore/3D/Pyramid.h"
#include "LumiCore/3D/Sphere.h"
#include "LumiCore/Shader.h"
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

void mat4_Rotation(glm::mat4 &matrix, const float radians, const int axis) {
  // glm::rotate(cubeModel, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  // Precalculate the cos and sine values for references
  const float cosValue = glm::cos(radians);
  const float sinValue = glm::sin(radians);

  // Why we are multiplying whole vectors instead of just the single digits
  // is because will then store existing position and scale values. And
  // because it will compute faster than doing individual single float
  // multiplications and then changing the value one at a time.

  // Why we are adding together two different columns is because when we rotate
  // the object on the local axis the old "up" vector is not the same as before
  // and would have moved into each other's spaces. And by adding the two
  // different columns we are calculating the new local up direction vectors.

  // x axis Rotation
  // Take out the columns to reference when we do the multiplications
  if (axis == 0) {
    glm::vec4 col1 = matrix[1];
    glm::vec4 col2 = matrix[2];
    matrix[1] = col1 * cosValue + col2 * sinValue;
    matrix[2] = col1 * -sinValue + col2 * cosValue;
  }

  // y axis Rotation
  if (axis == 1) {
    glm::vec4 col0 = matrix[0];
    glm::vec4 col2 = matrix[2];
    matrix[0] = col0 * cosValue - col2 * sinValue;
    matrix[2] = col0 * sinValue + col2 * cosValue;
  }

  // z axis Rotation
  if (axis >= 2) {
    glm::vec4 col0 = matrix[0];
    glm::vec4 col1 = matrix[1];
    matrix[0] = col0 * cosValue + col1 * sinValue;
    matrix[1] = col0 * -sinValue + col1 * cosValue;
  }
}
void mat4_Transform(glm::mat4 &matrix, const glm::vec3 newLocation) {
  // glm::translate(cubeModel, glm::vec3(0.4f, 0.0f, 0.0f));

  // clang-format off
  // What is happening in this line of code is that we are taking the matrix 
  // scale and mulipy it with the new vector locations that we want to move towards,
  // this is what we do for XYZ and in the end we add our old transform location and the sum
  // of all the values becomes our new location in local space.
  //
  // Why we are multiplying scale and rotation is becuase we need to know in what direction 
  // we are going to move the vertex location to. And why it does not clash with scale and
  // rotation in the vertex shader is because they are doing different things, in the vertex shader 
  // we are moving a single vertex point to its correct location while here we are moving the whole object 
  // to its correct location.
  //
  //                      Forward Vector                    Up Vector                         Right Vector
  // new Position = (scaleX * direction amount X) + (scaleY * direction amount Y) + (scaleZ * direction amount Z)
  //
  // clang-format on

  // clang-format off
  matrix[3] = matrix[0] * newLocation.x +
              matrix[1] * newLocation.y + 
              matrix[2] * newLocation.z +
              matrix[3];
  // clang-format on
}
void mat4_Scale(glm::mat4 &matrix, const glm::vec3 scaleValue) {
  // glm::scale(cubeModel, glm::vec3(0.5, 0.5, 0.5));

  // NOTE: DO NOT run scale after we have rotated the matrix as
  // the scale and roation matrix math uses the same indexes on
  // both math functions.
  // The transformation order should be Scale -> Rotation -> Transform

  matrix[0] = matrix[0] * scaleValue.x; // x scale value
  matrix[1] = matrix[1] * scaleValue.y; // y scale value
  matrix[2] = matrix[2] * scaleValue.z; // z scale value
}

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

  // Facade
  MeshData facadeData(glm::vec2(.00f, -.15f), glm::vec2(1.08f, .57f), .0f,
                      blackColor, 1);

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

    { // Cube 1
      glm::mat4 cubeModel = glm::mat4(1.0f);

      // cubeModel = glm::translate(cubeModel, glm::vec3(0.4f, -0.4f, 0.0f));

      const float rotationMovement = runningTime * 100;
      // cubeModel = glm::scale(cubeModel, glm::vec3(0.5, 0.5, 0.5));

      mat4_Transform(cubeModel, glm::vec3(0.4f, -0.4f, 0.0f));

      mat4_Rotation(cubeModel, glm::radians(-rotationMovement), 0);
      mat4_Rotation(cubeModel, glm::radians(rotationMovement + 5), 1);

      mat4_Scale(cubeModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("uColor", redColor);
      basic3DShader.SetMatrix4("u_Model", cubeModel);
      cubeObject.Render(basic3DShader);
    }

    { // Cube 2
      glm::mat4 cubeModel = glm::mat4(1.0f);

      // cubeModel = glm::translate(cubeModel, glm::vec3(-1.2f, 0.0f, 0.0f));
      mat4_Transform(cubeModel, glm::vec3(-1.2f, 0.0f, 0.0f));

      const float rotationMovement = runningTime * 100;

      mat4_Rotation(cubeModel, glm::radians(-rotationMovement), 0);
      mat4_Rotation(cubeModel, glm::radians(rotationMovement), 1);
      mat4_Rotation(cubeModel, glm::radians(rotationMovement), 2);

      // cubeModel = glm::scale(cubeModel, glm::vec3(0.5, 0.5, 0.5));
      mat4_Scale(cubeModel, glm::vec3(0.5, 0.5, 0.5));

      basic3DShader.SetVec3("uColor", redColor);
      basic3DShader.SetMatrix4("u_Model", cubeModel);
      cubeObject.Render(basic3DShader);
    }

    { // Pyramid 1
      glm::mat4 pyramidModel = glm::mat4(1.0f);

      mat4_Transform(pyramidModel, glm::vec3(-0.8f, -1.1f, 0.0f));

      const float frequency = 5.0f;
      const float amplitude = 0.35f;
      const float heightOffset = 0.65f;
      const float sinScale =
          amplitude * glm::sin(frequency * runningTime) + heightOffset;

      mat4_Scale(pyramidModel, glm::vec3(sinScale));

      basic3DShader.SetVec3("uColor", blueColor);
      basic3DShader.SetMatrix4("u_Model", pyramidModel);
      pyramidObject.Render(basic3DShader);
    }

    { // Pyramid 2
      glm::mat4 pyramidModel = glm::mat4(1.0f);

      mat4_Transform(pyramidModel, glm::vec3(1.8f, 2.1f, -2.0f));

      const float frequency = 5.0f;
      const float amplitude = 0.35f;
      const float heightOffset = 0.0f;
      const float sinScale =
          amplitude * glm::sin(frequency * runningTime) + heightOffset;

      mat4_Scale(pyramidModel, glm::vec3(sinScale));

      basic3DShader.SetVec3("uColor", blueColor);
      basic3DShader.SetMatrix4("u_Model", pyramidModel);
      pyramidObject.Render(basic3DShader);
    }

    { // Sphere 1
      glm::mat4 sphereModel = glm::mat4(1.0f);

      const float frequency = 5.0f;
      const float amplitude = 0.35f;
      const float heightOffset = 0.0f;
      const float sinScale =
          amplitude * glm::sin(frequency * runningTime) + heightOffset;

      sphereModel =
          glm::translate(sphereModel, glm::vec3(-sinScale, 1.1f, 0.0f));

      sphereModel = glm::scale(sphereModel, glm::vec3(0.3, 0.3, 0.3));

      basic3DShader.SetVec3("uColor", greenColor);
      basic3DShader.SetMatrix4("u_Model", sphereModel);
      sphereObject.Render(basic3DShader);
    }

    { // Sphere 2
      glm::mat4 sphereModel = glm::mat4(1.0f);

      const float frequency = 5.0f;
      const float amplitude = 0.35f;
      const float heightOffset = 0.0f;
      const float sinScale =
          amplitude * glm::sin(frequency * runningTime) + heightOffset;

      sphereModel = glm::translate(sphereModel,
                                   glm::vec3(0, sinScale + 0.7f, sinScale - 4));

      sphereModel = glm::scale(sphereModel, glm::vec3(0.3, 0.3, 0.3));

      basic3DShader.SetVec3("uColor", greenColor);
      basic3DShader.SetMatrix4("u_Model", sphereModel);
      sphereObject.Render(basic3DShader);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  quadMesh.Release();

  glfwTerminate();
  return 0;
}
