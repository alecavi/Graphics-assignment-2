#version 420

in vec4 fColour;		// vertex colour
in vec2 fTexCoord;
in vec3 fLightDir;		// in tangent space
in vec3 fViewDir;		// in tangent space

layout (binding=0) uniform sampler2D tex;		// texture
layout (binding=1) uniform sampler2D normalMap;		// normal map inside a texture

out vec4 fragColor;		// Output fragment colour

uniform uint colourMode;	// Enables us to cycle through drawing modes to show the textures

// Function to calculate per-vertex lighting
vec3 phongModel(vec3 norm, vec3 diffR) {
	// Calcuate dot product between normal (perturbed by normal map) and light direction in tangent space
    float sDotN = max(dot(fLightDir, norm), 0.0 );

	// Calculate per-fragment diffuse reflection in tangent space
    vec3 diffuse = vec3(fColour) * diffR * sDotN;
	vec3 ambient = diffR * vec3(fColour) * 0.2 + vec3(fColour) * 0.1;

	// Calculate per-fragment specular colour 
	// Using a lazy specular hard-coded colour, should really import specular colour and shineness from application!
    vec3 r = reflect(-fLightDir, norm);
	    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
        spec = vec3(0.8, 0.5, 0.2) * pow(max(dot(r,fViewDir), 0.0), 1.0);
	}

    return ambient + diffuse + spec;
}

void main() {
    // Lookup the normal from the normal map 
	vec4 normalFromMap = texture(normalMap, fTexCoord);  

	// Convert the extracted normal_from_map to a surface normal
    vec4 normal = 2.0 * normalFromMap - 1.0;
	
	// Extract the texture from the texture map
    vec4 texColor = texture(tex, fTexCoord);

	// Calculate phong lighting, using the normal from the normal map
	// to perturb the real noraml to simulate bumps and dips in the texture
    vec4 normalMapLighting = vec4(phongModel(normal.xyz, texColor.rgb), 1.0);

	// Switch the output colour based on colour mode to cycle through
	// texture, normal map extract and full normal map lighting
	if (colourMode == 0 || colourMode == 1)
		fragColor = normalMapLighting;
	else if (colourMode == 2)
		fragColor = texColor;
	else if (colourMode == 3)
		fragColor = normalFromMap;
	else if (colourMode == 4)
		fragColor = normal;
}
