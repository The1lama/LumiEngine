#include "Shader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  ID = createShaderProgram(vertexPath, fragmentPath);
}
Shader::~Shader() { DeleteShader(); }

void Shader::Use() const { glUseProgram(ID); }

void Shader::DeleteShader() const { glDeleteProgram(ID); }

///////// Initialize code section /////////

std::string Shader::readTextFile(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file)
    throw std::runtime_error("Could not open file: " + filePath);

  std::ostringstream contents;
  contents << file.rdbuf();
  return contents.str();
}

GLuint Shader::compileShader(GLenum type, const std::string &source,
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

GLuint Shader::createShaderProgram(const std::string &vertexPath,
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
