#version 420

// These are the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;		// vertex tangent vector

uniform mat4 model, view, projection;
uniform uint colourMode;

out vec4 fColour;
out vec2 fTexCoord;
out vec3 fLightDir;
out vec3 fViewDir;

vec3 lightDir = vec3(0.0, 5.0, 10.0);

void main() {	
	vec4 positionH = vec4(position, 1.0);		
	vec4 tangentH = vec4(tangent, 1.0);		
	
	vec4 diffuseColour;		
	if (colourMode == 1)
		diffuseColour = vec4(1.0, 1.0, 1.0, 1.0);
	else
		diffuseColour = vec4(0.9, 0.5, 0.3, 1.0);

	mat4 mvMatrix = view * model;
	mat3 normalMatrix = mat3(transpose(inverse(mvMatrix)));

	// Calculate the normal, tangent and binormal vectors in model-view space
	vec3 norm = normalize(normalMatrix * normal);
	vec3 tang = normalize(normalMatrix * tangent);
	vec3 binormal = normalize(cross(norm, tang)) * tangentH.w;

	// Define the matrix used to transform the light direction and view direction
	// into tangent space
	mat3 toObjectLocal = mat3(
		tang.x, binormal.x, norm.x,
		tang.y, binormal.y, norm.y,
		tang.z, binormal.z, norm.z
	);

	vec4 P = mvMatrix * positionH;
	vec3 pos = vec3(P);

	// Calculate the light direction in local vertex space (tangent space)
	fLightDir = normalize(toObjectLocal * lightDir);

	// Note, if using a positional light source then you would want to calculate the
	// light direction to the position as in the commented out line below
	//flightdir = normalize(toObjectLocal * (light_dir - pos));

	// Calculate the view direction in local vertex space (tangent space)
	fViewDir = toObjectLocal * normalize(-pos);

	// Define the vertex position
	gl_Position = projection * view * model * positionH;

	// Output the texture coordinates
	fTexCoord = texCoord.xy;

	// Output a vertex colour (this is optional, you may want to just use texture and normal map)
	fColour = diffuseColour;
}

