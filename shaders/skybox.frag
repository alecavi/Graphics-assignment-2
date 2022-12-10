//adapted from https://learnopengl.com/Advanced-OpenGL/Cubemaps
//Licensed under CC-BY 4.0. See ATTRIBUTION.txt for details

#version 400 core

in vec3 fTexCoord;

uniform samplerCube skybox;

out vec4 outputColor;

void main() {    
    outputColor = texture(skybox, fTexCoord);
}