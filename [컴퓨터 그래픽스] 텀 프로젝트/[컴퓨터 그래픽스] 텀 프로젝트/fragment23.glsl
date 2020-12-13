#version 330
//--- ex_Color: 버텍스 세이더에서 입력받는 색상 값
//--- gl_FragColor: 출력할 색상의 값으로 응용 프로그램으로 전달 됨.  

in  vec3 exColor; //--- 버텍스 세이더에게서 전달 받음
in vec2 TexCoord; 
out vec4 FragColor; //--- 색상 출력
uniform sampler2D exTexture;
void main(void) 
{
	//FragColor= vec4(outColor,1.0f);
	FragColor= texture(exTexture, TexCoord);
}