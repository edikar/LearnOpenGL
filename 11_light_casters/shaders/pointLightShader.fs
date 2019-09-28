#version 330 core

struct Material {
    //sampler2D diffuseMap;
    //sampler2D specularMap;
    float     shininess;
}; 

struct Light {
    vec3 position; //used for point light
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    //attenuation parameters
    float constant;
    float linear;
    float quadratic;
};

/* Uniforms */
uniform sampler2D diffuseMap; //for soem reason it doesnt work properly when in the Material structure :/
uniform sampler2D specularMap; //for soem reason it doesnt work properly when in the Material structure :/
uniform sampler2D emissionMap; 
uniform Material material;
uniform Light light;  
uniform vec3 viewPos; 

/* In attributes */
in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal; 

/* Out attributes */
out vec4 FragColor;


void main()
{    
     // ambient
    vec3 ambient = light.ambient * vec3(texture(diffuseMap, TexCoords));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, TexCoords));  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(specularMap, TexCoords));

    //emission
    vec3 emission = vec3(0.0);
/*     if (texture(specularMap, TexCoords).r == 0.0)   //rough check for blackbox inside spec texture 
    {
        emission = vec3(texture(emissionMap, TexCoords));
    } */

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                    light.quadratic * (distance * distance)); 

    vec3 result = (ambient + diffuse + specular) * attenuation + emission;
    FragColor = vec4(result, 1.0);

}

#if 0

out vec4 FragColor;
  
in vec2 TexCoords;


struct Material {
    sampler2D diffuse;
}; 

uniform Material material;

void main()
{
    FragColor = texture(material.diffuse, TexCoords);
}


#endif