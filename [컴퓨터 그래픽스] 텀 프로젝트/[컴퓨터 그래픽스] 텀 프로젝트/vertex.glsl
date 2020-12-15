#version 330

layout (location = 0)in vec3 in_Position;
layout (location = 1)in vec3 vNormal;
layout (location = 2)in vec2 vTexCoord;

uniform mat4 Transform;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 in_Color;

out vec3 ex_Color;
out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

void main()
{
	gl_Position = Projection * View * Transform * vec4 (in_Position.x, in_Position.y, in_Position.z, 1.0);
	ex_Color = in_Color;
	TexCoord = vTexCoord;
	FragPos = vec3(Transform * vec4(in_Position, 1.0));
	Normal = vNormal;
}