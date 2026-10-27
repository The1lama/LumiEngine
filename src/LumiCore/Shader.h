#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <string>

class Shader {
public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader();

  void Use() const;
  void DeleteShader() const;

  unsigned int GetProgramID() const { return m_ID; }

  void SetBool(const std::string &name, bool value) const;
  void SetFloat(const std::string &name, float value) const;
  void SetInt(const std::string &name, int value) const;
  void SetVec3(const std::string &name, glm::vec3 value) const;

private:
  unsigned int m_ID;

  GLuint compileShader(GLenum shaderType, const std::string &source,
                       const std::string &label);
  GLuint createShaderProgram(const std::string &vertexPath,
                             const std::string &fragmentPath);
  std::string readTextFile(const std::string &filePath);
};
