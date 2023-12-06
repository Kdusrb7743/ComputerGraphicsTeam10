#version 330

in vec3 a_Position;		// in = �Է� , out = ���
in vec2 a_UV;
in vec3 a_Normal;

// ������

uniform mat4 modelTransform;
uniform mat4 CameraTransfrom;
uniform mat4 projectionTransform;

out vec2 v_UV;
out vec3 Normal;
out vec3 FragPos;				//--- ��ü�� ��ġ���� �����׸�Ʈ ���̴��� ������.

void main()
{
	gl_Position = projectionTransform * CameraTransfrom * modelTransform * vec4(a_Position, 1.0);

	FragPos = vec3(modelTransform * vec4(a_Position, 1.0));	//--- ��ü�� ���� ���� ����� �����׸�Ʈ ���̴����� �Ѵ�.
	
	vec4 NormalVec4 = modelTransform * vec4(a_Normal, 1.0); //--- ��ְ��� �����׸�Ʈ ���̴��� ������.

	Normal = NormalVec4.xyz;

	v_UV = a_UV;
}
