#version 330 core 
layout (location = 0) in vec3 vPos; //---응용프로그램에서받아온도형좌표값 
layout (location = 1) in vec3 vColor;
uniform mat4 transform; //---모델링변환행렬: uniform 변수로선언
uniform mat4 projection; //---투영변환값: 응용프로그램에서전달 uniform 변수로선언: 변수이름“projection”로받아옴
uniform mat4 modelTransform;
uniform mat4 viewTransform;

out vec3 outColor;
void main() 
{ 
   gl_Position = projection *  viewTransform * modelTransform * vec4(vPos, 1.0); //---좌표값에modelTransform변환을적용한다. 
   outColor = vColor;
} 