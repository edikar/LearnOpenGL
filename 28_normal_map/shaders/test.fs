#version 330 core
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos; 

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

//if we use glBlend() then opengl will blend the source and destination according to blending function we set
void main()
{

	vec3 color = vec3(texture(texture1, TexCoords));
	// obtain normal from normal map in range [0,1]
	vec3 normal = texture(texture2, TexCoords).rgb;
	// transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(TBN * normal); 


	//Calculate ambient light component
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    //Calculate light direction and normal
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - FragPos); 
	//Calculate the diffuse light component
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//calculate the specular light component
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	vec3 specular = specularStrength * spec * lightColor;  

	//Calculate final color with all the components altogether

    vec3 result = (ambient + diffuse + specular) * color /* objectColor */;
    FragColor = vec4(result, 1.0);


	
}