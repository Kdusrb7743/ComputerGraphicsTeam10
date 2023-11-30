#version 330

in vec3 a_Position;		// in = �Է� , out = ���
in vec3 a_Color;
in vec2 a_UV;

// ������

uniform mat4 modelTransform;
uniform mat4 CameraTransfrom;
uniform mat4 projectionTransform;

out vec3 v_Color;			//v == Varying
out vec2 v_UV;

void main()
{
	gl_Position = projectionTransform * CameraTransfrom * modelTransform * vec4(a_Position, 1.0);


	v_Color = a_Color;
	v_UV = a_UV;
}
