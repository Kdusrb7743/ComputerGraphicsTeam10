#version 330

in vec3 a_Position;
in vec2 a_Texcoord;

out vec2 v_UV;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
	//v_UV = a_Texcoord;
	v_UV = a_Texcoord;
}
