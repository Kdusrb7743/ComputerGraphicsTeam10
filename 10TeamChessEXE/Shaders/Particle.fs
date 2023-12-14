#version 330

layout(location=0) out vec4 FragColor; //--- ���� ��ü�� �� ����				 

in vec2 v_UV;

in vec4 FragPos;					 //--- ��ġ��
in vec3 Normal;					 	 //--- ���ؽ� ���̴����� ���� ��ְ�

uniform sampler2D u_Texture;	// �ؽ�ó ���÷�

uniform vec3 lightPos;				 //--- ������ ��ġ
uniform vec3 lightColor;			 //--- ���� ���α׷����� ������ ���� ����
//uniform vec3 objectColor;			 //--- ���� ���α׷����� ������ ��ü�� ����
uniform vec3 viewPos;				 //--- ���� ���α׷����� ������ ī�޶� ��ġ

void main()
{
	float ambientLight = 0.3;					//--- �ֺ� ���� ���: 0.0 �� ambientLight �� 1.0
	vec3 ambient = ambientLight * lightColor;	//--- �ֺ� ����

	vec3 FragPosVec3 = FragPos.xyz;

	vec3 normalVector = normalize(Normal);						//--- �븻���� ����ȭ�Ѵ�.
	vec3 lightDir = normalize(lightPos - FragPosVec3);			//--- ǥ��� ������ ��ġ�� ������ ������ �����Ѵ�.
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);	//--- N�� L�� ���� ������ ���� ���� (���� ���� ���� �� ���� �Ѵ�.)
	vec3 diffuse = diffuseLight * lightColor;					//--- ����ݻ�����=����ݻ簪*�������

	int shininess = 128;										//--- ���� ���
	vec3 viewDir = normalize (viewPos - FragPosVec3);			//--- �������� ����
	vec3 reflectDir = reflect (-lightDir, normalVector);		//--- �ݻ� ����: reflect �Լ�  �Ի� ������ �ݻ� ���� ���
	float specularLight = max(dot(viewDir, reflectDir), 0.0);	//--- V�� R�� ���������� ���� ����: ���� ����
	specularLight = pow(specularLight, shininess);				//--- shininess ���� ���־� ���̶���Ʈ�� ������ش�.
	vec3 specular = specularLight * lightColor;	

	//vec3 result = (ambient + diffuse + specular) * objectColor; //--- ���� ���� ������ �ȼ� ����=(�ֺ�����+����ݻ�����)*��ü ����
	vec3 result = (ambient + diffuse + specular); //--- ���� ���� ������ �ȼ� ����=(�ֺ�����+����ݻ�����)*��ü ����

	FragColor = vec4(result, 1.0);

	FragColor = texture(u_Texture, v_UV) * FragColor;
}
