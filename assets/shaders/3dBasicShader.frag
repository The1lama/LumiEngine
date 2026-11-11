#version 460 core

in vec3 i_VertexColor;
in vec3 i_Normal;
in vec2 i_UvCoord;

uniform float uTime;
uniform sampler2D u_Texture;

out vec4 FragColor;

void main(){

  vec3 d = vec3(i_UvCoord.x, i_UvCoord.y, 0.0);
  vec4 newColor = vec4(d, 1.0) *0.5+0.5;

  FragColor = texture(u_Texture, i_UvCoord);
}

