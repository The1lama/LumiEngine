#version 460 core 

layout (location = 0) in vec3 aPosition;

uniform vec3 uColor;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 vertexColor;

void main(){
  vertexColor = uColor;

  vec4 aPos = u_Projection * u_View * u_Model * vec4(aPosition, 1.0);
  gl_Position = aPos;

}

