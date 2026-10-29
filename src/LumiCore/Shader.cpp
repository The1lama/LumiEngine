#include "Shader.h"

#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  m_ID = createShaderProgram(vertexPath, fragmentPath);
}
Shader::~Shader() { DeleteShader(); }

void Shader::Use() const { glUseProgram(m_ID); }

void Shader::DeleteShader() const { glDeleteProgram(m_ID); }

///////// Set Uniforms /////////

Shader Shader::SetBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
  return *this;
}
Shader Shader::SetInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
  return *this;
}

Shader Shader::SetFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
  return *this;
}

Shader Shader::SetVec3(const std::string &name, glm::vec3 value) const {
  glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1,
               glm::value_ptr(value));
  return *this;
}

///////// Set Uniforms /////////

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

  return program;
}
