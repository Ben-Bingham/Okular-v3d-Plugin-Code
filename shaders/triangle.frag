#version 450

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inFragPos;

layout (location = 0) out vec4 outFragColor;

void main() {
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 objectColor = vec3(0.35, 0.67, 0.82);
	vec3 lightPos = vec3(50.0, 50.0, 50.0);
	vec3 viewPos = vec3(0.0, 0.0, -10.0);
	float specularStrength = 0.5;

	// Ambient
	float ambientStrength = 0.3;

	vec3 ambient = ambientStrength * lightColor;

	// Diffuse
	vec3 norm = normalize(inNormal);
	vec3 lightDir = normalize(lightPos - inFragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// Specular
	vec3 viewDir = normalize(viewPos - inFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	// Total
	vec3 result = (ambient + diffuse + specular) * objectColor;

    outFragColor = vec4(result, 1.0);
}