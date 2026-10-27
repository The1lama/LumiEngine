#version 460 core 

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

uniform float xPosOffset;
uniform float yPosOffset;
uniform float xScale;
uniform float yScale;

uniform vec3 uColor;

out vec3 vertexColor;

void main(){
  vertexColor = uColor;

  float newX = (aPosition.x * xScale) + xPosOffset;
  float newY = (aPosition.y * yScale) + yPosOffset;

  gl_Position = vec4(newX, newY, 1.0, 1.0);
}

