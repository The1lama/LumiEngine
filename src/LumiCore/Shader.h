#pragma once

#include <glad/glad.h>
#include <string>

class Shader {
public:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader();

  void Use() const;
  void DeleteShader() const;

  unsigned int GetProgramID() const { return ID; }

  void SetBool(const std::string &name, bool value) const;
  void SetFloat(const std::string &name, float value) const;
  void SetInt(const std::string &name, int value) const;

private:
  unsigned int ID;

  GLuint compileShader(GLenum shaderType, const std::string &source,
                       const std::string &label);
  GLuint createShaderProgram(const std::string &vertexPath,
                             const std::string &fragmentPath);
  std::string readTextFile(const std::string &filePath);
};
