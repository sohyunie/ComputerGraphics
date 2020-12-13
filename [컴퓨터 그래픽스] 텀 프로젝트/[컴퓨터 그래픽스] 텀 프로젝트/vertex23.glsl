#version 330

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
layout(location=2) in vec2 vTexCoord;
out vec3 exColor;
out vec2 TexCoord;
uniform mat4 modelTransform;
void main(void) 
{ 
	gl_Position = modelTransform* vec4(vPos,1.0);

	exColor= vColor;
            TexCoord=vTexCoord;
}