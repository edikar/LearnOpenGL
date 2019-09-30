#version 330 core

#define NR_POINT_LIGHTS 4  

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

struct Material {
    //sampler2D diffuseMap;
    //sampler2D specularMap;
    float     shininess;
}; 
/* Uniforms */
uniform sampler2D diffuseMap; //for soem reason it doesnt work properly when in the Material structure :/
uniform sampler2D specularMap; //for soem reason it doesnt work properly when in the Material structure :/
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 viewPos; 

/* In attributes */
in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal; 

/* Out attributes */
out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // define an output color value
    vec3 final = vec3(0.0);
    //calculate view directionm
    vec3 viewDir = normalize(viewPos - FragPos);
    // add the directional light's contribution to the output
    final += CalcDirLight(dirLight, Normal, viewDir);
    // do the same for all point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        final += CalcPointLight(pointLights[i], Normal, FragPos, viewDir);
    }
    // and add others lights as well (like spotlights)
    final += CalcSpotLight(spotLight, Normal, FragPos, viewDir);

    FragColor = vec4(final, 1.0);
}  

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(diffuseMap, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(specularMap, TexCoords));
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(diffuseMap, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(specularMap, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 


// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(diffuseMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(specularMap, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}