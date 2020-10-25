#version 330 core
// TODO: define in/out and uniform variables.
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skyboxDay;
uniform samplerCube skyboxNight;

void main()
{   
	FragColor = texture(skyboxDay, TexCoords);
}
