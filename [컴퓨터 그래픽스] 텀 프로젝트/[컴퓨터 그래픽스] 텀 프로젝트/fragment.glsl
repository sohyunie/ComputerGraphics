#version 330 core

in vec3 Normal;
in vec3 FragPos; 
in vec3 ex_Color;

out vec4 out_color;

uniform vec3 g_lightAmbient;
uniform vec3 g_lightPos;
uniform vec3 g_lightColor;
uniform vec3 g_cameraPos;

void main(void)
{	
	vec3 ambient = g_lightAmbient * g_lightColor;

	vec3 normalVector = normalize(Normal);
	vec3 lightDir = normalize(g_lightPos - FragPos);
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * g_lightColor;

	int shininess = 128;
	vec3 viewDir = normalize(g_cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normalVector);
	float specularLight = max(dot(viewDir, reflectDir), 0.0);
	specularLight = pow(specularLight, shininess);
	vec3 specular = specularLight * g_lightColor;

	vec3 result = (ambient + diffuse + specular) * ex_Color;

	out_color = vec4(result, 1.0);
}