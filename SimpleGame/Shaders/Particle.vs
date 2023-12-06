#version 330

in vec3 a_Position;		// in = 입력 , out = 출력
in vec2 a_UV;
in vec3 a_Normal;

// 유니폼

uniform mat4 modelTransform;
uniform mat4 CameraTransfrom;
uniform mat4 projectionTransform;

out vec2 v_UV;
out vec3 Normal;
out vec3 FragPos;				//--- 객체의 위치값을 프래그먼트 세이더로 보낸다.

void main()
{
	gl_Position = projectionTransform * CameraTransfrom * modelTransform * vec4(a_Position, 1.0);

	FragPos = vec3(modelTransform * vec4(a_Position, 1.0));	//--- 객체에 대한 조명 계산을 프래그먼트 셰이더에서 한다.
	
	vec4 NormalVec4 = modelTransform * vec4(a_Normal, 1.0); //--- 노멀값을 프래그먼트 세이더로 보낸다.

	Normal = NormalVec4.xyz;

	v_UV = a_UV;
}
