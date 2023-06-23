//vertex.glsl
#version 120                  // GLSL 1.20

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec3 a_normal;

uniform mat4 u_PVM;

// for phong shading
uniform mat4 u_model_matrix;
uniform mat3 u_normal_matrix;

uniform vec3 u_light_position;
uniform vec3 u_light_ambient;
uniform vec3 u_light_diffuse;
uniform vec3 u_light_specular;

uniform vec3  u_obj_ambient;
uniform vec3  u_obj_diffuse;
uniform vec3  u_obj_specular;
uniform float u_obj_shininess;

uniform vec3 u_camera_position;
uniform mat4 u_view_matrix;

// varying vec3 v_color;
// varying 변수들은 fragment shader로 전달되어 per-pixel shading에 사용. 각 변수는 per-pixel 위치, 법선, 및 시선 방향을 나타냄
//todo 
varying vec3 v_position_wc;   // per-pixel position in world coordinates
varying vec3 v_normal_wc;     // per-pixel normal in world coordinates
varying vec3 v_view_dir_wc;   // per-pixel view direction in world coordinates

vec3 directional_light() 
{
  vec3 color = vec3(0.0);

  vec3 position_wc = (u_model_matrix * vec4(a_position, 1.0f)).xyz;
  vec3 normal_wc   = normalize(u_normal_matrix * a_normal);

  // vec3 light_dir = normalize(u_light_position);
  vec3 light_dir = normalize(u_light_position - position_wc);

  // ambient
  color += (u_light_ambient * u_obj_ambient);
  
  // diffuse 
  float ndotl = max(dot(normal_wc, light_dir), 0.0);
  color += (ndotl * u_light_diffuse * u_obj_diffuse);

  // specular
  vec3 view_dir = normalize(u_camera_position - position_wc);
  vec3 reflect_dir = reflect(-light_dir, normal_wc); 

  float rdotv = max(dot(view_dir, reflect_dir), 0.0);
  color += (pow(rdotv, u_obj_shininess) * u_light_specular * u_obj_specular);

  return color;
}

void main()
{
  //gl_Position = u_PVM * vec4(a_position, 1.0f);
  //v_color = directional_light();
  // varying 변수들을 계산하고 전달합니다. 위치, 법선, 및 시선 방향은 world coordinates에서 계산
  // todo
  v_position_wc = (u_model_matrix * vec4(a_position, 1.0f)).xyz;
  v_normal_wc   = normalize(u_normal_matrix * a_normal);
  v_view_dir_wc = normalize(u_camera_position - v_position_wc);

  // 정점의 최종 위치를 계산하고 gl_Position에 할당. 이는 변환된 정점을 프래그먼트 쉐이더로 전달함.
  gl_Position = u_PVM * vec4(a_position, 1.0f);
  
}