#version 460 core 

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_Uv;

uniform vec3 u_Color;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 i_VertexColor;
out vec2 i_UvCoord;
out vec3 i_Normal;

void main(){
  i_VertexColor = a_Position;
  i_UvCoord = a_Uv;
  i_Normal = a_Normal;

  vec4 newPosition = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
  gl_Position = newPosition; 
}

