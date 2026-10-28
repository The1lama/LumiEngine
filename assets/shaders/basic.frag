#version 460 core

in vec3 vertexColor;

uniform float uTime;

out vec4 FragColor;

void main(){
  float sinValue = (sin(uTime*1.4) * 0.5) + 0.8;
  vec4 newColor = vec4(vertexColor, 1.0);

  FragColor = newColor*sinValue;
}

