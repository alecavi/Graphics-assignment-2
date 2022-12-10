//Adapted from https://learnopengl.com/Advanced-OpenGL/Cubemaps.
//Licensed under CC-BY 4.0. See ATTRIBUTION.txt for details

#version 400 core
layout (location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

out vec3 fTexCoord;

void main() {
    fTexCoord = position;
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}