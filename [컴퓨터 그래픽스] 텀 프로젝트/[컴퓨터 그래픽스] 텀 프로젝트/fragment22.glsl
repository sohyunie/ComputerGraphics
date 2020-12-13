#version 330

uniform vec3 viewPos;        //ī�޶��� EYE
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientLight_on_off;

in vec3 ex_Color;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 Alpha; 
out vec4 FragColor;

void main()
{
	vec3 ambientLight = ambientLight_on_off; //--- �ֺ� ���� ����
    vec3 ambient = ambientLight * lightColor; //--- �ֺ� ���� ��

    vec3 normalVector = normalize (Normal);
    vec3 lightDir = normalize(lightPos - FragPos); //--- ǥ��� ������ ��ġ�� ������ ������ �����Ѵ�.
    float diffuseLight = max(dot(normalVector, lightDir), 0.0); //--- N�� L�� ���� ������ ���� ����: ���� ����
    vec3 diffuse = diffuseLight * lightColor * 0.5; //--- ��� �ݻ� ����: ����ݻ簪 * �������

    int shininess = 128; //--- ���� ���
    vec3 viewDir = normalize (viewPos-FragPos); //--- �������� ���� // ������ viewPos
    vec3 reflectDir = reflect (-lightDir, normalVector); //--- �ݻ� ����: reflect �Լ� - �Ի� ������ �ݻ� ���� ���
    float tmp =  max (dot (viewDir, reflectDir), 0.0);
    float specularLight = tmp;//--- V�� R�� ���������� ���� ����: ���� ����
    specularLight = pow(specularLight, shininess); //--- shininess ���� ���־� ���̶���Ʈ�� ������ش�.
    vec3 specular = specularLight * lightColor; //--- �ſ� �ݻ� ����: �ſ�ݻ簪 * �������
    
    vec3 result = (ambient + diffuse + specular) * ex_Color; //--- ���� ���� ������ �ȼ� ����: (�ֺ�+����ݻ�+�ſ�ݻ�����)*��ü ����

	FragColor = vec4(result, Alpha);
}