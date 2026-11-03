#version 460 core 

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

uniform float xPosOffset;
uniform float yPosOffset;
uniform float xScale;
uniform float yScale;

uniform vec3 uColor;
uniform float uRotationAngle;

out vec3 vertexColor;

void main(){
  vertexColor = uColor;
  const float w = radians(uRotationAngle);

  // Rotation matrix
  mat3 A = mat3(cos(w), -sin(w), 0.0,
                sin(w),  cos(w), 0.0,
                   0.0,     0.0, 0.0);

  float newX = (aPosition.x * xScale) + xPosOffset;
  float newY = (aPosition.y * yScale) + yPosOffset;

  vec3 aPos = vec3(newX, newY, 1.0);
  gl_Position = vec4(A*aPos, 1.0);
}

