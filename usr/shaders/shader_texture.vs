#version 330 core
// TODO: define in/out and uniform variables.
in vec3 aPos;
in vec2 aTexCoord;
in vec3 aNormal;
in vec3 aColor;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);

	FragPos = vec3(model * vec4(aPos, 1.0));
	TexCoord = aTexCoord;
	Normal =  mat3(transpose(inverse(model))) * aNormal;

	Color = aColor;
}
