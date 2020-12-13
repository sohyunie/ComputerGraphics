#version 330

uniform vec3 viewPos;        //카메라의 EYE
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
	vec3 ambientLight = ambientLight_on_off; //--- 주변 조명 세기
    vec3 ambient = ambientLight * lightColor; //--- 주변 조명 값

    vec3 normalVector = normalize (Normal);
    vec3 lightDir = normalize(lightPos - FragPos); //--- 표면과 조명의 위치로 조명의 방향을 결정한다.
    float diffuseLight = max(dot(normalVector, lightDir), 0.0); //--- N과 L의 내적 값으로 강도 조절: 음수 방지
    vec3 diffuse = diffuseLight * lightColor * 0.5; //--- 산란 반사 조명값: 산란반사값 * 조명색상값

    int shininess = 128; //--- 광택 계수
    vec3 viewDir = normalize (viewPos-FragPos); //--- 관찰자의 방향 // 원래는 viewPos
    vec3 reflectDir = reflect (-lightDir, normalVector); //--- 반사 방향: reflect 함수 - 입사 벡터의 반사 방향 계산
    float tmp =  max (dot (viewDir, reflectDir), 0.0);
    float specularLight = tmp;//--- V와 R의 내적값으로 강도 조절: 음수 방지
    specularLight = pow(specularLight, shininess); //--- shininess 승을 해주어 하이라이트를 만들어준다.
    vec3 specular = specularLight * lightColor; //--- 거울 반사 조명값: 거울반사값 * 조명색상값
    
    vec3 result = (ambient + diffuse + specular) * ex_Color; //--- 최종 조명 설정된 픽셀 색상: (주변+산란반사+거울반사조명)*객체 색상

	FragColor = vec4(result, Alpha);
}