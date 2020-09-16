#version 330 core

/* Uniforms */
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform bool useHeightColor;

/* In attributes */
in vec3 FragPos;  
in vec3 Normal; 
in float FragHeight;

/* Out attributes */
out vec4 FragColor;

const float HEIGHT_FACTOR = 0.2;

void main()
{
	//Calculate ambient light component
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    //Calculate light direction and normal
	vec3 norm = normalize(Normal);
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
	vec3 finalColor;
	if(useHeightColor == true){
		
		float len = FragHeight;
		vec3 yellow = (len >= 0.0 && len < 0.01) ? vec3(0.5, 0.5, 0.0): vec3(0.0);
		vec3 green = (len >= 0.01 && len < 0.2) ? vec3(0.0, 0.5, 0.0) * (len) : vec3(0.0);
		vec3 brown = (len >= 0.2 && len < 0.6) ? vec3(0.6, 0.2, 0.0) * (len) : vec3(0.0); // 0 - 0.7
		vec3 white = (len >= 0.6) ? vec3(1.0, 1.0, 1.0) * (len) : vec3(0.0); // 0.7  - 1.0
		finalColor = brown + white + green + yellow;
	}else{
		finalColor = objectColor;
	}
    vec3 result = (ambient + diffuse + specular) * finalColor;
    FragColor = vec4(result, 0.5);
}  