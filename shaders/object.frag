// Adapted from https://learnopengl.com/Lighting/Basic-Lighting
// Licensed under CC-BY 4.0. See ATTRIBUTION.txt for details

#version 400

vec3 lightColour = vec3(1.0, 1.0, 1.0);
float ambientLightStrength = 0.2;
float specularLightStrength = 0.5;
float shininess = 8;

uniform sampler2D tex;

in vec2 fTexCoord;
in vec3 fPosition_V;
in vec3 fNormal;
in vec3 fLightPosition_V;

out vec4 outputColor;

void main() {
	//ambient
	vec3 ambient = ambientLightStrength * lightColour;

	//diffuse
	vec3 lightVec_V = fLightPosition_V - fPosition_V;
	vec3 lightDir = normalize(lightVec_V);
	vec3 diffuse = max(dot(fNormal, lightDir), 0.0) * lightColour;

	//specular
	vec3 viewDir = normalize(-fPosition_V);
	vec3 reflectDir = reflect(-lightDir, fNormal);
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess) * specularLightStrength * lightColour;

	//texture
	vec4 texColour = texture(tex, fTexCoord);

	outputColor = vec4(ambient + diffuse + specular, 1.0) * texColour;
}