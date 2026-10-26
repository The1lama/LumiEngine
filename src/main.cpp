#include <GLFW/glfw3.h>
#include <fstream>
#include <glad/glad.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

void glfwErrorCallback(int error, const char *description) {
  std::cout << "GLFW error (" << error << "): " << description << "\n";
}

void framebufferSizeCallback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

std::string readTextFile(const std::string &path) {
  std::ifstream file(path);
  if (!file)
    throw std::runtime_error("Could not open file: " + path);

  std::ostringstream contents;
  contents << file.rdbuf();
  return contents.str();
}

GLuint compileShader(GLenum type, const std::string &source,
                     const std::string &label) {
  const GLuint shader = glCreateShader(type);
  const char *sourcePtr = source.c_str();

  glShaderSource(shader, 1, &sourcePtr, nullptr);
  glCompileShader(shader);

  GLint success = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (success == GL_FALSE) {
    GLint logLenght = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLenght);

    std::string log(static_cast<std::size_t>(logLenght), '\0');
    glGetShaderInfoLog(shader, logLenght, nullptr, log.data());

    glDeleteShader(shader);
    throw std::runtime_error("Shader compilation failed (" + label + "):\n" +
                             log);
  }
  return shader;
}

GLuint createShaderProgram(const std::string &vertexPath,
                           const std::string &fragmentPath) {
  const std::string vertexSource = readTextFile(vertexPath);
  const std::string fragmentSource = readTextFile(fragmentPath);

  const GLuint vertexShader =
      compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);

  const GLuint fragmentShader =
      compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

  const GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  GLint success = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (success == GL_FALSE) {
    GLint logLenght = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLenght);

    std::string log(static_cast<std::size_t>(logLenght), '\0');
    glGetProgramInfoLog(program, logLenght, nullptr, log.data());

    glDeleteProgram(program);
    throw std::runtime_error("Shader program link failed:\n" + log);
  }
  return program;
}

int main(int argc, char **argv) {
  std::cout << "hello world!" << std::endl;

  glfwSetErrorCallback(glfwErrorCallback);
  /* Initialize the library */
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW.\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  GLFWwindow *window =
      glfwCreateWindow(640, 480, "LumiEngine", nullptr, nullptr);

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

  float vertices[] = {// Position // Color
                      -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f,
                      0.0f,  1.0f,  0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,  1.0f};

  unsigned int VBO, VAO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);

  glBindVertexArray(VAO);

  // Create VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  constexpr GLsizei stride = 6 * sizeof(float);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        static_cast<void *>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  GLuint shaderProgram = 0;

  try {
    shaderProgram = createShaderProgram("assets/shaders/basic.vert",
                                        "assets/shaders/basic.frag");
  } catch (const std::exception &exception) {
    std::cout << exception.what() << '\n';
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  ///////////////

  std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClearColor(.1f, .09f, .12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // draws a wireframe of the
    // model
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    /* Poll for and process events */
    glfwPollEvents();
  }

  glDeleteProgram(shaderProgram);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);

  glfwTerminate();
  return 0;
}
