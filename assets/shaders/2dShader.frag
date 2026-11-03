#version 460 core

in vec3 vertexColor;

uniform float uTime;

out vec4 FragColor;

void main(){
  vec4 newColor = vec4(vertexColor, 1.0);

  FragColor = newColor;
}

