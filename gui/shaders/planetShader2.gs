#version 330 core
layout (points) in;
layout (points, max_vertices = 1) out;

in vec3 FragPos[];
in vec3 Normal[];
out vec3 FragPosGeo;
out vec3 NormalGeo;
  
void main() {    
    gl_Position = gl_in[0].gl_Position; 
    FragPosGeo = gl_in[0].gl_Position.xyz; //FragPos[0];
    NormalGeo = vec3(1.0f);//Normal[0];
    EmitVertex();
    EndPrimitive();
}
