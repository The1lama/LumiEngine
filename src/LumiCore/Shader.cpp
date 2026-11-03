#include "Shader.h"

#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  m_ID = createShaderProgram(vertexPath, fragmentPath);
}
Shader::~Shader() { DeleteShader(); }

void Shader::Use() const { glUseProgram(m_ID); }

bool Shader::IsValid() const {
  if (m_ID == 0 && !glIsProgram(m_ID))
    return false;

  GLint success;
  glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
  if (!success) {
    char infolog[1024];
    glGetProgramInfoLog(m_ID, 1024, NULL, infolog);
    std::cout << "The program failed to compile with the error:" << std::endl
              << infolog << std::endl;
    return false;
  }
  return true;
}

void Shader::DeleteShader() const { glDeleteProgram(m_ID); }

///////// Set Uniforms /////////

void Shader::SetBool(const std::string &name, bool value) const {
  glUniform1i(getUniformLocation(name), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const {
  glUniform1i(getUniformLocation(name), value);
}

void Shader::SetFloat(const std::string &name, float value) const {
  glUniform1f(getUniformLocation(name), value);
}

void Shader::SetVec3(const std::string &name, glm::vec3 value) const {
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMatrix4(const std::string &name, glm::mat4 value) const {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE,
                     glm::value_ptr(value));
}

///////// Set Uniforms /////////

GLint Shader::getUniformLocation(const std::string &name) const {
  return glGetUniformLocation(m_ID, name.c_str());
}

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

  // check if there was any linking errors
  int success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}
