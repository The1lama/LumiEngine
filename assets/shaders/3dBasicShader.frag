#version 460 core

in vec3 i_VertexColor;
in vec3 i_Normal;
in vec2 i_UvCoord;

in vec3 i_FragPos;
in vec3 i_ViewPosition;

in vec3 i_VertexNormal;

uniform float uTime;
uniform sampler2D u_Texture_0;
uniform sampler2D u_Texture_1;
uniform sampler2D u_Texture_2;

out vec4 FragColor;

vec3 CalculateLighting(){
  const vec3 lightColor = vec3(1.0, 1.0, 1.0);

  // Calculate ambient light
  const float ambientStrenght = 0.4;
  vec3 ambientLightColor = ambientStrenght * lightColor;

  // Calculate diffuse light
  const vec3 lightPos = vec3(100.0,4.0,10.0);
  vec3 normFace = normalize(i_VertexNormal);
  vec3 lightDir = normalize(lightPos - i_FragPos);

  float diffuseLight = max(dot(normFace, lightDir), 0.0);
  vec3 diffuse = vec3(diffuseLight);

  // Calculate specular light
  const float specularStrenght = 0.3; // how bright the specular light should be, 0-1
  vec3 viewDir = normalize(i_ViewPosition - i_FragPos);
  vec3 reflectionDir = reflect(-lightDir, normFace);
  float spec = pow(max(dot(viewDir, reflectionDir), 0.0), 16); // The last value should always be in the power of 2, it is the shinnyness of the material
  vec3 specular = specularStrenght * spec * lightColor;

  return ambientLightColor + diffuse + specular;
}

void main(){

  // Getting the color we should use in the fragment shader
  //vec3 d = vec3(i_Normal.x, i_Normal.y, i_Normal.z);
  //vec4 col = vec4(d, 1.0) *0.5+0.5;
  vec4 tex = texture(u_Texture_0, i_UvCoord);

  vec3 light = CalculateLighting();

  vec4 result = tex * vec4(light, 1.0);
  FragColor = result;
}

