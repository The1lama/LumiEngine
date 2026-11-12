#version 460 core

in vec3 i_VertexColor;
in vec3 i_Normal;
in vec2 i_UvCoord;

uniform float uTime;
uniform sampler2D u_Texture;

out vec4 FragColor;

void main(){

  vec3 d = vec3(i_Normal.x, i_Normal.y, i_Normal.z);
  vec4 col = vec4(d, 1.0) *0.5+0.5;
  vec4 tex = texture(u_Texture, i_UvCoord);

  FragColor = col;
}

