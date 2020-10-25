#version 330 core
// TODO: define in/out and uniform variables.
in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    // fill in
	TexCoords = vec3(aPos.x, aPos.y, aPos.z);
	vec4 pos = projection * view * vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
}