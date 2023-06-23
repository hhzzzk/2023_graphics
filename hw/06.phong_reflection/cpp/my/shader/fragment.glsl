// fragment.glsl
#version 120                  // GLSL 1.20

uniform vec3 u_camera_position;

varying vec3 v_position_wc;
varying vec3 v_normal_wc;

uniform vec3 u_light_position;
uniform vec3 u_light_ambient;
uniform vec3 u_light_diffuse;
uniform vec3 u_light_specular;

uniform vec3  u_obj_ambient;
uniform vec3  u_obj_diffuse;
uniform vec3  u_obj_specular;
uniform float u_obj_shininess;

vec4 phong_shading()
{
    vec3 color = vec3(0.0f);
    
    vec3 position_wc = v_position_wc;
    vec3 normal_wc = normalize(v_normal_wc);
    
    vec3 light_dir = normalize(u_light_position);
    vec3 reflect_dir = reflect(-light_dir, normal_wc);
    
    vec3 view_dir = normalize(u_camera_position - position_wc);
    
    // Ambient
    color += u_obj_ambient * u_light_ambient;
    
    // Diffuse
    float ndotl = max(dot(normal_wc, light_dir), 0.0);
    color += ndotl * u_obj_diffuse * u_light_diffuse;
    
    // Specular
    float rdotv = max(dot(view_dir, reflect_dir), 0.0);
    color += pow(rdotv, u_obj_shininess) * u_obj_specular * u_light_specular;
    
    return vec4(color, 1.0f);
}

void main()
{
    gl_FragColor = phong_shading();
}
