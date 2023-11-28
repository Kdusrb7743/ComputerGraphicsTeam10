#version 330

layout(Location = 0)in vec3 a_Position;	
// 사용자가 입력한 값, 외부에서 입력을 받으려는 값
//layout(Location = 0) -> 인덱스를 줄 수 있다. 없다면 자동으로 인덱스 부여
in vec3 a_Position1;	

uniform vec4 u_Trans;

uniform float u_Scale;

void main()
{
	vec4 newPosition;
	newPosition.xy = u_Scale * (a_Position.xy + a_Position1.xy) * u_Trans.w + u_Trans.xy;
	newPosition.z = 0;
	newPosition.w= 1;
	gl_Position = newPosition;
}
