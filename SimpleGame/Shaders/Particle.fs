#version 330

layout(location=0) out vec4 FragColor; //--- 최종 객체의 색 저장				 

in vec2 v_UV;

in vec4 FragPos;					 //--- 위치값
in vec3 Normal;					 	 //--- 버텍스 세이더에서 받은 노멀값

uniform sampler2D u_Texture;	// 텍스처 샘플러

uniform vec3 lightPos;				 //--- 조명의 위치
uniform vec3 lightColor;			 //--- 응용 프로그램에서 설정한 조명 색상
//uniform vec3 objectColor;			 //--- 응용 프로그램에서 설정한 객체의 색상
uniform vec3 viewPos;				 //--- 응용 프로그램에서 설정한 카메라 위치

void main()
{
	float ambientLight = 0.3;					//--- 주변 조명 계수: 0.0 ≤ ambientLight ≤ 1.0
	vec3 ambient = ambientLight * lightColor;	//--- 주변 조명값

	vec3 FragPosVec3 = FragPos.xyz;

	vec3 normalVector = normalize(Normal);						//--- 노말값을 정규화한다.
	vec3 lightDir = normalize(lightPos - FragPosVec3);			//--- 표면과 조명의 위치로 조명의 방향을 결정한다.
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);	//--- N과 L의 내적 값으로 강도 조절 (음의 값을 가질 수 없게 한다.)
	vec3 diffuse = diffuseLight * lightColor;					//--- 산란반사조명값=산란반사값*조명색상값

	int shininess = 128;										//--- 광택 계수
	vec3 viewDir = normalize (viewPos - FragPosVec3);			//--- 관찰자의 방향
	vec3 reflectDir = reflect (-lightDir, normalVector);		//--- 반사 방향: reflect 함수  입사 벡터의 반사 방향 계산
	float specularLight = max(dot(viewDir, reflectDir), 0.0);	//--- V와 R의 내적값으로 강도 조절: 음수 방지
	specularLight = pow(specularLight, shininess);				//--- shininess 승을 해주어 하이라이트를 만들어준다.
	vec3 specular = specularLight * lightColor;	

	//vec3 result = (ambient + diffuse + specular) * objectColor; //--- 최종 조명 설정된 픽셀 색상=(주변조명+산란반사조명)*객체 색상
	vec3 result = (ambient + diffuse + specular); //--- 최종 조명 설정된 픽셀 색상=(주변조명+산란반사조명)*객체 색상

	FragColor = vec4(result, 1.0);

	FragColor = texture(u_Texture, v_UV) * FragColor;
}
