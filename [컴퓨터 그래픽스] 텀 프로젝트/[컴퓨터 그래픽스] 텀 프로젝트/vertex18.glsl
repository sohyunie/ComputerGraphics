#version 330 core 
layout (location = 0) in vec3 vPos; //---�������α׷������޾ƿµ�����ǥ�� 
layout (location = 1) in vec3 vColor;
uniform mat4 transform; //---�𵨸���ȯ���: uniform �����μ���
uniform mat4 projection; //---������ȯ��: �������α׷��������� uniform �����μ���: �����̸���projection���ι޾ƿ�
uniform mat4 modelTransform;
uniform mat4 viewTransform;

out vec3 outColor;
void main() 
{ 
   gl_Position = projection *  viewTransform * modelTransform * vec4(vPos, 1.0); //---��ǥ����modelTransform��ȯ�������Ѵ�. 
   outColor = vColor;
} 