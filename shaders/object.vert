// Adapted from https://learnopengl.com/Lighting/Basic-Lighting
// Licensed under CC-BY 4.0. See ATTRIBUTION.txt for details

#version 400

layout(location = 0) in vec3 position_L; //_L: local space
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 model, view, projection;
uniform vec3 lightPosition_W;

out vec2 fTexCoord;
out vec3 fPosition_V; //_V: view space;
out vec3 fNormal;
out vec3 fLightPosition_V;

void main() {
	fTexCoord = texCoord;
	fPosition_V = (view * model * vec4(position_L, 1.0)).xyz;
	fNormal = normal;
	fLightPosition_V = (view * vec4(lightPosition_W, 1.0)).xyz;
	gl_Position = projection * vec4(fPosition_V, 1.0);
}

